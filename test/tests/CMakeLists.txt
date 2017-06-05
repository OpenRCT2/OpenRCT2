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
        BUILD_BYPRODUCTS "googletest-prefix/src/googletest-build/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
        BUILD_BYPRODUCTS "googletest-prefix/src/googletest-build/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
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
include_directories("${ROOT_DIR}/src")
include_directories(${SDL2_INCLUDE_DIRS})

set(GTEST_LIBRARIES gtest gtest_main pthread)

# Some most common files required in tests
set(COMMON_TEST_SOURCES
    "${ROOT_DIR}/src/openrct2/core/Console.cpp"
    "${ROOT_DIR}/src/openrct2/core/Diagnostics.cpp"
    "${ROOT_DIR}/src/openrct2/core/Guard.cpp"
    "${ROOT_DIR}/src/openrct2/core/String.cpp"
    "${ROOT_DIR}/src/openrct2/diagnostic.c"
    "${ROOT_DIR}/src/openrct2/localisation/format_codes.c"
    "${ROOT_DIR}/src/openrct2/localisation/utf8.c"
    "${ROOT_DIR}/src/openrct2/util/util.c"
    "${ROOT_DIR}/src/openrct2/Version.cpp"
    )

# Create a re-usable library to save some compilation time
add_library(test-common STATIC ${COMMON_TEST_SOURCES})

# Start of our tests

# sawyercoding test

set(SAWYERCODING_TEST_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/sawyercoding_test.cpp"
        "${ROOT_DIR}/src/openrct2/diagnostic.c"
        "${ROOT_DIR}/src/openrct2/util/sawyercoding.c"
        "${ROOT_DIR}/src/openrct2/localisation/utf8.c"
        )
add_executable(test_sawyercoding ${SAWYERCODING_TEST_SOURCES})
target_link_libraries(test_sawyercoding ${GTEST_LIBRARIES})
add_test(NAME sawyercoding COMMAND test_sawyercoding)

# LanguagePack test
set(LANGUAGEPACK_TEST_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/LanguagePackTest.cpp"
        "${ROOT_DIR}/src/openrct2/localisation/LanguagePack.cpp"
        )
add_executable(test_languagepack ${LANGUAGEPACK_TEST_SOURCES})
target_link_libraries(test_languagepack ${GTEST_LIBRARIES} test-common dl z SDL2)
add_test(NAME languagepack COMMAND test_languagepack)

# INI test
set(INI_TEST_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/IniWriterTest.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/IniReaderTest.cpp"
        "${ROOT_DIR}/src/openrct2/config/IniReader.cpp"
        "${ROOT_DIR}/src/openrct2/config/IniWriter.cpp"
        "${ROOT_DIR}/src/openrct2/core/IStream.cpp"
        "${ROOT_DIR}/src/openrct2/core/MemoryStream.cpp"
        )
add_executable(test_ini ${INI_TEST_SOURCES})
target_link_libraries(test_ini ${GTEST_LIBRARIES} test-common dl z)
add_test(NAME ini COMMAND test_ini)

# String test
set(STRING_TEST_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/StringTest.cpp"
        )
add_executable(test_string ${STRING_TEST_SOURCES})
target_link_libraries(test_string ${GTEST_LIBRARIES} test-common dl z)
add_test(NAME string COMMAND test_string)

# Ride ratings test
if (NOT "z$ENV{CI}" STREQUAL "z")
    message("Detected CI environment. Disabling ride rating test.")
else ()
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_LIST_DIR}/testdata/parks" "${CMAKE_CURRENT_BINARY_DIR}/parks")
    set(RIDE_RATINGS_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/RideRatings.cpp"
                                  "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp")
    add_executable(test_ride_ratings ${RIDE_RATINGS_TEST_SOURCES})
    target_link_libraries(test_ride_ratings ${GTEST_LIBRARIES} libopenrct2 dl z)
    add_test(NAME ride_ratings COMMAND test_ride_ratings)
endif ()
