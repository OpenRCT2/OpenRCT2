include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# 32-bit forcing
if(NOT MSVC AND FORCE_32BIT)
    target_compile_definitions(openrct2-flags-iface INTERFACE -m32)
    target_link_libraries(openrct2-flags-iface INTERFACE -m32)
endif()

# Validate
if(ENABLE_RCT2)
    # We require certain settings for using the original executable
    if(WIN32)
        message(SEND_ERROR "ENABLE_RCT2 is not supported on Windows")
    endif()
    if(NOT FORCE_32BIT)
        message(SEND_ERROR "ENABLE_RCT2 requires FORCE_32BIT")
    endif()
endif()

# Build an interface target to set all the compile options
add_library(openrct2-flags-iface INTERFACE)
add_library(openrct2::flags ALIAS openrct2-flags-iface)

# Check for required compilation flags
if(NOT MSVC)
    set(required_flags
        -fstrict-aliasing
        -Werror # Warnings as errors
        -Wundef
        -Wmissing-declarations
        -Winit-self
        -Wall
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-missing-braces
        -Wno-comment
        -Wshadow
        -Wmissing-declarations
        -Wnonnull
        -Wimplicit
        )
else()
    set(required_flags
        # Enable warnings
        # /W4
        /WX # Warnings as errors
        # Disable some warnings:
        /wd4068 # 'keyword' ignored on left of 'type'
        /wd4100 # Unused argument
        /wd4132 # Un-initialized const object
        /wd4200 # ext: zero-sized array in struct/union
        /wd4201 # ext: anonymous struct/union
        /wd4204 # ext: Non-constant aggregate init
        /wd4206 # ext: Empty translation unit
        /wd4221 # ext: 'ident' cannot be initialized using address of automatic variable
        /wd4244 # Lossy conversion
        /wd4245 # Sign/unsigned mismatch
        # Conformance
        /std:c++latest
        /permissive-
        # Misc
        /utf-8
        )
endif()

foreach(flag IN LISTS required_flags)
    string(MAKE_C_IDENTIFIER "HAVE_FLAG_${flag}" var)
    set(CMAKE_REQUIRED_QUIET TRUE)
    if(NOT DEFINED "${var}")
        message(STATUS "Checking support for required compile flag: ${flag}")
    endif()
    check_cxx_compiler_flag("${flag}" "${var}")
    if(NOT "${${var}}")
        message(SEND_ERROR "Your compiler does not support compile flag: ${flag}")
    else()
        target_compile_options(openrct2-flags-iface INTERFACE ${flag})
    endif()
endforeach()

if(NOT ENABLE_RCT2)
    target_compile_definitions(openrct2-flags-iface INTERFACE NO_RCT2)
endif()
if(ENABLE_LIGHTFX)
    target_compile_definitions(openrct2-flags-iface INTERFACE __ENABLE_LIGHTFX__)
endif()
if(NOT ENABLE_TTF)
    target_compile_definitions(openrct2-flags-iface INTERFACE NO_TTF)
endif()
if(NOT ENABLE_HTTP_TWITCH)
    target_compile_definitions(openrct2-flags-iface INTERFACE DISABLE_HTTP DISABLE_TWITCH)
endif()
if(NOT ENABLE_NETWORK)
    target_compile_definitions(openrct2-flags-iface INTERFACE DISABLE_NETWORK)
endif()
if(USE_MMAP_FOR_RCT2)
    target_compile_definitions(openrct2-flags-iface INTERFACE USE_MMAP)
endif()

if(MSVC)
    target_compile_definitions(openrct2-flags-iface INTERFACE
        _CRT_SECURE_NO_WARNINGS
        _USE_MATH_DEFINES
        _WINSOCK_DEPRECATED_NO_WARNINGS
        NOMINMAX
        CURL_STATICLIB
        ZIP_STATIC
        SDL_MAIN_HANDLED
        )
    target_link_libraries(openrct2-flags-iface INTERFACE
        -opt:nolbr
        -ignore:4099
        imm32.lib
        version.lib
        winmm.lib
        crypt32.lib
        wldap32.lib
        )
endif()

# Set the debug level macro
target_compile_definitions(openrct2-flags-iface INTERFACE DEBUG=${DEBUG_LEVEL})

# Turn on PIC
set_property(TARGET openrct2-flags-iface PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE TRUE)

if(NOT MSVC)
    # Ensure we have compiler version flags set
    # (CMake has trouble with the meta-features on MSVC)
    set(comp_features
        c_std_11 # C code should be compiled as C11
        cxx_std_14 # C++ code as C++14
        )
    set_target_properties(openrct2-flags-iface PROPERTIES
        INTERFACE_COMPILE_FEATURES "${comp_features}"
        )
endif()

if(APPLE)
    target_compile_options(openrtc2-flags INTERFACE -Wno-error=obj-method-access)
endif()

# Enable dlopen on Unix systems
if(UNIX AND NOT CMAKE_SYSTEM_NAME MATCHES BSD)
    target_link_libraries(openrct2-flags-iface INTERFACE dl)
endif()

# Enable multithreading
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
target_link_libraries(openrct2-flags-iface INTERFACE Threads::Threads)

if(APPLE OR CMAEK_SYSTEM_NAME MATCHES "BSD")
    find_library(ICONV_LIBRARY NAMES libiconv.a iconv libiconv libiconv-2)
    target_link_libraries(openrct2-flags-iface INTERFACE "${ICONV_LIBRARY}")
endif()