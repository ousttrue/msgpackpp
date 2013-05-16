#pragma once

struct Fixture 
{
    boost::asio::io_service server_io;
    msgpack::rpc::asio::server server;
    std::shared_ptr<boost::thread> server_thread;

    Fixture(int port) 
        : server(server_io)
    {
        server.get_dispatcher()->add_handler("zero", &Fixture::zero);
        server.get_dispatcher()->add_handler("acc", &Fixture::unary);
        server.get_dispatcher()->add_handler("add", &Fixture::binary);
        server.listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
        server_thread=std::make_shared<boost::thread>([&server_io]{ server_io.run(); });
    }   
    ~Fixture() {
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

