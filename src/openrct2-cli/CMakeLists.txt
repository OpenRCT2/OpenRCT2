# CMAKE project for openrct2-cli (CLI-only build of OpenRCT2)
cmake_minimum_required(VERSION 3.1)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

# Sources
file(GLOB_RECURSE OPENRCT2_CLI_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

# Outputs
set (PROJECT openrct2-cli)
project(${PROJECT} CXX)
add_executable(${PROJECT} ${OPENRCT2_CLI_SOURCES})

target_link_libraries(${PROJECT} "libopenrct2")

# Needed for interactive console
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT} Threads::Threads)

if (APPLE)
    target_link_libraries(${PROJECT} "-framework Cocoa")
endif ()

# Includes
target_include_directories(${PROJECT} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
