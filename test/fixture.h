#pragma once

struct Fixture 
{
    // dispatcher
    boost::asio::io_service dispatcher_io;
	msgpack::rpc::asio::dispatcher dispatcher;
    std::shared_ptr<boost::thread> dispatcher_thread;

    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server;
    std::shared_ptr<boost::thread> server_thread;

    Fixture(int port) 
        : dispatcher(dispatcher_io), server(server_io, dispatcher)
    {
        server.listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
        server_thread=std::make_shared<boost::thread>([&]{ server_io.run(); });

        dispatcher.add_handler("zero", &Fixture::zero);
        dispatcher.add_handler("acc", &Fixture::unary);
        dispatcher.add_handler("add", &Fixture::binary);
        dispatcher.add_handler("add3", [](int a, int b, int c){ return a+b+c; });
        dispatcher.add_handler("add4", [](int a, int b, int c, int d){ return a+b+c+d; });
        dispatcher_thread=std::make_shared<boost::thread>([&]{ dispatcher_io.run(); });
    }   
    ~Fixture() {
        dispatcher_io.stop();
        dispatcher_thread->join();
        server_io.stop();
        server_thread->join();
    }   

    static int zero()
    {
        return 0;
    }
    static int unary(int a)
    {
        static int acc=0;
        acc+=a;
        return acc;
    }
    static int binary(int a, int b)
    {
        return a+b;
    }
};

