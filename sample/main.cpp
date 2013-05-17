#include <msgpack/rpc/asio.h>
#include <boost/thread.hpp>


int main(int argc, char **argv)
{
    const static int PORT=8070;

    // server
    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server(server_io);
    server.listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));
    boost::thread server_thread([&server_io](){ server_io.run(); });

    // dispatcher
    auto queue=server.get_request_queue();
    auto dispatcher=std::make_shared<msgpack::rpc::asio::dispatcher>();
    dispatcher->add_handler("add", [](int a, int b)->int{ return a+b; });
    dispatcher->add_handler("mul", [](float a, float b)->float{ return a*b; });
    dispatcher->start_thread(server.get_request_queue());

    // client
    boost::asio::io_service client_io;
    auto client=msgpack::rpc::asio::session::create(client_io); 
    client->connect(boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), PORT));
    boost::thread clinet_thread([&client_io](){ client_io.run(); });

    // request
	int result1;
    std::cout << "add, 1, 2 = " << client->call_sync(&result1, "add", 1, 2) << std::endl;

    auto request=client->call_async("mul", 1.2f, 5.0f);
    std::cout << *request << std::endl;

    float result2;
    std::cout << "result = " << request->sync().convert(&result2) << std::endl;

    // stop asio
	dispatcher->stop();

    client_io.stop();
    clinet_thread.join();

    server_io.stop();
    server_thread.join();

    return 0;
}

