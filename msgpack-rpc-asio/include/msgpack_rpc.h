#pragma once
#include <asio.hpp>
#include <asio/streambuf.hpp>
#include <msgpackpp.h>
#include <unordered_map>

namespace msgpack_rpc {

enum error_code {
  success,
  error_dispatcher_no_handler,
  error_params_not_array,
  error_params_too_many,
  error_params_not_enough,
  error_params_convert,
  error_not_implemented,
  error_self_pointer_is_null,
};
typedef std::function<void(asio::error_code error)> error_handler_t;

enum connection_status {
  connection_none,
  connection_connecting,
  connection_connected,
  connection_error,
};
using connection_callback_t = std::function<void(connection_status)>;

struct msgerror : std::runtime_error {
  error_code code;

public:
  msgerror(const std::string &msg, error_code code)
      : std::runtime_error(msg), code(code) {}
};

class session : public std::enable_shared_from_this<session> {
  asio::io_service &m_io_service;
  std::shared_ptr<::asio::ip::tcp::socket> m_socket;
  asio::streambuf m_buf;
  // on_read
  using on_read_t = std::function<void(const std::vector<uint8_t> &,
                                       std::shared_ptr<session>)>;
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
    auto on_read = [shared =
                        shared_from_this()](const asio::error_code error,
                                            const size_t bytes_transferred) {
      if (error) {
        if (shared->m_error_handler) {
          shared->m_error_handler(error);
        }
        shared->set_connection_status(connection_none);
        // no more read
        return;
      } else {
        auto data = shared->to_vector();
        assert(!data.empty());
        if (shared->m_on_read) {
          shared->m_on_read(data, shared);
        }
        shared->m_buf.consume(bytes_transferred);

        // read loop
        shared->start_read();
      }
    };
    async_read(*m_socket, m_buf, asio::transfer_at_least(1), on_read);
  }

