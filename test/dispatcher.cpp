#include <boost/test/unit_test.hpp> 

#include <msgpack/rpc/asio.h>
#include <boost/thread.hpp>

static int add(int a, int b)
{
    return a+b;
}

BOOST_AUTO_TEST_CASE( dispatcher2 )
{
    const static int PORT=8070;

    // server
    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server(server_io);
    auto dispatcher=server.get_dispatcher();

    // function pointer
    dispatcher->add_handler("fp", &add);

    // lambda
    dispatcher->add_handler("lambda", [](int a, int b){ return a+b; });

    // std::function
    dispatcher->add_handler("std::function", std::function<int(int, int)>(add));

    /* ToDo ?
    // std::bind
    dispatcher->add_handler("std::bind", std::bind(&add, std::placeholders::_1, std::placeholders::_2));
    */
}

