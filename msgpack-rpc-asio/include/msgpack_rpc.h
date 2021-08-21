#pragma once
#include <asio.hpp>
#include <asio/detached.hpp>
#include <asio/streambuf.hpp>
#include <asio/write.hpp>
#include <functional>
#include <memory>
#include <msgpackpp.h>
#include <unordered_map>

namespace msgpack_rpc {

using on_accepted_t = std::function<void(asio::ip::tcp::socket)>;
using error_handler_t = std::function<void(asio::error_code error)>;

inline std::future<void> connect_async(asio::ip::tcp::socket &socket,
                                       const asio::ip::tcp::endpoint &ep) {
  std::promise<void> p1;
  auto f1 = p1.get_future();
  // asio::ip::tcp::socket socket(context);
  socket.async_connect(ep, [p1 = std::move(p1)](asio::error_code ec) mutable {
    p1.set_value();
  });
  return f1;
}

class server {
  ::asio::io_service &m_io_service;
  ::asio::ip::tcp::acceptor m_acceptor;

  on_accepted_t m_on_accepted;
  error_handler_t m_error_handler;

public:
  server(::asio::io_service &io_service)
      : m_io_service(io_service), m_acceptor(io_service) {}

  server(::asio::io_service &io_service, on_accepted_t on_accepted)
      : m_io_service(io_service), m_acceptor(io_service),
        m_on_accepted(on_accepted) {}

  server(::asio::io_service &io_service, on_accepted_t on_accepted,
         error_handler_t error_handler)
      : m_io_service(io_service), m_acceptor(io_service),
        m_on_accepted(on_accepted), m_error_handler(error_handler) {}

  ~server() {}

  void set_on_accepted(on_accepted_t on_accepted) {
    m_on_accepted = on_accepted;
  }

  void set_error_handler(error_handler_t error_handler) {
    m_error_handler = error_handler;
  }

  void listen(::asio::ip::tcp::endpoint endpoint) {
    m_acceptor.open(endpoint.protocol());
    m_acceptor.bind(endpoint);
    m_acceptor.listen();
    start_accept();
  }

  void stop() { m_acceptor.close(); }

private:
  void start_accept() {
    auto on_accept = [self = this](const ::asio::error_code &error,
                                   asio::ip::tcp::socket socket) {
      if (error) {
        if (self->m_error_handler) {
          self->m_error_handler(error);
        } else {
          throw error;
        }
      } else {
        if (!self->m_on_accepted) {
          throw std::runtime_error("m_on_accepted");
        }
        self->m_on_accepted(std::move(socket));
      }
      // next
      self->start_accept();
    };
    m_acceptor.async_accept(on_accept);
  }
};

enum class error_code {
  success,
  error_dispatcher_no_handler,
  error_params_not_array,
  error_params_too_many,
  error_params_not_enough,
  error_params_convert,
  error_not_implemented,
  error_self_pointer_is_null,
  no_request_for_response,
  invalid_rpc,
};

struct msgerror : std::runtime_error {
  error_code code;

public:
  msgerror(error_code code, const std::string &msg = "")
      : std::runtime_error(msg), code(code) {}
};

class SocketTransport {
  asio::ip::tcp::socket m_socket;

public:
  SocketTransport(asio::ip::tcp::socket socket) : m_socket(std::move(socket)) {}
  asio::ip::tcp::socket &Reader() { return m_socket; }
  asio::ip::tcp::socket &Writer() { return m_socket; }
};

template <typename Transport>
class session : public std::enable_shared_from_this<session<Transport>> {
  Transport m_transport;

  // on_read
  asio::streambuf m_buf;
  using on_read_t = std::function<void(const std::vector<uint8_t> &)>;
  on_read_t m_on_read;

  error_handler_t m_error_handler;

  // force shard_ptr
  session(Transport transport, on_read_t on_read, error_handler_t error_handler)
      : m_transport(std::move(transport)), m_on_read(on_read),
        m_error_handler(error_handler) {}

public:
  ~session() {}

