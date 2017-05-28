#include <catch.hpp> 
#include <msgpack/rpc/asio.h>
#include <thread>

#include "fixture.h"

TEST_CASE( "pipeline" )
{
    const static int PORT=8070;

    Fixture fixture(PORT);

    // client
    ::asio::io_service client_io;
	msgpack::rpc::asio::client client(client_io);
    client.connect_async(::asio::ip::tcp::endpoint(
                ::asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread client_thread([&client_io](){ client_io.run(); });

    // request
    int result1;
    REQUIRE(client.call_sync(&result1, "add", 1, 2)== 3);

    auto request2=client.call_async("add", 3, 4);
    request2->sync();
    int result2;
    REQUIRE(request2->convert(&result2)== 7);

    client_io.stop();
    client_thread.join();
}

