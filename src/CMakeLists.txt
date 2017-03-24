# CMAKE project for openrct2
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()

option (CLI_ONLY "CLI only build of OpenRCT2")

include("${CMAKE_CURRENT_SOURCE_DIR}/openrct2/CMakeLists.txt" NO_POLICY_SCOPE)
if (CLI_ONLY)
    include("${CMAKE_CURRENT_SOURCE_DIR}/openrct2-cli/CMakeLists.txt" NO_POLICY_SCOPE)
else ()
    include("${CMAKE_CURRENT_SOURCE_DIR}/openrct2-ui/CMakeLists.txt" NO_POLICY_SCOPE)
endif ()
