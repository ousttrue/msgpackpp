#include <asio/experimental/as_tuple.hpp>
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/use_awaitable.hpp>
#include <iostream>

constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);

class server {

  asio::io_context _io_context;
  asio::ip::tcp::acceptor _acceptor;

public:
  server() : _acceptor(_io_context) {}
  ~server() {}

  void listen(const asio::ip::tcp::endpoint &ep) {
    _acceptor.open(ep.protocol());
    _acceptor.bind(ep);
    _acceptor.listen();

    auto ex = _io_context.get_executor();
    asio::co_spawn(ex, accept_loop(), asio::detached);
  }

  asio::awaitable<void> accept_loop() {

    while (true) {

      auto [e, client] =
          co_await _acceptor.async_accept(use_nothrow_awaitable);
      if (e) {
        break;
      }

      //   auto ex = client.get_executor();
      //   co_spawn(ex, proxy(std::move(client), target), detached);
    }
  }
};

int main(int argc, char **argv) {

  std::cout << "start..." << std::endl;
  std::cout << _MSC_VER << std::endl;
  std::cout << _MSVC_LANG << std::endl;
  return 0;
}
