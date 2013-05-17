#pragma once
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <memory>
#include <functional>
#include <msgpack/rpc/protocol.h>
#include <msgpack/rpc/types.h>
#include <msgpack/rpc/exception.h>
#include "../tuple_util.h"
#include "asio/server_request_queue.h"
#include "asio/session.h"
#include "asio/dispatcher.h"
#include "asio/server.h"

