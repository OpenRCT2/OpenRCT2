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

INCLUDE(FindPkgConfig)

option(DISABLE_HTTP_TWITCH "Disable HTTP and Twitch support.")
if (DISABLE_HTTP_TWITCH)
	add_definitions(-DDISABLE_HTTP -DDISABLE_TWITCH)
else (DISABLE_HTTP_TWITCH)
	PKG_CHECK_MODULES(LIBCURL REQUIRED libcurl)
	PKG_CHECK_MODULES(JANSSON REQUIRED jansson)
	SET(HTTPLIBS ${LIBCURL_LIBRARIES} ${JANSSON_LIBRARIES})
	if (WIN32)
		SET(HTTPLIBS ${HTTPLIBS} ssl crypto winmm.lib ws2_32)
	endif (WIN32)
endif (DISABLE_HTTP_TWITCH)

option(DISABLE_NETWORK "Disable multiplayer functionality. Mainly for testing.")
if (DISABLE_NETWORK)
	add_definitions(-DDISABLE_NETWORK)
else (DISABLE_NETWORK)
	if (WIN32)
		SET(NETWORKLIBS ${NETWORKLIBS} ws2_32)
	endif (WIN32)
endif (DISABLE_NETWORK)

set(DEBUG_LEVEL 0 CACHE STRING "Select debug level for compilation. Use value in range 0–3.")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG=${DEBUG_LEVEL}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG=${DEBUG_LEVEL}")

# include lib
include_directories("lib/")
# add source files
file(GLOB_RECURSE ORCT2_SOURCES "src/*.c" "src/*.cpp" "lib/argparse/*.c" "lib/cutest/*.c" "lib/lodepng/*.c")

if (UNIX)
	# force 32bit build for now and set necessary flags to compile code as is
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -std=gnu99 -fno-omit-frame-pointer")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -std=gnu++11 -fno-omit-frame-pointer")
	set(CMAKE_SHARED_LINKER_FLAGS "-m32")
	set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})
endif (UNIX)

# find and include SDL2
PKG_CHECK_MODULES(SDL2 REQUIRED sdl2 SDL2_ttf)

# speex v1.1.15 is supplied in our zipped library, but distributions provide
# updated version, with required functions extracted out to libspeexdsp.
# This largely takes care of the problem
if (WIN32)
	include_directories("lib/libspeex/")
	file(GLOB_RECURSE SPEEX_SOURCES "lib/libspeex/*.c")
else (WIN32)
	PKG_CHECK_MODULES(SPEEX REQUIRED speexdsp)
endif (WIN32)

# Include libdl for dlopen
if (UNIX)
	set(DLLIB dl)
endif (UNIX)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${LIBCURL_INCLUDE_DIRS} ${JANSSON_INCLUDE_DIRS} ${SPEEX_INCLUDE_DIRS})

LINK_DIRECTORIES(${SDL2_LIBRARY_DIRS} ${JANSSON_LIBRARY_DIRS} ${LIBCURL_LIBRARY_DIRS})

if (WIN32)
	# build as library for now, replace with add_executable
	add_library(${PROJECT} SHARED ${ORCT2_SOURCES} ${SPEEX_SOURCES})
else (WIN32)
	add_executable(${PROJECT} ${ORCT2_SOURCES})
endif (WIN32)

# install into ${CMAKE_INSTALL_PREFIX}/bin/
#install (TARGETS ${PROJECT} DESTINATION bin)

# libopenrct2.dll -> openrct2.dll
set_target_properties(${PROJECT} PROPERTIES PREFIX "")

TARGET_LINK_LIBRARIES(${PROJECT} ${SDL2_LIBRARIES} ${ORCTLIBS_LIB} ${JANSSON_LIBRARIES} ${HTTPLIBS} ${NETWORKLIBS} ${SPEEX_LIBRARIES} ${DLLIB})
