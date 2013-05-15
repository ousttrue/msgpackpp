#include <boost/test/unit_test.hpp> 

#include <msgpack/rpc/asio.h>
#include <boost/thread.hpp>

BOOST_AUTO_TEST_CASE( request2 )
{
    msgpack::rpc::asio::request_factory factory;


    {
        auto request=factory.create("add", 1, 2);
        BOOST_CHECK_EQUAL(request.msgid, 1);
        BOOST_CHECK_EQUAL(request.method, "add");
        BOOST_CHECK_EQUAL(request.param.get<0>(), 1);
        BOOST_CHECK_EQUAL(request.param.get<1>(), 2);
    }
    {
        auto request=factory.create("mul", 1.0f, 0.5f);
        BOOST_CHECK_EQUAL(request.msgid, 2);
        BOOST_CHECK_EQUAL(request.method, "mul");
        BOOST_CHECK_EQUAL(request.param.get<0>(), 1.0f);
        BOOST_CHECK_EQUAL(request.param.get<1>(), 0.5f);
    }
}

