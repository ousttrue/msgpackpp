//
// msgpack::rpc::exception - MessagePack-RPC for C++
//
// Copyright (C) 2010 FURUHASHI Sadayuki
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
#ifndef MSGPACK_RPC_EXCEPTION_H__
#define MSGPACK_RPC_EXCEPTION_H__

#include <stdexcept>

namespace msgpack {
namespace rpc {


struct rpc_error : public std::runtime_error {
	rpc_error(const std::string& msg) :
		std::runtime_error(msg) {}
};


struct unknown_transport : rpc_error {
	unknown_transport() :
		rpc_error("unknown transport") {}

	unknown_transport(const std::string& msg) :
		rpc_error(msg) {}
};


struct timeout_error : rpc_error {
	timeout_error() :
		rpc_error("request timed out") {}

	timeout_error(const std::string& msg) :
		rpc_error(msg) {}
};

struct connect_error : timeout_error {
	connect_error() :
		timeout_error("connect failed") {}

	connect_error(const std::string& msg) :
		timeout_error(msg) {}
};


struct call_error : rpc_error {
	call_error(const std::string& msg) :
		rpc_error(msg) {}
};

struct no_method_error : call_error {
	no_method_error() :
		call_error("method not found") {}

	no_method_error(const std::string& msg) :
		call_error(msg) {}
};

struct argument_error : call_error {
	argument_error() :
		call_error("argument mismatch") {}

	argument_error(const std::string& msg) :
		call_error(msg) {}
};



}  // namespace rpc
}  // namespace msgpack

#endif /* msgpack/rpc/exception.h */

