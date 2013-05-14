#include <boost/test/unit_test.hpp> 

#include <msgpack/asiorpc.h>

static int add(int a, int b)
{
    return a+b;
}

BOOST_AUTO_TEST_CASE( func2 )
{
    const static int PORT=8070;

    // server
    boost::asio::io_service server_io;
    msgpack::asiorpc::server server(server_io);
    server.get_dispatcher()->add_handler(&add, "add");
    server.start(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));
    boost::thread server_thread([&server_io]{ server_io.run(); });

    // client
    boost::asio::io_service client_io;
    auto client=msgpack::asiorpc::session::create(client_io); 
    client->connect(boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), PORT));
    boost::thread client_thread([&client_io](){ client_io.run(); });

    // request
    auto request=client->call(std::function<int(int, int)>(), "add", 3, 4);
    BOOST_CHECK_EQUAL(request->get_sync<int>(), 7);

    client_io.stop();
    client_thread.join();
    server_io.stop();
    server_thread.join();
}

