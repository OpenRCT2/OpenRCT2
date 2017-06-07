# CMAKE project for openrct2-cli (CLI-only build of OpenRCT2)
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

if (PORTABLE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif ()

# CMake dependencies
include(FindPkgConfig)

# Third party libraries (which we want to eventually remove from openrct2-cli)
PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)

# Sources
file(GLOB_RECURSE OPENRCT2_CLI_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

# Outputs
set (PROJECT openrct2-cli)
project(${PROJECT})
add_executable(${PROJECT} EXCLUDE_FROM_ALL ${OPENRCT2_CLI_SOURCES})

target_link_libraries(${PROJECT} "libopenrct2")

# Includes
target_include_directories(${PROJECT} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/.."
                                              ${SDL2_INCLUDE_DIRS})

# Compiler flags
if (FORCE32)
    set(TARGET_M "-m32")
endif ()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 ${TARGET_M}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 ${TARGET_M}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${TARGET_M}")
