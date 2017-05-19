# CMAKE project for openrct2-cli (CLI-only build of OpenRCT2)
cmake_minimum_required(VERSION 3.0)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

# Sources
file(GLOB_RECURSE OPENRCT2_CLI_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

# Outputs
add_library(openrct2cli OBJECT ${OPENRCT2_CLI_SOURCES})

# Includes
target_include_directories(openrct2cli PRIVATE "${CMAKE_CURRENT_LIST_DIR}/.."
                                                ${SDL2_INCLUDE_DIRS})
