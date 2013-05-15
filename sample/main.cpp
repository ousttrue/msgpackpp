#include <msgpack/asiorpc.h>
#include <boost/thread.hpp>


int main(int argc, char **argv)
{
    const static int PORT=8070;

    // server
    boost::asio::io_service server_io;
    msgpack::asiorpc::server server(server_io);
    server.get_dispatcher()->add_handler([](int a, int b)->int{ return a+b; }, "add");
    std::function<float(float, float)> func2=[](float a, float b)->float{ return a*b; };
    server.get_dispatcher()->add_handler(func2, "mul");
    server.start(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));
    boost::thread server_thread([&server_io](){ server_io.run(); });

    // client
    boost::asio::io_service client_io;
    auto client=msgpack::asiorpc::session::create(client_io); 
    client->connect(boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), PORT));
    boost::thread clinet_thread([&client_io](){ client_io.run(); });

    // request
    auto request1=client->call(std::function<int(int, int)>(), "add", 1, 2);
    auto request2=client->call(std::function<float(float, float)>(), "mul", 1.2f, 5.0f);

    std::cout << request1->get_sync<int>() << std::endl;
    std::cout << request2->get_sync<float>() << std::endl;

    client_io.stop();
    clinet_thread.join();

    server_io.stop();
    server_thread.join();

    return 0;
}

