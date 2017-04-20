
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

set(DEBUG_LEVEL 0 CACHE STRING "Select debug level for compilation. Use value in range 0–3.")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG=${DEBUG_LEVEL}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG=${DEBUG_LEVEL}")


include(ExternalProject)
ExternalProject_Add(libs
    URL https://github.com/marijnvdwerf/openrct2-dependencies-android/releases/download/v0.3/openrct2-libs-android-${ANDROID_ABI}.zip

    SOURCE_DIR "${CMAKE_BINARY_DIR}/libs"

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""

    BUILD_BYPRODUCTS
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}freetype${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}jansson${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}png16${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}SDL2-2.0${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2main${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}speexdsp${CMAKE_SHARED_LIBRARY_SUFFIX}

    LOG_DOWNLOAD 1
    LOG_UPDATE 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_TEST 1
    LOG_INSTALL 1
)

add_custom_command(TARGET libs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/libs/lib/*.so" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

add_library(freetype SHARED IMPORTED)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}freetype${CMAKE_SHARED_LIBRARY_SUFFIX}
)
add_dependencies(freetype libs)

add_library(jansson SHARED IMPORTED)
set_target_properties(jansson PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}jansson${CMAKE_SHARED_LIBRARY_SUFFIX}
)
add_dependencies(jansson libs)

add_library(png SHARED IMPORTED)
set_target_properties(png PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}png16${CMAKE_SHARED_LIBRARY_SUFFIX}
)
add_dependencies(png libs)

add_library(SDL2 SHARED IMPORTED)
set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}SDL2-2.0${CMAKE_SHARED_LIBRARY_SUFFIX}
)
add_dependencies(SDL2 libs)

add_library(SDL2main STATIC IMPORTED)
set_target_properties(SDL2main PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2main${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(SDL2main libs)

add_library(speexdsp SHARED IMPORTED)
set_target_properties(speexdsp PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}speexdsp${CMAKE_SHARED_LIBRARY_SUFFIX}
)
add_dependencies(speexdsp libs)


include_directories("${CMAKE_BINARY_DIR}/libs/include")
include_directories("${CMAKE_BINARY_DIR}/libs/include/freetype2")
include_directories("${CMAKE_BINARY_DIR}/libs/include/SDL2")

# now build app's shared lib
include_directories(./ndk_helper
                    ${ANDROID_NDK}/sources/android/cpufeatures)
add_definitions(-DDISABLE_HTTP -DDISABLE_TWITCH -DDISABLE_NETWORK -DDISABLE_OPENGL -DGL_GLEXT_PROTOTYPES -D__STDC_LIMIT_MACROS -DNO_RCT2 -DNO_TTF -DSDL_MAIN_HANDLED)

set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -fstrict-aliasing -Werror -Wundef -Wmissing-declarations -Winit-self -Wall -Wno-unknown-pragmas -Wno-unused-function -Wno-missing-braces ")
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -Wno-comment -Wshadow  -Wmissing-declarations -Wnonnull")
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -fPIC")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 ${COMMON_COMPILE_OPTIONS} -Wimplicit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 ${COMMON_COMPILE_OPTIONS} -Wnon-virtual-dtor")
get_filename_component(ORCT2_ROOT "${CMAKE_SOURCE_DIR}/../../../../../" REALPATH)
file(GLOB_RECURSE ORCT2_SOURCES "${ORCT2_ROOT}/src/openrct2/*.c" "${ORCT2_ROOT}/src/openrct2/*.cpp" "${ORCT2_ROOT}/src/openrct2/*.h" "${ORCT2_ROOT}/src/openrct2/*.hpp")

message("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
add_library(openrct2 SHARED ${ORCT2_SOURCES})

# add lib dependencies
target_link_libraries(openrct2
                        android log dl GLESv1_CM GLESv2 z
                        SDL2 SDL2main png jansson speexdsp
                      )
