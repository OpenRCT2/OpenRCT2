# CMAKE project for openrct2
cmake_minimum_required(VERSION 3.9)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()

project(openrct2 VERSION 0.1.5 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Put all runtime binaries in <build>/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
# Copy data into the bin dir. This permits running easily out of the build directory.
file(COPY data DESTINATION bin)

include(CTest)
include(GNUInstallDirs)

set(CMAKE_MACOSX_RPATH 1)

option(FORCE32 "Force 32-bit build. It will add `-m32` to compiler flags.")
option(PORTABLE "Create a portable build (-rpath=$ORIGIN)" OFF)
option(DOWNLOAD_TITLE_SEQUENCES "Download title sequences during installation." ON)

if(DOWNLOAD_TITLE_SEQUENCES)
    # Store the downloaded title sequences in the build dir. This lets us see
    # the title sequences from a build-dir run as well.
    set(title_seq_dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data/title")
    # Just name one of the files that gets placed down
    set(sequence_file "${title_seq_dir}/openrct2.parkseq")
    # Download is done in an external CMake script which we invoke
    add_custom_command(
        OUTPUT "${sequence_file}"
        COMMAND "${CMAKE_COMMAND}"
            "-DSCRATCH_DIR=${CMAKE_CURRENT_BINARY_DIR}"
            "-DDEST=${title_seq_dir}"
            -P "${PROJECT_SOURCE_DIR}/get-title-sequences.cmake"
        COMMENT "Downloading title sequences"
        USES_TERMINAL
    )
    add_custom_target(get-title-sequences ALL DEPENDS "${sequence_file}")
    # Install the directory we created during the download
    install(DIRECTORY "${title_seq_dir}" DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
endif()

include(git.cmake)
include(options.cmake)
include(flags.cmake)
include(extern.cmake)
include(sprites.cmake)

# Include the actual source directories
add_subdirectory(src)

# Include tests
if(BUILD_TESTING)
    enable_testing()
    if(UNIX AND NOT USE_MMAP AND ENABLE_RCT2)
        add_subdirectory(test/testpaint)
    endif()
    add_subdirectory(test/tests)
endif()

# Various files to install
# Don't recurse, grab all *.txt and *.md files
file(GLOB DOC_FILES "${PROJECT_SOURCE_DIR}/distribution/*.txt")
list(APPEND DOC_FILES "${PROJECT_SOURCE_DIR}/contributors.md"
                      "${PROJECT_SOURCE_DIR}/licence.txt")
install(FILES ${DOC_FILES} DESTINATION "${CMAKE_INSTALL_DOCDIR}")

install(DIRECTORY "data/" DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")

install(FILES "distribution/linux/openrct2.appdata.xml" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/metainfo")
install(FILES "resources/logo/icon_x16.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x24.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/24x24/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x32.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x48.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x64.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x96.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/96x96/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x128.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x256.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_flag.svg" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps" RENAME "openrct2.svg")
install(FILES "distribution/linux/openrct2.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
install(FILES "distribution/linux/openrct2-savegame.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
install(FILES "distribution/linux/openrct2-scenario.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
install(FILES "distribution/linux/openrct2-mimeinfo.xml" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/mime/packages/" RENAME "openrct2.xml")
install(DIRECTORY "distribution/man/" DESTINATION "${CMAKE_INSTALL_MANDIR}/man6" FILES_MATCHING PATTERN "*.6")

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
include(CPack)