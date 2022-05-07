cmake_minimum_required(VERSION 3.9)
project(libopenrct2 CXX)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

file(GLOB_RECURSE OPENRCT2_CORE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE OPENRCT2_CORE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
list(REMOVE_ITEM OPENRCT2_CORE_HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/thirdparty/filesystem.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/thirdparty/linenoise.hpp"
)

if (APPLE)
    file(GLOB_RECURSE OPENRCT2_CORE_MM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    set_source_files_properties(${OPENRCT2_CORE_MM_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c++ -fmodules")
endif ()

if (ENABLE_SCRIPTING)
    include_directories("${CMAKE_CURRENT_LIST_DIR}/../thirdparty/duktape")

    # duktape is third party, ignore all warnings
    set(OPENRCT2_DUKTAPE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../thirdparty/duktape/duktape.cpp")
    if (MSVC)
        set_source_files_properties(${OPENRCT2_DUKTAPE_SOURCES} PROPERTIES COMPILE_FLAGS "/w")
    else ()
        set_source_files_properties(${OPENRCT2_DUKTAPE_SOURCES} PROPERTIES COMPILE_FLAGS "-w")
    endif ()
endif ()

add_library(${PROJECT_NAME} ${OPENRCT2_CORE_SOURCES} ${OPENRCT2_CORE_MM_SOURCES} ${OPENRCT2_DUKTAPE_SOURCES})
if (APPLE)
    target_link_platform_libraries(${PROJECT_NAME})
endif ()
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
SET_CHECK_CXX_FLAGS(${PROJECT_NAME})

# GCC / Clang likes us to pass the -lstdc++fs flag to link C++17 filesystem implementation.
if (NOT MINGW AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_link_libraries(${PROJECT_NAME} stdc++fs)
    endif()
endif()

if (NOT DISABLE_NETWORK OR NOT DISABLE_HTTP)
    if (WIN32 AND NOT MINGW_TARGET_NT5_1)
        target_link_libraries(${PROJECT_NAME} bcrypt)
    else ()
        if (APPLE AND NOT MACOS_USE_DEPENDENCIES)
            # note - we are not yet compatible with openssl3, which is now default brew version
            execute_process(COMMAND brew --prefix openssl@1.1 OUTPUT_VARIABLE HOMEBREW_PREFIX_OPENSSL OUTPUT_STRIP_TRAILING_WHITESPACE)
            # Needed for linking with non-broken OpenSSL on Apple platforms
            list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX_OPENSSL}")
        endif ()
        find_package(OpenSSL 1.0.0 REQUIRED)

        target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${OPENSSL_INCLUDE_DIR})

        if(STATIC)
            target_link_libraries(${PROJECT_NAME} ${SSL_STATIC_LIBRARIES})
        else ()
            target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
        endif()
    endif()
endif ()

if (NOT DISABLE_NETWORK AND WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32 crypt32 wldap32 version winmm imm32 advapi32 shell32 ole32)
endif ()

if (NOT DISABLE_HTTP)
    if (WIN32 AND NOT MINGW_TARGET_NT5_1)
        target_link_libraries(${PROJECT_NAME} winhttp)
    else ()
        PKG_CHECK_MODULES(LIBCURL REQUIRED libcurl)

        target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${LIBCURL_INCLUDE_DIRS})

        if (STATIC)
            target_link_libraries(${PROJECT_NAME} ${LIBCURL_STATIC_LIBRARIES})
        else ()
            target_link_libraries(${PROJECT_NAME} ${LIBCURL_LIBRARIES})
        endif ()
    endif ()
endif ()

if (NOT DISABLE_TTF)
    if (UNIX AND NOT APPLE AND NOT MSVC)
        PKG_CHECK_MODULES(FONTCONFIG REQUIRED fontconfig)
    endif ()
        
    if (MSVC)
        find_package(freetype REQUIRED)
    else ()
        PKG_CHECK_MODULES(FREETYPE REQUIRED IMPORTED_TARGET freetype2)
    endif ()
        
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${FREETYPE_INCLUDE_DIRS})
        
    if (UNIX AND NOT APPLE)
        target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${FONTCONFIG_INCLUDE_DIRS})
    endif ()
endif ()

if (NOT DISABLE_GOOGLE_BENCHMARK)
    find_package(benchmark)
    if (benchmark_FOUND)
        message("Found Google benchmark, enabling support")
        set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS USE_BENCHMARK)
        target_link_libraries(${PROJECT_NAME} benchmark::benchmark)
        target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${benchmark_INCLUDE_DIRS})
    else ()
        message("Google benchmark not found, disabling support")
    endif ()
