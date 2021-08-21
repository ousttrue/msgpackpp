set(TARGET_NAME dispatch_20)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE asio msgpack-rpc-asio)
