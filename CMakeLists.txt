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

# include lodepng header
include_directories("lodepng/")
# add source files
file(GLOB_RECURSE ORCT2_SOURCES "src/*.c" "lodepng/*.c")

if (UNIX)
	# force 32bit build for now and set necessary flags to compile code as is
	set(CMAKE_C_FLAGS "-m32 -masm=intel -std=gnu99")
	set(CMAKE_LDFLAGS_FLAGS "-m32")

	# find and include SDL2
	INCLUDE(FindPkgConfig)
	PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
	INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
endif (UNIX)

LINK_DIRECTORIES(${SDL2_LIBRARY_DIRS})

# build as library for now, replace with add_executable
add_library(${PROJECT} SHARED ${ORCT2_SOURCES})

# install into ${CMAKE_INSTALL_PREFIX}/bin/
#install (TARGETS ${PROJECT} DESTINATION bin)

# libopenrct2.dll -> openrct2.dll
set_target_properties(${PROJECT} PROPERTIES PREFIX "")

TARGET_LINK_LIBRARIES(${PROJECT} ${SDL2_LIBRARIES})

if (WIN32)
	target_link_libraries(${PROJECT} winmm.lib -limm32 -lversion)
endif (WIN32)

