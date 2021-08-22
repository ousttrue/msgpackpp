set(TARGET_NAME test)
add_executable(
  ${TARGET_NAME}
  main.cpp
  # msgpack
  msgpack.cpp
  serialize.cpp
  rpc.cpp
  stream.cpp
  request.cpp
  # rpc
  call.cpp
  client.cpp
  dispatcher.cpp
  error.cpp
  pipeline.cpp)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 17)

target_link_libraries(${TARGET_NAME} PRIVATE catch msgpackpp asio)
