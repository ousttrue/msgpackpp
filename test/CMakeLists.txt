add_executable(
  test
  main.cpp
  call.cpp
  # client.cpp
  dispatcher.cpp
  # error.cpp
  # pipeline.cpp
  # request.cpp
)
target_link_libraries(test PRIVATE catch msgpack-rpc-asio)
