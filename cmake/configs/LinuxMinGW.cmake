set(CMAKE_BUILD_TYPE Debug CACHE STRING "We build Debug for MinGW")

include(${CMAKE_CURRENT_LIST_DIR}/LinuxCommon.cmake)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../toolchains/MinGW.cmake" CACHE STRING "32-bit MinGW toolchain")
set(BUILD_TESTING FALSE CACHE BOOL "Tests disabled because we are MinGW")
set(BUILD_SHARED_LIBS TRUE BOOL "Build shared libraries on MinGW")
