# CMAKE project for libopenrct2 (core OpenRCT2 component)
cmake_minimum_required(VERSION 3.1)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

# Options
option(STATIC "Create a static build.")
option(USE_MMAP "Use mmap to try loading rct2's data segment into memory.")

option(DISABLE_HTTP_TWITCH "Disable HTTP and Twitch support.")
option(DISABLE_NETWORK "Disable multiplayer functionality. Mainly for testing.")
option(DISABLE_TTF "Disable support for TTF provided by freetype2.")
option(ENABLE_LIGHTFX "Enable lighting effects." ON)

# Needed for linking with non-broken OpenSSL on Apple platforms
if (APPLE)
    set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/opt/openssl/lib/pkgconfig")
endif ()

# Third party libraries
PKG_CHECK_MODULES(JANSSON REQUIRED jansson>=2.5)
PKG_CHECK_MODULES(LIBZIP REQUIRED libzip>=1.0)
PKG_CHECK_MODULES(ZLIB REQUIRED zlib)

PKG_CHECK_MODULES(PNG libpng>=1.6)
if (NOT PNG_FOUND)
    PKG_CHECK_MODULES(PNG libpng16)
    if (NOT PNG_FOUND)
        PKG_CHECK_MODULES(PNG libpng>=1.2)
        if (NOT PNG_FOUND)
            PKG_CHECK_MODULES(PNG REQUIRED libpng12)
        endif ()
    endif ()
endif ()

# Third party libraries (optional)
if (NOT DISABLE_HTTP_TWITCH OR NOT DISABLE_NETWORK)
    PKG_CHECK_MODULES(LIBCURL REQUIRED libcurl)
endif ()
if (NOT DISABLE_NETWORK)
    find_package(OpenSSL 1.0.0 REQUIRED)
endif ()

if (NOT DISABLE_TTF)
    if (UNIX AND NOT APPLE)
        PKG_CHECK_MODULES(FONTCONFIG REQUIRED fontconfig)
    endif ()
    PKG_CHECK_MODULES(FREETYPE REQUIRED freetype2)
endif ()

