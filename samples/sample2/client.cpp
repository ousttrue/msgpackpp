#include <msgpack/rpc/asio.h>
#include <asio.hpp>
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

    msgpack::rpc::asio::error_handler_t on_error=[](::asio::error_code error)
    {
        std::cerr << error.message() << std::endl;
    };

    // server
    asio::io_service server_io;
    auto on_receive=[&dispatcher](
            const msgpack::object &msg, 
            std::shared_ptr<msgpack::rpc::asio::session> session)
    {
        dispatcher.dispatch(msg, session);
    };
    msgpack::rpc::asio::server server(server_io, on_receive, on_error);
    server.listen(::asio::ip::tcp::endpoint(::asio::ip::tcp::v4(), PORT));
    std::thread server_thread([&server_io](){ server_io.run(); });

    // client
    ::asio::io_service client_io;

	// avoid stop client_io when client closed
	::asio::io_service::work work(client_io);

    auto on_connection_status=[](msgpack::rpc::asio::connection_status status)
    {
        std::cerr << status << std::endl;
    };
    msgpack::rpc::asio::client client(client_io, on_connection_status, on_error); 
    client.connect_async(::asio::ip::tcp::endpoint(
                    ::asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread clinet_thread([&client_io](){ client_io.run(); });

    // sync request
	int result1;
    std::cout << "add, 1, 2 = " << client.call_sync(&result1, "add", 1, 2) << std::endl;

    // close
    client.close();

    // reconnect
    client.connect_async(::asio::ip::tcp::endpoint(
                ::asio::ip::address::from_string("127.0.0.1"), PORT));

    // request callback
	auto on_result=[](msgpack::rpc::asio::func_call* result){
		int result2;
		std::cout << "add, 3, 4 = " << result->convert(&result2) << std::endl;
	};
    auto result2=client.call_async(on_result, "add", 3, 4);

	// block
	result2->sync();

    // stop asio
    client_io.stop();
    clinet_thread.join();

    server_io.stop();
    server_thread.join();

    return 0;
}
