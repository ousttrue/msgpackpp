#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/experimental/as_tuple.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#include <asio/system_timer.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/use_future.hpp>
#include <asio/write.hpp>
#include <functional>
#include <future>
#include <iostream>
#include <msgpackpp.h>
#include <span>
#include <string>
#include <thread>
#include <unordered_map>

const auto PORT = 8070;

constexpr auto use_nothrow_awaitable =
    asio::experimental::as_tuple(asio::use_awaitable);

static std::vector<uint8_t> to_vector(const asio::streambuf &buf) {
  auto p = asio::buffer_cast<const uint8_t *>(buf.data());
  return std::vector<uint8_t>(p, p + buf.size());
}

static std::span<const uint8_t> to_span(const asio::streambuf &buf) {
  auto p = asio::buffer_cast<const uint8_t *>(buf.data());
  return std::span<const uint8_t>{p, buf.size()};
}

using on_message = std::function<msgpackpp::bytes(const msgpackpp::parser &)>;

class server {

  asio::io_context &_context;
  asio::ip::tcp::acceptor _acceptor;
  on_message _callback;

public:
  server(asio::io_context &context, const on_message &callback)
      : _context(context), _acceptor(context), _callback(callback) {}
  ~server() {}

  void listen(const asio::ip::tcp::endpoint &ep) {
    std::cout << "[server]listen: " << ep << "..." << std::endl;
    _acceptor.open(ep.protocol());
    _acceptor.bind(ep);
    _acceptor.listen();

    auto ex = _context.get_executor();
    asio::co_spawn(ex, accept_loop(), asio::detached);
  }

  asio::awaitable<void> accept_loop() {

    while (true) {

      auto [e, socket] = co_await _acceptor.async_accept(use_nothrow_awaitable);
      if (e) {
        std::cout << "[server]accept error: " << e << std::endl;
        break;
      }
      std::cout << "[server]accepted" << std::endl;

      auto ex = _context.get_executor();
      asio::co_spawn(ex, session(std::move(socket)), asio::detached);
    }
  }

  asio::awaitable<void> session(asio::ip::tcp::socket socket) {

    asio::streambuf buf;
    while (true) {
      auto [e1, read_size] = co_await asio::async_read(
          socket, buf, asio::transfer_at_least(1), use_nothrow_awaitable);

      auto span = to_span(buf);

      auto parsed = msgpackpp::parser(span.data(), (int)span.size());
      while (true) {
        auto current = parsed;
        try {
          parsed = parsed.next();

          // callback
          auto result = _callback(current);
          std::cout << "[server]" << current << "=>"
                    << msgpackpp::parser(result) << std::endl;
          auto write_size = co_await asio::async_write(
              socket, asio::buffer(result), asio::use_awaitable);

        } catch (const std::runtime_error &) {
          break;
        }
      }
      buf.consume(parsed.data() - span.data());
    }
  }
};

class func_map {
  std::list<std::string> _names;
  std::unordered_map<std::string_view, msgpackpp::procedurecall> _map;

public:
  template <typename F>
  void add_handler(const std::string_view &method, const F &f) {
    _names.push_back(std::string(method.begin(), method.end()));
    std::string_view key = _names.back();
    auto proc = msgpackpp::make_procedurecall(f);
    _map.insert(std::make_pair(key, proc));
  }

  msgpackpp::bytes dispatch(const std::string_view method,
                            const msgpackpp::parser &arg) {
    auto found = _map.find(method);
    if (found == _map.end()) {
      throw std::runtime_error("method not found");
    }
    auto proc = found->second;
    return proc(arg);
  }
};

class messagepack_rpc {
  uint32_t _nextRequestID = 1;
  func_map _dispatcher;

public:
  func_map &dispatcher() { return _dispatcher; }

  msgpackpp::bytes on_message(const msgpackpp::parser &message) {
    if (!message.is_array()) {
      throw std::runtime_error("not array");
    }

    auto message_type = message[0].get_number<int>();
    switch (message_type) {
    case 0:
      // request:
      return on_request(message);

    case 1:
      // respone:
      on_response(message);
      return {};

    case 2:
      // notify:
      on_notify(message);
      return {};

    default:
      throw std::runtime_error("unknown message_type");
    }
  }

  template <typename R, typename... ARGS>
  asio::awaitable<R> request(asio::ip::tcp::socket &socket,
                             const std::string &method, ARGS... args) {
    auto request =
        msgpackpp::make_rpc_request(_nextRequestID++, method, args...);
    auto write_size = co_await asio::async_write(socket, asio::buffer(request),
                                                 asio::use_awaitable);

    asio::streambuf buf;
    auto read_size = co_await asio::async_read(
        socket, buf, asio::transfer_at_least(1), asio::use_awaitable);
    auto response = to_vector(buf);
    auto parsed = msgpackpp::parser(response);
    std::cout << "[client]" << msgpackpp::parser(request) << "=>" << parsed
              << std::endl;
    co_return parsed[3].get_number<int>();
  }

private:
  msgpackpp::bytes on_request(const msgpackpp::parser &request) {
    auto message_id = request[1].get_number<int>();
    try {
      // success
      auto result = _dispatcher.dispatch(request[2].get_string(), request[3]);
      auto response =
          msgpackpp::make_rpc_response_packed(message_id, "", result);
      return response;
    } catch (const std::runtime_error &e) {
      // error
      auto response =
          msgpackpp::make_rpc_response(message_id, e.what(), msgpackpp::nil);
      return response;
    }
  }

  void on_response(const msgpackpp::parser &response) {
    throw std::runtime_error("not implemented");
  }

  void on_notify(const msgpackpp::parser &notify) {
    throw std::runtime_error("not implemented");
  }
};

asio::awaitable<int> client(asio::io_context &context,
                            asio::ip::tcp::endpoint ep) {
  std::cout << "[client]connect: " << ep << "..." << std::endl;
  asio::ip::tcp::socket socket(context);
  co_await socket.async_connect(ep, asio::use_awaitable);
  std::cout << "[client]connected" << std::endl;

  messagepack_rpc rpc;
  auto result = co_await rpc.request<int>(socket, "add", 1, 2);
  co_return result;
}

int main(int argc, char **argv) {

  auto ep = asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"),
                                    PORT);

  // server rpc
  messagepack_rpc server_rpc;
  server_rpc.dispatcher().add_handler("add",
                                      [](int a, int b) { return a + b; });

  // server
  asio::io_context server_context;
  server server(server_context, std::bind(&messagepack_rpc::on_message,
                                          &server_rpc, std::placeholders::_1));
  server.listen(ep);
  std::thread server_thread([&server_context]() { server_context.run(); });

  // client
  asio::io_context client_context;
  auto co = std::bind(&client, std::ref(client_context), ep);
  auto result =
      asio::co_spawn(client_context.get_executor(), co, asio::use_future);
  client_context.run();
  std::cout << "result = " << result.get() << std::endl;

  // stop asio
  server_context.stop();
  server_thread.join();
  std::cout << "[server]join" << std::endl;

  return 0;
}
