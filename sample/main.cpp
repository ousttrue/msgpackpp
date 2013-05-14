#include <msgpack/asiorpc.h>
#include <boost/thread.hpp>

const static int PORT=8070;

static int add(int a, int b)
{
    return a+b;
}
static int mul(int a, int b)
{
    return a*b;
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
        m_server.get_dispatcher()->add_handler(&mul, "mul");

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


static int type_dummy(int, int)
{
    return 0;
}

int main(int argc, char **argv)
{
    server s;
    s.run();

    // start client
    {
        boost::asio::io_service io_service;
        auto endpoint=boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), PORT);
        auto client=msgpack::asiorpc::session::create(io_service); 
        client->connect(endpoint);

		// run background
		boost::thread clinet_thread([&io_service](){ io_service.run(); });

        /*
        std::cout << "call add, 1, 2 = " << client.call(&type_dummy, "add", 1, 2) << std::endl;
        std::cout << "call add, 3, 4 = " << client.call(std::function<int(int, int)>(), "add", 3, 4) << std::endl;
        std::cout << "call mul, 5, 7 = " << client.call(&type_dummy, "mul", 5, 7) << std::endl;
        */

		boost::this_thread::sleep( boost::posix_time::milliseconds(1000));

        auto request=client->call(&type_dummy, "add", 1, 2);

        std::cout << request->get_sync<int>() << std::endl;

        io_service.stop();
        clinet_thread.join();
    }


    return 0;
}

