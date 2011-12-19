// Copyright 2011 Google Inc. All Rights Reserved.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: nadavs@google.com <Nadav Samet>

#include "zrpc/connection_manager.h"

#include <algorithm>
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>
#include <map>
#include <ostream>
#include <pthread.h>
#include <sstream>
#include <stddef.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>
#include <zmq.h>
#include <zmq.hpp>

#include "glog/logging.h"
#include "google/protobuf/stubs/common.h"
#include "zrpc/callback.h"
#include "zrpc/clock.h"
#include "zrpc/connection_manager_controller.h"
#include "zrpc/event_manager.h"
#include "zrpc/macros.h"
#include "zrpc/reactor.h"
#include "zrpc/rpc_channel.h"
#include "zrpc/simple_rpc_channel.h"
#include "zrpc/zmq_utils.h"

namespace zrpc {
namespace {
static const uint64 kLargePrime = (1ULL << 63) - 165;
static const uint64 kGenerator = 2;

typedef uint64 EventId;

class EventIdGenerator {
 public:
  EventIdGenerator() {
    state_ = (reinterpret_cast<uint64>(this) << 32) + getpid();
  }

  EventId GetNext() {
    state_ = (state_ * kGenerator) % kLargePrime;
    return state_;
  }

 private:
  uint64 state_;
  DISALLOW_COPY_AND_ASSIGN(EventIdGenerator);
};
}  // unnamed namespace

struct RemoteResponseWrapper {
  RemoteResponse* remote_response;
  int64 deadline_ms;
  uint64 start_time;
  Closure* closure;
  MessageVector return_path;
};

class ConnectionThreadContext {
 public:
  ConnectionThreadContext(
      EventManager* internal_event_manager,
      EventManager::ThreadContext* thread_context) {
    internal_event_manager_ = internal_event_manager;
    thread_context_ = thread_context;
  }

  void HandleClientSocket(zmq::socket_t* socket) {
    MessageVector messages;
    ReadMessageToVector(socket, &messages);
    CHECK(messages.size() >= 1);
    CHECK_EQ(messages[0]->size(), 0);
    EventId event_id(InterpretMessage<EventId>(*messages[1]));
    RemoteResponseMap::iterator iter = remote_response_map_.find(event_id);
    if (iter == remote_response_map_.end()) {
      return;
    }
    RemoteResponseWrapper*& remote_response_wrapper = iter->second;
    RemoteResponse*& remote_response = remote_response_wrapper->remote_response;
    messages.erase(0);
    messages.erase(0);
    remote_response->reply.swap(messages);
    remote_response->status = RemoteResponse::DONE;
    LOG(INFO)<<"Notifying";
    remote_response->status_cond.notify_all();
    remote_response_map_.erase(iter);
  }

  inline zmq::socket_t* GetSocketForConnection(
      Connection* connection) {
    ConnectionThreadContext::ConnectionMap::const_iterator it =
        connection_map_.find(
      connection);
    if (it != connection_map_.end()) {
      return it->second;
    }
    zmq::socket_t *socket = connection->CreateConnectedSocket(
        thread_context_->zmq_context);
    connection_map_[connection] = socket;
    thread_context_->reactor->AddSocket(
        socket, NewPermanentCallback(
            this, &ConnectionThreadContext::HandleClientSocket, socket));
    return socket;
  }

  void SendRequest(Connection* connection,
                   MessageVector* request,
                   RemoteResponseWrapper* remote_response_wrapper) {
    zmq::socket_t* socket = CHECK_NOTNULL(GetSocketForConnection(connection));
    EventId event_id = event_id_generator_.GetNext();
    remote_response_map_[event_id] = remote_response_wrapper;
    if (remote_response_wrapper->deadline_ms != -1) {
      thread_context_->reactor->RunClosureAt(
          remote_response_wrapper->start_time +
              remote_response_wrapper->deadline_ms,
          NewCallback(this, &ConnectionThreadContext::HandleTimeout, event_id));
    }

    SendString(socket, "", ZMQ_SNDMORE);
    SendUint64(socket, event_id, ZMQ_SNDMORE);
    WriteVectorToSocket(socket, *request); 
  }