  template <typename T>
  static std::shared_ptr<session<T>>
  create(T t, on_read_t func = on_read_t(),
         error_handler_t error_handler = error_handler_t()) {
    auto s = std::shared_ptr<session<T>>(
        new session<T>(std::move(t), func, error_handler));
    s->start_read();
    return s;
  }

  Transport &transport() { return m_transport; }

private:
  std::vector<uint8_t> to_vector() const {
    auto p = asio::buffer_cast<const uint8_t *>(m_buf.data());
    return std::vector<uint8_t>(p, p + m_buf.size());
  }

  void start_read() {
    auto on_read =
        [shared = this->shared_from_this()](const asio::error_code error,
                                            const size_t bytes_transferred) {
          if (error) {
            if (shared->m_error_handler) {
              shared->m_error_handler(error);
            }
            // no more read
            return;
          } else {
            auto data = shared->to_vector();
            assert(!data.empty());
            if (shared->m_on_read) {
              shared->m_on_read(data);
            }
            shared->m_buf.consume(bytes_transferred);

            // read loop
            shared->start_read();
          }
        };
    async_read(m_transport.Reader(), m_buf, asio::transfer_at_least(1),
               on_read);
  }

public:
  void write_async(const std::vector<uint8_t> &buffer) {
    auto p = std::make_shared<std::vector<uint8_t>>();
    *p = buffer;
    asio::async_write(
        m_transport.Writer(), asio::buffer(*p),
        [p /* keep write buffer */](asio::error_code ec, size_t size) {
          //
          auto a = 0;
        });
  }
};

class func_call_error : public std::runtime_error {
public:
  func_call_error(const std::string &msg) : std::runtime_error(msg) {}
};

class func_call {
public:
  enum STATUS_TYPE {
    STATUS_WAIT,
    STATUS_RECEIVED,
    STATUS_ERROR,
  };

private:
  STATUS_TYPE m_status;
  msgpackpp::bytes m_error;
  msgpackpp::bytes m_result;
  std::string m_request;
  std::mutex m_mutex;
  std::condition_variable_any m_cond;

  using on_result_func = std::function<void(func_call *)>;
  on_result_func m_callback;

public:
  func_call(const std::string &s, const on_result_func &callback)
      : m_status(STATUS_WAIT), m_request(s), m_callback(callback) {}

  void set_result(const msgpackpp::parser &result) {
    if (m_status != STATUS_WAIT) {
      throw func_call_error("already finishded");
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    m_status = STATUS_RECEIVED;
    m_result = result.copy_bytes();
    notify();
  }

  void set_error(const msgpackpp::parser &error) {
    if (m_status != STATUS_WAIT) {
      throw func_call_error("already finishded");
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    m_status = STATUS_ERROR;
    m_error = error.copy_bytes();
    notify();
  }

  bool is_error() const { return m_status == STATUS_ERROR; }

  // blocking
  func_call &sync() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_status == STATUS_WAIT) {
      m_cond.wait(m_mutex);
    }
    return *this;
  }

  const msgpackpp::bytes &get_result() const {
    if (m_status == STATUS_RECEIVED) {
      return m_result;
    } else {
      throw func_call_error("not ready");
    }
  }

