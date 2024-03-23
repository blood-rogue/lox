cmake_minimum_required(VERSION 3.28.3)

project(lox)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_link_options(-flto=auto -Wl,--gc-sections)
add_compile_options(-Wall -Werror -Wextra -pedantic -Ofast -flto=auto -ffunction-sections -fdata-sections -g)

file(GLOB_RECURSE HEADERS ${lox_SOURCE_DIR}/include/*.h)
file(GLOB LIB_SRCS ${lox_SOURCE_DIR}/lib/*.c)
file(GLOB_RECURSE STD_SRCS ${lox_SOURCE_DIR}/std/*.c)

add_executable(lox main.c ${HEADERS} ${LIB_SRCS} ${STD_SRCS})

target_include_directories(lox PRIVATE include)
target_link_libraries(lox m readline unistring pcre2-8 z lz4 zstd snappy json-c yaml archive crypto ssl leveldb sqlite3)
