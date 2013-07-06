#pragma once
#include <functional>

namespace msgpack {
namespace rpc {
namespace asio {

enum connection_status 
{
    connection_none,
    connection_connecting,
    connection_connected,
    connection_error,
};
typedef std::function<void(connection_status)> connection_callback_t;


}}}
