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
    // must shard_ptr
    session(boost::asio::io_service& io_service, on_read_t on_read)
        : m_socket(io_service), m_pac(1024 * 1024), m_on_read(on_read)
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

    static std::shared_ptr<msgpack::sbuffer> error_notify(const std::string &msg)
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
						auto notify=error_notify(std::string("rpc server message: ")+error.message());
						shared->write_async(notify);
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
                });
    }

   void write_async(std::shared_ptr<msgpack::sbuffer> msg)
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
                std::cerr << error.message() << std::endl;
                }
		});
    }
};

}}}
