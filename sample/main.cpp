#include <msgpack/asiorpc.h>
#include <boost/thread.hpp>

const static int PORT=8080;

static int add(int a, int b)
{
    return a+b;
}

static void server()
{
    boost::asio::io_service io_service;

    // msgpack-rpc server
    msgpack::asiorpc::server server(io_service, 
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));

    // register callback
    server.add_handler("add", &add);

    io_service.run();
}

static int type_dummy(int, int)
{
    return 0;
}

int main(int argc, char **argv)
{
    // start server
    boost::thread(server);

    // start client
    {
        boost::asio::io_service io_service;
        msgpack::asiorpc::client client(io_service, boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), PORT)); 
        std::cout << "call add, 1, 2 = " << client.call(&type_dummy, "add", 1, 2) << std::endl;
        std::cout << "call add, 3, 4 = " << client.call(&type_dummy, "add", 3, 4) << std::endl;
    }

    return 0;
}

