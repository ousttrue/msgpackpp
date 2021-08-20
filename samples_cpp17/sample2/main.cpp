#include <asio.hpp>
#include <iostream>
#include <msgpack/rpc/asio.h>
#include <thread>

int main(int argc, char **argv) {
  const static int PORT = 8070;

  // dispatcher
  msgpack_rpc::dispatcher dispatcher;
  dispatcher.add_handler("add", [](int a, int b) -> int { return a + b; });
  dispatcher.add_handler("mul",
                         [](float a, float b) -> float { return a * b; });

  msgpack_rpc::error_handler_t on_error = [](::asio::error_code error) {
    std::cerr << error.message() << std::endl;
  };

  // server
  asio::io_service server_io;
  auto on_receive =
      [&dispatcher](const msgpackpp::bytes &msg,
                    std::shared_ptr<msgpack_rpc::session> session) {
        dispatcher.dispatch(msg, session);
      };
  msgpack_rpc::server server(server_io, on_receive, on_error);
  server.listen(::asio::ip::tcp::endpoint(::asio::ip::tcp::v4(), PORT));
  std::thread server_thread([&server_io]() { server_io.run(); });

  // client
  ::asio::io_service client_io;

  // avoid stop client_io when client closed
  ::asio::io_service::work work(client_io);

  auto on_connection_status = [](msgpack_rpc::connection_status status) {
    std::cerr << status << std::endl;
  };
  msgpack_rpc::client client(client_io, on_connection_status, on_error);
  client.connect_async(::asio::ip::tcp::endpoint(
      ::asio::ip::address::from_string("127.0.0.1"), PORT));
  std::thread clinet_thread([&client_io]() { client_io.run(); });

  // sync request
  auto result1 = client.call<int>("add", 1, 2);
  // result1.wait();
  std::cout << "add, 1, 2 = " << result1.get() << std::endl;

  // close
  client.close();

  // reconnect
  client.connect_async(::asio::ip::tcp::endpoint(
      ::asio::ip::address::from_string("127.0.0.1"), PORT));

  // request callback
  // auto on_result=[](msgpack_rpc::func_call* result){
  // 	int result2;
  // 	std::cout << "add, 3, 4 = " << result->convert(&result2) << std::endl;
  // };
  // auto result2=client.call_async(on_result, "add", 3, 4);

  // block
  // result2->sync();

  // stop asio
  client_io.stop();
  clinet_thread.join();

  server_io.stop();
  server_thread.join();

  return 0;
}
