#include <boost/test/unit_test.hpp> 

#include <msgpack/rpc/asio.h>
#include <boost/thread.hpp>

// 0
static float zero()
{
    return 0;
}

BOOST_AUTO_TEST_CASE( dispatcher0 )
{
    // server
    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server(server_io);
    auto dispatcher=server.get_dispatcher();

    // function pointer
    dispatcher->add_handler("fp", &zero);
}

// 1
static bool unary(double n)
{
    return false;
}

BOOST_AUTO_TEST_CASE( dispatcher1 )
{
    // server
    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server(server_io);
    auto dispatcher=server.get_dispatcher();

    // function pointer
    dispatcher->add_handler("fp", &unary);
}

// 2
static int binary(int a, int b)
{
    return a+b;
}

BOOST_AUTO_TEST_CASE( dispatcher2 )
{
    // server
    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server(server_io);
    auto dispatcher=server.get_dispatcher();

    // function pointer
    dispatcher->add_handler("fp", &binary);

    // lambda
    dispatcher->add_handler("lambda", [](int a, int b){ return a+b; });

    // std::function
    dispatcher->add_handler("std::function", std::function<int(int, int)>(binary));

    /* ToDo ?
    // std::bind
    dispatcher->add_handler("std::bind", std::bind(&binary, std::placeholders::_1, std::placeholders::_2));
    */
}

