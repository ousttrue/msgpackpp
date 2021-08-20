set(TARGET_NAME dispatcher)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE asio msgpack-rpc-asio)
