#include <msgpack/rpc/asio.h>
#include <thread>
#include <iostream>


class SomeClass
{
    int m_number;

public:
    void setNumber(const int &number){ m_number=number; }
    int getNumber()const{ return m_number; }
};


int main(int argc, char **argv)
{
    const static int PORT=8070;

    // dispatcher
    msgpack::rpc::asio::dispatcher dispatcher;
    dispatcher.add_handler("add", [](int a, int b)->int{ return a+b; });
    dispatcher.add_handler("mul", [](float a, float b)->float{ return a*b; });
    SomeClass s;
	dispatcher.add_property("number", std::function<SomeClass*()>([&s](){ return &s; })
            , &SomeClass::getNumber
            , &SomeClass::setNumber
            );

    // server
    ::asio::io_service server_io;
    msgpack::rpc::asio::server server(server_io, [&dispatcher](
                const msgpack::object &msg, 
                std::shared_ptr<msgpack::rpc::asio::session> session)
            {
                dispatcher.dispatch(msg, session);
            });
    server.listen(::asio::ip::tcp::endpoint(::asio::ip::tcp::v4(), PORT));
    std::thread server_thread([&server_io](){ server_io.run(); });

    // client
    ::asio::io_service client_io;
    msgpack::rpc::asio::client client(client_io); 
    client.connect_async(::asio::ip::tcp::endpoint(
                    ::asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread clinet_thread([&client_io](){ client_io.run(); });

    // sync request
	int result1;
    std::cout << "add, 1, 2 = " << client.call_sync(&result1, "add", 1, 2) << std::endl;

    // async request
    auto request2=client.call_async("mul", 1.2f, 5.0f);
    std::cout << *request2 << std::endl;
    float result2;
    std::cout << "result = " << request2->sync().convert(&result2) << std::endl;

	// property
	{
		auto request3=client.call_async("set_number", 64);
		std::cout << *request3 << std::endl;
		request3->sync();
	}
	{
		auto request3=client.call_async("get_number");
		request3->sync();
		std::cout << *request3 << std::endl;
	}

    // stop asio
    client_io.stop();
    clinet_thread.join();

    server_io.stop();
    server_thread.join();

    return 0;
}
