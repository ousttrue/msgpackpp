set(TARGET_NAME cpp17_dispatcher)
add_executable(${TARGET_NAME} main.cpp)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${TARGET_NAME} PRIVATE asio msgpackpp)
