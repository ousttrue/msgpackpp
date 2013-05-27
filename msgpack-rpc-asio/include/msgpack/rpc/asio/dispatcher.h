#pragma once

namespace msgpack {
namespace rpc {
namespace asio {


    template<typename F, typename R, typename C, typename Params>
        std::shared_ptr<msgpack::sbuffer> helper(
                F handler,
                ::msgpack::rpc::msgid_t msgid, 
                ::msgpack::object msg_params)
    {
        // args check
        if(msg_params.type != type::ARRAY) { 
            throw msgerror("", error_params_not_array); 
        }
        if(msg_params.via.array.size>std::tuple_size<Params>::value){
            throw msgerror("", error_params_too_many); 
        }
        else if(msg_params.via.array.size<std::tuple_size<Params>::value){
            throw msgerror("", error_params_not_enough); 
        }

        // extract args
        Params params;
        try {
            msg_params.convert(&params);
        }
        catch(msgpack::type_error){
            throw msgerror("fail to convert params", error_params_convert);
        }

        // call
        R result=std::call_with_tuple(handler, params);

        ::msgpack::rpc::msg_response<R&, msgpack::type::nil> msgres(
                result, 
                msgpack::type::nil(), 
                msgid);
        // result
        auto sbuf=std::make_shared<msgpack::sbuffer>();
        msgpack::pack(*sbuf, msgres);
        return sbuf;
    }



class dispatcher
{
    boost::asio::io_service &m_io_service;
    boost::asio::io_service::work m_work;
    typedef std::function<
        std::shared_ptr<msgpack::sbuffer>(msgpack::rpc::msgid_t, msgpack::object)
        > func;
    std::map<std::string, func> m_handlerMap;
    std::shared_ptr<boost::thread> m_thread;

public:
    dispatcher(boost::asio::io_service &io_service)
        : m_io_service(io_service), m_work(io_service)
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
            throw msgerror("no handler", error_dispatcher_no_handler);
        }
        else{
            auto func=found->second;
            return func(msgid, params);
        }
    }

    void post(const object &msg, std::shared_ptr<session> session)
    {
        auto self=this;
        m_io_service.post([self, msg, session](){
                self->dispatch(msg, session);
                });
    };

    void dispatch(const object &msg, std::shared_ptr<session> session)
    {
        // extract msgpack request
        ::msgpack::rpc::msg_request<msgpack::object, msgpack::object> req;
        msg.convert(&req);
        try{
            // execute callback
            std::shared_ptr<msgpack::sbuffer> result=request(req.msgid, req.method, req.param);
            // send 
            session->write_async(result);
        }
        catch(msgerror ex)
        {
            session->write_async(ex.to_msg(req.msgid));
        }
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
                        return helper<F, R, C, std::tuple<>>(
                            handler, msgid, msg_params);
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
                        return helper<F, R, C, std::tuple<A1>>(
                            handler, msgid, msg_params);
                        }));
        }

    // 2
    template<typename F, typename R, typename C, 
        typename A1, typename A2>
        void add_handler(const std::string &method, F handler, R(C::*p)(A1, A2)const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        return helper<F, R, C, std::tuple<A1, A2>>(
                            handler, msgid, msg_params);

                        }));
        }

    // 4
    template<typename F, typename R, typename C, 
        typename A1, typename A2, typename A3, typename A4>
        void add_handler(const std::string &method, F handler, R(C::*p)(A1, A2, A3, A4)const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        return helper<F, R, C, std::tuple<A1, A2, A3, A4>>(
                            handler, msgid, msg_params);

                        }));
        }

    // for lambda/std::function
    template<typename F>
        void add_handler(const std::string &method, F handler)
        {
            add_handler(method, handler, &F::operator());
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

};

}
}
}
