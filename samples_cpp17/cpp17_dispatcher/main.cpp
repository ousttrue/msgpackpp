#include <iostream>
#include <msgpack_rpc.h>

const auto PORT = 8070;

int client(asio::io_context &context, asio::ip::tcp::endpoint ep) {
  asio::io_context::work work(context);
  std::thread client_thread([&context]() {
    context.run();
    std::cout << "[client]exit" << std::endl;
  });

  // connect
  asio::ip::tcp::socket socket(context);
  msgpack_rpc::connect_async(socket, ep).get();
  std::cout << "[client]connected" << std::endl;

  // client
  msgpack_rpc::rpc client;
  client.attach(std::move(socket));
  auto result = client.call<int>("add", 1, 2).get();

  // stop
  context.stop();
  client_thread.join();

  return result;
}

int main(int argc, char **argv) {

  auto ep = asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"),
                                    PORT);

  // server rpc
  msgpack_rpc::rpc dispatcher;
  dispatcher.add_handler("add", [](int a, int b) { return a + b; });

  // server
  asio::io_context server_context;
  msgpack_rpc::server server(
      server_context, [&dispatcher](asio::ip::tcp::socket socket) mutable {
        dispatcher.attach(std::move(socket));
      });
  server.listen(ep);
  std::thread server_thread([&server_context]() { server_context.run(); });

  // client
  asio::io_context client_context;
  auto result = client(client_context, ep);
  std::cout << "result = " << result << std::endl;

  // stop asio
  server_context.stop();
  server_thread.join();
  std::cout << "[server]join" << std::endl;

  return 0;
}
