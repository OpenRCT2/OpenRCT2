cmake_minimum_required(VERSION 2.6)

#
# Execute these commands in this directory:
#
# 1. mkdir build/; cd build/
#
# 2. Choose compiler:
#   Build with native toolchain:
#    cmake -DCMAKE_BUILD_TYPE=Debug ..
#
#   Build with mingw:
#    cmake -DCMAKE_TOOLCHAIN_FILE=../CMakeLists_mingw.txt -DCMAKE_BUILD_TYPE=Debug ..
#
# 3. make
#


# project title
set (PROJECT openrct2)
# OpenRCT2 resource directory
set (ORCT2_RESOURCE_DIR ${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/)

project(${PROJECT})

add_definitions(-DORCT2_RESOURCE_DIR="${ORCT2_RESOURCE_DIR}")
add_definitions(-DHAVE_CONFIG_H)
add_definitions(-DCURL_STATICLIB)
#uncomment the line bellow if you don't want to build openrct2 with twitch integration
#add_definitions(-DDISABLE_HTTP -DDISABLE_TWITCH)

set(ORCTLIBS_INCLUDE /usr/local/cross-tools/orctlibs/include) 
set(JANSSON_INCLUDE /usr/local/cross-tools/orctlibs/include/jansson) 
set(ORCTLIBS_LIB_DIR /usr/local/cross-tools/orctlibs/lib)
set(ORCTLIBS_LIB jansson curl ssl crypto ws2_32)

# include lib
include_directories("lib/")
# include speex header
include_directories("lib/libspeex/")
# add source files
file(GLOB_RECURSE ORCT2_SOURCES "src/*.c" "src/*.cpp" "lib/*.c")

if (UNIX)
	# force 32bit build for now and set necessary flags to compile code as is
	set(CMAKE_C_FLAGS "-m32 -masm=intel -std=gnu99")
	set(CMAKE_LDFLAGS_FLAGS "-m32")

	# find and include SDL2
	INCLUDE(FindPkgConfig)
	PKG_CHECK_MODULES(SDL2 REQUIRED sdl2 SDL2_ttf)
	INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
endif (UNIX)

INCLUDE_DIRECTORIES(${ORCTLIBS_INCLUDE} ${JANSSON_INCLUDE})
LINK_DIRECTORIES(${SDL2_LIBRARY_DIRS} ${ORCTLIBS_LIB_DIR})

# build as library for now, replace with add_executable
add_library(${PROJECT} SHARED ${ORCT2_SOURCES})

# install into ${CMAKE_INSTALL_PREFIX}/bin/
#install (TARGETS ${PROJECT} DESTINATION bin)

# libopenrct2.dll -> openrct2.dll
set_target_properties(${PROJECT} PROPERTIES PREFIX "")

TARGET_LINK_LIBRARIES(${PROJECT} ${SDL2_LIBRARIES} ${ORCTLIBS_LIB})

if (WIN32)
	target_link_libraries(${PROJECT} winmm.lib -limm32 -lversion -ldsound)
endif (WIN32)

