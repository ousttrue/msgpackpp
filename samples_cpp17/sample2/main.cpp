#include <iostream>
#include <msgpack_rpc.h>
#include <thread>

int main(int argc, char **argv) {
  const static int PORT = 8070;

  // dispatcher
  msgpack_rpc::rpc dispatcher;
  dispatcher.add_handler("add", [](int a, int b) -> int { return a + b; });
  dispatcher.add_handler("mul",
                         [](float a, float b) -> float { return a * b; });

  msgpack_rpc::error_handler_t on_error = [](asio::error_code error) {
    std::cerr << "[server.on_error]" << error.message() << std::endl;
  };

  // server
  asio::io_context server_io;
  auto on_accepted = [&dispatcher](asio::ip::tcp::socket socket) {
    dispatcher.attach(std::move(socket));
  };
  msgpack_rpc::server server(server_io, on_accepted, on_error);
  server.listen(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), PORT));
  std::thread server_thread([&server_io]() { server_io.run(); });

  // client
  asio::io_context client_io;
  asio::io_context::work work(client_io);

  //
  // connect
  //
  asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), PORT);
  asio::ip::tcp::socket socket(client_io);
  std::thread clinet_thread([&client_io]() { client_io.run(); });
  std::cout << "[client]connect..." << std::endl;
  msgpack_rpc::connect_async(socket, ep).get();
  std::cout << "[client]connected" << std::endl;

  msgpack_rpc::rpc client;
  client.attach(std::move(socket));
  auto result1 = client.call<int>("add", 1, 2);
  std::cout << "add, 1, 2 = " << result1.get() << std::endl;

  client.session()->socket().close();


  // stop asio
  client_io.stop();
  clinet_thread.join();

  server_io.stop();
  server_thread.join();

  return 0;
}
