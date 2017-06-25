# CMAKE project for libopenrct2 (core OpenRCT2 component)
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

# CMake dependencies
include(FindPkgConfig)

# Options
option(STATIC "Create a static build.")
option(USE_MMAP "Use mmap to try loading rct2's data segment into memory.")
option(DISABLE_RCT2 "Build a standalone version, without using code and data segments from vanilla. On by default." ON)

option(DISABLE_HTTP_TWITCH "Disable HTTP and Twitch support.")
option(DISABLE_NETWORK "Disable multiplayer functionality. Mainly for testing.")
option(DISABLE_TTF "Disable support for TTF provided by freetype2.")
option(ENABLE_LIGHTFX "Enable lighting effects." ON)

if (NOT DISABLE_RCT2)
    if (WIN32)
        message(FATAL_ERROR "DISABLE_RCT2 not supported for Windows")
    endif ()
    if (NOT FORCE32)
        message(FATAL_ERROR "DISABLE_RCT2 requires FORCE32")
    endif ()
endif ()

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
file(GLOB_RECURSE OPENRCT2_CORE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.h"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
if (APPLE)
    file(GLOB_RECURSE OPENRCT2_CORE_MM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.m")
    set_source_files_properties(${OPENRCT2_CORE_MM_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c -fmodules")
endif ()

# Handle creating the rct2 text and data files on macOS and Linux
# See details in src/openrct2/rct2/interop.c:rct2_interop_setup_segment for how the values
# were derived.
if (NOT DISABLE_RCT2 AND UNIX)
    set(OPENRCT2_EXE "${ROOT_DIR}/openrct2.exe")
    add_custom_command(
        OUTPUT openrct2_text
        COMMAND dd if="${OPENRCT2_EXE}" of="${CMAKE_BINARY_DIR}/openrct2_text" bs=4096 skip=1 count=1187
        DEPENDS ${OPENRCT2_EXE}
    )
    add_custom_command(
        OUTPUT openrct2_data
        COMMAND dd if="${OPENRCT2_EXE}" of="${CMAKE_BINARY_DIR}/openrct2_data" bs=4096 skip=1188 count=318
        COMMAND dd if=/dev/zero of="${CMAKE_BINARY_DIR}/openrct2_data" bs=4096 seek=318 count=2630 conv=notrunc
        COMMAND dd if="${OPENRCT2_EXE}" of="${CMAKE_BINARY_DIR}/openrct2_data" bs=4096 skip=1506 seek=2948 count=1 conv=notrunc
        DEPENDS ${OPENRCT2_EXE}
    )
    add_custom_target(segfiles DEPENDS openrct2_text openrct2_data)
    if (NOT USE_MMAP)
        set(OBJ_FORMAT "elf32-i386")
        set(LINKER_SCRIPT "ld_script_i386.xc")
        if (APPLE)
            set(RCT2_SEGMENT_LINKER_FLAGS "-sectcreate rct2_text __text ${CMAKE_BINARY_DIR}/openrct2_text -sectcreate rct2_data __data ${CMAKE_BINARY_DIR}/openrct2_data -segaddr rct2_data 0x8a4000 -segprot rct2_data rwx rwx -segaddr rct2_text 0x401000 -segprot rct2_text rwx rwx -segaddr __TEXT 0x2000000 -read_only_relocs suppress")
        else ()
            # For Linux we have to use objcopy to wrap regular binaries into a linkable
            # format. We use specific section names which are then referenced in a
            # bespoke linker script so they can be placed at predefined VMAs.
            add_custom_command(
                OUTPUT openrct2_text_section.o
                COMMAND objcopy --input binary --output ${OBJ_FORMAT} --binary-architecture i386 openrct2_text openrct2_text_section.o --rename-section .data=.rct2_text,contents,alloc,load,readonly,code
                DEPENDS segfiles
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )
            add_custom_command(
                OUTPUT openrct2_data_section.o
                COMMAND objcopy --input binary --output ${OBJ_FORMAT} --binary-architecture i386 openrct2_data openrct2_data_section.o --rename-section .data=.rct2_data,contents,alloc,load,readonly,data
                DEPENDS segfiles
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )
            add_custom_target(linkable_sections DEPENDS openrct2_text_section.o openrct2_data_section.o)
            set_source_files_properties(
                openrct2_text_section.o openrct2_data_section.o
                PROPERTIES
                EXTERNAL_OBJECT true
                GENERATED true
            )
            # can't use GLOB here, as the files don't exist yet at cmake-time
            set(RCT2_SECTIONS "${CMAKE_BINARY_DIR}/openrct2_data_section.o" "${CMAKE_BINARY_DIR}/openrct2_text_section.o")
            set(RCT2_SEGMENT_LINKER_FLAGS "-Wl,-T,\"${ROOT_DIR}/distribution/linux/${LINKER_SCRIPT}\"")
        endif ()
    endif ()
elseif (USE_MMAP)
    # No dd here, can't extract data segment
    message(WARNING "Sorry, your platform is not supported, you have to extract data segment manually")
endif ()

# Outputs
set(PROJECT libopenrct2)
project(${PROJECT})
add_library(${PROJECT} SHARED ${OPENRCT2_CORE_SOURCES} ${OPENRCT2_CORE_MM_SOURCES} ${RCT2_SECTIONS})
set_target_properties(${PROJECT} PROPERTIES PREFIX "")
set_target_properties(${PROJECT} PROPERTIES COMPILE_FLAGS "-Wundef")

if (NOT DISABLE_RCT2)
    add_dependencies(${PROJECT} segfiles)
    if (NOT USE_MMAP)
        if (NOT APPLE)
            add_dependencies(${PROJECT} linkable_sections)
        endif ()
        set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS ${RCT2_SEGMENT_LINKER_FLAGS})
    endif ()
endif ()

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
    if (STATIC)
        target_link_libraries(${PROJECT} ${LIBCURL_STATIC_LIBRARIES}
                                         ${SSL_STATIC_LIBRARIES})
    else ()
        target_link_libraries(${PROJECT} ${LIBCURL_LIBRARIES}
                                         ${OPENSSL_LIBRARIES})
    endif ()
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

if (APPLE OR STATIC OR ${CMAKE_SYSTEM_NAME} MATCHES "BSD")
    find_library(ICONV_LIBRARIES NAMES iconv libiconv libiconv-2 c)
    target_link_libraries(${PROJECT} ${ICONV_LIBRARIES})
endif()

# Includes
target_include_directories(${PROJECT} SYSTEM PRIVATE ${LIBZIP_INCLUDE_DIRS})
target_include_directories(${PROJECT} PRIVATE ${JANSSON_INCLUDE_DIRS}
                                              ${PNG_INCLUDE_DIRS}
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

if (FORCE32)
    set(TARGET_M "-m32")
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 ${COMMON_COMPILE_OPTIONS} ${TARGET_M} -Wimplicit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 ${COMMON_COMPILE_OPTIONS} ${TARGET_M}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${TARGET_M}")

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
if (DISABLE_RCT2)
    add_definitions(-DNO_RCT2)
endif ()

if (NOT DISABLE_RCT2)
    # Disable optimizations for addresses.c for all compilers, to allow optimized
    # builds without need for -fno-omit-frame-pointer
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/rct2/addresses.c PROPERTIES COMPILE_FLAGS -O0)
endif ()
