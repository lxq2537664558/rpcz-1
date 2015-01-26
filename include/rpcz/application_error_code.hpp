// Copyright 2011 Google Inc. All Rights Reserved.
// Copyright 2015 Jin Qing.
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

#ifndef RPCZ_APPLICATION_ERROR_CODE_HPP
#define RPCZ_APPLICATION_ERROR_CODE_HPP

// Can not #include rpcz.pb.h for rpc_response_header,
// because rpcz.pb.h is only for internal use.

namespace rpcz {

typedef int application_error_code;

// Must equal rpc_response_header::application_error_code.
// See rpcz.proto.
namespace application_error {
static const application_error_code APPLICATION_NO_ERROR = 0;
static const application_error_code INVALID_HEADER = -1;
static const application_error_code NO_SUCH_SERVICE = -2;
static const application_error_code NO_SUCH_METHOD = -3;
static const application_error_code INVALID_MESSAGE = -4;
static const application_error_code METHOD_NOT_IMPLEMENTED = -5;
}  // namespace application_error

}  // namespace rpcz
#endif  // RPCZ_APPLICATION_ERROR_CODE_HPP
