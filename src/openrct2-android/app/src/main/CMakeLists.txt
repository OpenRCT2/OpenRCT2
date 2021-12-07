cmake_minimum_required(VERSION 3.8.0)

project(openrct2-android CXX)

set(CMAKE_VERBOSE_MAKEFILE on)

set(lib_src_DIR ${CMAKE_SOURCE_DIR}/../../../libs)
set(lib_build_DIR $ENV{HOME}/tmp)
file(MAKE_DIRECTORY ${lib_build_DIR})

set(DEBUG_LEVEL 0 CACHE STRING "Select debug level for compilation. Use value in range 0–3.")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG=${DEBUG_LEVEL}")

# Copied from https://github.com/opencv/opencv/blob/dcdd6af5a856826fe62c95322145731e702e54c5/cmake/OpenCVDetectCXXCompiler.cmake#L63-L70
if(MSVC64 OR MINGW64)
    set(X86_64 1)
elseif(MINGW OR (MSVC AND NOT CMAKE_CROSSCOMPILING))
    set(X86 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64.*|x86_64.*|AMD64.*")
    set(X86_64 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i686.*|i386.*|x86.*|amd64.*|AMD64.*")
    set(X86 1)
endif()

include(ExternalProject)
ExternalProject_Add(libs
    URL https://github.com/OpenRCT2/openrct2-dependencies-android/releases/download/v0.9/openrct2-libs-android-${ANDROID_ABI}.tar.gz

    SOURCE_DIR "${CMAKE_BINARY_DIR}/libs"

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""

    BUILD_BYPRODUCTS
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}freetype${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}png16${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}SDL2-2.0${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2main${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}speexdsp${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_STATIC_LIBRARY_PREFIX}icui18n${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_STATIC_LIBRARY_PREFIX}icuuc${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_STATIC_LIBRARY_PREFIX}icudata${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}ssl${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}crypto${CMAKE_SHARED_LIBRARY_SUFFIX}

    LOG_DOWNLOAD 1
    LOG_UPDATE 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_TEST 1
    LOG_INSTALL 1
)

# Files must be copied individually as the old Windows console does not support wildcards in the copy command
# Doing multiple individual commands looks "less bad" than one massive line
add_custom_command(TARGET libs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
#    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/libs/lib/*.so" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/libs/lib/libcrypto.so" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/libs/lib/libfreetype.so" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/libs/lib/libpng16.so" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/libs/lib/libSDL2-2.0.so" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/libs/lib/libspeexdsp.so" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/libs/lib/libssl.so" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

add_library(freetype SHARED IMPORTED)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}freetype${CMAKE_SHARED_LIBRARY_SUFFIX}
)
add_dependencies(freetype libs)

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

add_library(icu STATIC IMPORTED)
set_target_properties(icu PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_STATIC_LIBRARY_PREFIX}icui18n${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(icu libs)

add_library(icuuc STATIC IMPORTED)
set_target_properties(icuuc PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_STATIC_LIBRARY_PREFIX}icuuc${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(icuuc libs)

add_library(icudata STATIC IMPORTED)
set_target_properties(icudata PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_STATIC_LIBRARY_PREFIX}icudata${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(icudata libs)

add_library(ssl SHARED IMPORTED)
set_target_properties(ssl PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}ssl${CMAKE_SHARED_LIBRARY_SUFFIX}
)
add_dependencies(ssl libs)

add_library(crypto SHARED IMPORTED)
set_target_properties(crypto PROPERTIES IMPORTED_LOCATION
    ${CMAKE_BINARY_DIR}/libs/lib/${CMAKE_SHARED_LIBRARY_PREFIX}crypto${CMAKE_SHARED_LIBRARY_SUFFIX}
)
add_dependencies(crypto libs)


include_directories(SYSTEM "${CMAKE_BINARY_DIR}/libs/include")
include_directories(SYSTEM "${CMAKE_BINARY_DIR}/libs/include/freetype2")
include_directories(SYSTEM "${CMAKE_BINARY_DIR}/libs/include/SDL2")

# now build app's shared lib
add_definitions(-DDISABLE_HTTP -DDISABLE_DISCORD_RPC -DDISABLE_HTTP -DDISABLE_NETWORK -DDISABLE_OPENGL -DGL_GLEXT_PROTOTYPES -D__STDC_LIMIT_MACROS -DNO_TTF -DSDL_MAIN_HANDLED)

# Fix SpeexDSP compilation
add_definitions(-DHAVE_STDINT_H)

# -Werror -Wall
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -fstrict-aliasing -Wundef -Wmissing-declarations -Winit-self  -Wno-unknown-pragmas -Wno-unused-function -Wno-missing-braces ")
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -Wno-comment -Wshadow  -Wmissing-declarations -Wnonnull")
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -fPIC")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1z ${COMMON_COMPILE_OPTIONS} -Wnon-virtual-dtor")
get_filename_component(ORCT2_ROOT "${CMAKE_SOURCE_DIR}/../../../../../" REALPATH)

list(APPEND CMAKE_MODULE_PATH "${ORCT2_ROOT}/cmake")

find_package(CCache)

if (CCache_FOUND)
    option(OPENRCT2_USE_CCACHE "Use CCache to improve recompilation speed (optional)" ON)
    if (OPENRCT2_USE_CCACHE)
        # Use e.g. "ccache clang++" instead of "clang++"
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCache_EXECUTABLE}")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK    "${CCache_EXECUTABLE}")
    endif (OPENRCT2_USE_CCACHE)
endif (CCache_FOUND)

file(GLOB_RECURSE LIBOPENRCT2_SOURCES
    "${ORCT2_ROOT}/src/openrct2/*.cpp"
    "${ORCT2_ROOT}/src/openrct2/*.h"
    "${ORCT2_ROOT}/src/openrct2/*.hpp")

file(GLOB_RECURSE OPENRCT2_GUI_SOURCES
    "${ORCT2_ROOT}/src/openrct2-ui/*.cpp"
    "${ORCT2_ROOT}/src/openrct2-ui/*.h"
    "${ORCT2_ROOT}/src/openrct2-ui/*.hpp")

if(X86 OR X86_64)
    set_source_files_properties(${ORCT2_ROOT}/src/openrct2/drawing/SSE41Drawing.cpp PROPERTIES COMPILE_FLAGS -msse4.1)
    set_source_files_properties(${ORCT2_ROOT}/src/openrct2/drawing/AVX2Drawing.cpp PROPERTIES COMPILE_FLAGS -mavx2)
endif()

file(GLOB_RECURSE OPENRCT2_CLI_SOURCES
    "${ORCT2_ROOT}/src/openrct2-cli/*.cpp"
    "${ORCT2_ROOT}/src/openrct2-cli/*.h"
    "${ORCT2_ROOT}/src/openrct2-cli/*.hpp")

add_library(openrct2 SHARED ${LIBOPENRCT2_SOURCES})
target_link_libraries(openrct2 android stdc++ log dl z SDL2 png icu icuuc icudata)# ssl crypto)

add_library(openrct2-ui SHARED ${OPENRCT2_GUI_SOURCES})
target_link_libraries(openrct2-ui openrct2 android stdc++ GLESv1_CM GLESv2 SDL2main speexdsp)

add_executable(openrct2-cli ${OPENRCT2_CLI_SOURCES})
target_link_libraries(openrct2-cli openrct2 android stdc++ GLESv1_CM GLESv2)

target_include_directories(openrct2 SYSTEM PRIVATE "${ORCT2_ROOT}/src/thirdparty")
target_include_directories(openrct2-ui PRIVATE "${ORCT2_ROOT}/src")
target_include_directories(openrct2-ui SYSTEM PRIVATE "${ORCT2_ROOT}/src/thirdparty")
target_include_directories(openrct2-cli PRIVATE "${ORCT2_ROOT}/src")

# Header-only nlohmann library is installed in CI images at /nlohmann.
# To be tiny bit more specific, use '/nlohmann/../' instead of just '/'
target_include_directories(openrct2 PRIVATE "/nlohmann/../")
target_include_directories(openrct2-ui PRIVATE "/nlohmann/../")
