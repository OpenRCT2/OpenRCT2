set(CMAKE_SYSTEM_NAME Windows)

set(COMPILER_PREFIX i686-w64-mingw32)
set(CMAKE_C_COMPILER ${COMPILER_PREFIX}-gcc)
set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_COMPILER ${COMPILER_PREFIX}-c++)
set(CMAKE_CXX_FLAGS -m32)
set(CMAKE_RC_COMPILER ${COMPILER_PREFIX}-windres)
set(CMAKE_RC_FLAGS -m32)
set(CMAKE_PKGCONFIG_EXECUTABLE ${COMPILER_PREFIX}-pkg-config)
set(PKG_CONFIG_EXECUTABLE ${COMPILER_PREFIX}-pkg-config)

if(APPLE)
     set(TARGET_ENVIRONMENT /usr/local/mingw-w32-bin_i686-darwin/i686-w64-mingw32)
else()
     set(TARGET_ENVIRONMENT /usr/i686-w64-mingw32)
endif()

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH ${TARGET_ENVIRONMENT})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

# INCLUDE_DIRECTORIES(${ORCTLIBS_INCLUDE} ${JANSSON_INCLUDE})
