#pragma once
#include "connection_status.h"


namespace msgpack {
namespace rpc {
namespace asio {


inline std::shared_ptr<msgpack::sbuffer> error_notify(const std::string &msg)
{
    // notify type
    ::msgpack::rpc::msg_notify<std::string, std::string> notify(
            // method
            "error_notify",
            // params
            msg
            );
    // result
    auto sbuf=std::make_shared<msgpack::sbuffer>();
    msgpack::pack(*sbuf, notify);
    return sbuf;
}


class session: public std::enable_shared_from_this<session>
{
    boost::asio::ip::tcp::socket m_socket;
    unpacker m_pac;
    // on_read
    typedef std::function<void(const object &, std::shared_ptr<session>)> on_read_t;
    on_read_t m_on_read;

    connection_status m_connection_status;
    connection_callback_t m_connection_callback;

    // must shard_ptr
    session(boost::asio::io_service& io_service, on_read_t on_read,
            connection_callback_t connection_callback)
        : m_socket(io_service), m_pac(1024 * 1024), m_on_read(on_read), 
        m_connection_status(connection_none), m_connection_callback(connection_callback)
    {
    }
public:

    ~session()
    {
        close();
    }

    static std::shared_ptr<session> create(boost::asio::io_service &io_service, 
            on_read_t func=on_read_t(),
            connection_callback_t connection_callback=connection_callback_t())
    {
        return std::shared_ptr<session>(new session(io_service, 
                    func, connection_callback));
    }

    boost::asio::ip::tcp::socket& socket()
    {
        return m_socket;
    }

    void connect_async(const boost::asio::ip::tcp::endpoint &endpoint)
    {
        auto shared=shared_from_this();
        auto on_connect=[shared](const boost::system::error_code &error){
                    if(error){
                        std::cerr << "error !" << std::endl;
                        shared->set_connection_status(connection_error);
                    }
                    else{
                        shared->set_connection_status(connection_connected);
                        shared->start_read();
                    }
                };
        set_connection_status(connection_connecting);
        m_socket.async_connect(endpoint, on_connect); 
    }

    void close()
    {
        m_socket.close();
        set_connection_status(connection_none);
    }

    void start_read()
    {
        auto pac=&m_pac;
        auto shared=shared_from_this();
        auto on_read=[shared, pac](const boost::system::error_code &error,
                size_t bytes_transferred)
        {
            if (error) {
                auto notify=error_notify(std::string("rpc server message: ")+error.message());
                shared->write_async(notify);
                // closed ?
                shared->set_connection_status(connection_none);
                // no more read
                return;
            }
            else {
                pac->buffer_consumed(bytes_transferred);

                // extract object
                while(true) {
                    try{
                        if(!pac->execute()){
                            break;
                        }
                    }
                    catch(unpack_error ex){
                        auto msg=error_notify("msgpack::unpack_error. maybe msgpack buffer over flow by crupped msg");
                        shared->write_async(msg);
                        // no more read
                        // todo: close after write
                        return;
                    }
                    catch(...){
                        auto msg=error_notify("unknown error");
                        shared->write_async(msg);
                        // no more read
                        // todo: close after write
                        return;
                    }

                    // valid msgpack message
                    size_t size=pac->parsed_size();
                    ::msgpack::object msg = pac->data();

                    if(shared->m_on_read){
                        shared->m_on_read(msg, shared);
                    }

                    pac->reset();
                }

                // read loop
                shared->start_read();
            }
        };
        if(!m_socket.is_open()){
            set_connection_status(connection_none);
            return;
        }
        set_connection_status(connection_connected);
        m_socket.async_read_some(
                boost::asio::buffer(pac->buffer(), pac->buffer_capacity()),
                on_read
                );
    }

   void write_async(std::shared_ptr<msgpack::sbuffer> msg)
    {
        // for vc
        auto shared=shared_from_this();
        auto on_write=[shared, msg](
                const boost::system::error_code& error, 
                size_t bytes_transferred)
        {
            if(error){
                std::cerr << error.message() << std::endl;
                shared->set_connection_status(connection_error);
            }
        };
        if(!m_socket.is_open()){
            set_connection_status(connection_none);
            return;
        }
        set_connection_status(connection_connected);
        shared->socket().async_write_some(
                boost::asio::buffer(msg->data(), msg->size()), on_write
                );
    }

private:
   void set_connection_status(connection_status status)
   {
       if(m_connection_status==status){
           return;
       }
       m_connection_status=status;
       if(m_connection_callback){
           m_connection_callback(status);
       }
   }
};

}}}
