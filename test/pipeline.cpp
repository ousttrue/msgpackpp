#include <boost/test/unit_test.hpp> 

#include <msgpack/asiorpc.h>
#include <boost/thread.hpp>

const static int PORT=8070;

static int add(int a, int b)
{
    return a+b;
}

class server
{
    boost::asio::io_service m_io_service;
    msgpack::asiorpc::server m_server;
    std::shared_ptr<boost::thread> m_thread;
public:
    server()
        : m_server(m_io_service)
    {
    }

    ~server()
    {
		stop();
    }

    void run()
    {
        m_server.start(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));

        // register callback
        m_server.get_dispatcher()->add_handler(&add, "add");

        auto service=&m_io_service;
        m_thread=std::make_shared<boost::thread>([service]{ service->run(); });

        boost::this_thread::sleep( boost::posix_time::milliseconds(300));
    }

    void stop()
    {
		m_io_service.stop();
		m_thread->join();
    }
};


BOOST_AUTO_TEST_CASE( pipeline )
{
    server s;
    s.run();

    // start client
    {
        boost::asio::io_service io_service;
        auto endpoint=boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), PORT);
        msgpack::asiorpc::session client(io_service); 
        client.connect(endpoint);

        auto functype=std::function<int(int, int)>();
        BOOST_CHECK_EQUAL(client.call(functype, "add", 1, 2), 3);
        BOOST_CHECK_EQUAL(client.call(functype, "add", 3, 4), 7);
    }

    s.stop();
}

