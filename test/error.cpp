// ToDo

// request
//   long request

// server
//   BindError

// execution
//   exception throw

// result
//   type error
//   not ready error

#include <catch.hpp> 

#include <msgpack_rpc.h>
#include "fixture.h"


TEST_CASE( "error_handling" )
{
    const static int PORT=8070;

    Fixture f(PORT);

    // client
    ::asio::io_service client_io;
	msgpack_rpc::client client(client_io);
    client.connect_async(::asio::ip::tcp::endpoint(
                ::asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread client_thread([&client_io](){ client_io.run(); });

    // no method
    {
        auto request=client.call<int>("no_such_method");
        request.wait();
        // REQUIRE(request->is_error());
        // REQUIRE(request->get_error_code()== msgpack_rpc::error_dispatcher_no_handler);
    }
    // too many arguments
    {
        auto request=client.call<int>("acc", 1, 2);
        request.wait();
        // REQUIRE(request->is_error());
        // REQUIRE(request->get_error_code()== msgpack_rpc::error_params_too_many);
    }
    //  not enough arguments
    {
        auto request=client.call<int>("add", 1);
        request.wait();
        // REQUIRE(request->is_error());
        // REQUIRE(request->get_error_code()== msgpack_rpc::error_params_not_enough);
    }
    //   invalid argument type
    {
        auto request=client.call<int>("add", "a", "b");
        request.wait();
        // REQUIRE(request->is_error());
        // REQUIRE(request->get_error_code()== msgpack_rpc::error_params_convert);
    }

    client_io.stop();
    client_thread.join();
}