  void write_async(std::vector<uint8_t> msg) {
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

class dispatcher {
  std::unordered_map<std::string, msgpackpp::procedurecall> m_handlerMap;
  std::shared_ptr<std::thread> m_thread;

public:
  dispatcher() {}

  ~dispatcher() {}

  template <typename F>
  void add_handler(const std::string &method, const F &f) {
    auto proc = msgpackpp::make_procedurecall(f);
    m_handlerMap.insert(std::make_pair(method, proc));
  }

  void dispatch(const msgpackpp::parser &msg,
                std::shared_ptr<session> session) {
    // extract msgpack request
    auto id = msg[1].get_number<int>();
    try {
      auto method = msg[2].get_string();
      // execute callback
      auto result = request(std::string(method.begin(), method.end()), msg[3]);
      auto response = msgpackpp::make_rpc_response_packed(id, "", result);
      // send
      session->write_async(response);
    } catch (msgerror ex) {
      auto response =
          msgpackpp::make_rpc_response(id, ex.what(), msgpackpp::nil);
      session->write_async(response);
    }
  }

private:
  msgpackpp::bytes request(const std::string &method_name,
                           const msgpackpp::parser &params) {
    auto found = m_handlerMap.find(method_name);
    if (found == m_handlerMap.end()) {
      throw msgerror("no handler", error_dispatcher_no_handler);
    } else {
      auto func = found->second;
      return func(params);
    }
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

class client_error : public std::runtime_error {
public:
  client_error(const std::string &msg) : std::runtime_error(msg) {}
};

class client {
  ::asio::io_service &m_io_service;
  int m_next_msg_id = 1;

  std::shared_ptr<session> m_session;
  std::unordered_map<int, std::shared_ptr<func_call>> m_request_map;

  connection_callback_t m_connection_callback;
  error_handler_t m_error_handler;

public:
  client(::asio::io_service &io_service,
         connection_callback_t connection_callback = connection_callback_t(),
         error_handler_t error_handler = error_handler_t())
      : m_io_service(io_service), m_connection_callback(connection_callback),
        m_error_handler(error_handler) {}

  std::future<void> connect_async(const ::asio::ip::tcp::endpoint &endpoint) {
    auto p = std::make_shared<std::promise<void>>();
    auto f = p->get_future();

    auto c = this;
    auto on_read = [c](const msgpackpp::bytes &msg,
                       std::shared_ptr<session> session) {
      c->receive(msg, session);
    };
    auto on_connected = [p](connection_status s) {
      if (s == connection_status::connection_connected) {
        p->set_value();
      } else if (s == connection_error) {
        // p->set_exception();
      }
    };
    m_session = session::create(m_io_service, on_read, on_connected);
    m_session->connect_async(endpoint);

    return f;
  }

  template <typename R, typename... ARGS>
  std::future<R> call(const std::string &method, ARGS... args) {

    auto p = std::make_shared<std::promise<R>>();
    auto f = p->get_future();

    auto request =
        msgpackpp::make_rpc_request(m_next_msg_id++, method, args...);
    send_async(request, p);

    return f;
  }

  void close() { m_session->close(); }

  bool is_connect() {
    return m_session->get_connection_status() == connection_connected;
  }

private:
  template <typename R>
  void send_async(const msgpackpp::bytes &request,
                  std::shared_ptr<std::promise<R>> p) {

    auto parsed = msgpackpp::parser(request);

    auto req = std::make_shared<func_call>(parsed.to_json(), [p](func_call *f) {
      R value;
      msgpackpp::parser(f->get_result()) >> value;
      p->set_value(value);
    });
    m_request_map.insert(std::make_pair(parsed[1].get_number<int>(), req));

    m_session->write_async(request);
  }

private:
  void receive(const msgpackpp::bytes &bytes,
               std::shared_ptr<session> session) {
    auto msg = msgpackpp::parser(bytes);
    auto type = msg[0].get_number<int>();
    switch (type) {
    case 0:
      throw std::runtime_error("not implemented");

    case 1: {
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
        throw client_error("no request for response");
      }
    } break;

    case 2:
      throw std::runtime_error("not implemented");

    default:
      throw client_error("rpc type error");
    }
  }
};

class server {
  ::asio::io_service &m_io_service;
  ::asio::ip::tcp::acceptor m_acceptor;
  // std::list<std::weak_ptr<session>> m_sessions;
  using on_receive_t = std::function<void(const msgpackpp::bytes &msg,
                                          std::shared_ptr<session> session)>;

  on_receive_t m_on_receive;

  error_handler_t m_error_handler;

public:
  server(::asio::io_service &io_service)
      : m_io_service(io_service), m_acceptor(io_service) {}

  server(::asio::io_service &io_service, on_receive_t on_receive)
      : m_io_service(io_service), m_acceptor(io_service),
        m_on_receive(on_receive) {}

  server(::asio::io_service &io_service, on_receive_t on_receive,
         error_handler_t error_handler)
      : m_io_service(io_service), m_acceptor(io_service),
        m_on_receive(on_receive), m_error_handler(error_handler) {}

  ~server() {}

  void set_on_receive(on_receive_t on_receive) { m_on_receive = on_receive; }

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
    auto new_connection = session::create(
        m_io_service, m_on_receive, connection_callback_t(), m_error_handler);
    auto socket = std::make_shared<::asio::ip::tcp::socket>(m_io_service);

    // m_sessions.push_back(new_connection);

    auto self = this;
    auto on_accept = [self, new_connection,
                      socket /*keep socket*/](const ::asio::error_code &error) {
      if (error) {
        if (self->m_error_handler) {
          self->m_error_handler(error);
        } else {
          throw error;
        }
      } else {
        std::cout << "[server]accepted: " << std::endl;
        new_connection->accept(socket);
        // next
        self->start_accept();
      }
    };
    m_acceptor.async_accept(*socket.get(), on_accept);
  }
};

} // namespace msgpack_rpc
