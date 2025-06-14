file(GLOB_RECURSE OPENRCT2_CORE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE OPENRCT2_CORE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h"
                                        "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

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

add_library(libopenrct2 ${OPENRCT2_CORE_SOURCES} ${OPENRCT2_CORE_MM_SOURCES} ${OPENRCT2_DUKTAPE_SOURCES})
add_library(OpenRCT2::libopenrct2 ALIAS libopenrct2)

if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13")
	message(WARNING "Buggy GCC 12 detected! Disabling some warnings")
	set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/localisation/FormatCodes.cpp" PROPERTIES COMPILE_FLAGS "-Wno-restrict")
	set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/platform/Platform.Posix.cpp" PROPERTIES COMPILE_FLAGS "-Wno-restrict")
	set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/ride/Ride.cpp" PROPERTIES COMPILE_FLAGS "-Wno-null-dereference")
	set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/scenario/Scenario.cpp" PROPERTIES COMPILE_FLAGS "-Wno-restrict")
endif()
if (APPLE)
    target_link_platform_libraries(libopenrct2)
endif ()
set_target_properties(libopenrct2 PROPERTIES PREFIX "")
SET_CHECK_CXX_FLAGS(libopenrct2)

if (NOT DISABLE_NETWORK OR NOT DISABLE_HTTP)
    if (WIN32)
        target_link_libraries(libopenrct2 bcrypt)
    else ()
        if (APPLE AND NOT MACOS_USE_DEPENDENCIES)
            # note - we are not yet compatible with openssl3, which is now default brew version
            execute_process(COMMAND brew --prefix openssl@1.1 OUTPUT_VARIABLE HOMEBREW_PREFIX_OPENSSL OUTPUT_STRIP_TRAILING_WHITESPACE)
            # Needed for linking with non-broken OpenSSL on Apple platforms
            list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX_OPENSSL}")
        endif ()
        find_package(OpenSSL 1.0.0 REQUIRED)

        target_include_directories(libopenrct2 SYSTEM PUBLIC ${OPENSSL_INCLUDE_DIR})

        if(STATIC)
            target_link_libraries(libopenrct2 ${SSL_STATIC_LIBRARIES})
        else ()
            target_link_libraries(libopenrct2 ${OPENSSL_LIBRARIES})
        endif()
    endif()
endif ()

if (NOT DISABLE_NETWORK AND WIN32)
    target_link_libraries(libopenrct2 ws2_32 crypt32 wldap32 version winmm imm32 advapi32 shell32 ole32)
endif ()

if (NOT DISABLE_HTTP)
    if (WIN32)
        target_link_libraries(libopenrct2 winhttp)
    else ()
        PKG_CHECK_MODULES(LIBCURL REQUIRED libcurl)

        target_include_directories(libopenrct2 SYSTEM PRIVATE ${LIBCURL_INCLUDE_DIRS})

        if (STATIC)
            target_link_libraries(libopenrct2 ${LIBCURL_STATIC_LIBRARIES})
        else ()
            target_link_libraries(libopenrct2 ${LIBCURL_LIBRARIES})
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
        
    target_include_directories(libopenrct2 SYSTEM PRIVATE ${FREETYPE_INCLUDE_DIRS})
        
    if (UNIX AND NOT APPLE)
        target_include_directories(libopenrct2 SYSTEM PRIVATE ${FONTCONFIG_INCLUDE_DIRS})
    endif ()
endif ()

if (NOT DISABLE_GOOGLE_BENCHMARK)
    find_package(benchmark)
    if (benchmark_FOUND)
        message("Found Google benchmark, enabling support")
        set_target_properties(libopenrct2 PROPERTIES COMPILE_DEFINITIONS USE_BENCHMARK)
        target_link_libraries(libopenrct2 benchmark::benchmark)
        target_include_directories(libopenrct2 SYSTEM PRIVATE ${benchmark_INCLUDE_DIRS})
    else ()
        message("Google benchmark not found, disabling support")
    endif ()
endif ()

# Third party libraries
if (EMSCRIPTEN)
    target_include_directories(libopenrct2 SYSTEM PRIVATE ${ICU_INCLUDE_DIR})
    set(USE_FLAGS "${EMSCRIPTEN_FLAGS}")
    if (NOT DISABLE_VORBIS)
        set(USE_FLAGS "${USE_FLAGS} -s USE_VORBIS=1 -s USE_OGG=1")
    endif ()
    set(SHARED_FLAGS "-fexceptions")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS} ${SHARED_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSCRIPTEN_LDFLAGS} --bind ${SHARED_FLAGS}")
    find_package(SpeexDSP REQUIRED)
elseif (MSVC)
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
    target_link_libraries(libopenrct2
                                     ${PNG_STATIC_LIBRARIES}
                                     ${ZLIB_STATIC_LIBRARIES}
                                     ${LIBZIP_STATIC_LIBRARIES})
else ()
    if (NOT MSVC AND NOT EMSCRIPTEN)
        target_link_libraries(libopenrct2
                                     PkgConfig::PNG
                                     PkgConfig::ZLIB
                                     PkgConfig::LIBZIP)
    else ()
        target_link_libraries(libopenrct2
                                     ${PNG_LIBRARIES}
                                     ${ZLIB_LIBRARIES}
                                     ${LIBZIP_LIBRARIES})
    endif ()
endif ()

if (MINGW)
    # Hardcode libraries used on mingw
    target_link_libraries(libopenrct2 crypto ws2_32 tasn1 unistring iconv p11-kit hogweed gmp nettle)
    # Link in libssp
    target_link_libraries(libopenrct2 -fstack-protector-strong)
endif()

