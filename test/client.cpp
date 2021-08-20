#include <catch.hpp> 

#include <msgpack/rpc/asio.h>
#include <thread>

#include "fixture.h"

TEST_CASE( "client" )
{
    const static int PORT=8070;

    Fixture fixture(PORT);

    // client
    ::asio::io_service client_io;
	// avoid stop client_io when client closed
	::asio::io_service::work work(client_io);

	msgpack_rpc::client client(client_io, [](
                msgpack_rpc::connection_status status){

            std::cout << "connection_status: " << status << std::endl;

            });

    client.connect_async(::asio::ip::tcp::endpoint(
                ::asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread client_thread([&client_io](){ client_io.run(); });

    // request
    int result1;
    REQUIRE(client.call_sync(&result1, "add", 1, 2)== 3);

    auto request2=client.call_async("add", 3, 4);
    request2->sync();
    int result2;
    /*
    REQUIRE(request2->convert(&result2), 7);
    */
    // raw extract
    const msgpack::object &obj=request2->get_result();
    obj.convert(&result2);
    REQUIRE(result2== 7);

    // close
    client.close();
    // no active socket.

    // reconnect
    client.connect_async(::asio::ip::tcp::endpoint(
                ::asio::ip::address::from_string("127.0.0.1"), PORT));

    auto request3=client.call_async("add", 5, 6);
    request3->sync();
    int result3;
    REQUIRE(request3->convert(&result3)== 11);

    // close
    client_io.stop();
    client_thread.join();
}

