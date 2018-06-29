# CMAKE project for openrct2-ui (UI build of OpenRCT2)
cmake_minimum_required(VERSION 3.1)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

# Options
option(DISABLE_OPENGL "Disable OpenGL support.")

# Third party libraries
if (MSVC)
    find_package(jansson REQUIRED)

    find_path(SDL2_INCLUDE_DIRS SDL2/SDL.h)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        find_library(SDL2_LDFLAGS sdl2d)
    else ()
        find_library(SDL2_LDFLAGS sdl2)
    endif ()

    find_library(SPEEX_LDFLAGS libspeexdsp)
else ()
    PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)
    PKG_CHECK_MODULES(SPEEX REQUIRED speexdsp)
endif ()

if (NOT DISABLE_OPENGL)
    # GL doesn't work nicely with macOS, while find_package doesn't work with multiarch on Ubuntu.
    if (APPLE)
        find_package(OpenGL REQUIRED)
    elseif (NOT WIN32)
        PKG_CHECK_MODULES(GL REQUIRED gl)
    endif ()
endif ()

# Sources
file(GLOB_RECURSE OPENRCT2_UI_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE OPENRCT2_UI_HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

if (APPLE)
    file(GLOB_RECURSE OPENRCT2_UI_MM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    set_source_files_properties(${OPENRCT2_UI_MM_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c++ -fmodules")
endif ()

# Outputs
set (PROJECT openrct2)
project(${PROJECT} CXX)
add_executable(${PROJECT} ${OPENRCT2_UI_SOURCES} ${OPENRCT2_UI_MM_SOURCES})

target_link_libraries(${PROJECT} "libopenrct2"
                                 ${SDL2_LDFLAGS}
                                 ${SPEEX_LDFLAGS})

if (APPLE)
    target_link_libraries(${PROJECT} "-framework Cocoa")
endif ()

if (NOT DISABLE_OPENGL)
    if (WIN32)
        target_link_libraries(${PROJECT} opengl32)
    elseif (APPLE)
        target_link_libraries(${PROJECT} ${OPENGL_LIBRARY})
    else ()
        target_link_libraries(${PROJECT} ${GL_LIBRARIES})
    endif ()
endif ()

# Includes
target_include_directories(${PROJECT} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/.."
                                              ${SPEEX_INCLUDE_DIRS})
target_include_directories(${PROJECT} SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})

# Compiler flags
if (WIN32)
    # mingw complains about "%zu" not being a valid format specifier for printf, unless we
    # tell it that it is
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__USE_MINGW_ANSI_STDIO=1")
endif ()
if (MSVC)
    # Add DPI-aware manifest
    # HACK Add /MANIFEST:NO first to prevent cmake from adding its own manifest arguments
    if (NOT $ENV{VCToolsInstallDir} STREQUAL "")
        set(MANIFEST_DPI "$ENV{VCToolsInstallDir}\\Include\\Manifest\\PerMonitorHighDPIAware.manifest")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO /MANIFEST /manifest:embed /manifestinput:\"${MANIFEST_DPI}\"")
    endif ()
endif ()

# Defines
if (DISABLE_OPENGL)
    add_definitions(-DDISABLE_OPENGL)
else ()
    # Makes OpenGL function get queried in run-time rather than linked-in
    add_definitions(-DOPENGL_NO_LINK)
endif ()
if (MSVC)
    add_definitions(-D__DISABLE_DLL_PROXY__)
endif ()

# Add headers check to verify all headers carry their dependencies.
# Only valid for Clang for now:
# - GCC 8 does not support -Wno-pragma-once-outside-header
# - Other compilers status unknown
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(OPENRCT2_HEADERS_CHECK ${OPENRCT2_UI_HEADERS})
    # OpenGLAPIProc.h is not meant to be included directly.
    list(REMOVE_ITEM OPENRCT2_HEADERS_CHECK "${CMAKE_CURRENT_LIST_DIR}/drawing/engines/opengl/OpenGLAPIProc.h")
    add_library(${PROJECT}-headers-check OBJECT ${OPENRCT2_HEADERS_CHECK})
    set_target_properties(${PROJECT}-headers-check PROPERTIES LINKER_LANGUAGE CXX)
    set_source_files_properties(${OPENRCT2_HEADERS_CHECK} PROPERTIES LANGUAGE CXX)
    add_definitions("-x c++ -Wno-pragma-once-outside-header -Wno-unused-const-variable")
    get_target_property(OPENRCT2_INCLUDE_DIRS ${PROJECT} INCLUDE_DIRECTORIES)
    set_target_properties(${PROJECT}-headers-check PROPERTIES INCLUDE_DIRECTORIES "${OPENRCT2_INCLUDE_DIRS}")
endif ()
