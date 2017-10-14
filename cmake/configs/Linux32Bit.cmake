include(${CMAKE_CURRENT_LIST_DIR}/LinuxCommon.cmake)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../toolchains/gcc-32bit.cmake" CACHE STRING "32-bit toolchain")
set(ENABLE_RCT2 TRUE CACHE BOOL "We build with RCT2 support on 32-bit Linux")
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "No shared libraries for 32-bit")
