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

#include <rpcz/server_impl.hpp>

#include <boost/foreach.hpp>

#include <rpcz/manager.hpp>
#include <rpcz/service_factory_map.hpp>

namespace rpcz {

server_impl::server_impl()
  : manager_(manager::get()),
    service_factory_map_(new service_factory_map) {  // shared_ptr
  BOOST_ASSERT(manager_);
}

// TODO: unbine service factory...

server_impl::~server_impl() {
  // unbind first
  BOOST_FOREACH(const bind_endpoint_set::value_type& v, endpoints_)
    manager_->unbind(v);
  endpoints_.clear();
}

void server_impl::register_service_factory(
    const std::string& name,
    const service_factory_ptr& factory) {
  BOOST_ASSERT(factory);
  service_factory_map_->insert(name, factory);
}

void server_impl::bind(const std::string& endpoint) {
  // Record endpoints for unbind later. (Server can multi bind.)
  if (!endpoints_.insert(endpoint).second)
    return;  // already bound
  manager_->bind(endpoint, service_factory_map_);
}

}  // namespace rpcz
