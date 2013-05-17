#pragma once

namespace msgpack {
namespace rpc {
namespace asio {


class server
{
    boost::asio::io_service &m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::shared_ptr<dispatcher> m_dispatcher;
    std::list<std::weak_ptr<session>> m_sessions;

public:
    server(boost::asio::io_service &io_service)
        : m_io_service(io_service), m_acceptor(io_service), m_dispatcher(new dispatcher)
    {
    }

    ~server()
    {
        for(auto it=m_sessions.begin(); it!=m_sessions.end(); ++it){
            auto session=it->lock();
            if(session){
                session->socket().close();
            }
        }
    }

    void listen(boost::asio::ip::tcp::endpoint endpoint)
    {
        m_acceptor.open(endpoint.protocol());
        m_acceptor.bind(endpoint);
        m_acceptor.listen();
        start_accept();
    }

    void stop()
    {
        m_acceptor.close();
    }

    std::shared_ptr<dispatcher> get_dispatcher(){ return m_dispatcher; }

private:
    void start_accept()
    {
        auto self=this;
        auto new_connection = session::create(m_io_service, m_dispatcher);
        m_sessions.push_back(new_connection);
        m_acceptor.async_accept(new_connection->socket(),
                [self, new_connection](const boost::system::error_code& error){
                if (error){
                std::cerr << "error !" << std::endl;
                }
                else{
                new_connection->startRead();

                // next
                self->start_accept();
                }
                });
    }
};

}}}
