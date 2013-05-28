#include <boost/test/unit_test.hpp> 

#include <msgpack/rpc/asio.h>
#include <boost/thread.hpp>

class Dummy
{
public:
    bool unary(int a)
    {
        return true;
    }
	int binary(int a, int b)
	{ 
		return a+b;
	}
    int func3(int a, int b, int c)
    {
        return a+b+c;
    }
    int func4(int a, int b, int c, int d)
    {
        return a+b+c+d;
    }
};

// 0
static float zero()
{
    return 0;
}

BOOST_AUTO_TEST_CASE( dispatcher0 )
{
    boost::asio::io_service io_service;
    msgpack::rpc::asio::dispatcher dispatcher(io_service);

    // function pointer
    dispatcher.add_handler("fp", &zero);

    // lambda
    dispatcher.add_handler("lambda", [](){ return 0; });

    // std::function
    dispatcher.add_handler("std::function", std::function<float()>(zero));
}

// 1
static bool unary(double n)
{
    return false;
}

BOOST_AUTO_TEST_CASE( dispatcher1 )
{
    boost::asio::io_service io_service;
    msgpack::rpc::asio::dispatcher dispatcher(io_service);

    // function pointer
    dispatcher.add_handler("fp", &unary);

    // lambda
    dispatcher.add_handler("lambda", [](int a){ return a; });

    // std::function
    dispatcher.add_handler("std::function", std::function<bool(double)>(unary));

    // std::bind
	Dummy d;
    dispatcher.add_bind("std::bind", &Dummy::unary, 
            &d, std::placeholders::_1);
}

// 2
static int binary(int a, int b)
{
    return a+b;
}

BOOST_AUTO_TEST_CASE( dispatcher2 )
{
    boost::asio::io_service io_service;
    msgpack::rpc::asio::dispatcher dispatcher(io_service);

    // function pointer
    dispatcher.add_handler("fp", &binary);

    // lambda
    dispatcher.add_handler("lambda", [](int a, int b){ return a+b; });

    // std::function
    dispatcher.add_handler("std::function", std::function<int(int, int)>(binary));

    // std::bind
	Dummy d;
    dispatcher.add_bind("std::bind", &Dummy::binary, 
            &d, std::placeholders::_1, std::placeholders::_2);
}

// 3
static int func3(int a, int b, int c)
{
    return a+b+c;
}

BOOST_AUTO_TEST_CASE( dispatcher3 )
{
    boost::asio::io_service io_service;
    msgpack::rpc::asio::dispatcher dispatcher(io_service);

    // function pointer
    dispatcher.add_handler("fp", &func3);

    // lambda
    dispatcher.add_handler("lambda", [](int a, int b, int c){ return a+b+c; });

    // std::function
    dispatcher.add_handler("std::function", std::function<int(int, int, int)>(func3));

    // std::bind
	Dummy d;
    dispatcher.add_bind("std::bind", &Dummy::func3, 
            &d, std::placeholders::_1, std::placeholders::_2, 3);
}

// 4
static int func4(int a, int b, int c, int d)
{
    return a+b+c+d;
}

BOOST_AUTO_TEST_CASE( dispatcher4 )
{
    boost::asio::io_service io_service;
    msgpack::rpc::asio::dispatcher dispatcher(io_service);

    // function pointer
    dispatcher.add_handler("fp", &func4);

    // lambda
    dispatcher.add_handler("lambda", [](int a, int b, int c, int d){ return a+b+c+d; });

    // std::function
    dispatcher.add_handler("std::function", std::function<int(int, int, int, int)>(func4));

    // std::bind
	Dummy d;
    dispatcher.add_bind("std::bind", &Dummy::func4, 
            &d, std::placeholders::_1, std::placeholders::_2, 3, 4);
}

