#include <msgpack/asiorpc.h>
#include <boost/thread.hpp>

const static int PORT=8080;

static int add(int a, int b)
{
    return a+b;
}
static int mul(int a, int b)
{
    return a*b;
}

static void server()
{
    boost::asio::io_service io_service;

    // msgpack-rpc server
    msgpack::asiorpc::server server(io_service, 
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));

    // register callback
    server.get_dispatcher()->add_handler("add", &add);
    server.get_dispatcher()->add_handler("mul", &mul);

    std::cout << "start server..." << std::endl;
    io_service.run();
}

static int type_dummy(int, int)
{
    return 0;
}

int main(int argc, char **argv)
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
        std::cout << "connect to... " << endpoint << std::endl;
        client.connect(endpoint);
        std::cout << "done" << std::endl;
        //std::cout << "call add, 1, 2 = " << client.call(&type_dummy, "add", 1, 2) << std::endl;
        //std::cout << "call add, 3, 4 = " << client.call(std::function<int(int, int)>(), "add", 3, 4) << std::endl;
        std::cout << "call mul, 5, 7 = " << client.call(&type_dummy, "mul", 5, 7) << std::endl;
    }

    boost::this_thread::sleep( boost::posix_time::milliseconds(300));

    return 0;
}

