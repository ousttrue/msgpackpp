#pragma once

namespace msgpack {
namespace rpc {
namespace asio {


class server;
class queue_item
{
    object m_msg;
    std::weak_ptr<server> m_server;

public:
    queue_item(const object &msg, std::shared_ptr<server> server)
        : m_msg(msg), m_server(server)
    {}

    const object& msg()const{ return m_msg; }
    std::shared_ptr<server> server()const{ return m_server.lock(); }
};


class server_request_queue
{
    std::list<std::shared_ptr<queue_item>> m_queue;
    boost::mutex m_mutex;
public:
    void enqueue(std::shared_ptr<queue_item> item)
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_queue.push_back(item);
    }

    std::shared_ptr<queue_item> dequeue()
    {
        boost::mutex::scoped_lock lock(m_mutex);
        if(m_queue.empty()){
            return std::shared_ptr<queue_item>();
        }
        else{
            auto item=m_queue.front();
            m_queue.pop_front();
            return item;
        }
    }
};


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