endif ()

# Third party libraries
if (MSVC)
    find_package(png 1.6 REQUIRED)
    find_package(zlib REQUIRED)

    find_path(LIBZIP_INCLUDE_DIRS zip.h)
    find_library(LIBZIP_LIBRARIES zip)
else ()
    PKG_CHECK_MODULES(LIBZIP REQUIRED IMPORTED_TARGET libzip>=1.0)
    PKG_CHECK_MODULES(ZLIB REQUIRED IMPORTED_TARGET zlib)

    PKG_CHECK_MODULES(PNG IMPORTED_TARGET libpng>=1.6)
    if (NOT PNG_FOUND)
        PKG_CHECK_MODULES(PNG IMPORTED_TARGET libpng16)
        if (NOT PNG_FOUND)
            PKG_CHECK_MODULES(PNG IMPORTED_TARGET libpng>=1.2)
            if (NOT PNG_FOUND)
                PKG_CHECK_MODULES(PNG REQUIRED IMPORTED_TARGET libpng12)
            endif ()
        endif ()
    endif ()
endif ()

if (STATIC)
    target_link_libraries(${PROJECT_NAME}
                                     ${PNG_STATIC_LIBRARIES}
                                     ${ZLIB_STATIC_LIBRARIES}
                                     ${LIBZIP_STATIC_LIBRARIES})
else ()
    if (NOT MSVC)
        target_link_libraries(${PROJECT_NAME}
                                     PkgConfig::PNG
                                     PkgConfig::ZLIB
                                     PkgConfig::LIBZIP)
    else ()
        target_link_libraries(${PROJECT_NAME}
                                     ${PNG_LIBRARIES}
                                     ${ZLIB_LIBRARIES}
                                     ${LIBZIP_LIBRARIES})
    endif ()
endif ()

if (MINGW)
    # Hardcode libraries used on mingw
    target_link_libraries(${PROJECT_NAME} crypto ws2_32 tasn1 unistring iconv p11-kit hogweed gmp nettle)
    # Link in libssp
    target_link_libraries(${PROJECT_NAME} -fstack-protector-strong)
endif()

if (UNIX AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "BSD")
    # Include libdl for dlopen
    target_link_libraries(${PROJECT_NAME} dl)
endif ()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

if (NOT MINGW AND NOT MSVC)
    if (APPLE AND NOT MACOS_USE_DEPENDENCIES)
        execute_process(COMMAND brew --prefix icu4c OUTPUT_VARIABLE HOMEBREW_PREFIX_ICU OUTPUT_STRIP_TRAILING_WHITESPACE)
        # Needed for linking with non-broken icu on Apple platforms
        list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX_ICU}")
    endif ()
    # For unicode code page conversion.
    find_package(ICU 59.0 REQUIRED COMPONENTS uc)
        
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${ICU_INCLUDE_DIRS})
        
    if (STATIC)
        target_link_libraries(${PROJECT_NAME} ${ICU_STATIC_LIBRARIES})
    else ()
        target_link_libraries(${PROJECT_NAME} ${ICU_LIBRARIES})
    endif ()
endif ()

