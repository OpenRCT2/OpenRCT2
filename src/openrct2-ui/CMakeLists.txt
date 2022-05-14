# CMAKE project for openrct2-ui (UI build of OpenRCT2)
cmake_minimum_required(VERSION 3.9)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

# Options
option(DISABLE_OPENGL "Disable OpenGL support.")

# Third party libraries
if (MSVC)
    find_package(SDL2 REQUIRED)
    find_library(SPEEX_LDFLAGS libspeexdsp)
else ()
    PKG_CHECK_MODULES(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    PKG_CHECK_MODULES(SPEEX REQUIRED IMPORTED_TARGET speexdsp)
    PKG_CHECK_MODULES(OGG REQUIRED IMPORTED_TARGET ogg)
    PKG_CHECK_MODULES(VORBISFILE REQUIRED IMPORTED_TARGET vorbisfile)
    PKG_CHECK_MODULES(FLAC REQUIRED IMPORTED_TARGET flac)
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
project(openrct2 CXX)
add_executable(${PROJECT_NAME} ${OPENRCT2_UI_SOURCES} ${OPENRCT2_UI_MM_SOURCES})
SET_CHECK_CXX_FLAGS(${PROJECT_NAME})
ipo_set_target_properties(${PROJECT_NAME})

# mingw builds cannot use the PkgConfig imported targets
if (NOT MSVC AND NOT WIN32)
    target_link_libraries(${PROJECT_NAME} "libopenrct2"
                                    PkgConfig::SDL2
                                    PkgConfig::SPEEX
                                    PkgConfig::OGG
                                    PkgConfig::VORBISFILE
                                    PkgConfig::FLAC)
else ()
    target_link_libraries(${PROJECT_NAME} "libopenrct2"
                                    ${SDL2_LDFLAGS}
                                    ${SPEEX_LDFLAGS}
                                    ${OGG_LDFLAGS}
                                    ${VORBIS_LDFLAGS}
                                    ${FLAC_LDFLAGS})
endif ()
target_link_platform_libraries(${PROJECT_NAME})

if (NOT DISABLE_OPENGL)
    if (WIN32)
        target_link_libraries(${PROJECT_NAME} opengl32)
    elseif (APPLE)
        target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY})
    else ()
        target_link_libraries(${PROJECT_NAME} ${GL_LIBRARIES})
    endif ()
endif ()

# Includes
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/.."
                                              ${SPEEX_INCLUDE_DIRS}
                                              ${OGG_INCLUDE_DIRS}
                                              ${VORBIS_INCLUDE_DIRS}
                                              ${FLAC_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS}
                                                     "${CMAKE_CURRENT_LIST_DIR}/../thirdparty")

# Compiler flags
if (WIN32)
    # mingw complains about "%zu" not being a valid format specifier for printf, unless we
    # tell it that it is
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__USE_MINGW_ANSI_STDIO=1")
    target_link_libraries(${PROJECT_NAME} comdlg32)
    if (MSVC)
        target_link_libraries(${PROJECT_NAME} SDL2::SDL2-static)
        target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE SDL2::SDL2-static)
    else ()
        # mingw does not provide proper CMake information like other configurations
        find_path(SDL2_INCLUDE_DIRS SDL2/SDL.h)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            find_library(SDL2_LDFLAGS sdl2d)
        else ()
            find_library(SDL2_LDFLAGS sdl2)
        endif ()
        # Hardcode some of the libraries used by mingw builds
        target_link_libraries(${PROJECT_NAME} imm32 winmm setupapi version)
    endif ()
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
    add_library(${PROJECT_NAME}-headers-check OBJECT ${OPENRCT2_HEADERS_CHECK})
    set_target_properties(${PROJECT_NAME}-headers-check PROPERTIES LINKER_LANGUAGE CXX)
    set_source_files_properties(${OPENRCT2_HEADERS_CHECK} PROPERTIES LANGUAGE CXX)
    add_definitions("-x c++ -Wno-pragma-once-outside-header -Wno-unused-const-variable")
    get_target_property(OPENRCT2_INCLUDE_DIRS ${PROJECT_NAME} INCLUDE_DIRECTORIES)
    set_target_properties(${PROJECT_NAME}-headers-check PROPERTIES INCLUDE_DIRECTORIES "${OPENRCT2_INCLUDE_DIRS}")
