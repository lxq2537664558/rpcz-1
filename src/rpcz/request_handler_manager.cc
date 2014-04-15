// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "request_handler_manager.hpp"

#include <boost/foreach.hpp>
#include "request_handler.hpp"

namespace rpcz {

request_handler_manager::request_handler_manager(void)
{
}

request_handler_manager::~request_handler_manager(void)
{
}

request_handler * request_handler_manager::create_handler(
	const std::string & sender, const service_factory_map & factories)
{
	assert(handler_map_.find(sender) == handler_map_.end());
	// New request_handler. TODO: delete request_handler
	request_handler_ptr handler_ptr(new request_handler);  // shared_ptr
	// XXX Create service for this handler...
	BOOST_FOREACH(const service_factory_map::value_type & v, factories)
	{
		// XXX handler_ptr->register_rpc_service();
	}
	handler_map_.insert(std::make_pair(sender, handler_ptr));
	return handler_ptr.get();
}

}  // namespace rpcz