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
//         Jin Qing (http://blog.csdn.net/jq0123)

#include "rpcz/cpp_service.hpp"

#include <assert.h>
#include <string>

#include <google/protobuf/descriptor.h>  // for FindMethodByName()
#include <google/protobuf/message.h>  // for Message

#include "logging.hpp"  // for INFO
#include "rpcz/common.hpp"  // for scoped_ptr
#include "rpcz/replier.hpp"
#include "rpcz/rpc_controller.hpp"  // for application_error

namespace rpcz {

void cpp_service::dispatch_request(const std::string& method,
                                const void* payload, size_t payload_len,
                                replier replier_copy) {
    const ::google::protobuf::MethodDescriptor* descriptor =
        GetDescriptor()->FindMethodByName(method);
    if (descriptor == NULL) {
      // Invalid method name
      DLOG(INFO) << "Invalid method name: " << method;
      replier_copy.send_error(application_error::NO_SUCH_METHOD);
      return;
    }

    scoped_ptr<google::protobuf::Message> request;
    request.reset(CHECK_NOTNULL(
        GetRequestPrototype(descriptor).New()));
    if (!request->ParseFromArray(payload, payload_len)) {
      DLOG(INFO) << "Failed to parse request.";
      // Invalid proto;
      replier_copy.send_error(application_error::INVALID_MESSAGE);
      return;
    }
    call_method(descriptor, *request, replier_copy);
}  // dispatch_request()

}  // namespace rpcz