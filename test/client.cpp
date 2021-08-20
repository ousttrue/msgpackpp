#include <catch.hpp>

#include <msgpack_rpc.h>
#include <thread>

#include "fixture.h"

TEST_CASE("client") {
  const static int PORT = 8070;

  Fixture fixture(PORT);

  // client
  ::asio::io_service client_io;
  // avoid stop client_io when client closed
  ::asio::io_service::work work(client_io);

  msgpack_rpc::client client(
      client_io, [](msgpack_rpc::connection_status status) {
        std::cout << "connection_status: " << status << std::endl;
      });

  client.connect_async(::asio::ip::tcp::endpoint(
      ::asio::ip::address::from_string("127.0.0.1"), PORT));
  std::thread client_thread([&client_io]() { client_io.run(); });

  // request
  REQUIRE(client.call<int>("add", 1, 2).get() == 3);

  auto request2 = client.call<int>("add", 3, 4);
  request2.wait();
  int result2 = request2.get();
  REQUIRE(result2 == 7);

  // close
  client.close();
  // no active socket.

  // reconnect
  client.connect_async(::asio::ip::tcp::endpoint(
      ::asio::ip::address::from_string("127.0.0.1"), PORT));

  auto request3 = client.call<int>("add", 5, 6);
  request3.wait();
  int result3 = request3.get();
  REQUIRE(result3 == 11);

  // close
  client_io.stop();
  client_thread.join();
}
