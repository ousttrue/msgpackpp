#pragma once
namespace std {

    // 0
    inline std::ostream &operator<<(std::ostream &os, const std::tuple<> &t)
    {
        os << "()";
        return os;
    }

    // 1
    template<typename A1>
        inline std::ostream &operator<<(std::ostream &os, const std::tuple<A1> &t)
        {
            os 
                << "("
                << std::get<0>(t)
                << ")";
            return os;
        }

    // 2
    template<typename A1, typename A2>
        inline std::ostream &operator<<(std::ostream &os, const std::tuple<A1, A2> &t)
        {
            os 
                << "("
                << std::get<0>(t)
                << ", " << std::get<1>(t)
                << ")";
            return os;
        }


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

    // 3
    template<typename F, typename A1, typename A2, typename A3>
        auto call_with_tuple(const F &handler, const std::tuple<A1, A2, A3> &args)->decltype(handler(A1(), A2(), A3(), A4()))
        {
            return handler(std::get<0>(args), std::get<1>(args), std::get<2>(args));
        }

    // 4
    template<typename F, typename A1, typename A2, typename A3, typename A4>
        auto call_with_tuple(const F &handler, const std::tuple<A1, A2, A3, A4> &args)->decltype(handler(A1(), A2(), A3(), A4()))
        {
            return handler(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args));
        }

    // void
    // 4
    template<typename F, typename A1, typename A2, typename A3, typename A4>
        void call_with_tuple_void(const F &handler, const std::tuple<A1, A2, A3, A4> &args)
        {
            handler(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args));
        }
}


namespace msgpack {

    // std::tuple pack/unpack
    // 0
    template <typename Stream>
        inline packer<Stream>& operator<< (packer<Stream>& o, const std::tuple<>& t)
        {
            o.pack_array(0);
            return o;
        }
    // 1
    template <typename Stream, typename A1>
        inline packer<Stream>& operator<< (packer<Stream>& o, const std::tuple<A1>& t)
        {
            o.pack_array(1);
            o.pack(std::get<0>(t));
            return o;
        }
    // 2
    template <typename Stream, typename A1, typename A2>
        inline packer<Stream>& operator<< (packer<Stream>& o, const std::tuple<A1, A2>& t)
        {
            o.pack_array(2);
            o.pack(std::get<0>(t));
            o.pack(std::get<1>(t));
            return o;
        }
    // 3
    template <typename Stream, typename A1, typename A2, typename A3>
        inline packer<Stream>& operator<< (packer<Stream>& o, const std::tuple<A1, A2, A3>& t)
        {
            o.pack_array(3);
            o.pack(std::get<0>(t));
            o.pack(std::get<1>(t));
            o.pack(std::get<2>(t));
            return o;
        }
    // 4
    template <typename Stream, typename A1, typename A2, typename A3, typename A4>
        inline packer<Stream>& operator<< (packer<Stream>& o, const std::tuple<A1, A2, A3, A4>& t)
        {
            o.pack_array(4);
            o.pack(std::get<0>(t));
            o.pack(std::get<1>(t));
            o.pack(std::get<2>(t));
            o.pack(std::get<3>(t));
            return o;
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

    // 3
    template <typename A1, typename A2, typename A3>
        inline std::tuple<A1, A2, A3>& operator>> (object o, std::tuple<A1, A2, A3>& t)
        {
            if(o.type != type::ARRAY) { throw type_error(); }
            if(o.via.array.size > 0) {
                object* p = o.via.array.ptr;
                p->convert(&std::get<0>(t));
                ++p;
                p->convert(&std::get<1>(t));
                ++p;
                p->convert(&std::get<2>(t));
            }
            return t;
        }

    // 4
    template <typename A1, typename A2, typename A3, typename A4>
        inline std::tuple<A1, A2, A3, A4>& operator>> (object o, std::tuple<A1, A2, A3, A4>& t)
        {
            if(o.type != type::ARRAY) { throw type_error(); }
            if(o.via.array.size > 0) {
                object* p = o.via.array.ptr;
                p->convert(&std::get<0>(t));
                ++p;
                p->convert(&std::get<1>(t));
                ++p;
                p->convert(&std::get<2>(t));
                ++p;
                p->convert(&std::get<3>(t));
            }
            return t;
        }
}

