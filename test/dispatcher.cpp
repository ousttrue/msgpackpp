#include <catch.hpp>

#include <msgpackpp/rpc.h>
#include <thread>

class Dummy {
public:
  bool unary(int a) { return true; }
  int binary(int a, int b) { return a + b; }
  int func3(int a, int b, int c) { return a + b + c; }
  int func4(int a, int b, int c, int d) { return a + b + c + d; }
};

// 0
static float zero() { return 0; }

TEST_CASE("dispatcher0") {
  ::asio::io_service io_service;
  msgpackpp::rpc dispatcher;

  // function pointer
  dispatcher.add_handler("fp", &zero);

  // lambda
  dispatcher.add_handler("lambda", []() { return 0; });

  // std::function
  dispatcher.add_handler("std::function", std::function<float()>(zero));
}

// 1
static bool unary(double n) { return false; }

TEST_CASE("dispatcher1") {
  ::asio::io_service io_service;
  msgpackpp::rpc dispatcher;

  // function pointer
  dispatcher.add_handler("fp", &unary);

  // lambda
  dispatcher.add_handler("lambda", [](int a) { return a; });

  // std::function
  dispatcher.add_handler("std::function", std::function<bool(double)>(unary));

  // std::bind
  Dummy d;
  dispatcher.add_bind("std::bind", &d, &Dummy::unary);
}

// 2
static int binary(int a, int b) { return a + b; }

TEST_CASE("dispatcher2") {
  ::asio::io_service io_service;
  msgpackpp::rpc dispatcher;

  // function pointer
  dispatcher.add_handler("fp", &binary);

  // lambda
  dispatcher.add_handler("lambda", [](int a, int b) { return a + b; });

  // std::function
  dispatcher.add_handler("std::function", std::function<int(int, int)>(binary));

  // std::bind
  Dummy d;
  dispatcher.add_bind("std::bind", &d, &Dummy::binary);
}

// 3
static int func3(int a, int b, int c) { return a + b + c; }

TEST_CASE("dispatcher3") {
  ::asio::io_service io_service;
  msgpackpp::rpc dispatcher;

  // function pointer
  dispatcher.add_handler("fp", &func3);

  // lambda
  dispatcher.add_handler("lambda",
                         [](int a, int b, int c) { return a + b + c; });

  // std::function
  dispatcher.add_handler("std::function",
                         std::function<int(int, int, int)>(func3));

  // std::bind
  Dummy d;
  dispatcher.add_bind("std::bind", &d, &Dummy::func3);
}

// 4
static int func4(int a, int b, int c, int d) { return a + b + c + d; }

TEST_CASE("dispatcher4") {
  ::asio::io_service io_service;
  msgpackpp::rpc dispatcher;

  // function pointer
  dispatcher.add_handler("fp", &func4);

  // lambda
  dispatcher.add_handler(
      "lambda", [](int a, int b, int c, int d) { return a + b + c + d; });

  // std::function
  dispatcher.add_handler("std::function",
                         std::function<int(int, int, int, int)>(func4));

  // std::bind
  Dummy d;
  dispatcher.add_bind("std::bind", &d, &Dummy::func4);
}
