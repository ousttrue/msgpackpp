#pragma once

struct Fixture 
{
    boost::asio::io_service server_io;
	msgpack::rpc::asio::dispatcher dispatcher;
    msgpack::rpc::asio::server server;
    std::shared_ptr<boost::thread> server_thread;

    Fixture(int port) 
        : server(server_io)
    {
        server.listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
		auto io_service=&server_io;
        server_thread=std::make_shared<boost::thread>([io_service]{ io_service->run(); });

        dispatcher.add_handler("zero", &Fixture::zero);
        dispatcher.add_handler("acc", &Fixture::unary);
        dispatcher.add_handler("add", &Fixture::binary);
		dispatcher.start_thread(server.get_request_queue());
    }   
    ~Fixture() {
        dispatcher.stop();
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

