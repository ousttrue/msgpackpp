#include <msgpack/rpc/asio.h>
#include <boost/thread.hpp>


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

    auto on_error=[](boost::system::error_code error)
    {
        std::cerr << error.message() << std::endl;
    };

    // server
    boost::asio::io_service server_io;
    auto on_receive=[&dispatcher](
            const msgpack::object &msg, 
            std::shared_ptr<msgpack::rpc::asio::session> session)
    {
        dispatcher.dispatch(msg, session);
    };
    msgpack::rpc::asio::server server(server_io, on_receive, on_error);
    server.listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));
    boost::thread server_thread([&server_io](){ server_io.run(); });

    // client
    boost::asio::io_service client_io;

	// avoid stop client_io when client closed
	boost::asio::io_service::work work(client_io);

    auto on_connection_status=[](msgpack::rpc::asio::connection_status status)
    {
        std::cerr << status << std::endl;
    };
    msgpack::rpc::asio::client client(client_io, on_connection_status, on_error); 
    client.connect_async(boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), PORT));
    boost::thread clinet_thread([&client_io](){ client_io.run(); });

    // sync request
	int result1;
    std::cout << "add, 1, 2 = " << client.call_sync(&result1, "add", 1, 2) << std::endl;

    // close
    client.close();

    // reconnect
    client.connect_async(boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), PORT));

    // sync request
	int result2;
    std::cout << "add, 1, 2 = " << client.call_sync(&result2, "add", 3, 4) << std::endl;

    // stop asio
    client_io.stop();
    clinet_thread.join();

    server_io.stop();
    server_thread.join();

    return 0;
}
