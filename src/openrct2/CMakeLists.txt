# CMAKE project for libopenrct2 (core OpenRCT2 component)
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()

# CMake dependencies
include(FindPkgConfig)

# Third party libraries
PKG_CHECK_MODULES(JANSSON REQUIRED jansson>=2.5)
PKG_CHECK_MODULES(LIBCURL REQUIRED libcurl)
PKG_CHECK_MODULES(LIBZIP REQUIRED libzip>=1.0)
PKG_CHECK_MODULES(SSL REQUIRED openssl>=1.0.0)
PKG_CHECK_MODULES(PNG libpng>=1.6)
PKG_CHECK_MODULES(ZLIB REQUIRED zlib)

# Third party libraries (which we want to eventually remove from libopenrct2)
PKG_CHECK_MODULES(FONTCONFIG REQUIRED fontconfig)
PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)
PKG_CHECK_MODULES(SDL2_TTF REQUIRED SDL2_ttf)
PKG_CHECK_MODULES(SPEEX REQUIRED speexdsp)
PKG_CHECK_MODULES(GL REQUIRED gl)

# Sources
file(GLOB_RECURSE OPENRCT2_CORE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.h"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

# Outputs
set(PROJECT libopenrct2)
project(${PROJECT})
add_library(${PROJECT} SHARED ${OPENRCT2_CORE_SOURCES})
set_target_properties(${PROJECT} PROPERTIES PREFIX "")

# Includes
target_include_directories(${PROJECT} SYSTEM PRIVATE ${LIBZIP_INCLUDE_DIRS})
target_include_directories(${PROJECT} PRIVATE ${SDL2_INCLUDE_DIRS}
                                              ${LIBCURL_INCLUDE_DIRS}
                                              ${JANSSON_INCLUDE_DIRS}
                                              ${SPEEX_INCLUDE_DIRS}
                                              ${PNG_INCLUDE_DIRS}
                                              ${ZLIB_INCLUDE_DIRS}
                                              ${BREAKPAD_INCLUDE_DIR}
                                              ${SSL_INCLUDE_DIRS}
                                              ${FONTCONFIG_INCLUDE_DIRS})

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_M} -std=gnu11 ${COMMON_COMPILE_OPTIONS} -Wimplicit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_M} -std=gnu++14 ${COMMON_COMPILE_OPTIONS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${TARGET_M}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS} ${PIE_FLAG}")