  std::string string() const {
    std::stringstream ss;
    ss << m_request << " = ";
    switch (m_status) {
    case func_call::STATUS_WAIT:
      ss << "?";
      break;
    case func_call::STATUS_RECEIVED:
      ss << msgpackpp::parser(m_result);
      break;
    case func_call::STATUS_ERROR:
      ss << "!";
      break;
    default:
      ss << "!?";
      break;
    }

    return ss.str();
  }

private:
  void notify() {
    if (m_callback) {
      m_callback(this);
    }
    m_cond.notify_all();
  }
};

inline std::ostream &operator<<(std::ostream &os, const func_call &request) {
  os << request.string();
  return os;
}

template <typename Transport> class rpc_base {
  std::unordered_map<std::string, msgpackpp::procedurecall> m_handlerMap;
  int m_next_msg_id = 1;
  std::unordered_map<int, std::shared_ptr<func_call>> m_request_map;
  std::shared_ptr<session<Transport>> m_session;

public:
  void attach(Transport t) {
    // start socket read
    m_session = msgpack_rpc::session<Transport>::create(
        std::move(t),
        [self = this](const auto &data) mutable { self->on_receive(data); });
  }

  std::shared_ptr<session<Transport>> session() const { return m_session; }

public:
  template <typename F>
  void add_handler(const std::string &method, const F &f) {
    auto proc = msgpackpp::make_procedurecall(f);
    m_handlerMap.insert(std::make_pair(method, proc));
  }

  template <typename C, typename R, typename... AS>
  void add_bind(const std::string &method, C *c, R (C::*f)(AS...)) {
    auto proc = msgpackpp::make_methodcall(c, f);
    m_handlerMap.insert(std::make_pair(method, proc));
  }

  void on_receive(const msgpackpp::bytes &bytes) {
    auto msg = msgpackpp::parser(bytes);
    auto type = msg[0].get_number<int>();
    switch (type) {
    case 0:
      // request [0, id, method, args]
      {
        auto id = msg[1].get_number<int>();
        try {
          auto method = msg[2].get_string();
          // execute callback
          auto result =
              on_request(std::string(method.begin(), method.end()), msg[3]);
          auto response = msgpackpp::make_rpc_response_packed(id, "", result);
          m_session->write_async(response);
        } catch (const msgerror &ex) {
          auto response =
              msgpackpp::make_rpc_response(id, ex.what(), msgpackpp::nil);
          m_session->write_async(response);
        }
      }
      break;

    case 1: {
      // response
      auto id = msg[1].get_number<int>();
      auto found = m_request_map.find(id);
      if (found != m_request_map.end()) {
        if (msg[2].is_nil() ||
            msg[2].is_string() && msg[2].get_string() == "") {
          found->second->set_result(msg[3]);
        } else if (msg[2].is_bool()) {
          bool isError = msg[2].get_bool();
          if (isError) {
            found->second->set_error(msg[3]);
          } else {
            found->second->set_result(msg[3]);
          }
        }
      } else {
        throw msgerror(error_code::no_request_for_response);
      }
    } break;

    case 2:
      throw msgerror(error_code::error_not_implemented);

    default:
      throw msgerror(error_code::invalid_rpc);
    }
  }

private:
  msgpackpp::bytes on_request(const std::string &method_name,
                              const msgpackpp::parser &params) {
    auto found = m_handlerMap.find(method_name);
    if (found == m_handlerMap.end()) {
      throw msgerror(error_code::error_dispatcher_no_handler);
    } else {
      auto func = found->second;
      return func(params);
    }
  }

public:
  template <typename... ARGS>
  std::future<std::vector<uint8_t>> call(const std::string &method,
                                         ARGS... args) {

    auto p = std::make_shared<std::promise<std::vector<uint8_t>>>();
    auto f = p->get_future();

    auto request =
        msgpackpp::make_rpc_request(m_next_msg_id++, method, args...);

    send_request_async(request, p);

    return f;
  }

private:
  std::vector<uint8_t> m_write_buffer;

  void
  send_request_async(const msgpackpp::bytes &request,
                     std::shared_ptr<std::promise<std::vector<uint8_t>>> p) {

    auto parsed = msgpackpp::parser(request);

    auto req = std::make_shared<func_call>(
        parsed.to_json(),
        [p](func_call *f) mutable { p->set_value(f->get_result()); });
    m_request_map.insert(std::make_pair(parsed[1].get_number<int>(), req));

    m_session->write_async(request);
  }
};
using rpc = rpc_base<SocketTransport>;

} // namespace msgpack_rpc
