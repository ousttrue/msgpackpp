#include <catch.hpp>
#include <msgpackpp/msgpackpp.h>

TEST_CASE("request0") {
  {
    auto request = msgpackpp::make_rpc_request(1, "count");
    msgpackpp::parser parsed(request);
    REQUIRE(parsed.is_array());
    REQUIRE(parsed.count() == 4);
    auto data = parsed.copy_bytes();
    REQUIRE(parsed[1].get_number<int>() == 1);
    REQUIRE(parsed[2].get_string() == "count");
  }
}

TEST_CASE("request1") {
  {
    auto request = msgpackpp::make_rpc_request(1, "sin", 3.14);
    msgpackpp::parser parsed(request);
    REQUIRE(parsed[1].get_number<int>() == 1);
    REQUIRE(parsed[2].get_string() == "sin");
    REQUIRE(parsed[3][0].get_number<double>() == 3.14);
  }
}

TEST_CASE("request2") {
  {
    auto request = msgpackpp::make_rpc_request(1, "add", 1, 2);
    msgpackpp::parser parsed(request);
    REQUIRE(parsed[1].get_number<int>() == 1);
    REQUIRE(parsed[2].get_string() == "add");
    REQUIRE(parsed[3][0].get_number<int>() == 1);
    REQUIRE(parsed[3][1].get_number<int>() == 2);
  }
  {
    auto request = msgpackpp::make_rpc_request(2, "mul", 1.0f, 0.5f);
    msgpackpp::parser parsed(request);
    REQUIRE(parsed[1].get_number<int>() == 2);
    REQUIRE(parsed[2].get_string() == "mul");
    REQUIRE(parsed[3][0].get_number<float>() == 1.0f);
    REQUIRE(parsed[3][1].get_number<float>() == 0.5f);
  }
}

TEST_CASE("request3") {
  {
    auto request = msgpackpp::make_rpc_request(1, "add", 1, 2, 3);
    msgpackpp::parser parsed(request);
    REQUIRE(parsed[1].get_number<int>() == 1);
    REQUIRE(parsed[2].get_string() == "add");
    REQUIRE(parsed[3][0].get_number<int>() == 1);
    REQUIRE(parsed[3][1].get_number<int>() == 2);
    REQUIRE(parsed[3][2].get_number<int>() == 3);
  }
}

TEST_CASE("request4") {
  {
    auto request = msgpackpp::make_rpc_request(1, "add", 1, 2, 3, 4);
    msgpackpp::parser parsed(request);
    REQUIRE(parsed[1].get_number<int>() == 1);
    REQUIRE(parsed[2].get_string() == "add");
    REQUIRE(parsed[3][0].get_number<int>() == 1);
    REQUIRE(parsed[3][1].get_number<int>() == 2);
    REQUIRE(parsed[3][2].get_number<int>() == 3);
    REQUIRE(parsed[3][3].get_number<int>() == 4);
  }
}
