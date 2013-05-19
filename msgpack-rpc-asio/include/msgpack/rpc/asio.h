#pragma once
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <memory>
#include <functional>
#include <msgpack/rpc/protocol.h>
#include "../tuple_util.h"

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
};

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
        ::msgpack::rpc::msg_response<int, bool> msgres(
                m_code,
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