endif ()


if(MACOS_BUNDLE)
    add_dependencies(${PROJECT_NAME} openrct2-cli)
    set(OUTPUT_NAME "OpenRCT2")
    set(MACOS_APP_NAME "${OUTPUT_NAME}.app")
    set(BUNDLE_FRAMEWORK_DIR "${MACOS_APP_NAME}/Contents/Frameworks")
    set(BUNDLE_RESOURCE_DIR "${MACOS_APP_NAME}/Contents/Resources")
    set(SOURCE_DATA_DIR "${ROOT_DIR}/data")

    # Add distribution sources
    target_sources(${PROJECT_NAME}
        PUBLIC ${ROOT_DIR}/distribution/readme.txt
        PUBLIC ${ROOT_DIR}/distribution/changelog.txt
        PUBLIC ${ROOT_DIR}/resources/mac/openrct2.icns
        )

    # Specify the resources to move to the bundle
    set(BUNDLE_RESOURCES
        ${ROOT_DIR}/distribution/readme.txt
        ${ROOT_DIR}/distribution/changelog.txt
        ${ROOT_DIR}/resources/mac/openrct2.icns
        )

    if (CMAKE_OSX_ARCHITECTURES MATCHES "${SYSTEM_MACOS_ARCH}")
        target_sources(${PROJECT_NAME}
        PUBLIC ${CMAKE_BINARY_DIR}/g2.dat
        )
        list(APPEND BUNDLE_RESOURCES ${CMAKE_BINARY_DIR}/g2.dat)
    endif()


    if(${OPENRCT2_BRANCH} EQUAL master)
        set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${OPENRCT2_VERSION_TAG}")
    else()
        set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${OPENRCT2_VERSION_TAG} ${OPENRCT2_BRANCH}")
    endif()

    set(PRODUCT_BUNDLE_IDENTIFIER "io.openrct2.OpenRCT2")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${OPENRCT2_COMMIT_SHA1_SHORT}")
    set(EXECUTABLE_NAME "${OUTPUT_NAME}")
    set(MACOSX_DEPLOYMENT_TARGET "${CMAKE_OSX_DEPLOYMENT_TARGET}")
    set(PRODUCT_NAME "${OUTPUT_NAME}")

    # copy data
    file(COPY ${SOURCE_DATA_DIR}/language DESTINATION "${BUNDLE_RESOURCE_DIR}")
    file(COPY ${SOURCE_DATA_DIR}/shaders DESTINATION "${BUNDLE_RESOURCE_DIR}")

    # download objects and sequences
    set(OBJECTS_DIR ${CMAKE_BINARY_DIR}/object)
    set(TITLE_SEQUENCE_DIR ${CMAKE_BINARY_DIR}/sequence)
    download_openrct2_zip(
        ZIP_VERSION ${OBJECTS_VERSION}
        DOWNLOAD_DIR ${OBJECTS_DIR}
        ZIP_URL ${OBJECTS_URL}
        SHA1 ${OBJECTS_SHA1}
    )

    download_openrct2_zip(
        ZIP_VERSION ${TITLE_SEQUENCE_VERSION}
        DOWNLOAD_DIR ${TITLE_SEQUENCE_DIR}
        ZIP_URL ${TITLE_SEQUENCE_URL}
        SHA1 ${TITLE_SEQUENCE_SHA1}
    )
    
    file(COPY ${OBJECTS_DIR} DESTINATION "${BUNDLE_RESOURCE_DIR}")
    file(COPY ${TITLE_SEQUENCE_DIR} DESTINATION "${BUNDLE_RESOURCE_DIR}")

    # Create as a bundle
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE ON
        OUTPUT_NAME ${OUTPUT_NAME}
        MACOSX_BUNDLE_BUNDLE_NAME ${OUTPUT_NAME}
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/distribution/macos/Info.plist
        RESOURCE "${BUNDLE_RESOURCES}")

    install(CODE "
        include(BundleUtilities)
        fixup_bundle(${CMAKE_BINARY_DIR}/${MACOS_APP_NAME} \"\" \"\")
        "  BUNDLE DESTINATION ${CMAKE_BINARY_DIR}
    )
endif ()
