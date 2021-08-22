#include <catch.hpp>
#include <iostream>
#include <msgpackpp/msgpackpp.h>

static void v() {}
static void v1(int a) {}
static float f() {}
static float f1(int a) {}

template <typename F> void a(const F &f) { std::cout << "a" << std::endl; }
template <typename R, typename... AS> void a(R (*f)(AS...)) {
  std::cout << "b" << std::endl;
}

struct Some {
  void Go() { ; }
  int To() { return 0; }
  static void St() {}
};

auto l = []() {};

TEST_CASE("make_procedure") {
  msgpackpp::make_procedurecall(l);
  msgpackpp::make_procedurecall(v);
  // msgpackpp::make_procedurecall(b);
  Some s;
  msgpackpp::make_methodcall(&s, &Some::Go);
  msgpackpp::make_methodcall(&s, &Some::To);
  msgpackpp::make_procedurecall(&Some::St);
}

TEST_CASE("procedure_call") {
  // 0 args
  REQUIRE(1 == msgpackpp::procedure_call([] { return 1; }));

  // 2 args
  REQUIRE(3 ==
          msgpackpp::procedure_call([](int a, int b) { return a + b; }, 1, 2));

  REQUIRE(-1 ==
          msgpackpp::procedure_call([](int a, int b) { return a - b; }, 1, 2));

  // string
  REQUIRE("a" == msgpackpp::procedure_call([]() { return std::string("a"); }));

  REQUIRE("b" == msgpackpp::procedure_call([](std::string src) { return src; },
                                           std::string("b")));
}
