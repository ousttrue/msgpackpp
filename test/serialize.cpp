#include <catch.hpp>
#include <msgpackpp/msgpackpp.h>

template <typename T> void Test(T src) {
  // serialize
  msgpackpp::packer packer;
  packer << src;
  auto p = packer.get_payload();

  // deserialize
  auto parsed = msgpackpp::parser(p.data(), p.size());
  T value;
  parsed >> value;

  REQUIRE(src == value);
}

struct Person {
  std::string name;
  int age;

  bool operator==(const Person &rhs) const {
    return name == rhs.name && age == rhs.age;
  }
};
MPPP_MAP_SERIALIZER(Person, 2, name, age)

struct Point {
  float x;
  float y;

  bool operator==(const Point &rhs) const { return x == rhs.x && y == rhs.y; }
};
MPPP_ARRAY_SERIALIZER(Point, 2, x, y);

TEST_CASE("serialize") {
  Test(true);
  Test(false);
  Test(0);
  Test(1);
  Test(std::numeric_limits<std::uint8_t>::max());
  Test(std::numeric_limits<std::uint8_t>::max() + 1);
  Test(std::numeric_limits<std::uint16_t>::max() + 1);
  Test(std::numeric_limits<std::uint32_t>::max() + 1);
  Test(-32);
  Test(std::numeric_limits<std::int8_t>::min());
  Test(std::numeric_limits<std::int8_t>::min() - 1);
  Test(std::numeric_limits<std::int16_t>::min() - 1);
  Test(std::numeric_limits<std::int32_t>::min() - 1);
  Test(1.1f);
  Test(1.1);
  Test(std::vector<int>{1, 2, 3});
  Test(std::vector<std::string>{"a", "ab", "hoge"});

  Test(std::vector<std::uint8_t>{1, 2, 3});

  Test(std::string(u8"日本語"));

  Test(std::make_tuple(true, 0, 1.5, std::string(u8"ABC")));

  // user type
  Test(Person{"hoge", 100});
  Test(Point{1.5f, 2.5f});
}
