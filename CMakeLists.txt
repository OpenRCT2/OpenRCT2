# CMAKE project for openrct2
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()
set(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

set(TITLE_SEQUENCE_URL  "https://github.com/OpenRCT2/title-sequences/releases/download/v0.0.5/title-sequence-v0.0.5.zip")
set(TITLE_SEQUENCE_SHA1 "79ffb2585d12abcbfce205d7696e3472a504b005")

option(FORCE32 "Force 32-bit build. It will add `-m32` to compiler flags.")
option(CLI_ONLY "CLI only build of OpenRCT2")
option(WITH_TESTS "Build tests")

# Define current git branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${ROOT_DIR}
    OUTPUT_VARIABLE OPENRCT2_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
add_definitions(-DOPENRCT2_BRANCH="${OPENRCT2_BRANCH}")

# Define commit hash
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${ROOT_DIR}
    OUTPUT_VARIABLE OPENRCT2_COMMIT_SHA1
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
add_definitions(-DOPENRCT2_COMMIT_SHA1="${OPENRCT2_COMMIT_SHA1}")

# Define short commit hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${ROOT_DIR}
    OUTPUT_VARIABLE OPENRCT2_COMMIT_SHA1_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Tagged builds are not meant to display commit info
if (NOT OPENRCT2_COMMIT_SHA1_SHORT STREQUAL "HEAD")
    add_definitions(-DOPENRCT2_COMMIT_SHA1_SHORT="${OPENRCT2_COMMIT_SHA1_SHORT}")
endif()

# Include sub-projects
include("${ROOT_DIR}/src/openrct2/CMakeLists.txt" NO_POLICY_SCOPE)
if (CLI_ONLY)
    include("${ROOT_DIR}/src/openrct2-cli/CMakeLists.txt" NO_POLICY_SCOPE)
else ()
    include("${ROOT_DIR}/src/openrct2-ui/CMakeLists.txt" NO_POLICY_SCOPE)
endif ()

# g2
add_custom_command(
    OUTPUT g2.dat
    COMMAND ./openrct2 sprite build ${CMAKE_BINARY_DIR}/g2.dat ${ROOT_DIR}/resources/g2/sprites.json
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_target(g2 DEPENDS ${PROJECT} g2.dat)

# Include tests
if (WITH_TESTS)
    enable_testing()
    if (UNIX AND (NOT USE_MMAP) AND (NOT DISABLE_RCT2) AND (FORCE32))
        include("${ROOT_DIR}/test/testpaint/CMakeLists.txt" NO_POLICY_SCOPE)
    endif ()
    include("${ROOT_DIR}/test/tests/CMakeLists.txt" NO_POLICY_SCOPE)
endif ()

# Install
# Don't recurse, grab all *.txt and *.md files
file(GLOB DOC_FILES "${ROOT_DIR}/distribution/*.txt")
list(APPEND DOC_FILES "${ROOT_DIR}/contributors.md"
                      "${ROOT_DIR}/licence.txt")

# CMake does not allow specifying a dependency chain which includes built-in
# targets, like `install`, so we have to trick it and execute dependency ourselves.
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" --build \"${CMAKE_CURRENT_BINARY_DIR}\" --target g2)")
install(CODE "file(DOWNLOAD ${TITLE_SEQUENCE_URL} \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/title/title-sequences.zip EXPECTED_HASH SHA1=${TITLE_SEQUENCE_SHA1} SHOW_PROGRESS)")
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E chdir \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/title/ \"${CMAKE_COMMAND}\" -E tar xvf title-sequences.zip)")
install(CODE "file(REMOVE \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/title/title-sequences.zip)")
install(TARGETS "libopenrct2" LIBRARY DESTINATION lib)
install(TARGETS "openrct2" RUNTIME DESTINATION bin)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/g2.dat" DESTINATION "share/openrct2")
install(DIRECTORY "data/" DESTINATION "share/openrct2")
install(FILES ${DOC_FILES} DESTINATION "share/doc/openrct2")
install(FILES "resources/logo/icon_x16.png" DESTINATION "share/icons/hicolor/16x16/apps RENAME openrct2.png")
install(FILES "resources/logo/icon_x32.png" DESTINATION "share/icons/hicolor/32x32/apps RENAME openrct2.png")
install(FILES "resources/logo/icon_x64.png" DESTINATION "share/icons/hicolor/64x64/apps RENAME openrct2.png")
install(FILES "resources/logo/icon_x128.png" DESTINATION "share/icons/hicolor/128x128/apps RENAME openrct2.png")
install(FILES "resources/logo/icon_x256.png" DESTINATION "share/icons/hicolor/256x256/apps RENAME openrct2.png")
install(FILES "resources/logo/icon_flag.svg" DESTINATION "share/icons/hicolor/scalable/apps RENAME openrct2.svg")
install(FILES "distribution/linux/openrct2.desktop" DESTINATION "share/applications")
