#pragma once
#include <asio.hpp>
#include <asio/streambuf.hpp>
#include <msgpackpp.h>

namespace msgpack_rpc {

class session : public std::enable_shared_from_this<session> {
  asio::io_service &m_io_service;
  std::shared_ptr<::asio::ip::tcp::socket> m_socket;
  asio::streambuf m_buf;
  // on_read
  using on_read_t =
      std::function<void(const msgpackpp::bytes &, std::shared_ptr<session>)>;
  on_read_t m_on_read;

  connection_status m_connection_status;
  connection_callback_t m_connection_callback;

  error_handler_t m_error_handler;

  // must shard_ptr
  session(::asio::io_service &io_service, on_read_t on_read,
          connection_callback_t connection_callback,
          error_handler_t error_handler)
      : m_io_service(io_service), m_on_read(on_read),
        m_connection_status(connection_none),
        m_connection_callback(connection_callback),
        m_error_handler(error_handler) {}

public:
  ~session() {}

  static std::shared_ptr<session>
  create(::asio::io_service &io_service, on_read_t func = on_read_t(),
         connection_callback_t connection_callback = connection_callback_t(),
         error_handler_t error_handler = error_handler_t()) {
    return std::shared_ptr<session>(
        new session(io_service, func, connection_callback, error_handler));
  }

  connection_status get_connection_status() const {
    return m_connection_status;
  }

  std::vector<uint8_t> to_vector() const {
    auto p = asio::buffer_cast<const uint8_t *>(m_buf.data());
    return std::vector<uint8_t>(p, p + m_buf.size());
  }

  void connect_async(const ::asio::ip::tcp::endpoint &endpoint) {
    auto shared = shared_from_this();
    auto socket = std::make_shared<::asio::ip::tcp::socket>(m_io_service);
    auto on_connect =
        [shared, socket /*keep socket*/](const ::asio::error_code &error) {
          if (error) {
            if (shared->m_error_handler) {
              shared->m_error_handler(error);
            }
            shared->set_connection_status(connection_error);
          } else {
            shared->set_connection_status(connection_connected);
            shared->start_read();
          }
        };
    m_socket = socket;
    set_connection_status(connection_connecting);
    m_socket->async_connect(endpoint, on_connect);
  }

  void close() { set_connection_status(connection_none); }

  void accept(std::shared_ptr<::asio::ip::tcp::socket> socket) {
    m_socket = socket;
    set_connection_status(connection_connected);
    start_read();
  }

  void start_read() {
    if (!m_socket) {
      // closed
      // assert(false);
      return;
    }
    // auto pac = &m_pac;
    auto on_read = [shared = shared_from_this()](const asio::error_code error,
                                                 const size_t bytes_transferred) {
      // if (error && (error != ::asio::error::eof)) {
      //   if (shared->m_error_handler) {
      //     shared->m_error_handler(error);
      //   }
      //   shared->set_connection_status(connection_none);
      //   // no more read
      //   return;
      // } else {
      //   auto data = shared->to_vector();
      //   if (shared->m_on_read) {
      //     shared->m_on_read(data, shared);
      //   }
      //   shared->m_buf.consume(bytes_transferred);

      //   // read loop
      //   shared->start_read();
      // }
    };
    async_read(*m_socket, m_buf, asio::transfer_at_least(1), on_read);
  }

  void write_async(msgpackpp::bytes msg) {
    if (!m_socket) {
      assert(false);
      return;
    }
    auto shared = shared_from_this();
    auto socket = m_socket;
    auto on_write = [shared, msg,
                     socket /*keep socket*/](const ::asio::error_code &error,
                                             size_t bytes_transferred) {
      if (error) {
        if (shared->m_error_handler) {
          shared->m_error_handler(error);
        }
        shared->set_connection_status(connection_error);
      }
    };
    socket->async_write_some(asio::buffer(msg.data(), msg.size()), on_write);
  }

private:
  void set_connection_status(connection_status status) {
    if (m_connection_status == status) {
      return;
    }
    if (status == connection_none || status == connection_error) {
      if (m_socket) {
        ::asio::error_code ec;
        m_socket->shutdown(::asio::socket_base::shutdown_both, ec);
        if (!ec) {
          m_socket->close(ec);
        }
        m_socket = 0;
      }
    }
    m_connection_status = status;
    if (m_connection_callback) {
      m_connection_callback(status);
    }
  }
};

} // namespace msgpack_rpc
