set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Use RelWithDebInfo for the main Linux build")

include(${CMAKE_CURRENT_LIST_DIR}/LinuxCommon.cmake)

set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Build shared libs on main Linux build")