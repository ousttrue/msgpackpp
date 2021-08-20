#pragma once
#include <msgpackpp.h>
#include <unordered_map>

namespace msgpack_rpc {

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

  void connect_async(const ::asio::ip::tcp::endpoint &endpoint) {
    auto c = this;
    auto on_read = [c](const msgpackpp::bytes &msg,
                       std::shared_ptr<session> session) {
      c->receive(msg, session);
    };
    m_session = session::create(m_io_service, on_read, m_connection_callback);
    m_session->connect_async(endpoint);
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
  void send_async(const msgpackpp::bytes &request, std::shared_ptr<std::promise<R>> p) {

    auto parsed = msgpackpp::parser(request);

    auto req = std::make_shared<func_call>(
        parsed.to_json(), [p](func_call *f) {
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
        if (msg[2].is_nil()) {
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

} // namespace msgpack_rpc
