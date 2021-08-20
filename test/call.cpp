#include <catch.hpp>

#include "fixture.h"
#include <msgpack_rpc.h>

TEST_CASE("func0") {
  const static int PORT = 8070;

  Fixture f(PORT);

  // client
  ::asio::io_service client_io;
  msgpack_rpc::client client(client_io);
  client.connect_async(::asio::ip::tcp::endpoint(
      ::asio::ip::address::from_string("127.0.0.1"), PORT));
  std::thread client_thread([&client_io]() { client_io.run(); });

  // request
  REQUIRE(client.call<int>("zero").get() == 0);

  client_io.stop();
  client_thread.join();
}

TEST_CASE("func1") {
  const static int PORT = 8070;

  Fixture f(PORT);

  // client
  ::asio::io_service client_io;
  msgpack_rpc::client client(client_io);
  client.connect_async(::asio::ip::tcp::endpoint(
      ::asio::ip::address::from_string("127.0.0.1"), PORT));
  std::thread client_thread([&client_io]() { client_io.run(); });

  // request
  auto req = client.call<int>("acc", 1);
  req.wait();
  int result = req.get();
  REQUIRE(result == 1);

  client_io.stop();
  client_thread.join();
}

TEST_CASE("func2") {
  const static int PORT = 8070;

  Fixture f(PORT);

  // client
  ::asio::io_service client_io;
  msgpack_rpc::client client(client_io);
  client.connect_async(::asio::ip::tcp::endpoint(
      ::asio::ip::address::from_string("127.0.0.1"), PORT));
  std::thread client_thread([&client_io]() { client_io.run(); });

  // request
  REQUIRE(client.call<int>("add", 3, 4).get() == 7);

  client_io.stop();
  client_thread.join();
}

TEST_CASE("func3") {
  const static int PORT = 8070;

  Fixture f(PORT);

  // client
  ::asio::io_service client_io;
  msgpack_rpc::client client(client_io);
  client.connect_async(::asio::ip::tcp::endpoint(
      ::asio::ip::address::from_string("127.0.0.1"), PORT));
  std::thread client_thread([&client_io]() { client_io.run(); });

  // request
  REQUIRE(client.call<int>("add3", 3, 4, 5).get() == 12);

  client_io.stop();
  client_thread.join();
}

TEST_CASE("func4") {
  const static int PORT = 8070;

  Fixture f(PORT);

  // client
  ::asio::io_service client_io;
  msgpack_rpc::client client(client_io);
  client.connect_async(::asio::ip::tcp::endpoint(
      ::asio::ip::address::from_string("127.0.0.1"), PORT));
  std::thread client_thread([&client_io]() { client_io.run(); });

  // request
  REQUIRE(client.call<int>("add4", 3, 4, 5, 6).get() == 18);

  client_io.stop();
  client_thread.join();
}
