#pragma once
#include <Windows.h>

namespace msgpack_rpc {

class WindowsPipeTransport {
  asio::io_context &m_context;
  HANDLE m_reader;
  HANDLE m_writer;
  std::thread m_thread;

public:
  WindowsPipeTransport(WindowsPipeTransport &&rhs)
      : m_context(rhs.m_context) {
    m_reader = rhs.m_reader;
    rhs.m_reader = nullptr;
    m_writer = rhs.m_writer;
    rhs.m_writer = nullptr;
    m_thread = std::move(rhs.m_thread);
    rhs.m_thread = {};
  }
  WindowsPipeTransport(asio::io_context &context, HANDLE reader,
                         HANDLE writer)
      : m_context(context), m_reader(reader), m_writer(writer) {}

  ~WindowsPipeTransport() {
    if (m_thread.joinable()) {
      CancelSynchronousIo(m_thread.native_handle());
      m_thread.join();
    }
  }

  void start_read(
      const std::function<void(const uint8_t *p, size_t size)> &callback) {

    assert(!m_thread.joinable());

    m_thread = std::thread([self = this, callback]() {
      char buf[1024];
      while (true) {
        DWORD bytes_read;
        BOOL success =
            ReadFile(self->m_reader, buf, static_cast<DWORD>(sizeof(buf)),
                     &bytes_read, nullptr);
        if (success && bytes_read) {
          // execute on asio_context
          auto copy = std::vector<uint8_t>(buf, buf + bytes_read);
          self->m_context.post(
              [callback, copy]() { callback(copy.data(), copy.size()); });
        } else {
          // execute on asio_context
          self->m_context.post([callback]() { callback(nullptr, 0); });
          break;
        }
      }
    });
  }

  void write_async(const std::vector<uint8_t> &bytes) {
    DWORD bytes_write;
    BOOL success =
        WriteFile(m_writer, bytes.data(), static_cast<DWORD>(bytes.size()),
                  &bytes_write, nullptr);
  }
};

} // namespace msgpack_rpc