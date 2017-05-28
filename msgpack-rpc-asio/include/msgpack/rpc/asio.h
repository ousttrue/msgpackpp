#pragma once
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4819)
#endif
#include <asio.hpp>
#include <thread>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <memory>
#include <functional>
#include <msgpack/rpc/protocol.h>
#include "../tuple_util.h"
#include "../pack_util.h"

namespace msgpack {
namespace rpc {
namespace asio {

enum error_code
{
    success,
    error_dispatcher_no_handler,
    error_params_not_array,
    error_params_too_many,
    error_params_not_enough,
    error_params_convert,
    error_not_implemented,
    error_self_pointer_is_null,
};
typedef std::function<void(::asio::error_code error)> error_handler_t;


enum connection_status 
{
    connection_none,
    connection_connecting,
    connection_connected,
    connection_error,
};
typedef std::function<void(connection_status)> connection_callback_t;


class msgerror: std::runtime_error
{
    error_code m_code;

public:
    msgerror(const std::string &msg, error_code code):std::runtime_error(msg), m_code(code)
    {
    }

    std::shared_ptr<msgpack::sbuffer> to_msg(msgpack::rpc::msgid_t msgid)const
    {
        // error type
        ::msgpack::rpc::msg_response<std::tuple<int, std::string>, bool> msgres(
                std::make_tuple(m_code, what()),
                true,
                msgid);
        // result
        auto sbuf=std::make_shared<msgpack::sbuffer>();
        msgpack::pack(*sbuf, msgres);
        return sbuf;
    }

};

}}}

#include "asio/session.h"
#include "asio/dispatcher.h"
#include "asio/server.h"
#include "asio/client.h"

