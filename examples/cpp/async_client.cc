// Licensed under the Apache License, Version 2.0 (the "License");
//     http://www.apache.org/licenses/LICENSE-2.0
// Author: Jin Qing (http://blog.csdn.net/jq0123)

// Asynchronous client example.

#include <iostream>

#include "cpp/search.pb.h"
#include "cpp/search.rpcz.h"
#include "rpcz/callback.hpp"  // for new_callback
#include "rpcz/rpc_controller.hpp"  // for rpc_controller
#include "rpcz/rpcz.hpp"

using namespace std;

void done(examples::SearchResponse *response) {
  cout << response->DebugString() << endl;
}

int main() {
  examples::SearchService_Stub search_stub("tcp://localhost:5555");
  examples::SearchRequest request;
  examples::SearchResponse response;
  request.set_query("gold");

  cout << "Sending request." << endl;
  rpcz::rpc_controller ctrl;
  search_stub.Search(request, &response, &ctrl,
      rpcz::new_callback(&done, &response));

  // Do other works...
  std::cin.get();
}