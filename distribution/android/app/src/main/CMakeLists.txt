
#
# Copyright (C)  The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)

# build native_app_glue as a static lib
# include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
# add_library(app-glue STATIC
#            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)


set(lib_src_DIR ${CMAKE_SOURCE_DIR}/../../../libs)
set(lib_build_DIR $ENV{HOME}/tmp)
file(MAKE_DIRECTORY ${lib_build_DIR})

# SDL
set(VIDEO_DUMMY OFF)
set(HAVE_SDL_THREADS ON)
set(DISKAUDIO OFF)
set(PTHREADS ON)
set(PTHREADS_SEM ON)
set(SDL_STATIC OFF)
set(EXTRA_LIBS dl GLESv1_CM GLESv2 log android)
list(APPEND EXTRA_CFLAGS -DGL_GLEXT_PROTOTYPES)
add_subdirectory(${lib_src_DIR}/sdl ${lib_build_DIR}/sdl)
include_directories(${lib_src_DIR}/sdl/include)

## Remove unavailable library
get_target_property(SDL_LINK_LIBRARIES SDL2 LINK_LIBRARIES)
list(REMOVE_ITEM SDL_LINK_LIBRARIES -lpthread)
set_target_properties(SDL2 PROPERTIES LINK_LIBRARIES "${SDL_LINK_LIBRARIES}")

## Remove source files
get_target_property(SDL_SOURCES SDL2 SOURCES)
file(GLOB SDL_REMOVE_SOURCE_FILES
    "${lib_src_DIR}/sdl/src/libm/*.c"
    "${lib_src_DIR}/sdl/src/thread/generic/*.c"
    "${lib_src_DIR}/sdl/src/filesystem/unix/*.c"
    "${lib_src_DIR}/sdl/src/main/android/*.c"
)
foreach(file ${SDL_REMOVE_SOURCE_FILES})
  get_filename_component(file ${file} REALPATH)
  list(REMOVE_ITEM SDL_SOURCES "${file}")
endforeach()

get_filename_component(SDL_SYSSEM_PATH "${lib_src_DIR}/sdl/src/thread/pthread/SDL_syssem.c" REALPATH)
list(APPEND SDL_SOURCES ${SDL_SYSSEM_PATH})

list(SORT SDL_SOURCES)
set_target_properties(SDL2 PROPERTIES SOURCES "${SDL_SOURCES}")


# Freetype
set(BUILD_SHARED_LIBS ON)
add_subdirectory(${lib_src_DIR}/freetype ${lib_build_DIR}/freetype)
include_directories(${lib_src_DIR}/freetype/include)

# SDL-ttf
include_directories(${lib_src_DIR}/sdl-ttf)
SET(SDL2_ttf_LIB_SRCS "${lib_src_DIR}/sdl-ttf/SDL_ttf.c")
add_library(SDL2_ttf SHARED ${SDL2_ttf_LIB_SRCS})
target_link_libraries(SDL2_ttf SDL2 freetype)

# png
set(PNG_STATIC OFF)
set(PNG_TESTS OFF)
add_subdirectory(${lib_src_DIR}/png ${lib_build_DIR}/png)
include_directories("${lib_src_DIR}/png")
include_directories("${lib_build_DIR}/png") # Generated config file

# speexdsp
set(SPEEXDSP_ROOT "${lib_src_DIR}/speexdsp")
add_subdirectory(${CMAKE_SOURCE_DIR}/../speexdsp ${lib_build_DIR}/speexdsp)
include_directories(${lib_src_DIR}/speexdsp/include)
include_directories("${CMAKE_SOURCE_DIR}/../speexdsp")


# jansson
set(JANSSON_ROOT "${lib_src_DIR}/jansson")
add_subdirectory(${CMAKE_SOURCE_DIR}/../jansson ${lib_build_DIR}/jansson)
include_directories(${lib_src_DIR}/jansson/android)
include_directories(${lib_src_DIR}/jansson/src)


# now build app's shared lib
include_directories(./ndk_helper
                    ${ANDROID_NDK}/sources/android/cpufeatures)
add_definitions(-DDISABLE_HTTP -DDISABLE_TWITCH -DDISABLE_NETWORK -DDISABLE_OPENGL -DGL_GLEXT_PROTOTYPES -DDEBUG=3)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wno-unknown-pragmas -fno-omit-frame-pointer -fno-pie -fstrict-aliasing -Werror=strict-aliasing -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fexceptions -fno-rtti -Wno-unknown-pragmas -fno-omit-frame-pointer -fno-pie -fstrict-aliasing -Werror=strict-aliasing -fexceptions -fPIC")
get_filename_component(ORCT2_ROOT "${CMAKE_SOURCE_DIR}/../../../../../" REALPATH)
file(GLOB_RECURSE ORCT2_SOURCES "${ORCT2_ROOT}/src/*.c" "${ORCT2_ROOT}/src/*.cpp" "${ORCT2_ROOT}/src/*.h" "${ORCT2_ROOT}/src/*.hpp")

add_library(openrct2 SHARED ${ORCT2_SOURCES})

# add lib dependencies
target_link_libraries(openrct2
                        android log dl GLESv1_CM GLESv2 z
                        SDL2 SDL2_ttf png jansson speexdsp
                      )

# moduleName "main"
# CFlags.addAll("-O3", "-DGL_GLEXT_PROTOTYPES", "")
# cppFlags.addAll("-O3", "-std=gnu++11", "")
# ldLibs.addAll(["dl", "GLESv1_CM", "GLESv2", "log", "android", "z"])
# cppFlags.add("-DDEBUG=0")
# CFlags.add("-DDEBUG=0")
# stl 'c++_shared'
# abiFilters.addAll(["armeabi", "armeabi-v7a", "x86"])
# platformVersion 16
