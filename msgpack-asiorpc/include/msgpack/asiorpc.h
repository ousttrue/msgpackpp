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
        typedef std::function<
            std::shared_ptr<msgpack::sbuffer>(msgpack::rpc::msgid_t, msgpack::object)
            > func;
        std::map<std::string, func> m_handlerMap;

    public:
        std::shared_ptr<msgpack::sbuffer> request(::msgpack::rpc::msgid_t msgid, 
                ::msgpack::object method, ::msgpack::object params)
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

        std::shared_ptr<msgpack::sbuffer> response(::msgpack::rpc::msgid_t msgid, 
                ::msgpack::object result, ::msgpack::object error)
        {
            throw ::msgpack::rpc::no_method_error();
        }

        std::shared_ptr<msgpack::sbuffer> notify(
				::msgpack::object method, ::msgpack::object params)
        {
            throw ::msgpack::rpc::no_method_error();
        }

        // 2
        template<typename R, typename A1, typename A2>
        void add_handler(R(*handler)(A1, A2), const std::string &method)
        {
            add_handler(std::function<R(A1, A2)>(handler), method);
        }
        template<typename R, typename A1, typename A2>
        void add_handler(std::function<R(A1, A2)> handler, const std::string &method)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                            // extract args
                            ::msgpack::type::tuple<A1, A2> params;
                            msg_params.convert(&params);
                            // error type
                            typedef ::msgpack::type::nil Error;
                            auto result=handler(
                                params.template get<0>(), 
                                params.template get<1>()
                                );
                            ::msgpack::rpc::msg_response<R&, Error> msgres(
                                result, 
                                msgpack::type::nil(), 
                                msgid);
                            // result
                            auto sbuf=std::make_shared<msgpack::sbuffer>();
                            msgpack::pack(*sbuf, msgres);
                            return sbuf;
                        }));
        }

    };


    class session: public std::enable_shared_from_this<session>
    {
        boost::asio::ip::tcp::socket m_socket;
        enum { max_length = 1024 };
        char m_data[max_length];
        unpacker m_pac;
        std::shared_ptr<dispatcher> m_dispatcher;

        bool m_writing;
        std::list<std::shared_ptr<msgpack::sbuffer>> m_write_queue;
    public:
        session(boost::asio::io_service& io_service, std::shared_ptr<dispatcher> dispatcher)
            : m_socket(io_service), m_pac(1024), m_dispatcher(dispatcher),
            m_writing(false)
            {
            }

        boost::asio::ip::tcp::socket& socket()
        {
            return m_socket;
        }

        // read connection
        void startRead()
        {
            auto pac=&m_pac;
            auto shared=shared_from_this();
            m_socket.async_read_some(
                    boost::asio::buffer(pac->buffer(), pac->buffer_capacity()),
                    [shared, pac](const boost::system::error_code &error,
                        size_t bytes_transferred)
                    {
                        if (error) {
                            // todo
                        }
                        else {

                            char *p=pac->buffer();
                            pac->buffer_consumed(bytes_transferred);
                            msgpack::unpacked result;

                            // オブジェクトを取り出す
                            while(pac->execute()) {
                                size_t size=pac->parsed_size();
                                // msgpack message
                                ::msgpack::object msg = pac->data();
                                ::msgpack::rpc::msgid_t msgid=0;
                                try{
                                    ::msgpack::rpc::msg_rpc rpc;
                                    msg.convert(&rpc);

                                    switch(rpc.type) {
									case ::msgpack::rpc::REQUEST: 
                                            {
                                                ::msgpack::rpc::msg_request<object, object> req;
                                                msg.convert(&req);
                                                std::shared_ptr<msgpack::sbuffer> result=
                                                    shared->m_dispatcher->request(
                                                            req.msgid, req.method, req.param);
                                                shared->enqueueWrite(result);
                                            }
                                            break;

                                        case ::msgpack::rpc::RESPONSE: 
                                            {
                                                ::msgpack::rpc::msg_response<object, object> res;
                                                msg.convert(&res);
                                                shared->m_dispatcher->response(
                                                        res.msgid, res.result, res.error);
                                            }
                                            break;

                                        case ::msgpack::rpc::NOTIFY: 
                                            {
                                                ::msgpack::rpc::msg_notify<object, object> req;
                                                msg.convert(&req);
                                                shared->m_dispatcher->notify(
                                                        req.method, req.param);
                                            }
                                            break;

                                        default:
											throw msgpack::rpc::rpc_error("rpc type error");
                                    }

                                }
								catch(::msgpack::rpc::rpc_error ex){
                                    //shared->enqueueWrite(shared->create_error_message(msgid, ex));
                                }
								catch(...){
									//shared->enqueueWrite(shared->create_error_message(msgid, ::msgpack::rpc::rpc_error("unknown error")));
								}

                                pac->reset();
                                p+=size;
                            }

                            // 永遠に読み込み続ける
                            // pipeline
                            shared->startRead();
                        }
                    });
        }

    private:
        void enqueueWrite(std::shared_ptr<msgpack::sbuffer> msg)
        {
            // lock
            if(m_writing){
                // queueing...
                m_write_queue.push_back(msg);
            }
            else{
                // start aync write
                m_writing=true;
                startWrite(msg);
            }
        }

        void startWrite(std::shared_ptr<msgpack::sbuffer> msg)
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
                            shared->startWrite(next_msg);
                        }
                    });
        }

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


    class request_factory
    {
        ::msgpack::rpc::msgid_t m_next_msgid;
    public:
        request_factory()
            : m_next_msgid(1)
        {
        }

        ::msgpack::rpc::msgid_t next_msgid()
        {
            return m_next_msgid++;
        }

        // 2
        template<typename R, typename A1, typename A2>
        ::msgpack::rpc::msg_request<std::string, ::msgpack::type::tuple<A1, A2>> 
        create(std::function<R(A1, A2)>, const std::string &method, A1 a1, A2 a2)
        {
            ::msgpack::rpc::msgid_t msgid = next_msgid();
            typedef ::msgpack::type::tuple<A1, A2> Parameter;
            return ::msgpack::rpc::msg_request<std::string, Parameter>(
                    method, Parameter(a1, a2), msgid);
        }
    };


    class client
    {
        boost::asio::io_service &m_io_service;
        boost::asio::ip::tcp::socket m_socket;
        unpacker m_pac;
        request_factory m_request_factory;

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
            return request<R>(m_request_factory.create(
                        std::function<R(A1, A2)>(handler), 
                        method, a1, a2));
        }
        template<typename R, typename A1, typename A2>
        R call(std::function<R(A1, A2)> func, const std::string &method, A1 a1, A2 a2)
        {
            return request<R>(m_request_factory.create(
                        func,
                        method, a1, a2));

        }

        template<typename R, typename Parameter>
        R request(const ::msgpack::rpc::msg_request<std::string, Parameter> msgreq)
        {
            ::msgpack::sbuffer sbuf;
            ::msgpack::pack(sbuf, msgreq);

            // request
            size_t len=boost::asio::write(m_socket, boost::asio::buffer(sbuf.data(), sbuf.size()));

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
