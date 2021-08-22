project(msgpackpp)
cmake_minimum_required(VERSION 3.14)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  add_compile_options(-Wall -Wno-unknown-pragmas -Wno-switch)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
  add_compile_options(/source-charset:utf-8)
endif()

subdirs(_external msgpackpp samples_cpp17 samples_cpp20 test)
