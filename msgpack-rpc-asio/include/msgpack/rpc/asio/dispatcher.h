#pragma once

namespace msgpack {
namespace rpc {
namespace asio {


// handler call
// 0
template<typename F>
    auto call_with_tuple(const F &handler, const std::tuple<> &args)->decltype(handler())
    {
        return handler();
    }

// 1
template<typename F, typename A1>
    auto call_with_tuple(const F &handler, const std::tuple<A1> &args)->decltype(handler(A1()))
    {
        return handler(std::get<0>(args));
    }

// 2
template<typename F, typename A1, typename A2>
    auto call_with_tuple(const F &handler, const std::tuple<A1, A2> &args)->decltype(handler(A1(), A2()))
    {
        return handler(std::get<0>(args), std::get<1>(args));
    }

class dispatcher
{
    typedef std::function<
        std::shared_ptr<msgpack::sbuffer>(msgpack::rpc::msgid_t, msgpack::object)
        > func;
    std::map<std::string, func> m_handlerMap;
    bool m_is_running;
    std::shared_ptr<boost::thread> m_thread;

public:
    dispatcher()
        : m_is_running(false)
    {
    }

    ~dispatcher()
    {
        stop();
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

    void start_thread(std::shared_ptr<received_msg_queue> queue)
    {
        if(m_thread){
            return;
        }
        m_is_running=true;

        auto d=this;
        m_thread=std::make_shared<boost::thread>([d, queue](){

                while(d->m_is_running)
                {
                    auto item=queue->dequeue();
                    if(item){

                        // extract msgpack request
                        ::msgpack::rpc::msg_request<msgpack::object, msgpack::object> req;
                        item->msg().convert(&req);
                        // execute callback
                        std::shared_ptr<msgpack::sbuffer> result=d->request(
                            req.msgid, req.method, req.param);
                        // send 
                        item->session()->enqueue_write(result);
                    }

                    // ToDo: queue cond
                    boost::this_thread::sleep(boost::posix_time::milliseconds(300));
                }

				//std::cout << "stop dispatcher loop" << std::endl;
            });
    }

    void stop()
    {
        m_is_running=false;
        if(m_thread){
            m_thread->join();
        }
        m_thread=0;
    }

    ////////////////////
    // 0
    template<typename F, typename R, typename C>
        void add_handler(const std::string &method, F handler, R(C::*p)()const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        // extract args
                        std::tuple<> params;
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
    // 1
    template<typename F, typename R, typename C, typename A1>
        void add_handler(const std::string &method, F handler, R(C::*p)(A1)const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        // extract args
                        std::tuple<A1> params;
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
    // 2
    template<typename F, typename R, typename C, typename A1, typename A2>
        void add_handler(const std::string &method, F handler, R(C::*p)(A1, A2)const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        // extract args
                        std::tuple<A1, A2> params;
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
    // for lambda
    template<typename F>
        void add_handler(const std::string &method, F handler)
        {
            typedef decltype(handler) functor;
            add_handler(method, handler, &functor::operator());
        }

    // for function pointer
    // 0
    template<typename R>
        void add_handler(const std::string &method, R(*handler)())
        {
            add_handler(method, std::function<R()>(handler));
        }
    // 1
    template<typename R, typename A1>
        void add_handler(const std::string &method, R(*handler)(A1))
        {
            add_handler(method, std::function<R(A1)>(handler));
        }
    // 2
    template<typename R, typename A1, typename A2>
        void add_handler(const std::string &method, R(*handler)(A1, A2))
        {
            add_handler(method, std::function<R(A1, A2)>(handler));
        }

    // for std::function
    // 0
    template<typename R>
        void add_handler(const std::string &method, std::function<R()> handler)
        {
            typedef decltype(handler) functor;
            add_handler(method, handler, &functor::operator());
        }
    // 1
    template<typename R, typename A1>
        void add_handler(const std::string &method, std::function<R(A1)> handler)
        {
            typedef decltype(handler) functor;
            add_handler(method, handler, &functor::operator());
        }
    // 2
    template<typename R, typename A1, typename A2>
        void add_handler(const std::string &method, std::function<R(A1, A2)> handler)
        {
            typedef decltype(handler) functor;
            add_handler(method, handler, &functor::operator());
        }
};

}
}
}
