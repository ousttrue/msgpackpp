#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include <msgpack/rpc/protocol.h>
#include <msgpack/rpc/types.h>
#include <msgpack/rpc/exception.h>


namespace msgpack {
namespace asiorpc {


    class dispatcher
    {
        typedef std::function<std::shared_ptr<msgpack::sbuffer>(msgpack::rpc::msgid_t, msgpack::object)> func;
        std::map<std::string, func> m_handlerMap;

    public:
        std::shared_ptr<msgpack::sbuffer> dispatch(::msgpack::object msg, ::msgpack::rpc::auto_zone z)
        {
            ::msgpack::rpc::msg_rpc rpc;
            msg.convert(&rpc);
            if(rpc.type==::msgpack::rpc::REQUEST){
                ::msgpack::rpc::msg_request<msgpack::object, msgpack::object> req;
                msg.convert(&req);
                return dispatch(req.msgid, req.method, req.param, z);
            }
            else{
                throw ::msgpack::type_error();  // FIXME
            }
        }

        // 2
        template<typename R, typename A1, typename A2>
        void add_handler(const std::string &method, R(*handler)(A1, A2))
        {
            add_handler(method, std::function<R(A1, A2)>(handler));
        }
        template<typename R, typename A1, typename A2>
        void add_handler(const std::string &method, std::function<R(A1, A2)> handler)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>{
                        // extract args
                        ::msgpack::type::tuple<A1, A2> params;
                        msg_params.convert(&params);
                        // error type
                        typedef ::msgpack::type::nil Error;
                        auto result=handler(params.template get<0>(), params.template get<1>());
                        ::msgpack::rpc::msg_response<R&, Error> msgres(result, msgpack::type::nil(), msgid);
                        // result
                        auto sbuf=std::make_shared<msgpack::sbuffer>();
                        msgpack::pack(*sbuf, msgres);
                        return sbuf;
                        }));
        }

    private:
        std::shared_ptr<msgpack::sbuffer> dispatch(::msgpack::rpc::msgid_t msgid, 
                ::msgpack::object method, ::msgpack::object params, ::msgpack::rpc::auto_zone z)
        {
            std::string method_name;
            method.convert(&method_name);

            auto found=m_handlerMap.find(method_name);
            if(found==m_handlerMap.end()){
                throw ::msgpack::rpc::no_method_error();
            }
            else{
                auto func=found->second;
                return func(msgid, params);
            }
        }
    };


    class session: public std::enable_shared_from_this<session>
    {
        boost::asio::ip::tcp::socket m_socket;
        enum { max_length = 1024 };
        char m_data[max_length];
        unpacker m_pac;
        std::shared_ptr<dispatcher> m_dispatcher;
    public:
        session(boost::asio::io_service& io_service, std::shared_ptr<dispatcher> dispatcher)
            : m_socket(io_service), m_pac(1024), m_dispatcher(dispatcher)
            {
            }

        boost::asio::ip::tcp::socket& socket()
        {
            return m_socket;
        }

        void start()
        {
            auto self=shared_from_this();
            auto &pac=m_pac;
            auto data=m_data;
            auto dispatcher=m_dispatcher;
            m_socket.async_read_some(boost::asio::buffer(m_data, max_length),
                    [self, &pac, data, dispatcher](const boost::system::error_code& error, size_t bytes_transferred){
                        if (!error){
                            std::cout << "read " << bytes_transferred << " bytes" << std::endl;

                            memcpy(pac.buffer(), data, bytes_transferred);
                            pac.buffer_consumed(bytes_transferred);
                            if(pac.execute()){
                                ::msgpack::object msg = pac.data();
                                ::msgpack::rpc::auto_zone z(pac.release_zone());
                                pac.reset();
                                try{
                                    std::shared_ptr<msgpack::sbuffer> result=dispatcher->dispatch(msg, z);
                                    std::cout << "dispatch" << std::endl;

                                    self->socket().async_write_some(boost::asio::buffer(result->data(), result->size()),
                                    [self, result](const boost::system::error_code& error, size_t bytes_transferred){
                                        std::cout << "write " << bytes_transferred << " bytes" << std::endl;
                                    });
                                }
                                catch(...){
                                    // error
                                    std::cerr << "error" << std::endl;
                                }
                            }
                        }
                        else
                        {
                            // finish
                        }
                    });
        }
    };


    class server
    {
        boost::asio::io_service &m_io_service;
        boost::asio::ip::tcp::acceptor m_acceptor;
        std::shared_ptr<dispatcher> m_dispatcher;

    public:
        server(boost::asio::io_service &io_service, boost::asio::ip::tcp::endpoint endpoint)
            : m_io_service(io_service), m_acceptor(m_io_service, endpoint), m_dispatcher(new dispatcher)
        {
            start_accept();
        }

        std::shared_ptr<dispatcher> get_dispatcher(){ return m_dispatcher; }

    private:
        void start_accept()
        {
            auto self=this;
            auto new_connection = std::make_shared<session>(m_io_service, m_dispatcher);
            m_acceptor.async_accept(new_connection->socket(),
                    [self, new_connection](const boost::system::error_code& error){
                        if (!error){
                            std::cout << "accepted" << std::endl;
                            new_connection->start();
                        }
                        // next
                        self->start_accept();
                    });
        }
    };


    class client
    {
        boost::asio::io_service &m_io_service;
        boost::asio::ip::tcp::socket m_socket;
        unpacker m_pac;

    public:
        client(boost::asio::io_service &io_service)
            : m_io_service(io_service), m_socket(m_io_service), m_pac(1024)
        {
        }

        void connect(boost::asio::ip::tcp::endpoint endpoint)
        {
           m_socket.connect(endpoint); 
        }

        // 2
        template<typename R, typename A1, typename A2>
        R call(R(*handler)(A1, A2), const std::string &method, A1 a1, A2 a2)
        {
            return call(std::function<R(A1, A2)>(handler), method, a1, a2);
        }
        template<typename R, typename A1, typename A2>
        R call(std::function<R(A1, A2)>, const std::string &method, A1 a1, A2 a2)
        {
            ::msgpack::rpc::msgid_t msgid = 0;
            typedef type::tuple<const A1&, const A2&> Parameter;
            ::msgpack::rpc::msg_request<std::string, Parameter> msgreq(method, Parameter(a1, a2), msgid);

            return request<R>(msgreq);
        }

        template<typename R, typename Parameter>
        R request(const ::msgpack::rpc::msg_request<std::string, Parameter> msgreq)
        {
            ::msgpack::sbuffer sbuf;
            ::msgpack::pack(sbuf, msgreq);

            // request
            size_t len=boost::asio::write(m_socket, boost::asio::buffer(sbuf.data(), sbuf.size()));
            std::cout << "write " << len << " bytes" << std::endl;

            // response
            size_t pos=0;
            for (;;)
            {
                std::vector<char> buf(128);
                boost::system::error_code error;

                size_t len = m_socket.read_some(boost::asio::buffer(buf), error);
                memcpy(m_pac.buffer()+pos, &buf[0], len);
                pos+=len;
                m_pac.buffer_consumed(pos);
                if(m_pac.execute()){
                    ::msgpack::object msg = m_pac.data();
                    //std::cout << "result " << msg << std::endl;
                    //::msgpack::rpc::auto_zone z(m_pac.release_zone());
                    m_pac.reset();

                    ::msgpack::rpc::msg_response<object, object> res;
                    msg.convert(&res);

                    R value;
                    res.result.convert(&value);
                    return value;
                }

                if (error == boost::asio::error::eof){
                    break; // Connection closed cleanly by peer.
                }
                else if (error){
                    throw boost::system::system_error(error); // Some other error.
                }

            }

            return R();
        }
    };

}}
