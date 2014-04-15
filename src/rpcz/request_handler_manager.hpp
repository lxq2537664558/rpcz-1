// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef RPCZ_REQUEST_HANDLER_MANAGER
#define RPCZ_REQUEST_HANDLER_MANAGER

#include <boost/unordered_map.hpp>
#include "request_handler_ptr.hpp"
#include "service_factory_map.hpp"

namespace rpcz {

// Managers all senders' request_handlers.
// Run in broker thread.
// Non-thread-safe.
class request_handler_manager
{
public:
	request_handler_manager(void);
	virtual ~request_handler_manager(void);

public:
	inline request_handler * get_handler(const std::string & sender,
		const service_factory_map & factories);

private:
	request_handler * create_handler(const std::string & sender,
		const service_factory_map & factories);

private:
	typedef boost::unordered_map<std::string, request_handler_ptr> handler_map;
	handler_map handler_map_;
};

request_handler * request_handler_manager::get_handler(
	const std::string & sender, const service_factory_map & factories)
{
	handler_map::const_iterator iter = handler_map_.find(sender);
	if (iter != handler_map_.end())
		return (*iter).second.get();
	return create_handler(sender, factories);
}

}  // namespace rpcz
#endif  // RPCZ_REQUEST_HANDLER_MANAGER