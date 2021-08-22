#include <catch.hpp>
#include <msgpackpp/rpc.h>
#include <thread>

#include "fixture.h"

TEST_CASE("pipeline") {
  const static int PORT = 8070;

  Fixture fixture(PORT);

  // client
  asio::io_service client_io;
  asio::io_context::work work(client_io);
  asio::ip::tcp::socket socket(client_io);
  std::thread client_thread([&client_io]() { client_io.run(); });
  msgpackpp::connect_async(
      socket, asio::ip::tcp::endpoint(
                  ::asio::ip::address::from_string("127.0.0.1"), PORT))
      .get();

  // request
  msgpackpp::rpc client;
  client.attach(std::move(socket));
  REQUIRE(msgpackpp::deserialize<int>(client.request("add", 1, 2).get()) == 3);

  auto request2 = client.request("add", 3, 4);
  request2.wait();
  int result2 = msgpackpp::deserialize<int>(request2.get());
  REQUIRE(result2 == 7);

  client_io.stop();
  client_thread.join();
}
