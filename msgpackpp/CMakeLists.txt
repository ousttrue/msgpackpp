set(TARGET_NAME msgpackpp)
add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME}
                           INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_options(${TARGET_NAME} INTERFACE /wd4267)
