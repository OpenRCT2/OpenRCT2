# CMAKE project for openrct2
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()
set(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")

option(FORCE32 "Force 32-bit build. It will add `-m32` to compiler flags.")
option(CLI_ONLY "CLI only build of OpenRCT2")

# Define current git branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${ROOT_DIR}
    OUTPUT_VARIABLE OPENRCT2_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
add_definitions(-DOPENRCT2_BRANCH="${OPENRCT2_BRANCH}")

# Define commit hash
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${ROOT_DIR}
    OUTPUT_VARIABLE OPENRCT2_COMMIT_SHA1
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
add_definitions(-DOPENRCT2_COMMIT_SHA1="${OPENRCT2_COMMIT_SHA1}")

# Define short commit hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${ROOT_DIR}
    OUTPUT_VARIABLE OPENRCT2_COMMIT_SHA1_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Tagged builds are not meant to display commit info
if (NOT OPENRCT2_COMMIT_SHA1_SHORT STREQUAL "HEAD")
    add_definitions(-DOPENRCT2_COMMIT_SHA1_SHORT="${OPENRCT2_COMMIT_SHA1_SHORT}")
endif()

# Include sub-projects
include("${CMAKE_CURRENT_SOURCE_DIR}/openrct2/CMakeLists.txt" NO_POLICY_SCOPE)
if (CLI_ONLY)
    include("${CMAKE_CURRENT_SOURCE_DIR}/openrct2-cli/CMakeLists.txt" NO_POLICY_SCOPE)
else ()
    include("${CMAKE_CURRENT_SOURCE_DIR}/openrct2-ui/CMakeLists.txt" NO_POLICY_SCOPE)
endif ()

# g2
add_custom_command(
    OUTPUT g2.dat
    COMMAND ./openrct2 sprite build ${CMAKE_BINARY_DIR}/g2.dat ${ROOT_DIR}/resources/g2/
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_target(g2 DEPENDS ${PROJECT} g2.dat)
