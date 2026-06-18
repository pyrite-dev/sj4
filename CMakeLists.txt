cmake_minimum_required(VERSION 3.11)

project(sj3)

option(UTF8 "UTF8" OFF)

file(GLOB_RECURSE SJ3CORE_SRCS lib/sj3core/**.c)
add_library(sj3core STATIC ${SJ3CORE_SRCS})
target_include_directories(sj3core PUBLIC include/sj3common include/sj3core)
target_include_directories(sj3core PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE SJ3MKDIC_SRCS src/sj3mkdic/**.c)
add_executable(sj3mkdic ${SJ3MKDIC_SRCS})
target_include_directories(sj3mkdic PRIVATE include/sj3common include/sj3core)

if(UTF8)
	target_compile_definitions(sj3core PRIVATE UTF8)
	target_compile_definitions(sj3mkdic PRIVATE UTF8)
endif()
