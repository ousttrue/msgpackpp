#pragma once


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
    boost::asio::io_service &m_io_service;
    std::shared_ptr<boost::asio::ip::tcp::socket> m_socket;
    unpacker m_pac;
    // on_read
    typedef std::function<void(const object &, std::shared_ptr<session>)> on_read_t;
    on_read_t m_on_read;

    connection_status m_connection_status;
    connection_callback_t m_connection_callback;

    error_handler_t m_error_handler;

    // must shard_ptr
    session(boost::asio::io_service& io_service, 
            on_read_t on_read,
            connection_callback_t connection_callback,
            error_handler_t error_handler
            )
        : m_io_service(io_service),
        m_pac(1024 * 1024), m_on_read(on_read), 
        m_connection_status(connection_none), 
        m_connection_callback(connection_callback),
        m_error_handler(error_handler)
    {
    }
public:

    ~session()
    {
    }

    static std::shared_ptr<session> create(boost::asio::io_service &io_service, 
            on_read_t func=on_read_t(),
            connection_callback_t connection_callback=connection_callback_t(),
            error_handler_t error_handler=error_handler_t())
    {
        return std::shared_ptr<session>(new session(io_service, 
                    func, connection_callback, error_handler));
    }

    connection_status get_connection_status()const{ return m_connection_status; }

    void connect_async(const boost::asio::ip::tcp::endpoint &endpoint)
    {
        auto shared=shared_from_this();
        auto socket=std::make_shared<boost::asio::ip::tcp::socket>(m_io_service);
        auto on_connect=[shared, socket/*keep socket*/](
                const boost::system::error_code &error){
            if(error){
                if(shared->m_error_handler){
                    shared->m_error_handler(error);
                }
                shared->set_connection_status(connection_error);
            }
            else{
                shared->set_connection_status(connection_connected);
                shared->start_read();
            }
        };
        m_socket=socket;
        set_connection_status(connection_connecting);
        m_socket->async_connect(endpoint, on_connect); 
    }

    void close()
    {
        set_connection_status(connection_none);
    }

    void accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
    {
        m_socket=socket;
        set_connection_status(connection_connected);
        start_read();
    }

    void start_read()
    {
        if(!m_socket){
			// closed
            //assert(false);
            return;
        }
        auto pac=&m_pac;
        auto shared=shared_from_this();
        auto socket=m_socket;
        auto on_read=[shared, pac, socket/*keep socket*/](
                const boost::system::error_code &error,
                size_t bytes_transferred)
        {
            if (error && (error != boost::asio::error::eof)) {
                if(shared->m_error_handler){
                    shared->m_error_handler(error);
                }
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
        m_socket->async_read_some(
                boost::asio::buffer(pac->buffer(), pac->buffer_capacity()),
                on_read
                );
    }

   void write_async(std::shared_ptr<msgpack::sbuffer> msg)
    {
        if(!m_socket){
            assert(false);
            return;
        }
        auto shared=shared_from_this();
        auto socket=m_socket;
        auto on_write=[shared, msg, socket/*keep socket*/](
                const boost::system::error_code& error, 
                size_t bytes_transferred)
        {
            if(error){
                if(shared->m_error_handler){
                    shared->m_error_handler(error);
                }
                shared->set_connection_status(connection_error);
            }
        };
        socket->async_write_some(
                boost::asio::buffer(msg->data(), msg->size()), on_write
                );
    }

private:
   void set_connection_status(connection_status status)
   {
       if(m_connection_status==status){
           return;
       }
       if(status==connection_none
               || status==connection_error){
           if(m_socket){
               boost::system::error_code ec;
               m_socket->shutdown(boost::asio::socket_base::shutdown_both, ec);
               if(!ec){
                   m_socket->close(ec);
               }
               m_socket=0;
           }
       }
       m_connection_status=status;
       if(m_connection_callback){
           m_connection_callback(status);
       }
   }
};

}}}
