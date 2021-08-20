#pragma once
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif
#include <asio.hpp>
#include <thread>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <functional>
#include <memory>

namespace msgpack_rpc {

enum error_code {
  success,
  error_dispatcher_no_handler,
  error_params_not_array,
  error_params_too_many,
  error_params_not_enough,
  error_params_convert,
  error_not_implemented,
  error_self_pointer_is_null,
};
typedef std::function<void(asio::error_code error)> error_handler_t;

enum connection_status {
  connection_none,
  connection_connecting,
  connection_connected,
  connection_error,
};
typedef std::function<void(connection_status)> connection_callback_t;

struct msgerror : std::runtime_error {
  error_code code;

public:
  msgerror(const std::string &msg, error_code code)
      : std::runtime_error(msg), code(code) {}
};

} // namespace msgpack_rpc

#include "asio/session.h"
#include "asio/dispatcher.h"
#include "asio/client.h"
#include "asio/server.h"
