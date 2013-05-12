#pragma once
#include <boost/asio.hpp>
#include <functional>


namespace msgpack {
namespace asiorpc {

    class server
    {
        boost::asio::io_service &m_service;
        boost::asio::ip::tcp::endpoint m_endpoint;
    public:
        server(boost::asio::io_service &io_service, boost::asio::ip::tcp::endpoint endpoint)
            : m_service(io_service), m_endpoint(endpoint)
        {}

        // 2
        template<typename R, typename A1, typename A2>
        void add_handler(const std::string &name, R(*handler)(A1, A2))
        {
            add_handler(name, std::function<R(A1, A2)>(handler));
        }

        template<typename R, typename A1, typename A2>
        void add_handler(const std::string &name, std::function<R(A1, A2)> handler)
        {
        }
    };


    class client
    {
        boost::asio::io_service &m_service;
        boost::asio::ip::tcp::endpoint m_endpoint;

    public:
        client(boost::asio::io_service &io_service, boost::asio::ip::tcp::endpoint endpoint)
            : m_service(io_service), m_endpoint(endpoint)
        {}

        // 2
        template<typename R, typename A1, typename A2>
        R call(R(*handler)(A1, A2), const std::string &name, A1 a1, A2 a2)
        {
            return call(std::function<R(A1, A2)>(handler), name, a1, a2);
        }

        template<typename R, typename A1, typename A2>
        R call(std::function<R(A1, A2)>, const std::string &name, A1 a1, A2 a2)
        {
            return R();
        }
    };

}}
