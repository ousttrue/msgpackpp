msgpack-rpc-asio
================
msgpack-rpc

<https://github.com/msgpack/msgpack-rpc>

のバックエンドをboost::asioに置き換えてWindowsで使えるようにする試み

ToDo
----
* 5引数関数
* 6引数関数
* 7引数関数
* 8引数関数
* 9引数関数

Files
-----
### msgpack-0.5.7
* msgpack/src/*

<https://github.com/msgpack/msgpack-c>
Apache License Version 2.0

### msgpack-rpc
* msgpack-rpc-asio/include/msgpack/rpc/protocol.h

<https://github.com/msgpack/msgpack-rpc>
Apache License Version 2.0

### msgpack-rpc-asio
* msgpack/premake4.lua
* msgpack-rpc-asio/premake4.lua
* msgpack-rpc-asio/include/msgpack/rpc/asio.h
* msgpack-rpc-asio/include/msgpack/rpc/asio/*
* msgpack-rpc-asio/include/msgpack/tuple_util.h
* sapmple/*
* test/*
* premake4.lua
* README.md

msgpackおよびmsgpack-rpcの本家と同じ Apache License Version 2.0

Build
-----
- vs2010.batを実行
- msgpack-rpc-asio.slnを開く

Requied
-------
* gcc-4.5.4/vc2010
* msgpack
* boost

Sample
------
```c++
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

    // server
    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server(server_io, [&dispatcher](
                const msgpack::object &msg, 
                std::shared_ptr<msgpack::rpc::asio::session> session)
            {
                dispatcher.dispatch(msg, session);
            });
    server.listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));
    boost::thread server_thread([&server_io](){ server_io.run(); });

    // client
    boost::asio::io_service client_io;
    msgpack::rpc::asio::client client(client_io); 
    client.connect_async(boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), PORT));
    boost::thread clinet_thread([&client_io](){ client_io.run(); });

    // sync request
	int result1;
    std::cout << "add, 1, 2 = " << client.call_sync(&result1, "add", 1, 2) << std::endl;

    // async request
    auto request2=client.call_async("mul", 1.2f, 5.0f);
    std::cout << *request2 << std::endl;
    float result2;
    std::cout << "result = " << request2->sync().convert(&result2) << std::endl;

    // property
    auto request3=client.call_async("set_number", 64);
    std::cout << *request3 << std::endl;
    request3->sync();

    request3=client.call_sync("get_number");
    request3->sync();
    std::cout << *request3 << std::endl;

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

