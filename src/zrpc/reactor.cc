// Copyright 2011, Nadav Samet.
// All rights reserved.
//
// Author: thesamet@gmail.com <Nadav Samet>

#include <signal.h>
#include <vector>
#include "glog/logging.h"
#include "zrpc/clock.h"
#include "zrpc/macros.h"
#include "zrpc/reactor.h"
#include "zmq.hpp"

namespace zrpc {
namespace {
static bool g_interrupted = false;
void SignalHandler(int signal_value) {
  LOG(INFO) << "Caught "
      << ((signal_value == SIGTERM) ? "SIGTERM" :
          (signal_value == SIGINT) ? "SIGINT" : "signal") << ".";
  g_interrupted = true;
}
}  // unnamed namespace

Reactor::Reactor() : should_quit_(false) {
};

Reactor::~Reactor() {
  DeleteContainerPairPointers(sockets_.begin(), sockets_.end());
  for (ClosureRunMap::const_iterator it = closure_run_map_.begin();
       it != closure_run_map_.end(); ++it) {
    DeleteContainerPointers(it->second.begin(), it->second.end());
  }
}

void Reactor::AddSocket(zmq::socket_t* socket, Closure* closure) {
  sockets_.push_back(std::make_pair(socket, closure));
  is_dirty_ = true;
}

namespace {
void RebuildPollItems(
    const std::vector<std::pair<zmq::socket_t*, Closure*> >& sockets,
    std::vector<zmq::pollitem_t>* pollitems) {
  pollitems->resize(sockets.size());
  for (size_t i = 0; i < sockets.size(); ++i) {
    zmq::socket_t& socket = *sockets[i].first;
    zmq::pollitem_t pollitem = {socket, 0, ZMQ_POLLIN, 0};
    (*pollitems)[i] = pollitem;
  }
}
}  // namespace

void Reactor::RunClosureAt(uint64 timestamp, Closure* closure) {
  closure_run_map_[timestamp].push_back(closure);
}

void Reactor::LoopUntil(StoppingCondition* stop_condition) {
  while (!should_quit_ && !g_interrupted && (stop_condition == NULL ||
                                             !stop_condition->ShouldStop())) {
    if (is_dirty_) {
      RebuildPollItems(sockets_, &pollitems_);
      is_dirty_ = false;
    }

    long poll_timeout = ProcessClosureRunMap();
    int rc = zmq::poll(&pollitems_[0], pollitems_.size(), poll_timeout);
    CHECK_NE(rc, -1);
    for (size_t i = 0; i < pollitems_.size(); ++i) {
      if (!pollitems_[i].revents & ZMQ_POLLIN) {
        continue;
      }
      pollitems_[i].revents = 0;
      sockets_[i].second->Run();
    }
  }
}

long Reactor::ProcessClosureRunMap() {
  uint64 now = zclock_time();
  ClosureRunMap::iterator ub(closure_run_map_.upper_bound(now));
  for (ClosureRunMap::const_iterator it = closure_run_map_.begin();
       it != ub;
       ++it) {
    for (std::vector<Closure*>::const_iterator vit = it->second.begin();
         vit != it->second.end(); ++vit) {
      (*vit)->Run();
    }
  }
  long poll_timeout = -1;
  if (ub != closure_run_map_.end()) {
    poll_timeout = 1000 * (ub->first - now);
  }
  closure_run_map_.erase(closure_run_map_.begin(), ub);
  return poll_timeout;
}

void Reactor::SetShouldQuit() {
  should_quit_ = true;
}

void InstallSignalHandler() {
  struct sigaction action;
  action.sa_handler = SignalHandler;
  action.sa_flags = 0;
  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);
}
}  // namespace zrpc