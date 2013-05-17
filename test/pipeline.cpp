#include <boost/test/unit_test.hpp> 

#include <msgpack/rpc/asio.h>
#include <boost/thread.hpp>

#include "fixture.h"

BOOST_AUTO_TEST_CASE( pipeline )
{
    const static int PORT=8070;

    Fixture fixture(PORT);

    // client
    boost::asio::io_service client_io;
    auto client=msgpack::rpc::asio::session::create(client_io); 
    client->connect(boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), PORT));
    boost::thread client_thread([&client_io](){ client_io.run(); });

    // request
    int request1;
    BOOST_CHECK_EQUAL(request1->convert(client->sync(&result1, "add", 1, 2)), 3);

    auto request2=client->call_async("add", 3, 4);
    request2->sync();
    int result2;
    BOOST_CHECK_EQUAL(request2->convert(&result2), 7);

    client_io.stop();
    client_thread.join();
}

