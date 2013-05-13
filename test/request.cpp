#include <boost/test/unit_test.hpp> 

#include <msgpack/asiorpc.h>
#include <boost/thread.hpp>

BOOST_AUTO_TEST_CASE( request2 )
{
    msgpack::asiorpc::request_factory factory;


    {
	    auto functype=std::function<int(int, int)>();
        auto request=factory.create(functype, "add", 1, 2);
        BOOST_CHECK_EQUAL(request.msgid, 1);
        BOOST_CHECK_EQUAL(request.method, "add");
        BOOST_CHECK_EQUAL(request.param.get<0>(), 1);
        BOOST_CHECK_EQUAL(request.param.get<1>(), 2);
    }
    {
	    auto functype=std::function<float(float, float)>();
        auto request=factory.create(functype, "mul", 1.0f, 0.5f);
        BOOST_CHECK_EQUAL(request.msgid, 2);
        BOOST_CHECK_EQUAL(request.method, "mul");
        BOOST_CHECK_EQUAL(request.param.get<0>(), 1.0f);
        BOOST_CHECK_EQUAL(request.param.get<1>(), 0.5f);
    }
}

