# CMAKE project for openrct2
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/openrct2/CMakeLists.txt" NO_POLICY_SCOPE)
include("${CMAKE_CURRENT_SOURCE_DIR}/openrct2-cli/CMakeLists.txt" NO_POLICY_SCOPE)
