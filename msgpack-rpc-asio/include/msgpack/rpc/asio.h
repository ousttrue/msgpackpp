#pragma once
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <memory>
#include <functional>
#include <msgpack/rpc/protocol.h>
#include <msgpack/rpc/types.h>
#include <msgpack/rpc/exception.h>


namespace msgpack {

    // ret
    template<typename F, typename Ret, typename... Rest>
        Ret
        helper0(Ret (F::*)(Rest...));

    template<typename F, typename Ret, typename... Rest>
        Ret
        helper0(Ret (F::*)(Rest...) const);

    template<typename F>
    struct result_type
    {
        typedef decltype(F()) func_type;
        typedef decltype(helper0(&func_type::operator())) type;
    };

    // 1
    template<typename F, typename Ret, typename A1, typename... Rest>
        A1
        helper1(Ret (F::*)(A1, Rest...));

    template<typename F, typename Ret, typename A1, typename... Rest>
        A1
        helper1(Ret (F::*)(A1, Rest...) const);

    // 2
    template<typename F, typename Ret, typename A1, typename A2, typename... Rest>
        A2
        helper2(Ret (F::*)(A1, A2, Rest...));

    template<typename F, typename Ret, typename A1, typename A2, typename... Rest>
        A2
        helper2(Ret (F::*)(A1, A2, Rest...) const);

    // 0
    template <typename Stream>
    inline packer<Stream>& operator<< (packer<Stream>& o, const std::tuple<>& t)
    {
        o.pack_array(0);
    }
    // 1
    template <typename Stream, typename A1>
    inline packer<Stream>& operator<< (packer<Stream>& o, const std::tuple<A1>& t)
    {
        o.pack_array(1);
        o.pack(std::get<0>(t));
    }
    // 2
    template <typename Stream, typename A1, typename A2>
    inline packer<Stream>& operator<< (packer<Stream>& o, const std::tuple<A1, A2>& t)
    {
        o.pack_array(2);
        o.pack(std::get<0>(t));
        o.pack(std::get<1>(t));
    }
    // 3
    template <typename Stream, typename A1, typename A2, typename A3>
    inline packer<Stream>& operator<< (packer<Stream>& o, const std::tuple<A1, A2, A3>& t)
    {
        o.pack_array(3);
        o.pack(std::get<0>(t));
        o.pack(std::get<1>(t));
        o.pack(std::get<2>(t));
    }

    // 0
    inline std::tuple<>& operator>> (object o, std::tuple<>& t)
    {
        if(o.type != type::ARRAY) { throw type_error(); }
        return t;
    }

    // 1
    template <typename A1>
    inline std::tuple<A1>& operator>> (object o, std::tuple<A1>& t)
    {
        if(o.type != type::ARRAY) { throw type_error(); }
        if(o.via.array.size > 0) {
            object* p = o.via.array.ptr;
            p->convert(&std::get<0>(t));
        }
        return t;
    }

    // 2
    template <typename A1, typename A2>
    inline std::tuple<A1, A2>& operator>> (object o, std::tuple<A1, A2>& t)
    {
        if(o.type != type::ARRAY) { throw type_error(); }
        if(o.via.array.size > 0) {
            object* p = o.via.array.ptr;
            p->convert(&std::get<0>(t));
            ++p;
            p->convert(&std::get<1>(t));
        }
        return t;
    }


namespace rpc {
namespace asio {

    // 0
    template<typename F>
    auto call_with_tuple(const F &handler, std::tuple<> &args)->typename result_type<F>::type
    {
        return handler();
    }

    // 1
    template<typename F, typename A1>
    auto call_with_tuple(const F &handler, std::tuple<A1> &args)->typename result_type<F>::type
    {
        return handler(std::get<0>(args));
    }

    // 2
    template<typename F, typename A1, typename A2>
    auto call_with_tuple(const F &handler, std::tuple<A1, A2> &args)->typename result_type<F>::type
    {
        return handler(std::get<0>(args), std::get<1>(args));
    }

    class dispatcher
    {
        typedef std::function<
            std::shared_ptr<msgpack::sbuffer>(msgpack::rpc::msgid_t, msgpack::object)
            > func;
        std::map<std::string, func> m_handlerMap;

    public:
		dispatcher()
		{
		}

		~dispatcher()
		{
		}

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

        // for lambda
        template<typename F, typename R, typename T, typename ...A>
        void add_handler(const std::string &method, F handler, R(T::*)(A...)const)
        {
            add_handler(method, std::function<R(A...)>(handler));
        }

        template<typename F>
        void add_handler(const std::string &method, F handler)
        {
            typedef decltype(handler) functor;
            add_handler(method, handler, &functor::operator());
        }

        // for function pointer
        template<typename R, typename ...A>
        void add_handler(const std::string &method, R(*handler)(A...))
        {
            add_handler(method, std::function<R(A...)>(handler));
        }

        // for std::function
        template<typename R, typename ...A>
        void add_handler(const std::string &method, std::function<R(A...)> handler)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                            // extract args
                            std::tuple<A...> params;
                            msg_params.convert(&params);