  void HandleTimeout(EventId event_id) {
    RemoteResponseMap::iterator iter = remote_response_map_.find(event_id);
    if (iter == remote_response_map_.end()) {
      return;
    }
    RemoteResponseWrapper*& remote_response_wrapper = iter->second;
    RemoteResponse*& remote_response = remote_response_wrapper->remote_response;
    remote_response->status = RemoteResponse::DEADLINE_EXCEEDED;
    WriteVectorToSocket(thread_context_->app_socket,
                        remote_response_wrapper->return_path,
                        ZMQ_SNDMORE);
    SendPointer(thread_context_->app_socket, remote_response, 0);
    remote_response_map_.erase(iter);
  }

 private:
  typedef std::map<Connection*, zmq::socket_t*> ConnectionMap;
  typedef std::map<EventId, RemoteResponseWrapper*> RemoteResponseMap;
  typedef std::map<uint64, EventId> DeadlineMap;
  ConnectionMap connection_map_;
  RemoteResponseMap remote_response_map_;
  DeadlineMap deadline_map_;
  EventIdGenerator event_id_generator_;
  EventManager* internal_event_manager_;
  EventManager::ThreadContext* thread_context_;
};

void InitContext(EventManager* em, EventManager::ThreadContext* context,
                 void* user_data) {
  ConnectionThreadContext* conn_context = new ConnectionThreadContext(
      em, context);
  ConnectionManager *conn = static_cast<ConnectionManager*>(user_data);
  conn->thread_context_->reset(conn_context);
}

ConnectionManager::ConnectionManager(
    zmq::context_t* context, EventManager* event_manager,
    int nthreads)
  : context_(context),
    external_event_manager_(event_manager),
    thread_context_(new boost::thread_specific_ptr<ConnectionThreadContext>()) {
  internal_event_manager_.reset(
      new EventManager(context, nthreads, InitContext, this));
}

namespace internal_thread {
// This namespace is used to contain functions that run in a thread that belongs
// to the internal event manager.
void SendRequest(
    ConnectionManager* connection_manager,
    Connection* connection, 
    MessageVector* request,
    RemoteResponseWrapper* response_wrapper) {
  CHECK_NOTNULL(connection_manager);
  CHECK_NOTNULL(connection_manager->thread_context_->get());
  connection_manager->thread_context_->get()->SendRequest(
      connection, request, response_wrapper);
}
}  // namespace internal_thread

class ConnectionImpl : public Connection {
 public:
  ConnectionImpl(ConnectionManager* connection_manager,
                 std::string endpoint)
      : Connection(),
        connection_manager_(connection_manager),
        endpoint_(endpoint) {}

  virtual RpcChannel* MakeChannel() {
    return new SimpleRpcChannel(this);
  }

  virtual void SendRequest(
      MessageVector* request,
      RemoteResponse* response,
      int64 deadline_ms,
      Closure* closure) {
    RemoteResponseWrapper* wrapper = new RemoteResponseWrapper;
    wrapper->remote_response = response;
    wrapper->start_time = zclock_time();
    wrapper->deadline_ms = deadline_ms;
    wrapper->closure = closure;

    connection_manager_->internal_event_manager_->Add(
        NewCallback(
            &internal_thread::SendRequest,
            connection_manager_,
            static_cast<Connection*>(this), request, wrapper));
  }

  virtual int WaitFor(RemoteResponse* response) {
    boost::mutex mut;
    boost::unique_lock<boost::mutex> lock(mut);
    while ((response->status == RemoteResponse::ACTIVE) ||
           (response->status == RemoteResponse::INACTIVE)) {
      response->status_cond.wait(lock);
    }
    return 0;
  }

  virtual zmq::socket_t* CreateConnectedSocket(zmq::context_t* context) {
    zmq::socket_t* socket = new zmq::socket_t(*context, ZMQ_DEALER);
    socket->connect(endpoint_.c_str());
    return socket;
  }

 private:
  ConnectionManager* connection_manager_;
  std::string endpoint_;
  DISALLOW_COPY_AND_ASSIGN(ConnectionImpl);
};

Connection* ConnectionManager::Connect(const std::string& endpoint) {
  Connection* connection = new ConnectionImpl(
      this,
      endpoint);
  return connection;
}
 
ConnectionManager::~ConnectionManager() {}
}  // namespace zrpc