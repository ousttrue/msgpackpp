#include <catch.hpp> 

#include <msgpack/rpc/asio.h>
#include "fixture.h"


TEST_CASE( "func0" )
{
    const static int PORT=8070;

    Fixture f(PORT);

    // client
    ::asio::io_service client_io;
	msgpack_rpc::client client(client_io);
    client.connect_async(::asio::ip::tcp::endpoint(
                ::asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread client_thread([&client_io](){ client_io.run(); });

    // request
    int result;
    REQUIRE(client.call_sync(&result, "zero")== 0);

    client_io.stop();
    client_thread.join();
}

TEST_CASE( "func1" )
{
    const static int PORT=8070;

    Fixture f(PORT);

    // client
    ::asio::io_service client_io;
	msgpack_rpc::client client(client_io);
    client.connect_async(::asio::ip::tcp::endpoint(
                ::asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread client_thread([&client_io](){ client_io.run(); });

    // request
    std::shared_ptr<msgpack_rpc::func_call> req=client.call_async("acc", 1);
    req->sync();
    int result;
    REQUIRE(req->sync().convert(&result)== 1);

    client_io.stop();
    client_thread.join();
}

TEST_CASE( "func2" )
{
    const static int PORT=8070;

    Fixture f(PORT);

    // client
    ::asio::io_service client_io;
	msgpack_rpc::client client(client_io);
    client.connect_async(::asio::ip::tcp::endpoint(
                ::asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread client_thread([&client_io](){ client_io.run(); });

    // request
    int result;
    REQUIRE(client.call_sync(&result, "add", 3, 4)== 7);

    client_io.stop();
    client_thread.join();
}

TEST_CASE( "func3" )
{
    const static int PORT=8070;

    Fixture f(PORT);

    // client
    ::asio::io_service client_io;
	msgpack_rpc::client client(client_io);
    client.connect_async(::asio::ip::tcp::endpoint(
                ::asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread client_thread([&client_io](){ client_io.run(); });

    // request
    int result;
    REQUIRE(client.call_sync(&result, "add3", 3, 4, 5)== 12);

    client_io.stop();
    client_thread.join();
}

TEST_CASE( "func4" )
{
    const static int PORT=8070;

    Fixture f(PORT);

    // client
    ::asio::io_service client_io;
	msgpack_rpc::client client(client_io);
    client.connect_async(::asio::ip::tcp::endpoint(
                ::asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread client_thread([&client_io](){ client_io.run(); });

    // request
    int result;
    REQUIRE(client.call_sync(&result, "add4", 3, 4, 5, 6)== 18);

    client_io.stop();
    client_thread.join();
}
