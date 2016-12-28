cmake_minimum_required(VERSION 2.6)

# Bootstrap GoogleTest
INCLUDE(ExternalProject)

ExternalProject_Add(
		googletest-distribution
		URL https://github.com/google/googletest/archive/release-1.8.0.tar.gz
		URL_HASH SHA1=e7e646a6204638fe8e87e165292b8dd9cd4c36ed
		TIMEOUT 10
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ""
)

# Specify include dir
ExternalProject_Get_Property(googletest-distribution SOURCE_DIR)
set(GOOGLETEST_DISTRIB_SOURCE_DIR "${SOURCE_DIR}")

ExternalProject_Add(
		googletest
		DEPENDS googletest-distribution
		DOWNLOAD_COMMAND ""
		SOURCE_DIR "${GOOGLETEST_DISTRIB_SOURCE_DIR}/googletest"
		CMAKE_ARGS "-DCMAKE_CXX_FLAGS=${TARGET_M}"
		# Disable install step
		INSTALL_COMMAND ""
		# Wrap download, configure and build steps in a script to log output
		LOG_DOWNLOAD ON
		LOG_CONFIGURE ON
		LOG_BUILD ON)


# Specify include dir
set(GTEST_INCLUDE_DIR ${GOOGLETEST_DISTRIB_SOURCE_DIR}/googletest/include)

# Library
ExternalProject_Get_Property(googletest BINARY_DIR)
set(GOOGLETEST_BINARY_DIR "${BINARY_DIR}")
set(GTEST_LIBRARY_PATH ${GOOGLETEST_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})
set(GTEST_MAIN_LIBRARY_PATH ${GOOGLETEST_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX})
set(GTEST_LIBRARY gtest)
set(GTEST_MAIN_LIBRARY gtest_main)
add_library(${GTEST_LIBRARY} STATIC IMPORTED)
add_library(${GTEST_MAIN_LIBRARY} STATIC IMPORTED)
set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION ${GTEST_LIBRARY_PATH})
set_property(TARGET ${GTEST_MAIN_LIBRARY} PROPERTY IMPORTED_LOCATION ${GTEST_MAIN_LIBRARY_PATH})
add_dependencies(${GTEST_LIBRARY} googletest)
add_dependencies(${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY})

include_directories(SYSTEM ${GTEST_INCLUDE_DIR})
include_directories("../../src")

set(GTEST_LIBRARIES gtest gtest_main pthread)

# Start of our tests

# sawyercoding test

set(SAWYERCODING_TEST_SOURCES
		"sawyercoding_test.cpp"
		"../../src/openrct2/diagnostic.c"
		"../../src/openrct2/util/sawyercoding.c"
		"../../src/openrct2/localisation/utf8.c"
		)
add_executable(test_sawyercoding ${SAWYERCODING_TEST_SOURCES})
target_link_libraries(test_sawyercoding ${GTEST_LIBRARIES})
add_test(NAME sawyercoding COMMAND test_sawyercoding)

# LanguagePack test
set(LANGUAGEPACK_TEST_SOURCES
		"LanguagePackTest.cpp"
		"../../src/openrct2/localisation/LanguagePack.cpp"
		"../../src/openrct2/core/Console.cpp"
		"../../src/openrct2/core/Diagnostics.cpp"
		"../../src/openrct2/core/Guard.cpp"
		"../../src/openrct2/core/String.cpp"
		"../../src/openrct2/diagnostic.c"
		"../../src/openrct2/localisation/format_codes.c"
		"../../src/openrct2/localisation/utf8.c"
		"../../src/openrct2/util/util.c"
		)
add_executable(test_languagepack ${LANGUAGEPACK_TEST_SOURCES})
target_link_libraries(test_languagepack ${GTEST_LIBRARIES} dl z SDL2 SDL2_ttf ssl crypto)
add_test(NAME languagepack COMMAND test_languagepack)
