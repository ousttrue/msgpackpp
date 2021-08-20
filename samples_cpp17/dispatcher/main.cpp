#include <iostream>
#include <msgpack_rpc.h>

const auto PORT = 8070;

int client(asio::io_context &context, asio::ip::tcp::endpoint ep) {
  // client
  asio::io_context client_io;
  msgpack_rpc::client client(client_io);
  // asio::io_context::work work(client_io);
  std::thread client_thread([&client_io]() {
    client_io.run();
    std::cout << "[client]exit" << std::endl;
  });

  std::cout << "[client]connect: " << ep << "..." << std::endl;
  client
      .connect_async(asio::ip::tcp::endpoint(
          asio::ip::address::from_string("127.0.0.1"), PORT))
      .get();
  std::cout << "[client]connected" << std::endl;

  auto result = client.call<int>("add", 1, 2).get();

  client_io.stop();
  client_thread.join();

  return result;
}

int main(int argc, char **argv) {

  auto ep = asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"),
                                    PORT);

  // server rpc
  msgpack_rpc::dispatcher dispatcher;
  dispatcher.add_handler("add", [](int a, int b) { return a + b; });

  // server
  asio::io_context server_context;
  msgpack_rpc::server server(
      server_context,
      [&dispatcher](const msgpackpp::bytes &msg,
                    std::shared_ptr<msgpack_rpc::session> session) {
        dispatcher.dispatch(msg, session);
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
