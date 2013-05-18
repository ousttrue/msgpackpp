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
}
