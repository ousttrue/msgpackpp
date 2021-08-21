#include <catch.hpp>

#include <msgpack_rpc.h>
#include <thread>

#include "fixture.h"

TEST_CASE("client") {
  const static int PORT = 8070;

  Fixture fixture(PORT);

  // client
  asio::io_service client_io;
  asio::io_context::work work(client_io);
  asio::ip::tcp::socket socket(client_io);
  std::thread client_thread([&client_io]() { client_io.run(); });
  msgpack_rpc::connect_async(
      socket, asio::ip::tcp::endpoint(
                  ::asio::ip::address::from_string("127.0.0.1"), PORT))
      .get();

  // request
  msgpack_rpc::rpc client;
  client.attach(std::move(socket));
  REQUIRE(client.call<int>("add", 1, 2).get() == 3);

  auto request2 = client.call<int>("add", 3, 4);
  request2.wait();
  int result2 = request2.get();
  REQUIRE(result2 == 7);

  auto request3 = client.call<int>("add", 5, 6);
  request3.wait();
  int result3 = request3.get();
  REQUIRE(result3 == 11);

  // close
  client_io.stop();
  client_thread.join();
}
