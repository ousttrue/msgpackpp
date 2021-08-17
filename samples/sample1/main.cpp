#include "asio/io_service.hpp"
#include <iterator>
#include <memory>
#include <msgpackpp.h>
#include <asio.hpp>
#include <stdexcept>
#include <stdint.h>
#include <thread>
#include <iostream>
#include <string_view>
#include <functional>
#include <unordered_map>
#include <utility>

namespace msgpackpp
{
namespace rpc
{
using on_message_func = std::function<bytes(const parser &)>;
using on_response_func = std::function<void(const parser &)>;

class transport : std::enable_shared_from_this<transport>
{
    std::shared_ptr<asio::ip::tcp::socket> _socket;
    std::unordered_map<int, on_response_func> _call_map;

    transport(const on_message_func &callback)
        : _callback(callback)
    {
    }

public:
    ~transport()
    {
        auto a = 0;
    }

    on_message_func _callback;
    std::vector<uint8_t> _buffer;
    static std::shared_ptr<transport> create(const on_message_func &callback)
    {
        return std::shared_ptr<transport>(new transport(callback));
    }

    void accept(std::shared_ptr<::asio::ip::tcp::socket> socket)
    {
        _socket = socket;
        // set_connection_status(connection_connected);
        start_read();
    }

    void start_read()
    {
        if (!_socket)
        {
            // closed
            //assert(false);
            return;
        }
        auto on_read = [shared = shared_from_this()](
                           const ::asio::error_code &error,
                           size_t bytes_transferred)
        {
            if (error && (error != ::asio::error::eof))
            {
                // no more read
                return;
            }
            else
            {
                auto parsed = parser(shared->_buffer);
                while (true)
                {
                    try
                    {
                        auto current = parsed;
                        // stream message. get next
                        parsed = parsed.next();
                        // msgpack-rpc request [0, id, method, args]
                        auto result = shared->_callback(current);
                        shared->write_async(result);
                    }
                    catch (...)
                    {
                        // no more read
                        // todo: close after write
                        break;
                    }
                }

                // consume used buffer
                auto d = parsed.data() - shared->_buffer.data();
                shared->_buffer.erase(shared->_buffer.begin(), shared->_buffer.begin() + d);

                // next loop
                shared->start_read();
            }
        };
        _socket->async_read_some(
            asio::buffer(_buffer),
            on_read);
    }

    void write_async(const bytes &bytes)
    {
        if (!_socket)
        {
            assert(false);
            return;
        }
        auto on_write = [shared = shared_from_this()](
                            const ::asio::error_code &error,
                            size_t bytes_transferred)
        {
            if (error)
            {
                throw std::runtime_error("write_async");
            }
        };
        _socket->async_write_some(
            ::asio::buffer(bytes.data(), bytes.size()), on_write);
    }

    static std::shared_ptr<transport> connect_async(asio::io_service &io_service, const asio::ip::tcp::endpoint &ep)
    {
        auto new_connection = transport::create({});
        auto socket = std::make_shared<::asio::ip::tcp::socket>(io_service);
        auto on_connect = [t = new_connection, socket /*keep socket*/](
                              const ::asio::error_code &error)
        {
            if (error)
            {
                throw std::runtime_error("connect_async");
            }
            else
            {
                t->start_read();
            }
        };
        socket->async_connect(ep, on_connect);
        return new_connection;
    }

    template <typename R, typename... ARGS>
    std::future<R> call(const std::string &method, ARGS...)
    {
        auto p = std::make_shared<std::promise<R>>();

        auto f = p->get_future();

        // register response callback
        _call_map.insert(std::make_pair(1, [p](const msgpackpp::parser &msg)
                                        { p->set_value({}); }));

        // send request

        return f;
    }
};

class server
{
    asio::io_service &_io_service;
    asio::ip::tcp::acceptor m_acceptor;

public:
    server(asio::io_service &io_service) : _io_service(io_service), m_acceptor(io_service)
    {
    }

    void start_accept(const on_message_func &on_receive)
    {
        auto new_connection = transport::create(on_receive);
        auto socket = std::make_shared<asio::ip::tcp::socket>(_io_service);

        auto on_accept = [self = this, on_receive, new_connection, socket /*keep socket*/](
                             const ::asio::error_code &error)
        {
            if (error)
            {
                throw error;
            }
            else
            {
                new_connection->accept(socket);
                // next
                self->start_accept(on_receive);
            }
        };
        m_acceptor.async_accept(*socket.get(), on_accept);
    }

    void listen(const asio::ip::tcp::endpoint &ep, const on_message_func &callback)
    {
        m_acceptor.open(ep.protocol());
        m_acceptor.bind(ep);
        m_acceptor.listen();
        start_accept(callback);
    }
};

class dispatcher
{
    std::unordered_map<std::string, procedurecall> _map;

public:
    // [0, id, method, args]
    bytes dispatch(const parser &parsed)
    {
        if (parsed[0].get_number<int>() != 0)
        {
            throw std::runtime_error("not implemented");
        }

        auto id = parsed[1].get_number<int>();
        auto found = _map.find(std::string(parsed[2].get_string()));
        if (found == _map.end())
        {
            throw std::runtime_error("not implemented");
        }

        auto result = (found->second)(parsed[3]);
        return make_rpc_response(id, "", result);
    }

    template <typename T>
    void add_handler(const std::string_view name, const T &callback)
    {
        _map.insert(std::make_pair(name, make_procedurecall(callback)));
    }
};

} // namespace rpc
} // namespace msgpackpp

class SomeClass
{
    int m_number;

public:
    void setNumber(const int &number) { m_number = number; }
    int getNumber() const { return m_number; }
};

int main(int argc, char **argv)
{
    const static int PORT = 8070;

    // dispatcher
    msgpackpp::rpc::dispatcher dispatcher;
    dispatcher.add_handler("add", [](int a, int b) -> int
                           { return a + b; });
    dispatcher.add_handler("mul", [](float a, float b) -> float
                           { return a * b; });
    SomeClass s;
    std::function<int()> getter = std::bind(&SomeClass::getNumber, &s);
    std::function<void(int)> setter = std::bind(&SomeClass::setNumber, &s, std::placeholders::_1);

    // server
    asio::io_service server_io;
    msgpackpp::rpc::server server(server_io);
    server.listen(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), PORT), [&dispatcher](const msgpackpp::parser &msg) -> msgpackpp::bytes
                  { return dispatcher.dispatch(msg); });
    std::thread server_thread([&server_io]()
                              { server_io.run(); });

    // client
    asio::io_service client_io;
    auto client = msgpackpp::rpc::transport::connect_async(client_io, asio::ip::tcp::endpoint(
                                                                          asio::ip::address::from_string("127.0.0.1"), PORT));
    std::thread clinet_thread([&client_io]()
                              { client_io.run(); });

    // request
    auto result = client->call<int>("add", 1, 2);
    std::cout << "add, 1, 2 = " << result.get() << std::endl;

    // stop asio
    client_io.stop();
    clinet_thread.join();

    server_io.stop();
    server_thread.join();

    return 0;
}
