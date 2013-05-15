#include <boost/test/unit_test.hpp> 

#include <msgpack/rpc/asio.h>
#include "fixture.h"

const static int PORT=8070;

BOOST_AUTO_TEST_CASE( func2 )
{
    Fixture f(PORT);

    // client
    boost::asio::io_service client_io;
    auto client=msgpack::rpc::asio::session::create(client_io); 
    client->connect(boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), PORT));
    boost::thread client_thread([&client_io](){ client_io.run(); });

    // request
    auto request=client->call(std::function<int(int, int)>(), "add", 3, 4);
    BOOST_CHECK_EQUAL(request->get_sync<int>(), 7);

    client_io.stop();
    client_thread.join();
}

