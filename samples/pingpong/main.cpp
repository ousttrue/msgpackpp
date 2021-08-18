#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/experimental/as_tuple.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/system_timer.hpp>
#include <asio/use_awaitable.hpp>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
using namespace std::chrono;

constexpr auto use_nothrow_awaitable =
    asio::experimental::as_tuple(asio::use_awaitable);

class server {

  asio::io_context &_context;
  asio::ip::tcp::acceptor _acceptor;

public:
  server(asio::io_context &context) : _context(context), _acceptor(context) {}
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

      auto [e, client] = co_await _acceptor.async_accept(use_nothrow_awaitable);
      if (e) {
        break;
      }
    }
  }
};

class client {

  asio::ip::tcp::socket _socket;

public:
  asio::io_context &_context;
  client(asio::io_context &context)
      : _socket(context.get_executor()), _context(context) {}

  asio::awaitable<void> connect_async(const asio::ip::tcp::endpoint &ep) {
    auto [e] = co_await _socket.async_connect(ep, use_nothrow_awaitable);
  }

  template <typename R>
  std::future<R> spawn(const std::function<asio::awaitable<R>()> &co) {
    std::promise<R> p;
    auto f = p.get_future();
    auto task = [p = std::move(p), co]() mutable -> asio::awaitable<void> {
      co_await co();
      p.set_value();
    };

    auto ex = _context.get_executor();
    asio::co_spawn(ex, std::move(task), asio::detached);

    return f;
  }
};

const auto PORT = 8070;

int main(int argc, char **argv) {

  auto ep = asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"),
                                    PORT);

  // server
  asio::io_context server_context;
  server server(server_context);
  server.listen(ep);
  std::thread server_thread([&server_context]() { server_context.run(); });

  // client
  asio::io_context client_context;
  client client(client_context);
  auto co = [&client, ep]() -> asio::awaitable<void> {
    std::cout << "[client]wait 1000ms..." << std::endl;
    asio::system_timer timer(client._context);
    timer.expires_from_now(1000ms);
    co_await timer.async_wait(asio::use_awaitable);

    std::cout << "[client]connect: " << ep << "..." << std::endl;
    co_await client.connect_async(ep);
    std::cout << "[client]connected" << std::endl;
  };
  auto connected = client.spawn<void>(co);
  std::thread client_thread([&client_context]() { client_context.run(); });
  connected.get();

  // stop asio
  server_context.stop();
  client_thread.join();
  server_thread.join();
  std::cout << "join" << std::endl;

  return 0;
}
