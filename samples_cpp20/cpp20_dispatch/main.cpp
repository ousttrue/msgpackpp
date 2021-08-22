#include <iostream>
#include <msgpackpp/rpc.h>

#include <asio/awaitable.hpp>
#include <asio/experimental/awaitable_operators.hpp>

const auto PORT = 8070;

asio::awaitable<int> client(asio::io_context &context,
                            asio::ip::tcp::endpoint ep) {

  // auto client = co_await msgpackpp::client::connect_awaitable(context, ep);

  // // client
  // asio::io_context client_io;
  // msgpackpp::client client(client_io);
  // // asio::io_context::work work(client_io);
  // std::thread client_thread([&client_io]() {
  //   client_io.run();
  //   std::cout << "[client]exit" << std::endl;
  // });

  // std::cout << "[client]connect: " << ep << "..." << std::endl;
  // client
  //     .connect_async(asio::ip::tcp::endpoint(
  //         asio::ip::address::from_string("127.0.0.1"), PORT))
  //     .get();
  // std::cout << "[client]connected" << std::endl;

  // auto result = client.call<int>("add", 1, 2).get();

  // client_io.stop();
  // client_thread.join();

  // co_return result;

  co_return 1;
}

int main(int argc, char **argv) {

  auto ep = asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"),
                                    PORT);

  // server rpc
  msgpackpp::rpc dispatcher;
  dispatcher.add_handler("add", [](int a, int b) { return a + b; });

  // server
  asio::io_context server_context;
  msgpackpp::server server(server_context,
                             [&dispatcher](asio::ip::tcp::socket socket) {
                               dispatcher.attach(std::move(socket));
                             });
  server.listen(ep);
  std::thread server_thread([&server_context]() { server_context.run(); });

  // client
  asio::io_context client_context;
  auto result = asio::co_spawn(client_context.get_executor(),
                               client(client_context, ep), asio::use_future);
  std::cout << "result = " << result.get() << std::endl;

  // stop asio
  server_context.stop();
  server_thread.join();
  std::cout << "[server]join" << std::endl;

  return 0;
}
