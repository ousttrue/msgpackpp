#include <boost/test/unit_test.hpp> 

#include <msgpack/asiorpc.h>
#include <boost/thread.hpp>

const static int PORT=8070;

static int add(int a, int b)
{
    return a+b;
}

static void server()
{
    boost::asio::io_service io_service;

    // msgpack-rpc server
    msgpack::asiorpc::server server(io_service);
	server.start(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));

    // register callback
    server.get_dispatcher()->add_handler(&add, "add");

    io_service.run();
}

BOOST_AUTO_TEST_CASE( func2 )
{
    // start server
    boost::thread server_thread(server);
    boost::this_thread::sleep( boost::posix_time::milliseconds(300));

    // start client
    {
        boost::asio::io_service io_service;
        auto endpoint=boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), PORT);
        msgpack::asiorpc::client client(io_service); 
        client.connect(endpoint);

        auto functype=std::function<int(int, int)>();
        BOOST_CHECK_EQUAL(client.call(functype, "add", 3, 4), 7);
    }
}

