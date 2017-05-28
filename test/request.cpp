#include <catch.hpp> 
#include <msgpack/rpc/asio.h>
#include <thread>


TEST_CASE( "request0" )
{
    msgpack::rpc::asio::request_factory factory;

    {
        auto request=factory.create("count");
        REQUIRE(request.msgid== 1);
        REQUIRE(request.method== "count");
    }
}

TEST_CASE( "request1" )
{
    msgpack::rpc::asio::request_factory factory;

    {
        auto request=factory.create("sin", 3.14);
        REQUIRE(request.msgid== 1);
        REQUIRE(request.method== "sin");
        REQUIRE(std::get<0>(request.param)== 3.14);
    }
}

TEST_CASE( "request2" )
{
    msgpack::rpc::asio::request_factory factory;


    {
        auto request=factory.create("add", 1, 2);
        REQUIRE(request.msgid== 1);
        REQUIRE(request.method== "add");
        REQUIRE(std::get<0>(request.param)== 1);
        REQUIRE(std::get<1>(request.param)== 2);
    }
    {
        auto request=factory.create("mul", 1.0f, 0.5f);
        REQUIRE(request.msgid== 2);
        REQUIRE(request.method== "mul");
        REQUIRE(std::get<0>(request.param)== 1.0f);
        REQUIRE(std::get<1>(request.param)== 0.5f);
    }
}

TEST_CASE( "request3" )
{
    msgpack::rpc::asio::request_factory factory;


    {
        auto request=factory.create("add", 1, 2, 3);
        REQUIRE(request.msgid== 1);
        REQUIRE(request.method== "add");
        REQUIRE(std::get<0>(request.param)== 1);
        REQUIRE(std::get<1>(request.param)== 2);
        REQUIRE(std::get<2>(request.param)== 3);
    }
}

TEST_CASE( "request4" )
{
    msgpack::rpc::asio::request_factory factory;


    {
        auto request=factory.create("add", 1, 2, 3, 4);
        REQUIRE(request.msgid== 1);
        REQUIRE(request.method== "add");
        REQUIRE(std::get<0>(request.param)== 1);
        REQUIRE(std::get<1>(request.param)== 2);
        REQUIRE(std::get<2>(request.param)== 3);
        REQUIRE(std::get<3>(request.param)== 4);
    }
}
