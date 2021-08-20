#include <catch.hpp>
#include <msgpack_rpc.h>
#include <thread>

#include "fixture.h"

TEST_CASE("pipeline") {
  const static int PORT = 8070;

  Fixture fixture(PORT);

  // client
  ::asio::io_service client_io;
  msgpack_rpc::client client(client_io);
  client.connect_async(::asio::ip::tcp::endpoint(
      ::asio::ip::address::from_string("127.0.0.1"), PORT));
  std::thread client_thread([&client_io]() { client_io.run(); });

  // request
  REQUIRE(client.call<int>("add", 1, 2).get() == 3);

  auto request2 = client.call<int>("add", 3, 4);
  request2.wait();
  int result2 = request2.get();
  REQUIRE(result2 == 7);

  client_io.stop();
  client_thread.join();
}
