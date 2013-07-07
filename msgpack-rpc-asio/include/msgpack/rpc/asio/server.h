#pragma once

namespace msgpack {
namespace rpc {
namespace asio {


class server
{
    boost::asio::io_service &m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
    //std::list<std::weak_ptr<session>> m_sessions;
    typedef std::function<
        void(const object &msg, std::shared_ptr<session> session)> on_receive_t;
    on_receive_t m_on_receive;

    error_handler_t m_error_handler;
public:
    server(boost::asio::io_service &io_service)
		: m_io_service(io_service), m_acceptor(io_service)
	{
	}

    server(boost::asio::io_service &io_service, on_receive_t on_receive)
        : m_io_service(io_service), m_acceptor(io_service), m_on_receive(on_receive)
    {
    }

    server(boost::asio::io_service &io_service, 
            on_receive_t on_receive, error_handler_t error_handler)
        : m_io_service(io_service), m_acceptor(io_service), 
        m_on_receive(on_receive), m_error_handler(error_handler)
    {
    }

    ~server()
    {
    }

	void set_on_receive(on_receive_t on_receive)
	{
		m_on_receive=on_receive;
	}

    void set_error_handler(error_handler_t error_handler)
    {
        m_error_handler=error_handler;
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
        auto new_connection = session::create(m_io_service, m_on_receive, 
                connection_callback_t(),
                m_error_handler);
        auto socket=std::make_shared<boost::asio::ip::tcp::socket>(m_io_service);

        //m_sessions.push_back(new_connection);

        auto self=this;
        auto on_accept=[self, new_connection, socket/*keep socket*/](
                const boost::system::error_code& error){
            if (error){
                if(self->m_error_handler){
                    self->m_error_handler(error);
                }
                else{
                    throw error;
                }
            }
            else{
                new_connection->accept(socket);
                // next
                self->start_accept();
            }
        };
        m_acceptor.async_accept(*socket.get(), on_accept);
    }
};

}}}
