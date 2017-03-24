# CMAKE project for openrct2-cli (CLI-only build of OpenRCT2)
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()

# Includes
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/..")

# Sources
file(GLOB_RECURSE OPENRCT2_CLI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_M} -std=gnu11 ${COMMON_COMPILE_OPTIONS} -Wimplicit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_M} -std=gnu++14 ${COMMON_COMPILE_OPTIONS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${TARGET_M}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS} ${PIE_FLAG}")

# Outputs
set (PROJECT openrct2)
project(${PROJECT})
add_executable(${PROJECT} ${OPENRCT2_CLI_SOURCES})
