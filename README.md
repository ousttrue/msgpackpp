# msgpack-rpc-asio

msgpack-rpc for windows.

## Version 2.0

* require latest compiler
* `c++20` co_await

## ChangeLog
* 20210816: replace premake to cmake(vscode settings)
* 20170528: remove boost dependencies. use [asio standalone](https://github.com/chriskohlhoff/asio), [Catch](https://github.com/philsquared/Catch)
* 20170528: fix vs2017 build.

## Build

* Open folder by vscode. `CMake: Configure`, `CMake: Build`

## Sample

```c++
#include <msgpack/rpc/asio.h>
#include <thread>


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
    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server(server_io, [&dispatcher](
                const msgpack::object &msg, 
                std::shared_ptr<msgpack::rpc::asio::session> session)
            {
                dispatcher.dispatch(msg, session);
            });
    server.listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));
    std::thread server_thread([&server_io](){ server_io.run(); });

    // client
    boost::asio::io_service client_io;
    msgpack::rpc::asio::client client(client_io); 
    client.connect_async(boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), PORT));
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
```

```
add, 1, 2 = 3
mul(1.2, 5) = ?
result = 6
set_number(64) = ?
get_number() = 64
```
