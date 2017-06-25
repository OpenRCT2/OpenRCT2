# CMAKE project for openrct2
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()

include(FindPkgConfig)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

set(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(CMAKE_MACOSX_RPATH 1)

set(TITLE_SEQUENCE_URL  "https://github.com/OpenRCT2/title-sequences/releases/download/v0.0.5/title-sequence-v0.0.5.zip")
set(TITLE_SEQUENCE_SHA1 "79ffb2585d12abcbfce205d7696e3472a504b005")

option(FORCE32 "Force 32-bit build. It will add `-m32` to compiler flags.")
option(WITH_TESTS "Build tests")
option(PORTABLE "Create a portable build (-rpath=$ORIGIN)" OFF)
option(DOWNLOAD_TITLE_SEQUENCES "Download title sequences during installation." ON)

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

function (ADD_CHECK_C_COMPILER_FLAG _CFLAGS _CACHE_VAR _FLAG)
    CHECK_C_COMPILER_FLAG("${_FLAG}" "${_CACHE_VAR}")
    if (${_CACHE_VAR})
        set(${_CFLAGS} "${${_CFLAGS}} ${_FLAG}" PARENT_SCOPE)
    else ()
        message(STATUS "Unsupported CFLAG: ${_FLAG}")
    endif ()
endfunction ()

function (ADD_CHECK_CXX_COMPILER_FLAG _CXXFLAGS _CACHE_VAR _FLAG)
    CHECK_CXX_COMPILER_FLAG("${_FLAG}" "${_CACHE_VAR}")
    if (${_CACHE_VAR})
        set(${_CXXFLAGS} "${${_CXXFLAGS}} ${_FLAG}" PARENT_SCOPE)
    else ()
        message(STATUS "Unsupported CXXFLAG: ${_FLAG}")
    endif ()
endfunction ()

# Launchpad turns on -Wdate-time for compilers that support it, this shouldn't break our build
ADD_CHECK_C_COMPILER_FLAG(CMAKE_C_FLAGS C_WARN_WRITE_STRINGS -Wno-error=date-time)
ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_WRITE_STRINGS -Wno-error=date-time)
ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_NULL_DEREFERENCE -Wnull-dereference)
ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_SUGGEST_FINAL_TYPES -Wsuggest-final-types)
ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_SUGGEST_FINAL_METHODS -Wsuggest-final-methods)
ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_SUGGEST_OVERRIDE -Wsuggest-override)
ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_DUPLICATED_COND -Wduplicated-cond)
ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_NON_VIRTUAL_DTOR -Wnon-virtual-dtor)
if (MINGW)
    ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_SUGGEST_OVERRIDE -Wno-error=redundant-decls)
endif ()

# Items below are not supported by ICC
ADD_CHECK_C_COMPILER_FLAG(CMAKE_C_FLAGS C_WARN_REDUNDANT_DECLS -Wredundant-decls)
ADD_CHECK_C_COMPILER_FLAG(CMAKE_C_FLAGS C_WARN_IGNORED_QUALIFIERS -Wignored-qualifiers)
ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_REDUNDANT_DECLS -Wredundant-decls)
ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_IGNORED_QUALIFIERS -Wignored-qualifiers)

# -Wstrict-overflow is only active when -fstrict-overflow is enabled, but -fstrict-overflow
# is enabled on -O2, -O3, -Os. This should help catch bugs locally before they reach Travis
# As of 2a435bf -Wstrict-overflow=1 passes, but higher values do not.
set(COMMON_COMPILE_OPTIONS "${COMMON_COMPILE_OPTIONS} -fstrict-overflow")
ADD_CHECK_C_COMPILER_FLAG(CMAKE_C_FLAGS C_WARN_STRICT_OVERFLOW -Wstrict-overflow=1)
ADD_CHECK_CXX_COMPILER_FLAG(CMAKE_CXX_FLAGS CXX_WARN_STRICT_OVERFLOW -Wstrict-overflow=1)


# Include sub-projects
include("${ROOT_DIR}/src/openrct2/CMakeLists.txt" NO_POLICY_SCOPE)
include("${ROOT_DIR}/src/openrct2-cli/CMakeLists.txt" NO_POLICY_SCOPE)
include("${ROOT_DIR}/src/openrct2-ui/CMakeLists.txt" NO_POLICY_SCOPE)

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
IF (DOWNLOAD_TITLE_SEQUENCES)
    install(CODE "file(DOWNLOAD ${TITLE_SEQUENCE_URL} \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/title/title-sequences.zip EXPECTED_HASH SHA1=${TITLE_SEQUENCE_SHA1} SHOW_PROGRESS)")
    install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E chdir \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/title/ \"${CMAKE_COMMAND}\" -E tar xvf title-sequences.zip)")
    install(CODE "file(REMOVE \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/title/title-sequences.zip)")
endif ()
if (WIN32)
    install(TARGETS "libopenrct2" RUNTIME DESTINATION "bin")
else ()
    if (PORTABLE)
        install(TARGETS "libopenrct2" LIBRARY DESTINATION "bin")
    else ()
        install(TARGETS "libopenrct2" LIBRARY DESTINATION "lib")
    endif ()
endif ()
install(TARGETS "openrct2" RUNTIME DESTINATION "bin")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/g2.dat" DESTINATION "share/openrct2")
install(DIRECTORY "data/" DESTINATION "share/openrct2")
install(FILES ${DOC_FILES} DESTINATION "share/doc/openrct2")
install(FILES "resources/logo/icon_x16.png" DESTINATION "share/icons/hicolor/16x16/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x32.png" DESTINATION "share/icons/hicolor/32x32/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x64.png" DESTINATION "share/icons/hicolor/64x64/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x128.png" DESTINATION "share/icons/hicolor/128x128/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_x256.png" DESTINATION "share/icons/hicolor/256x256/apps" RENAME "openrct2.png")
install(FILES "resources/logo/icon_flag.svg" DESTINATION "share/icons/hicolor/scalable/apps" RENAME "openrct2.svg")
install(FILES "distribution/linux/openrct2.desktop" DESTINATION "share/applications")
install(DIRECTORY "distribution/man/" DESTINATION "share/man/man6" FILES_MATCHING PATTERN "*.6")
install(CODE "execute_process(COMMAND find \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/man/man6 -type f -exec gzip -f \"{}\" \;)")