if (NOT DISABLE_TTF)
    if (STATIC)
        target_link_libraries(${PROJECT_NAME} ${FREETYPE_STATIC_LIBRARIES})
            
        if (UNIX AND NOT APPLE)
            target_link_libraries(${PROJECT_NAME} ${FONTCONFIG_STATIC_LIBRARIES})
        endif ()
    else ()
        if (NOT MSVC)
            target_link_libraries(${PROJECT_NAME} PkgConfig::FREETYPE)
        else ()
            target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})
        endif ()
            
        if (UNIX AND NOT APPLE)
            target_link_libraries(${PROJECT_NAME} ${FONTCONFIG_LIBRARIES})
        endif ()
    endif ()

    if (${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
        target_link_libraries(${PROJECT_NAME} -L${OPENBSD_X11BASE}/lib)
    endif ()
endif ()

if (HAVE_DISCORD_RPC)
    if(${DISCORDRPC_FOUND})
        target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${DISCORDRPC_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} ${DISCORDRPC_LIBRARY})
    else()
        target_link_libraries(libopenrct2 discord-rpc)
    endif()
endif()

# Includes
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${LIBZIP_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${PNG_INCLUDE_DIRS}
                                                          ${ZLIB_INCLUDE_DIRS})
include_directories(${PROJECT_NAME} SYSTEM ${CMAKE_CURRENT_LIST_DIR}/../thirdparty)

# To avoid unnecessary rebuilds set the current branch and
# short sha1 only for the two files that use these
# definitions: Version.cpp and Crash/Platform.cpp
if (NOT OPENRCT2_VERSION_TAG STREQUAL "")
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/Version.cpp ${CMAKE_CURRENT_LIST_DIR}/Crash/Platform.cpp
        PROPERTY COMPILE_DEFINITIONS
        OPENRCT2_VERSION_TAG="${OPENRCT2_VERSION_TAG}")
endif()

if (NOT OPENRCT2_BRANCH STREQUAL "master" AND NOT OPENRCT2_BRANCH STREQUAL "")
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/Version.cpp ${CMAKE_CURRENT_LIST_DIR}/Crash/Platform.cpp
        APPEND PROPERTY COMPILE_DEFINITIONS
        OPENRCT2_BRANCH="${OPENRCT2_BRANCH}")
endif()

if (NOT OPENRCT2_COMMIT_SHA1_SHORT STREQUAL "HEAD" AND NOT OPENRCT2_COMMIT_SHA1_SHORT STREQUAL "")
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/Version.cpp ${CMAKE_CURRENT_LIST_DIR}/Crash/Platform.cpp
        APPEND PROPERTY COMPILE_DEFINITIONS
        OPENRCT2_COMMIT_SHA1_SHORT="${OPENRCT2_COMMIT_SHA1_SHORT}")
endif()

if((X86 OR X86_64) AND NOT MSVC)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/drawing/SSE41Drawing.cpp PROPERTIES COMPILE_FLAGS -msse4.1)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/drawing/AVX2Drawing.cpp PROPERTIES COMPILE_FLAGS -mavx2)
endif()

# Add headers check to verify all headers carry their dependencies.
# Only valid for Clang for now:
# - GCC 8 does not support -Wno-pragma-once-outside-header
# - Other compilers status unknown
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_library(${PROJECT_NAME}-headers-check OBJECT ${OPENRCT2_CORE_HEADERS})
    set_target_properties(${PROJECT_NAME}-headers-check PROPERTIES LINKER_LANGUAGE CXX)
    set_source_files_properties(${OPENRCT2_CORE_HEADERS} PROPERTIES LANGUAGE CXX)
    add_definitions("-x c++ -Wno-pragma-once-outside-header -Wno-unused-const-variable")
    get_target_property(LIBOPENRCT2_INCLUDE_DIRS ${PROJECT_NAME} INCLUDE_DIRECTORIES)
    set_target_properties(${PROJECT_NAME}-headers-check PROPERTIES INCLUDE_DIRECTORIES "${LIBOPENRCT2_INCLUDE_DIRS}")
else ()
    # Dummy target to ease invocation
    add_custom_target(${PROJECT_NAME}-headers-check)
endif ()
