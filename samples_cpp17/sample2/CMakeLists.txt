set(TARGET_NAME sample2)
add_executable(${TARGET_NAME} main.cpp)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
target_link_libraries(${TARGET_NAME} msgpack-rpc-asio)
