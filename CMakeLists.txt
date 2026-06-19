cmake_minimum_required(VERSION 3.11)

project(sj4)

file(GLOB_RECURSE SJ4CORE_SRCS lib/sj4core/**.c)
add_library(sj4core STATIC ${SJ4CORE_SRCS})
target_include_directories(sj4core PUBLIC include/sj4common include/sj4core)
target_include_directories(sj4core PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#file(GLOB_RECURSE SJ4RKCV_SRCS lib/sj4rkcv/**.c)
#add_library(sj4rkcv STATIC ${SJ4RKCV_SRCS})
#target_include_directories(sj4rkcv PUBLIC include/sj4common include/sj4rkcv)
#target_include_directories(sj4rkcv PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE SJ4MKDIC_SRCS src/sj4mkdic/**.c)
add_executable(sj4mkdic ${SJ4MKDIC_SRCS})
target_include_directories(sj4mkdic PRIVATE include/sj4common include/sj4core)

file(GLOB_RECURSE SJ4TEST_SRCS src/sj4test/**.c)
add_executable(sj4test ${SJ4TEST_SRCS})
target_link_libraries(sj4test PRIVATE sj4core)

#target_compile_definitions(sj4core PUBLIC SJ4_GLOBAL)

target_compile_definitions(sj4mkdic PRIVATE UTF8 ENGLISH)
