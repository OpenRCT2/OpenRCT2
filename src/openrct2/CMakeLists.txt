# CMAKE project for libopenrct2 (core OpenRCT2 component)
cmake_minimum_required(VERSION 3.0)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

# Sources
file(GLOB_RECURSE OPENRCT2_CORE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.h"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
if (APPLE)
    file(GLOB_RECURSE OPENRCT2_CORE_MM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.m")
    set_source_files_properties(${OPENRCT2_CORE_MM_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c -fmodules")
endif ()


# Outputs
add_library(libopenrct2 SHARED ${OPENRCT2_CORE_SOURCES} ${OPENRCT2_CORE_MM_SOURCES} ${RCT2_SECTIONS})
set_target_properties(libopenrct2 PROPERTIES PREFIX "")
set_target_properties(libopenrct2 PROPERTIES COMPILE_FLAGS "-Wundef")

# Includes
target_include_directories(libopenrct2 SYSTEM PRIVATE ${LIBZIP_INCLUDE_DIRS})
target_include_directories(libopenrct2 PRIVATE ${SDL2_INCLUDE_DIRS}
                                              ${JANSSON_INCLUDE_DIRS}
                                              ${SPEEX_INCLUDE_DIRS}
                                              ${PNG_INCLUDE_DIRS}
                                              ${ZLIB_INCLUDE_DIRS})

if (STATIC)
    set(STATIC_START "-static")
    SET(REQUIREDLIBS ${PNG_STATIC_LIBRARIES} ${JANSSON_STATIC_LIBRARIES} ${ZLIB_STATIC_LIBRARIES} ${SSL_STATIC_LIBRARIES} ${LIBZIP_STATIC_LIBRARIES})
else (STATIC)
    SET(REQUIREDLIBS ${PNG_LIBRARIES} ${JANSSON_LIBRARIES} ${ZLIB_LIBRARIES} ${SSL_LIBRARIES} ${LIBZIP_LIBRARIES})
endif (STATIC)

if (NOT DISABLE_HTTP_TWITCH OR NOT DISABLE_NETWORK)
    target_include_directories(libopenrct2 PRIVATE ${LIBCURL_INCLUDE_DIRS})
endif ()
if (NOT DISABLE_NETWORK)
    target_include_directories(libopenrct2 PUBLIC ${OPENSSL_INCLUDE_DIR})
endif ()
if (NOT DISABLE_TTF AND UNIX AND NOT APPLE)
    target_include_directories(libopenrct2 PRIVATE ${FONTCONFIG_INCLUDE_DIRS})
endif ()

target_link_libraries(libopenrct2 ${REQUIREDLIBS} ${SDL2LIBS} ${FONTCONFIG_LIBRARIES} ${HTTPLIBS})

# macOS builds fail on the use of tmpnam otherwise (#4959)
if(APPLE)
    set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -Wno-error=deprecated-declarations -Wno-error=objc-method-access")
endif()

if (NOT DISABLE_RCT2)
    # Disable optimizations for addresses.c for all compilers, to allow optimized
    # builds without need for -fno-omit-frame-pointer
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/rct2/addresses.c PROPERTIES COMPILE_FLAGS -O0)
endif ()
