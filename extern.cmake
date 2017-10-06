add_library(openrct2-deps-iface INTERFACE)
add_library(openrct2::deps ALIAS openrct2-deps-iface)

if(MSVC)
    set(deps_zip "${CMAKE_CURRENT_BINARY_DIR}/deps.zip")
    include(ExternalProject)
    set(deps_dir "${CMAKE_CURRENT_BINARY_DIR}/deps")
    ExternalProject_Add(
        OpenRCT2-Deps
        URL "https://github.com/OpenRCT2/Dependencies/releases/download/v13/openrct2-libs-vs2017.zip"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        SOURCE_DIR "${deps_dir}"
        USES_TERMINAL_DOWNLOAD 1
        )
    add_library(openrct2-deps-static STATIC IMPORTED)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(arch x64)
    else()
        set(arch x86)
    endif()
    set(includes
        "${deps_dir}/include"
        "${deps_dir}/include/SDL2"
        )
    set(lib_file "${deps_dir}/openrct2-libs-vs2017-${arch}.lib")
    add_custom_command(OUTPUT "${lib_file}" DEPENDS OpenRCT2-Deps)
    add_custom_target(get-deps DEPENDS "${lib_file}")
    set_target_properties(openrct2-deps-static PROPERTIES
        IMPORTED_LOCATION "${lib_file}"
        INTERFACE_INCLUDE_DIRECTORIES "${includes}"
        )
    target_link_libraries(openrct2-deps-iface INTERFACE openrct2-deps-static)
endif()

if(ENABLE_NETWORK OR ENABLE_HTTP_TWITCH)
    if(NOT MSVC)
        find_package(OpenSSL REQUIRED)
        target_link_libraries(openrct2-flags-iface INTERFACE
            OpenSSL::SSL
            OpenSSL::Crypto
            )
    endif()
    if(WIN32)
        target_link_libraries(openrct2-deps-iface INTERFACE ws2_32)
    endif()
endif()

target_link_libraries(openrct2-flags-iface INTERFACE openrct2-deps-iface)