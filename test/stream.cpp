#include <catch.hpp>
#include <msgpackpp/msgpackpp.h>
#include <stdexcept>

TEST_CASE("stream") {
  // packing
  auto p = msgpackpp::packer().pack_nil().pack_nil().pack_nil().get_payload();

  // push incomplete message
  p.push_back(msgpackpp::EXT32);

  // unpacking
  auto parsed = msgpackpp::parser(p);
  for (int i = 0; i < 3; ++i) {
    REQUIRE(parsed.is_nil());
    parsed = parsed.next();
  }

  try {
    auto last = parsed.next();
  } catch (const std::runtime_error &) {
    // throw if incomplete message
    auto a = 0;
  }
}
