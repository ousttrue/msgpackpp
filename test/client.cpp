#include <boost/test/unit_test.hpp> 

#include <msgpack/rpc/asio.h>
#include <boost/thread.hpp>

#include "fixture.h"

BOOST_AUTO_TEST_CASE( client )
{
    const static int PORT=8070;

    Fixture fixture(PORT);

    // client
    boost::asio::io_service client_io;
	msgpack::rpc::asio::client client(client_io, [](
                msgpack::rpc::asio::connection_status status){

            std::cout << "connection_status: " << status << std::endl;

            });

    client.connect_async(boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), PORT));
    boost::thread client_thread([&client_io](){ client_io.run(); });

    // request
    int result1;
    BOOST_CHECK_EQUAL(client.call_sync(&result1, "add", 1, 2), 3);

    auto request2=client.call_async("add", 3, 4);
    request2->sync();
    int result2;
    BOOST_CHECK_EQUAL(request2->convert(&result2), 7);

    client_io.stop();
    client_thread.join();
}