if (UNIX AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "BSD")
    # Include libdl for dlopen
    target_link_libraries(libopenrct2 dl)
endif ()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(libopenrct2 Threads::Threads)

# For some reason, these flags break the check for pthreads. Add them after.
if (EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_FUNCTIONS=_GetVersion,_main --js-library ${ROOT_DIR}/emscripten/deps.js")
endif()

if (NOT MINGW AND NOT MSVC AND NOT EMSCRIPTEN)
    if (APPLE AND NOT MACOS_USE_DEPENDENCIES)
        execute_process(COMMAND brew --prefix icu4c OUTPUT_VARIABLE HOMEBREW_PREFIX_ICU OUTPUT_STRIP_TRAILING_WHITESPACE)
        # Needed for linking with non-broken icu on Apple platforms
        list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX_ICU}")
    endif ()
    # For unicode code page conversion.
    find_package(ICU 59.0 REQUIRED COMPONENTS uc)
        
    target_include_directories(libopenrct2 SYSTEM PRIVATE ${ICU_INCLUDE_DIRS})
        
    if (STATIC)
        target_link_libraries(libopenrct2 ${ICU_STATIC_LIBRARIES})
    else ()
        target_link_libraries(libopenrct2 ${ICU_LIBRARIES})
    endif ()
endif ()

if (NOT DISABLE_TTF)
    if (STATIC)
        target_link_libraries(libopenrct2 ${FREETYPE_STATIC_LIBRARIES})
            
        if (UNIX AND NOT APPLE)
            target_link_libraries(libopenrct2 ${FONTCONFIG_STATIC_LIBRARIES})
        endif ()
    else ()
        if (NOT MSVC)
            target_link_libraries(libopenrct2 PkgConfig::FREETYPE)
        else ()
            target_link_libraries(libopenrct2 ${FREETYPE_LIBRARIES})
        endif ()
            
        if (UNIX AND NOT APPLE)
            target_link_libraries(libopenrct2 ${FONTCONFIG_LIBRARIES})
        endif ()
    endif ()

    if (${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
        target_link_libraries(libopenrct2 -L${OPENBSD_X11BASE}/lib)
    endif ()
endif ()

if (HAVE_DISCORD_RPC)
    if(${DISCORDRPC_FOUND})
        target_include_directories(libopenrct2 SYSTEM PRIVATE ${DISCORDRPC_INCLUDE_DIR})
        target_link_libraries(libopenrct2 ${DISCORDRPC_LIBRARY})
    else()
        target_link_libraries(libopenrct2 discord-rpc)
    endif()
endif()

# Includes
target_include_directories(libopenrct2 SYSTEM PRIVATE ${LIBZIP_INCLUDE_DIRS})
target_include_directories(libopenrct2 SYSTEM PRIVATE ${PNG_INCLUDE_DIRS}
                                                          ${ZLIB_INCLUDE_DIRS})
include_directories(libopenrct2 SYSTEM ${CMAKE_CURRENT_LIST_DIR}/../thirdparty)

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

if((X86 OR X86_64) AND NOT MSVC AND NOT EMSCRIPTEN)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/drawing/SSE41Drawing.cpp PROPERTIES COMPILE_FLAGS -msse4.1)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/drawing/AVX2Drawing.cpp PROPERTIES COMPILE_FLAGS -mavx2)
endif()

# Add headers check to verify all headers carry their dependencies.
# Only valid for Clang for now:
# - GCC 8 does not support -Wno-pragma-once-outside-header
# - Other compilers status unknown
if (ENABLE_HEADERS_CHECK AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_library(libopenrct2-headers-check OBJECT ${OPENRCT2_CORE_HEADERS})
    set_target_properties(libopenrct2-headers-check PROPERTIES LINKER_LANGUAGE CXX)
    set_source_files_properties(${OPENRCT2_CORE_HEADERS} PROPERTIES LANGUAGE CXX)
    add_definitions("-x c++ -Wno-pragma-once-outside-header -Wno-unused-const-variable")
    get_target_property(LIBOPENRCT2_INCLUDE_DIRS libopenrct2 INCLUDE_DIRECTORIES)
    set_target_properties(libopenrct2-headers-check PROPERTIES INCLUDE_DIRECTORIES "${LIBOPENRCT2_INCLUDE_DIRS}")
else ()
    # Dummy target to ease invocation
    add_custom_target(libopenrct2-headers-check)
endif ()

if (UNIX)
    add_definitions(-D_FILE_OFFSET_BITS=64)
endif ()

# Defines
target_compile_definitions(libopenrct2 PUBLIC
    $<$<BOOL:${USE_MMAP}>:USE_MMAP>
    $<$<BOOL:${DISABLE_NETWORK}>:DISABLE_NETWORK>
    $<$<BOOL:${DISABLE_HTTP}>:DISABLE_HTTP>
    $<$<BOOL:${DISABLE_TTF}>:DISABLE_TTF>
    $<$<BOOL:${DISABLE_VERSION_CHECKER}>:DISABLE_VERSION_CHECKER>
    $<$<BOOL:${ENABLE_SCRIPTING}>:ENABLE_SCRIPTING>
)

target_compile_options(libopenrct2 PUBLIC $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>)
target_link_options(libopenrct2 PUBLIC $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>)

target_compile_options(libopenrct2 PUBLIC $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>)
target_link_options(libopenrct2 PUBLIC  $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>)

if (CMAKE_LIBRARY_ARCHITECTURE MATCHES "arm-linux-gnueabihf")
    message(STATUS "Linking to armhf libs; adding libatomic")
    target_link_libraries(libopenrct2 atomic)
endif()
