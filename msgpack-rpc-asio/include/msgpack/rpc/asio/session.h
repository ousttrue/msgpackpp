#pragma once

namespace msgpack {
namespace rpc {
namespace asio {


class session: public std::enable_shared_from_this<session>
{
    boost::asio::ip::tcp::socket m_socket;
    unpacker m_pac;
    // on_read
    typedef std::function<void(const object &, std::shared_ptr<session>)> on_read_t;
    on_read_t m_on_read;
    // write queue
    bool m_writing;
    std::list<std::shared_ptr<msgpack::sbuffer>> m_write_queue;
    // must shard_ptr
    session(boost::asio::io_service& io_service, on_read_t on_read)
        : m_socket(io_service), m_pac(1024), m_on_read(on_read), m_writing(false)
    {
    }
public:

    ~session()
    {
    }

    static std::shared_ptr<session> create(boost::asio::io_service &io_service, on_read_t func=on_read_t())
    {
        return std::shared_ptr<session>(new session(io_service, func));
    }

    boost::asio::ip::tcp::socket& socket()
    {
        return m_socket;
    }

    void connect_async(const boost::asio::ip::tcp::endpoint &endpoint)
    {
        auto shared=shared_from_this();
        m_socket.async_connect(endpoint, [shared](const boost::system::error_code &error){
                    if(error){
                        std::cerr << "error !" << std::endl;
                    }
                    else{
                        shared->start_read();
                    }
                }); 
    }

    void start_read()
    {
        auto pac=&m_pac;
        auto shared=shared_from_this();
        m_socket.async_read_some(
                boost::asio::buffer(pac->buffer(), pac->buffer_capacity()),
                [shared, pac](const boost::system::error_code &error,
                    size_t bytes_transferred)
                {
                    if (error) {
                        //throw rpc_error("asio error ?");
                    }
                    else {
                        //char *p=pac->buffer();
                        pac->buffer_consumed(bytes_transferred);
                        //msgpack::unpacked result;

                        // extract object
                        while(pac->execute()) {
                            size_t size=pac->parsed_size();
                            // msgpack message
                            ::msgpack::object msg = pac->data();

                            if(shared->m_on_read){
                                shared->m_on_read(msg, shared);
                            }

                            pac->reset();
                            //p+=size;
                        }

                        // read loop
                        shared->start_read();
                    }
                });
    }

    // write
    void enqueue_write(std::shared_ptr<msgpack::sbuffer> msg)
    {
        // lock
        if(m_writing){
            // queueing...
            m_write_queue.push_back(msg);
        }
        else{
            // start async write
            m_writing=true;
            start_write(msg);
        }
    }

private:
   void start_write(std::shared_ptr<msgpack::sbuffer> msg)
    {
        // for vc
        auto shared=shared_from_this();
        shared->socket().async_write_some(
                boost::asio::buffer(msg->data(), msg->size()),
                [shared, msg](
                    const boost::system::error_code& error, 
                    size_t bytes_transferred)
                {
                if(error){
                std::cerr << "write error" << std::endl;
                }

                // lock
                if(shared->m_write_queue.empty()){
                shared->m_writing=false;
                }
                else{
                shared->m_writing=true;
                auto next_msg=shared->m_write_queue.front();
                shared->m_write_queue.pop_front();
                shared->start_write(next_msg);
                }
                });
    }

    /*
    template<typename E>
        std::shared_ptr<msgpack::sbuffer> create_error_message(msgpack::rpc::msgid_t msgid, const E &error)
        {
            // error type
            ::msgpack::rpc::msg_response<msgpack::type::nil, E> msgres(
                    ::msgpack::type::nil(), 
                    error, 
                    msgid);
            // result
            auto sbuf=std::make_shared<msgpack::sbuffer>();
            msgpack::pack(*sbuf, msgres);
            return sbuf;
        }
    */
};

}}}
