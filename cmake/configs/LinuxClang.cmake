include(${CMAKE_CURRENT_LIST_DIR}/LinuxCommon.cmake)

set(CMAKE_C_COMPILER clang CACHE STRING "Use Clang in this config")
set(CMAKE_CXX_COMPILER clang++ CACHE STRING "Use Clang in this config")
set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Build shared libs with Clang")