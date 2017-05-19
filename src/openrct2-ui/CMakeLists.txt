# CMAKE project for openrct2-cli (UI build of OpenRCT2)
cmake_minimum_required(VERSION 3.0)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

# Sources
file(GLOB_RECURSE OPENRCT2_UI_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

if (APPLE)
    file(GLOB_RECURSE OPENRCT2_UI_M_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.m")
    set_source_files_properties(${OPENRCT2_UI_M_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c -fmodules")

    file(GLOB_RECURSE OPENRCT2_UI_MM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    set_source_files_properties(${OPENRCT2_UI_MM_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c++ -fmodules")
endif ()

# Outputs
add_library(openrct2ui OBJECT ${OPENRCT2_UI_SOURCES} ${OPENRCT2_UI_M_SOURCES} ${OPENRCT2_UI_MM_SOURCES})

# Includes
target_include_directories(openrct2ui PRIVATE "${CMAKE_CURRENT_LIST_DIR}/.."
                                              ${SDL2_INCLUDE_DIRS})