                            // call
                            R result=call_with_tuple(handler, params);

                            // error type
                            typedef ::msgpack::type::nil Error;
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

        template<typename ...A>
        ::msgpack::rpc::msg_request<std::string, std::tuple<A...>> 
        create(const std::string &method, A...args)
        {
            ::msgpack::rpc::msgid_t msgid = next_msgid();
            typedef std::tuple<A...> Parameter;
            return ::msgpack::rpc::msg_request<std::string, Parameter>(
                    method, Parameter(args...), msgid);
        }
    };


    class func_call
    {
        enum STATUS_TYPE
        {
            STATUS_WAIT,
            STATUS_RECEIVED,
            STATUS_ERROR,
        };
        STATUS_TYPE m_status;
        ::msgpack::object m_result;
        ::msgpack::object m_error;
    public:
        func_call()
            : m_status(STATUS_WAIT)
        {
        }

        void set_result(const ::msgpack::object &result)
        {
            m_result=result;
            m_status=STATUS_RECEIVED;
        }

        void set_error(const ::msgpack::object &error)
        {
            m_error=error;
            m_status=STATUS_ERROR;
        }

        // blocking
        // ToDo: boost::condition
        func_call& sync()
        {
            while(m_status==STATUS_WAIT)
            {
                boost::this_thread::sleep(boost::posix_time::milliseconds(300));
            }
            return *this;
        }

        template<typename R>
        R& convert(R *value)const
        {
            if(m_status==STATUS_RECEIVED){
                m_result.convert(value);
                return *value;
            }
            else{
                throw rpc_error("not ready");
            }
        }
    };


    class session: public std::enable_shared_from_this<session>
    {
        request_factory m_request_factory;
        boost::asio::ip::tcp::socket m_socket;
        enum { max_length = 1024 };
        char m_data[max_length];
        unpacker m_pac;
        std::weak_ptr<dispatcher> m_dispatcher;

        bool m_writing;
        std::list<std::shared_ptr<msgpack::sbuffer>> m_write_queue;
		std::map<msgpack::rpc::msgid_t, std::shared_ptr<func_call>> m_requestMap;

        // must shard_ptr
        session(boost::asio::io_service& io_service, 
                std::shared_ptr<dispatcher> d=std::shared_ptr<dispatcher>())
            : m_socket(io_service), m_pac(1024), m_dispatcher(d),
            m_writing(false)
            {
            }
 public:

		~session()
		{
		}

        static std::shared_ptr<session> create(boost::asio::io_service &io_service,
                std::shared_ptr<dispatcher> d=std::shared_ptr<dispatcher>())
        {
            return std::shared_ptr<session>(new session(io_service, d));
        }

        boost::asio::ip::tcp::socket& socket()
        {
            return m_socket;
        }

        void connect(boost::asio::ip::tcp::endpoint endpoint)
        {
            auto shared=shared_from_this();
            m_socket.async_connect(endpoint, [shared](const boost::system::error_code &error){
                    if(error){
                        std::cerr << "error !" << std::endl;
                    }
                    else{
                        shared->startRead();
                    }

                    }); 
        }

        template<typename ...A>
        std::shared_ptr<func_call> call(const std::string &method, A...args)
        {
            auto request=m_request_factory.create(method, args...);
            return sendRequest(request);
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
                        auto dispatcher=shared->m_dispatcher.lock();
                        if (error) {
							int a=0;
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
                                                    dispatcher->request(
                                                            req.msgid, req.method, req.param);
                                                shared->enqueueWrite(result);
                                            }
                                            break;

                                        case ::msgpack::rpc::RESPONSE: 
                                            {
                                                ::msgpack::rpc::msg_response<object, object> res;
                                                msg.convert(&res);
                                                auto found=shared->m_requestMap.find(res.msgid);
                                                if(found!=shared->m_requestMap.end()){
													// ToDo: error check
													found->second->set_result(res.result);
                                                }
												else{
													// ToDo:error
													int a=0;
												}
                                            }
                                            break;

                                        case ::msgpack::rpc::NOTIFY: 
                                            {
                                                ::msgpack::rpc::msg_notify<object, object> req;
                                                msg.convert(&req);
                                                        //req.method, req.param);
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
        template<typename Parameter>
        std::shared_ptr<func_call> sendRequest(const ::msgpack::rpc::msg_request<std::string, Parameter> &msgreq)
        {
            auto sbuf=std::make_shared<msgpack::sbuffer>();
            ::msgpack::pack(*sbuf, msgreq);

            auto req=std::make_shared<func_call>();            
            m_requestMap.insert(std::make_pair(msgreq.msgid, req));

            enqueueWrite(sbuf);

            return req;
        }

            /*
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
            */

        // write
        void enqueueWrite(std::shared_ptr<msgpack::sbuffer> msg)
        {
            // lock
            if(m_writing){
                // queueing...
                m_write_queue.push_back(msg);
            }
            else{
                // start async write
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
