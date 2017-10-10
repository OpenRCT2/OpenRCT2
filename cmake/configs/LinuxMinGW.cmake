set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../toolchains/MinGW.cmake" CACHE STRING "32-bit MinGW toolchain")
set(BUILD_TESTING FALSE CACHE BOOL "Tests disabled because we are MinGW")