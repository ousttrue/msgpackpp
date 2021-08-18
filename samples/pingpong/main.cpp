#include "asio/read.hpp"
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/experimental/as_tuple.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/streambuf.hpp>
#include <asio/system_timer.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>
#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <thread>

using namespace std::chrono;

constexpr auto use_nothrow_awaitable =
    asio::experimental::as_tuple(asio::use_awaitable);

static std::string to_string(const asio::streambuf &buf) {
  auto p = asio::buffer_cast<const char *>(buf.data());
  return std::string(p, p + buf.size());
}

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
    auto [e1, read_size] = co_await asio::async_read(
        socket, buf, asio::transfer_at_least(1), use_nothrow_awaitable);

    auto pong = to_string(buf);
    std::cout << "[server]ping: " << pong << std::endl;
    pong += "pong";
    auto [e2, write_size] = co_await asio::async_write(
        socket, asio::buffer(pong), use_nothrow_awaitable);
    std::cout << "[server]pong: " << write_size << std::endl;
  }
};

std::future<void> spawn(asio::io_context &context,
                        const std::function<asio::awaitable<void>()> &co) {
  std::promise<void> p;
  auto f = p.get_future();
  auto task = [p = std::move(p), co]() mutable -> asio::awaitable<void> {
    co_await co();
    p.set_value();
  };

  auto ex = context.get_executor();
  asio::co_spawn(ex, std::move(task), asio::detached);

  return f;
}

template <typename R>
std::future<R> spawn(asio::io_context &context,
                     const std::function<asio::awaitable<R>()> &co) {
  std::promise<R> p;
  auto f = p.get_future();
  auto task = [p = std::move(p), co]() mutable -> asio::awaitable<void> {
    auto value = co_await co();
    p.set_value(value);
  };

  auto ex = context.get_executor();
  asio::co_spawn(ex, std::move(task), asio::detached);

  return f;
}

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
  // client client(client_context);
  auto co = [&context = client_context, ep]() -> asio::awaitable<std::string> {
    std::cout << "[client]wait 1000ms..." << std::endl;
    asio::system_timer timer(context);
    timer.expires_from_now(1000ms);
    co_await timer.async_wait(asio::use_awaitable);

    std::cout << "[client]connect: " << ep << "..." << std::endl;
    asio::ip::tcp::socket socket(context);
    co_await socket.async_connect(ep, use_nothrow_awaitable);
    std::cout << "[client]connected" << std::endl;

    std::cout << "[client]ping..." << std::endl;
    std::string ping("ping");
    auto [e1, write_size] = co_await asio::async_write(
        socket, asio::buffer(ping), use_nothrow_awaitable);
    assert(write_size == 4);

    std::cout << "[client]read..." << std::endl;
    asio::streambuf buf;
    auto [e2, read_size] = co_await asio::async_read(
        socket, buf, asio::transfer_at_least(1), use_nothrow_awaitable);
    co_return to_string(buf);
  };
  auto result = spawn<std::string>(client_context, co);
  client_context.run();
  auto pong = result.get();
  std::cout << "pong: " << pong << std::endl;

  // stop asio
  server_context.stop();
  server_thread.join();
  std::cout << "[server]join" << std::endl;

  return 0;
}
