cmake_minimum_required(VERSION 3.11)

project(sj3)

configure_file(config.h.in config.h)

file(GLOB_RECURSE SJ3CORE_SRCS lib/sj3core/**.c)
add_library(sj3core STATIC ${SJ3CORE_SRCS})
target_include_directories(sj3core PUBLIC include/sj3common include/sj3core)
target_include_directories(sj3core PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
