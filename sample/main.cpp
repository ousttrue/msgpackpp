#include <msgpack/rpc/asio.h>
#include <boost/thread.hpp>


int main(int argc, char **argv)
{
    const static int PORT=8070;

    // server
    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server(server_io);
    server.get_dispatcher()->add_handler("add", [](int a, int b)->int{ return a+b; });
    server.get_dispatcher()->add_handler("mul", [](float a, float b)->float{ return a*b; });
    server.listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));
    boost::thread server_thread([&server_io](){ server_io.run(); });

    // client
    boost::asio::io_service client_io;
    auto client=msgpack::rpc::asio::session::create(client_io); 
    client->connect(boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), PORT));
    boost::thread clinet_thread([&client_io](){ client_io.run(); });

    // request
    auto request1=client->call("add", 1, 2);
    auto request2=client->call("mul", 1.2f, 5.0f);

    int result1;
    request1->sync().convert(&result1);
    float result2;
    request2->sync().convert(&result2);

    std::cout << result1 << std::endl;
    std::cout << result2 << std::endl;

    client_io.stop();
    clinet_thread.join();

    server_io.stop();
    server_thread.join();

    return 0;
}

