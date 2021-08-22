set(TARGET_NAME cpp17_dispatcher)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE asio msgpackpp)
