add_library(
  msgpack STATIC
  src/objectc.cpp
  src/unpack.cpp
  src/version.cpp
  src/vrefbuffer.cpp
  src/zone.cpp
  src/object.cpp
  src/gcc_atomic.cpp)
target_compile_definitions(msgpack PRIVATE _WIN32_WINNT=0x0501 UNICODE)
target_compile_options(msgpack PUBLIC /wd4476 /wd4474 /wd4267)
target_include_directories(msgpack PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
