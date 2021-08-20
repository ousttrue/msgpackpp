# msgpack-rpc-asio

msgpack-rpc for windows.

## dependencies

* asio standalone
* msgpackpp

## Version 2

* [x] `c++11` std::promise/future
* [ ] `c++20` co_await
* [x] use https://github.com/ousttrue/msgpackpp backend
* [x] fix unittest

## Build

* Open folder by vscode. `CMake: Configure`, `CMake: Build`

## Sample

```cpp
#include <iostream>
#include <msgpack_rpc.h>
#include <thread>


int main(int argc, char **argv) {
  const static int PORT = 8070;

  // dispatcher
  msgpack_rpc::dispatcher dispatcher;
  dispatcher.add_handler("add", [](int a, int b) -> int { return a + b; });
  dispatcher.add_handler("mul",
                         [](float a, float b) -> float { return a * b; });

  msgpack_rpc::error_handler_t on_error = [](asio::error_code error) {
    std::cerr << "[server.on_error]" << error.message() << std::endl;
  };

  // server
  asio::io_context server_io;
  auto on_receive =
      [&dispatcher](const msgpackpp::bytes &msg,
                    std::shared_ptr<msgpack_rpc::session> session) {
        dispatcher.dispatch(msg, session);
      };
  msgpack_rpc::server server(server_io, on_receive, on_error);
  server.listen(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), PORT));
  std::thread server_thread([&server_io]() { server_io.run(); });

  // client
  asio::io_context client_io;

  // avoid stop client_io when client closed
  asio::io_context::work work(client_io);

  auto on_connection_status = [](msgpack_rpc::connection_status status) {
    std::cout << "[client]" << status << std::endl;
  };
  msgpack_rpc::client client(client_io, on_connection_status, on_error);

  //
  // connect
  //
  client.connect_async(asio::ip::tcp::endpoint(
      asio::ip::address::from_string("127.0.0.1"), PORT));
  std::thread clinet_thread([&client_io]() { client_io.run(); });

  // sync request
  auto result1 = client.call<int>("add", 1, 2);
  // result1.wait();
  std::cout << "add, 1, 2 = " << result1.get() << std::endl;

  // close
  client.close();

  // stop asio
  client_io.stop();
  clinet_thread.join();

  server_io.stop();
  server_thread.join();

  return 0;
}
```
