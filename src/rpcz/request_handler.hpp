// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef RPCZ_REQUEST_HANDLER_H
#define RPCZ_REQUEST_HANDLER_H

#include <map>
#include "client_connection.hpp"
#include "rpcz/common.hpp"  // for uint64

namespace rpcz {

class message_iterator;
class rpc_service;

// Each client has its request_hander.
// Used in broker thread.
// TODO: Run in the same worker thread.
// Non-thread-safe.
class request_handler {
 public:
  request_handler(uint64 server_socket_idx, const std::string & sender);
  ~request_handler();

 public:
  void handle_request(message_iterator& iter);

 public:
  // register_rpc_service() will take the ownership of rpc_service.
  void register_rpc_service(rpcz::rpc_service* rpc_service,
                            const std::string& name);

 private:
  void unregister_rpc_service(const std::string& name);

  // DEL
 //private:
 // const uint64 server_socket_idx_;
 // const std::string sender_;

 private:
  typedef std::map<std::string, rpcz::rpc_service*> rpc_service_map;
  rpc_service_map service_map_;  // Owns rpc_service. Delete in destructor.

  client_connection client_connection_;
};

}  // namespace rpcz

#endif  // RPCZ_REQUEST_HANDLER_H
