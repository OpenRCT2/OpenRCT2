cmake_minimum_required(VERSION 3.6.0)

set(CMAKE_VERBOSE_MAKEFILE on)

set(lib_src_DIR ${CMAKE_SOURCE_DIR}/../../../libs)
set(lib_build_DIR $ENV{HOME}/tmp)
file(MAKE_DIRECTORY ${lib_build_DIR})

set(DEBUG_LEVEL 0 CACHE STRING "Select debug level for compilation. Use value in range 0–3.")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG=${DEBUG_LEVEL}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG=${DEBUG_LEVEL}")

include(ExternalProject)
ExternalProject_Add(libs
    URL https://github.com/OpenRCT2/openrct2-dependencies-android/releases/download/v0.7/openrct2-libs-android-${ANDROID_ABI}.zip

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

# Fix SpeexDSP compilation
add_definitions(-DHAVE_STDINT_H)

set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -fstrict-aliasing -Werror -Wundef -Wmissing-declarations -Winit-self -Wall -Wno-unknown-pragmas -Wno-unused-function -Wno-missing-braces ")
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -Wno-comment -Wshadow  -Wmissing-declarations -Wnonnull")
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -fPIC")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 ${COMMON_COMPILE_OPTIONS} -Wimplicit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 ${COMMON_COMPILE_OPTIONS} -Wnon-virtual-dtor")
get_filename_component(ORCT2_ROOT "${CMAKE_SOURCE_DIR}/../../../../../" REALPATH)

file(GLOB_RECURSE LIBOPENRCT2_SOURCES
    "${ORCT2_ROOT}/src/openrct2/*.c"
    "${ORCT2_ROOT}/src/openrct2/*.cpp"
    "${ORCT2_ROOT}/src/openrct2/*.h"
    "${ORCT2_ROOT}/src/openrct2/*.hpp")

file(GLOB_RECURSE OPENRCT2_GUI_SOURCES
    "${ORCT2_ROOT}/src/openrct2-ui/*.c"
    "${ORCT2_ROOT}/src/openrct2-ui/*.cpp"
    "${ORCT2_ROOT}/src/openrct2-ui/*.h"
    "${ORCT2_ROOT}/src/openrct2-ui/*.hpp")

add_library(openrct2 SHARED ${LIBOPENRCT2_SOURCES})
target_link_libraries(openrct2
                        android log dl GLESv1_CM GLESv2 z
                        SDL2 png jansson speexdsp
                      )

add_library(openrct2-ui SHARED ${OPENRCT2_GUI_SOURCES})
target_link_libraries(openrct2-ui openrct2 SDL2main)
target_include_directories(openrct2-ui PRIVATE "${ORCT2_ROOT}/src")