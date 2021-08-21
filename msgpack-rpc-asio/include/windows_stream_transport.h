#pragma once
#include <asio/windows/stream_handle.hpp>

namespace msgpack_rpc {

class WindowsStreamTransport {
  asio::windows::stream_handle m_reader;
  asio::windows::stream_handle m_writer;

public:
  WindowsStreamTransport(asio::windows::stream_handle reader,
                         asio::windows::stream_handle writer)
      : m_reader(std::move(reader)), m_writer(std::move(writer)) {}
  asio::windows::stream_handle &Reader() { return m_reader; }
  asio::windows::stream_handle &Writer() { return m_writer; }
};

} // namespace msgpack_rpc