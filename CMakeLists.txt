cmake_minimum_required(VERSION 3.11)

project(sj4)

option(SJ4_GLOBAL "Use global variables or not" OFF)

file(GLOB_RECURSE SJ4COMMON_SRCS lib/sj4common/**.c)
add_library(sj4common STATIC ${SJ4COMMON_SRCS})
target_include_directories(sj4common PUBLIC include/sj4common)
target_include_directories(sj4common PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE SJ4CORE_SRCS lib/sj4core/**.c)
add_library(sj4core STATIC ${SJ4CORE_SRCS})
target_include_directories(sj4core PUBLIC include/sj4common include/sj4core)
target_include_directories(sj4core PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(sj4core PUBLIC sj4common)

file(GLOB_RECURSE SJ4RKCV_SRCS lib/sj4rkcv/**.c)
add_library(sj4rkcv STATIC ${SJ4RKCV_SRCS})
target_include_directories(sj4rkcv PUBLIC include/sj4common include/sj4rkcv)
target_include_directories(sj4rkcv PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(sj4rkcv PUBLIC sj4common)

file(GLOB_RECURSE SJ4MKDIC_SRCS src/sj4mkdic/**.c)
add_executable(sj4mkdic ${SJ4MKDIC_SRCS})
target_include_directories(sj4mkdic PRIVATE include/sj4common include/sj4core)

file(GLOB_RECURSE SJ4TEST_SRCS src/sj4test/**.c)
add_executable(sj4test ${SJ4TEST_SRCS})
target_link_libraries(sj4test PRIVATE sj4core)

if(SJ4_GLOBAL)
	target_compile_definitions(sj4core PUBLIC SJ4_GLOBAL)
endif()

target_compile_definitions(sj4mkdic PRIVATE UTF8 ENGLISH)
