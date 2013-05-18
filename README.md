msgpack-rpc-asio
================
msgpack-rpc

https://github.com/msgpack/msgpack-rpc

のバックエンドをboost::asioに置き換えてWindowsで使えるようにする試み

ToDo
----
* 3引数関数
* 4引数関数
* 5引数関数
* 6引数関数
* 7引数関数
* 8引数関数
* 9引数関数

License
-------
msgpackおよびmsgpack-rpcの本家と同じApache License Version 2.0

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


int main(int argc, char **argv)
{
    const static int PORT=8070;

    // dispatcher
    boost::asio::io_service dispatcher_io;
    msgpack::rpc::asio::dispatcher dispatcher(dispatcher_io);
    dispatcher.add_handler("add", [](int a, int b)->int{ return a+b; });
    dispatcher.add_handler("mul", [](float a, float b)->float{ return a*b; });
    boost::thread dispatcher_thread([&dispatcher_io](){ dispatcher_io.run(); });

    // server
    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server(server_io, dispatcher);
    server.listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));
    boost::thread server_thread([&server_io](){ server_io.run(); });

    // client
    boost::asio::io_service client_io;
    msgpack::rpc::asio::client client(client_io); 
    client.connect_async(boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), PORT));
    boost::thread clinet_thread([&client_io](){ client_io.run(); });

    // request
	int result1;
    std::cout << "add, 1, 2 = " << client.call_sync(&result1, "add", 1, 2) << std::endl;

    auto request=client.call_async("mul", 1.2f, 5.0f);
    std::cout << *request << std::endl;

    float result2;
    std::cout << "result = " << request->sync().convert(&result2) << std::endl;

    // stop asio
	dispatcher_io.stop();
    dispatcher_thread.join();

    client_io.stop();
    clinet_thread.join();

    server_io.stop();
    server_thread.join();

    return 0;
}
```
