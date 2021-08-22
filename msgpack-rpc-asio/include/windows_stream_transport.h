#pragma once
#include <Windows.h>

namespace msgpack_rpc {

class WindowsStreamTransport {
  HANDLE m_reader;
  HANDLE m_writer;
  std::thread m_thread;

public:
  WindowsStreamTransport(HANDLE reader, HANDLE writer)
      : m_reader(reader), m_writer(writer) {}

  void read_async(
      const std::function<void(const uint8_t *p, size_t size)> &callback) {

    m_thread = std::thread([self = this, callback]() {
      char buf[1024];
      while (true) {
        DWORD bytes_read;
        BOOL success =
            ReadFile(self->m_reader, buf, static_cast<DWORD>(sizeof(buf)),
                     &bytes_read, nullptr);
        if (success) {
          callback((const uint8_t *)buf, bytes_read);
        } else {
          callback(nullptr, 0);
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