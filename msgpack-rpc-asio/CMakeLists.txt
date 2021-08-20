set(TARGET_NAME msgpack-rpc-asio)
add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME}
                           INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${TARGET_NAME} INTERFACE msgpackpp asio)
