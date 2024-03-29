// ToDo

// request
//   long request

// server
//   BindError

// execution
//   exception throw

// result
//   type error
//   not ready error

#include <catch.hpp>

#include "fixture.h"
#include <msgpackpp/rpc.h>

TEST_CASE("error_handling") {
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

  // no method
  {
    auto request = client.request_async("no_such_method");
    request.wait();
    // REQUIRE(request->is_error());
    // REQUIRE(request->get_error_code()==
    // msgpackpp::error_dispatcher_no_handler);
  }
  // too many arguments
  {
    auto request = client.request_async("acc", 1, 2);
    request.wait();
    // REQUIRE(request->is_error());
    // REQUIRE(request->get_error_code()== msgpackpp::error_params_too_many);
  }
  //  not enough arguments
  {
    auto request = client.request_async("add", 1);
    request.wait();
    // REQUIRE(request->is_error());
    // REQUIRE(request->get_error_code()==
    // msgpackpp::error_params_not_enough);
  }
  //   invalid argument type
  {
    auto request = client.request_async("add", "a", "b");
    request.wait();
    // REQUIRE(request->is_error());
    // REQUIRE(request->get_error_code()== msgpackpp::error_params_convert);
  }

  client_io.stop();
  client_thread.join();
}
