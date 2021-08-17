add_executable(sample1 main.cpp)
target_link_libraries(sample1 PRIVATE msgpackpp asio)
set_property(TARGET sample1 PROPERTY CXX_STANDARD 17)
