# CMAKE project for openrct2-cli (UI build of OpenRCT2)
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()

# CMake dependencies
include(FindPkgConfig)

# Third party libraries
PKG_CHECK_MODULES(GL REQUIRED gl)
PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)

# Sources
file(GLOB_RECURSE OPENRCT2_CLI_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

# Outputs
set (PROJECT openrct2)
project(${PROJECT})
add_executable(${PROJECT} ${OPENRCT2_CLI_SOURCES})

target_link_libraries(${PROJECT} "libopenrct2"
                                 ${GL_LIBRARIES}
                                 ${SDL2_LIBRARIES}
                                 ${SDL2_TTF_LIBRARIES})

# Includes
target_include_directories(${PROJECT} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/.."
                                              ${OPENGL_INCLUDE_DIRS}
                                              ${SDL2_INCLUDE_DIRS})

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_M} -std=gnu11 ${COMMON_COMPILE_OPTIONS} -Wimplicit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_M} -std=gnu++14 ${COMMON_COMPILE_OPTIONS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${TARGET_M}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS} ${PIE_FLAG}")

add_definitions(-DOPENGL_NO_LINK)