# Sources
file(GLOB_RECURSE OPENRCT2_CORE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.h"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
if (APPLE)
    file(GLOB_RECURSE OPENRCT2_CORE_MM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    set_source_files_properties(${OPENRCT2_CORE_MM_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c++ -fmodules")
endif ()

# Outputs
set(PROJECT libopenrct2)
project(${PROJECT})
add_library(${PROJECT} ${OPENRCT2_CORE_SOURCES} ${OPENRCT2_CORE_MM_SOURCES} ${RCT2_SECTIONS})
set_target_properties(${PROJECT} PROPERTIES PREFIX "")
set_target_properties(${PROJECT} PROPERTIES COMPILE_FLAGS "-Wundef")

# Libraries
if (STATIC)
    target_link_libraries(${PROJECT} ${JANSSON_STATIC_LIBRARIES}
                                     ${PNG_STATIC_LIBRARIES}
                                     ${ZLIB_STATIC_LIBRARIES}
                                     ${LIBZIP_STATIC_LIBRARIES})
else ()
    target_link_libraries(${PROJECT} ${JANSSON_LIBRARIES}
                                     ${PNG_LIBRARIES}
                                     ${ZLIB_LIBRARIES}
                                     ${LIBZIP_LIBRARIES})
endif ()

if (UNIX AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "BSD")
    # Include libdl for dlopen
    target_link_libraries(${PROJECT} dl)
endif ()

if (NOT DISABLE_NETWORK)
    if (WIN32)
        target_link_libraries(${PROJECT} ws2_32)
    endif ()

    # our HTTP implementation requires use of threads
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT} Threads::Threads)

    if (STATIC)
        target_link_libraries(${PROJECT} ${LIBCURL_STATIC_LIBRARIES}
                                         ${SSL_STATIC_LIBRARIES})
    else ()
        target_link_libraries(${PROJECT} ${LIBCURL_LIBRARIES}
                                         ${OPENSSL_LIBRARIES})
    endif ()
endif ()

if (NOT APPLE AND NOT MINGW)
    # This is ugly hack to work around https://bugs.launchpad.net/ubuntu/+source/gcc-5/+bug/1568899.
    # Once C++17 is enabled (and thus old compilers are no longer supported, this needs to be gone.
    # We cannot simply detect the _compiler_ version, as the bug exists with the C++ _library_
    target_link_libraries(${PROJECT} gcc_s gcc)
endif ()

if (NOT DISABLE_TTF)
    if (STATIC)
        target_link_libraries(${PROJECT} ${FREETYPE_STATIC_LIBRARIES})
        if (UNIX AND NOT APPLE)
            target_link_libraries(${PROJECT} ${FONTCONFIG_STATIC_LIBRARIES})
        endif ()
    else ()
        target_link_libraries(${PROJECT} ${FREETYPE_LIBRARIES})
        if (UNIX AND NOT APPLE)
            target_link_libraries(${PROJECT} ${FONTCONFIG_LIBRARIES})
        endif ()
    endif ()
endif ()

if (UNIX OR STATIC OR ${CMAKE_SYSTEM_NAME} MATCHES "BSD")
    find_library(ICONV_LIBRARIES NAMES iconv libiconv libiconv-2 c)
    target_link_libraries(${PROJECT} ${ICONV_LIBRARIES})
endif()

if (HAVE_DISCORD_RPC)
    target_link_libraries(libopenrct2 discord-rpc)
endif()

# Includes
target_include_directories(${PROJECT} SYSTEM PRIVATE ${LIBZIP_INCLUDE_DIRS})
target_include_directories(${PROJECT} PUBLIC ${JANSSON_INCLUDE_DIRS})
target_include_directories(${PROJECT} PRIVATE ${PNG_INCLUDE_DIRS}
                                              ${ZLIB_INCLUDE_DIRS})
if (NOT DISABLE_HTTP_TWITCH OR NOT DISABLE_NETWORK)
    target_include_directories(${PROJECT} PRIVATE ${LIBCURL_INCLUDE_DIRS})
endif ()
if (NOT DISABLE_NETWORK)
    target_include_directories(${PROJECT} PUBLIC ${OPENSSL_INCLUDE_DIR})
endif ()
if (NOT DISABLE_TTF)
    target_include_directories(${PROJECT} PRIVATE ${FREETYPE_INCLUDE_DIRS})
    if (UNIX AND NOT APPLE)
        target_include_directories(${PROJECT} PRIVATE ${FONTCONFIG_INCLUDE_DIRS})
    endif ()
endif ()

if(APPLE)
    set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -Wno-error=objc-method-access")
endif()

# Compiler flags
set(DEBUG_LEVEL 0 CACHE STRING "Select debug level for compilation. Use value in range 0–3.")
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -fstrict-aliasing -Werror -Wundef -Wmissing-declarations -Winit-self -Wall -Wno-unknown-pragmas -Wno-unused-function -Wno-missing-braces ")
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -Wno-comment -Wshadow  -Wmissing-declarations -Wnonnull")
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -DDEBUG=${DEBUG_LEVEL}")

# On mingw all code is already PIC, this will avoid compiler error on redefining this option
if (NOT MINGW)
    set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -fPIC")
endif ()

if (APPLE AND NOT USE_MMAP)
    set(PIE_FLAG "-fno-pie")
else ()
    set(PIE_FLAG "-fpie")
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 ${COMMON_COMPILE_OPTIONS} -Wimplicit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 ${COMMON_COMPILE_OPTIONS}")

# Defines
if (USE_MMAP)
    add_definitions(-DUSE_MMAP)
endif ()
if (DISABLE_NETWORK)
    add_definitions(-DDISABLE_NETWORK)
endif ()
if (DISABLE_HTTP_TWITCH)
    add_definitions(-DDISABLE_HTTP)
    add_definitions(-DDISABLE_TWITCH)
endif ()
if (DISABLE_TTF)
    add_definitions(-DNO_TTF)
endif ()
if (ENABLE_LIGHTFX)
    add_definitions(-D__ENABLE_LIGHTFX__)
endif ()

if (CXX_WARN_SUGGEST_FINAL_TYPES)
    # Disable -Wsuggest-final-types via pragmas where due.
   add_definitions(-D__WARN_SUGGEST_FINAL_TYPES__)
endif ()

if (CXX_WARN_SUGGEST_FINAL_METHODS)
    # Disable -Wsuggest-final-methods via pragmas where due.
   add_definitions(-D__WARN_SUGGEST_FINAL_METHODS__)
endif ()

# To avoid unnecessary rebuilds set the current branch and
# short sha1 only for the two files that use these
# definitions: Version.cpp and Crash/Platform.cpp
set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/Version.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Crash/Platform.cpp PROPERTY
    COMPILE_DEFINITIONS OPENRCT2_BRANCH="${OPENRCT2_BRANCH}")
if (NOT OPENRCT2_COMMIT_SHA1_SHORT STREQUAL "HEAD")
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/Version.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Crash/Platform.cpp PROPERTY
    COMPILE_DEFINITIONS OPENRCT2_COMMIT_SHA1_SHORT="${OPENRCT2_COMMIT_SHA1_SHORT}")
endif()

if(X86 OR X86_64)
set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/drawing/SSE41Drawing.cpp PROPERTIES COMPILE_FLAGS -msse4.1)
set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/drawing/AVX2Drawing.cpp PROPERTIES COMPILE_FLAGS -mavx2)
endif()
