#pragma once

namespace msgpack {
namespace rpc {
namespace asio {


class server
{
    boost::asio::io_service &m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::list<std::weak_ptr<session>> m_sessions;
    typedef std::function<void(const object &msg, std::shared_ptr<session> session)> on_receive_t;
    on_receive_t m_on_receive;
public:
    server(boost::asio::io_service &io_service, on_receive_t on_receive)
        : m_io_service(io_service), m_acceptor(io_service), m_on_receive(on_receive)
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

private:
    void start_accept()
    {
        auto new_connection = session::create(m_io_service, m_on_receive);
        m_sessions.push_back(new_connection);

        auto self=this;
        m_acceptor.async_accept(new_connection->socket(),
                [self, new_connection](const boost::system::error_code& error){
                if (error){
                    throw error;
                }
                else{
                    new_connection->start_read();
                    // next
                    self->start_accept();
                }
                });
    }
};

}}}
