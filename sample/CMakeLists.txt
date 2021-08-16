add_executable(sample main.cpp)
target_link_libraries(sample PRIVATE msgpack-rpc-asio)
