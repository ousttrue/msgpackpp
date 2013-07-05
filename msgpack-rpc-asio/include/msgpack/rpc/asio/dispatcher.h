#pragma once
#pragma warning(push)
#pragma warning(disable: 4819)        //warning about encoding of charactor in source code.
#include <boost/type_traits.hpp>
#pragma warning(pop)


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
            throw msgerror("error_params_not_array", error_params_not_array); 
        }
        if(msg_params.via.array.size>std::tuple_size<Params>::value){
            throw msgerror("error_params_too_many", error_params_too_many); 
        }
        else if(msg_params.via.array.size<std::tuple_size<Params>::value){
            throw msgerror("error_params_not_enough", error_params_not_enough); 
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

        ::msgpack::rpc::msg_response<R&, bool> msgres(
                result, 
				false, 
                msgid);
        // result
        auto sbuf=std::make_shared<msgpack::sbuffer>();
        msgpack::pack(*sbuf, msgres);
        return sbuf;
    }

    // void
    template<typename F, typename C, typename Params>
        std::shared_ptr<msgpack::sbuffer> helper(
                F handler,
                ::msgpack::rpc::msgid_t msgid, 
                ::msgpack::object msg_params)
    {
        // args check
        if(msg_params.type != type::ARRAY) { 
            throw msgerror("error_params_not_array", error_params_not_array); 
        }
        if(msg_params.via.array.size>std::tuple_size<Params>::value){
            throw msgerror("error_params_too_many", error_params_too_many); 
        }
        else if(msg_params.via.array.size<std::tuple_size<Params>::value){
            throw msgerror("error_params_not_enough", error_params_not_enough); 
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
        std::call_with_tuple_void(handler, params);

        ::msgpack::rpc::msg_response<msgpack::type::nil, bool> msgres(
                msgpack::type::nil(), 
                false, 
                msgid);

        // result
        auto sbuf=std::make_shared<msgpack::sbuffer>();
        msgpack::pack(*sbuf, msgres);
        return sbuf;
    }


class dispatcher
{
    typedef std::function<
        std::shared_ptr<msgpack::sbuffer>(msgpack::rpc::msgid_t, msgpack::object)
        > func;
    std::map<std::string, func> m_handlerMap;
    std::shared_ptr<boost::thread> m_thread;

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
            throw msgerror("no handler", error_dispatcher_no_handler);
        }
        else{
            auto func=found->second;
            return func(msgid, params);
        }
    }

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
                        typedef typename boost::remove_const<typename boost::remove_reference<A1>::type>::type B1;
                        return helper<F, R, C, std::tuple<B1>>(
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
				typedef typename boost::remove_const<typename boost::remove_reference<A1>::type>::type B1;
				typedef typename boost::remove_const<typename boost::remove_reference<A2>::type>::type B2;
				return helper<F, R, C, std::tuple<B1, B2>>(
					handler, msgid, msg_params);

			}));
        }

    // 3
    template<typename F, typename R, typename C, 
        typename A1, typename A2, typename A3>
        void add_handler(const std::string &method, F handler, R(C::*p)(A1, A2, A3)const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        typedef typename boost::remove_const<typename boost::remove_reference<A1>::type>::type B1;
                        typedef typename boost::remove_const<typename boost::remove_reference<A2>::type>::type B2;
                        typedef typename boost::remove_const<typename boost::remove_reference<A3>::type>::type B3;
                        return helper<F, R, C, std::tuple<B1, B2, B3>>(
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
                        typedef typename boost::remove_const<typename boost::remove_reference<A1>::type>::type B1;
                        typedef typename boost::remove_const<typename boost::remove_reference<A2>::type>::type B2;
                        typedef typename boost::remove_const<typename boost::remove_reference<A3>::type>::type B3;
                        typedef typename boost::remove_const<typename boost::remove_reference<A4>::type>::type B4;
                        return helper<F, R, C, std::tuple<B1, B2, B3, B4>>(
                            handler, msgid, msg_params);

                        }));
        }

    // void
    // 0
    template<typename F, typename C
        >
        void add_handler(const std::string &method, F handler, void(C::*p)()const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        return helper<F, C, std::tuple<>>(
                            handler, msgid, msg_params);

                        }));
        }

    // 1
    template<typename F, typename C, 
        typename A1>
        void add_handler(const std::string &method, F handler, void(C::*p)(A1)const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        typedef typename boost::remove_const<typename boost::remove_reference<A1>::type>::type B1;
                        return helper<F, C, std::tuple<B1>>(
                            handler, msgid, msg_params);

                        }));
        }

    // 2
    template<typename F, typename C, 
        typename A1, typename A2>
        void add_handler(const std::string &method, F handler, void(C::*p)(A1, A2)const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        typedef typename boost::remove_const<typename boost::remove_reference<A1>::type>::type B1;
                        typedef typename boost::remove_const<typename boost::remove_reference<A2>::type>::type B2;
                        return helper<F, C, std::tuple<B1, B2>>(
                            handler, msgid, msg_params);

                        }));
        }

    // 3
    template<typename F, typename C, 
        typename A1, typename A2, typename A3>
        void add_handler(const std::string &method, F handler, void(C::*p)(A1, A2, A3)const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        typedef typename boost::remove_const<typename boost::remove_reference<A1>::type>::type B1;
                        typedef typename boost::remove_const<typename boost::remove_reference<A2>::type>::type B2;
                        typedef typename boost::remove_const<typename boost::remove_reference<A3>::type>::type B3;
                        return helper<F, C, std::tuple<B1, B2, B3>>(
                            handler, msgid, msg_params);

                        }));
        }

    // 4
    template<typename F, typename C, 
        typename A1, typename A2, typename A3, typename A4>
        void add_handler(const std::string &method, F handler, void(C::*p)(A1, A2, A3, A4)const)
        {
            m_handlerMap.insert(std::make_pair(method, [handler](
                            ::msgpack::rpc::msgid_t msgid, 
                            ::msgpack::object msg_params)->std::shared_ptr<msgpack::sbuffer>
                        {
                        typedef typename boost::remove_const<typename boost::remove_reference<A1>::type>::type B1;
                        typedef typename boost::remove_const<typename boost::remove_reference<A2>::type>::type B2;
                        typedef typename boost::remove_const<typename boost::remove_reference<A3>::type>::type B3;
                        typedef typename boost::remove_const<typename boost::remove_reference<A4>::type>::type B4;
                        return helper<F, C, std::tuple<B1, B2, B3, B4>>(
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
    // 3
    template<typename R, typename A1, typename A2, typename A3>
        void add_handler(const std::string &method, R(*handler)(A1, A2, A3))
        {
            add_handler(method, std::function<R(A1, A2, A3)>(handler));
        }
    // 4
    template<typename R, typename A1, typename A2, typename A3, typename A4>
        void add_handler(const std::string &method, R(*handler)(A1, A2, A3, A4))
        {
            add_handler(method, std::function<R(A1, A2, A3, A4)>(handler));
        }

    // for std::bind
    // 0
    template<typename R, typename C>
        void add_bind(const std::string &method, R(C::*handler)(), 
                C *self)
        {
            add_handler(method, std::function<R()>(std::bind(handler, self)));
        }

    // 1
    template<typename R, typename C, typename A1, 
        typename B1>
        void add_bind(const std::string &method, R(C::*handler)(A1), 
                C *self, B1 b1)
        {
            add_handler(method, std::function<R(A1)>(std::bind(handler, self, b1)));
        }

    // 2
    template<typename R, typename C, typename A1, typename A2, 
        typename B1, typename B2>
        void add_bind(const std::string &method, R(C::*handler)(A1, A2), 
                C *self, B1 b1, B2 b2)
        {
            add_handler(method, std::function<R(A1, A2)>(
                        std::bind(handler, self, b1, b2)));
        }

    // 3
    template<typename R, typename C, typename A1, typename A2, typename A3, 
        typename B1, typename B2, typename B3>
        void add_bind(const std::string &method, R(C::*handler)(A1, A2, A3), 
                C *self, B1 b1, B2 b2, B3 b3)
        {
            add_handler(method, std::function<R(A1, A2, A3)>(
                        std::bind(handler, self, b1, b2, b3)));
        }

    // 4
    template<typename R, typename C, typename A1, typename A2, typename A3, typename A4,
        typename B1, typename B2, typename B3, typename B4>
        void add_bind(const std::string &method, R(C::*handler)(A1, A2, A3, A4), 
                C *self, B1 b1, B2 b2, B3 b3, B4 b4)
        {
            add_handler(method, std::function<R(A1, A2, A3, A4)>(
                        std::bind(handler, self, b1, b2, b3, b4)));
        }

    // for std::bind(void)
    // 4
    template<typename C, typename A1, typename A2, typename A3, typename A4,
        typename B1, typename B2, typename B3, typename B4>
        void add_bind(const std::string &method, void(C::*handler)(A1, A2, A3, A4), 
                C *self, B1 b1, B2 b2, B3 b3, B4 b4)
        {
            add_handler(method, std::function<void(A1, A2, A3, A4)>(
                        std::bind(handler, self, b1, b2, b3, b4)));
        }

    // for std::bind(const)
    // 0
    template<typename R, typename C>
        void add_bind(const std::string &method, R(C::*handler)()const, 
                C *self)
        {
            add_handler(method, std::function<R()>(std::bind(handler, self)));
        }

    // 1
    template<typename R, typename C, typename A1, typename B1>
        void add_bind(const std::string &method, R(C::*handler)(A1)const, 
                C *self, B1 b1)
        {
            add_handler(method, std::function<R(A1)>(std::bind(handler, self, b1)));
        }

    // 2
    template<typename R, typename C, typename A1, typename A2, typename B1, typename B2>
        void add_bind(const std::string &method, R(C::*handler)(A1, A2)const, 
                C *self, B1 b1, B2 b2)
        {
            add_handler(method, std::function<R(A1, A2)>(std::bind(handler, self, b1, b2)));
        }

    // utility
    template<typename T, typename V>
        void add_property(const std::string &property,
                std::function<T*()> thisGetter,
                V(T::*getMethod)()const,
                void(T::*setMethod)(V)
                )
        {
            add_handler(std::string("get_")+property, [thisGetter, getMethod](
                        )->V{
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    return (self->*getMethod)();
                    });
            add_handler(std::string("set_")+property, [thisGetter, setMethod](
                        const V& value){
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    (self->*setMethod)(value);
                    });
        }
    // utility(const &)
    template<typename T, typename V>
        void add_property(const std::string &property,
                std::function<T*()> thisGetter,
                V(T::*getMethod)()const,
                void(T::*setMethod)(const V&)
                )
        {
            add_handler(std::string("get_")+property, [thisGetter, getMethod](
                        )->V{
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    return (self->*getMethod)();
                    });
            add_handler(std::string("set_")+property, [thisGetter, setMethod](
                        const V& value){
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    (self->*setMethod)(value);
                    });
        }


    // ToDo
    // removeMethod
    template<typename T, typename V>
        void add_list_property(const std::string &property,
                std::function<T*()> thisGetter,
                void(T::*clearMethod)(),
                void(T::*addMethod)(const V&),
                //void(T::*updateMethod)(const V&),
                void(T::*updateAtMethod)(size_t, const V&),
                void(T::*removeAtMethod)(size_t),
                void(T::*movefromtoMethod)(size_t, size_t),
                std::list<V>(T::*listMethod)()const              
                )
        {
            if(clearMethod){
            add_handler(std::string("clear_")+property, [thisGetter, clearMethod](
                        ){
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    (self->*clearMethod)();
                    });
            }
            if(addMethod){
            add_handler(std::string("additem_")+property, [thisGetter, addMethod](
                        const V &item){
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    (self->*addMethod)(item);
                    });
            }
            /*
            if(updateMethod){
            add_handler(std::string("updateitem_")+property, [thisGetter, updateMethod](
                        const V &item){
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    (self->*updateMethod)(item);
                    });
            }
            */
            if(updateAtMethod){
            add_handler(std::string("updateitemat_")+property, [thisGetter, updateAtMethod](
                        size_t index, const V &item){
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    (self->*updateAtMethod)(index, item);
                    });
            }
            if(removeAtMethod){
            add_handler(std::string("removeat_")+property, [thisGetter, removeAtMethod](
                        size_t index){
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    (self->*removeAtMethod)(index);
                    });
            }
            if(movefromtoMethod){
            add_handler(std::string("movefromto_")+property, [thisGetter, movefromtoMethod](
                        size_t from, size_t to){
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    (self->*movefromtoMethod)(from, to);
                    });
            }
            if(listMethod){
            add_handler(std::string("list_")+property, [thisGetter, listMethod](
                        )->std::list<V>{
                    auto self=thisGetter();
                    if(!self){
                    throw msgerror("fail to convert params", error_self_pointer_is_null);
                    }
                    return (self->*listMethod)();
                    });
            }
        }

};

}
}
}
