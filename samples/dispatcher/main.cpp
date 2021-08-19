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

using namespace std::chrono;

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

class dispatcher {
  std::unordered_map<std::string, msgpackpp::procedurecall> _map;

public:
  template <typename F>
  void add_handler(const std::string_view &method, const F &f) {
    auto proc = msgpackpp::make_procedurecall(f);
    _map.insert(std::make_pair(method, proc));
  }

  msgpackpp::bytes dispatch(const msgpackpp::parser &msg) {
    auto method = msg[2].get_string();
    auto found = _map.find(std::string(method.begin(), method.end()));
    auto proc = found->second;

    auto result = proc(msg[3]);
    auto response =
        msgpackpp::make_rpc_response_packed(msg[1].get_number<int>(), "", result);
    std::cout << "[dispatch]" << msg << "=>" << msgpackpp::parser(response)
              << std::endl;
    return response;
  }
};

const auto PORT = 8070;

int main(int argc, char **argv) {

  auto ep = asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"),
                                    PORT);

  // dispatcher
  dispatcher d;
  d.add_handler("add", [](int a, int b) { return a + b; });

  // server
  asio::io_context server_context;
  server server(server_context,
                std::bind(&dispatcher::dispatch, &d, std::placeholders::_1));
  server.listen(ep);
  std::thread server_thread([&server_context]() { server_context.run(); });

  // client
  asio::io_context client_context;
  auto co = [&context = client_context, ep]() -> asio::awaitable<int> {
    std::cout << "[client]wait 1000ms..." << std::endl;
    asio::system_timer timer(context);
    timer.expires_from_now(1000ms);
    co_await timer.async_wait(asio::use_awaitable);

    std::cout << "[client]connect: " << ep << "..." << std::endl;
    asio::ip::tcp::socket socket(context);
    co_await socket.async_connect(ep, asio::use_awaitable);
    std::cout << "[client]connected" << std::endl;

    auto request = msgpackpp::make_rpc_request(1, "add", 1, 2);
    std::cout << "[client]" << msgpackpp::parser(request) << std::endl;
    auto write_size = co_await asio::async_write(socket, asio::buffer(request),
                                                 asio::use_awaitable);
    assert(write_size == 10);

    std::cout << "[client]read..." << std::endl;
    asio::streambuf buf;
    auto read_size = co_await asio::async_read(
        socket, buf, asio::transfer_at_least(1), asio::use_awaitable);
    auto response = to_vector(buf);
    auto parsed = msgpackpp::parser(response);
    std::cout << "[client]" << parsed << std::endl;
    return parsed[3].get_number<int>();
  };
  auto result =
      asio::co_spawn(client_context.get_executor(), co, asio::use_future);
  client_context.run();
  auto pong = result.get();
  std::cout << "pong: " << pong << std::endl;

  // stop asio
  server_context.stop();
  server_thread.join();
  std::cout << "[server]join" << std::endl;

  return 0;
}
