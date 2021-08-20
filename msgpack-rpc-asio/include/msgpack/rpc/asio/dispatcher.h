#pragma once
#include <msgpackpp.h>
#include <unordered_map>

namespace msgpack_rpc {

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

} // namespace msgpack_rpc
