#include <catch.hpp>

#include "fixture.h"
#include <msgpackpp/rpc.h>

TEST_CASE("func0") {
  const static int PORT = 8070;

  Fixture f(PORT);

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
  REQUIRE(msgpackpp::deserialize<int>(client.request("zero").get()) == 0);

  client_io.stop();
  client_thread.join();
}

TEST_CASE("func1") {
  const static int PORT = 8070;

  Fixture f(PORT);

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
  auto req = client.request("acc", 1);
  req.wait();
  int result = msgpackpp::deserialize<int>(req.get());
  REQUIRE(result == 1);

  client_io.stop();
  client_thread.join();
}

TEST_CASE("func2") {
  const static int PORT = 8070;

  Fixture f(PORT);

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
  REQUIRE(msgpackpp::deserialize<int>(client.request("add", 3, 4).get()) == 7);

  client_io.stop();
  client_thread.join();
}

TEST_CASE("func3") {
  const static int PORT = 8070;

  Fixture f(PORT);

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
  REQUIRE(msgpackpp::deserialize<int>(client.request("add3", 3, 4, 5).get()) ==
          12);

  client_io.stop();
  client_thread.join();
}

TEST_CASE("func4") {
  const static int PORT = 8070;

  Fixture f(PORT);

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
  REQUIRE(msgpackpp::deserialize<int>(client.request("add4", 3, 4, 5, 6).get()) ==
          18);

  client_io.stop();
  client_thread.join();
}
