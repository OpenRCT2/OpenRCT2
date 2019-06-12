cmake_minimum_required(VERSION 2.6)

option(SYSTEM_GTEST "Use the googletest library provided by the system.")

if (SYSTEM_GTEST)
    find_package(GTest REQUIRED)

    set(GTEST_LIBRARIES ${GTEST_BOTH_LIBRARIES})

    message(WARNING "Gtest strongly advices against using a system installation, see https://github.com/google/googletest/blob/master/googletest/docs/FAQ.md#why-is-it-not-recommended-to-install-a-pre-compiled-copy-of-google-test-for-example-into-usrlocal for detailed information. If errors occur please double-check without the SYSTEM_GTEST flag.")
else (SYSTEM_GTEST)

        # Bootstrap GoogleTest
        INCLUDE(ExternalProject)

        # Use googletest with https://github.com/google/googletest/pull/1045 applied
        ExternalProject_Add(
                googletest-distribution
                URL https://github.com/google/googletest/archive/ad49eaa0466b98f21449efaaaae817683daa8b99.tar.gz
                URL_HASH SHA1=e468bfdffe7c46c14ce06bfb48a29b6201908504
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
        set(GTEST_INCLUDE_DIRS ${GOOGLETEST_DISTRIB_SOURCE_DIR}/googletest/include)

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

        set(GTEST_LIBRARIES gtest gtest_main pthread)

endif (SYSTEM_GTEST)

include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
include_directories("${ROOT_DIR}/src")
include_directories(${SDL2_INCLUDE_DIRS})

# Setup testdata. It should be fine here, as the only way to reach here is by explicitly requesting tests.
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_LIST_DIR}/testdata" "${CMAKE_CURRENT_BINARY_DIR}/testdata")
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"\$ENV{DESTDIR}${CMAKE_INSTALL_FULL_DATADIR}/openrct2\" \"${CMAKE_CURRENT_BINARY_DIR}/data\")")

if (UNIX AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "BSD")
    # Include libdl for dlopen
    set(LDL dl)
endif ()

# Start of our tests

# sawyercoding test
set(SAWYERCODING_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/sawyercoding_test.cpp"
)

# LanguagePack test
set(LANGUAGEPACK_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/LanguagePackTest.cpp"
)

# INI test
set(INI_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/IniWriterTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/IniReaderTest.cpp"
)

# Platform
set(PLATFORM_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Platform.cpp"
)

# String test
set(STRING_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/StringTest.cpp"
)

# Localisation test
set(LOCALISATION_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Localisation.cpp"
)

if (NOT DISABLE_NETWORK)
    # Crypt tests
    set(CRYPT_TEST_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/CryptTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp"
    )
endif ()

# ImageImporter tests
set(IMAGEIMPORTER_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ImageImporterTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp"
)

# Ride ratings test
set(RIDE_RATINGS_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/RideRatings.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp"
)

# Multi-launch test
set(MULTILAUNCH_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/MultiLaunch.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp"
)

# Tile element test
set(TILE_ELEMENT_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TileElements.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp"
)

if (NOT DISABLE_NETWORK)
    # Replay tests
    set(REPLAY_TEST_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/ReplayTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp"
    )
endif ()

# Pathfinding test
set(PATHFINDING_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Pathfinding.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp"
)

# LoadSave test
set(NETWORKLOADSAVE_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/NetworkLoadSave.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp"
)

add_executable(test_all
    ${CRYPT_TEST_SOURCES}
    ${IMAGEIMPORTER_TEST_SOURCES}
    ${INI_TEST_SOURCES}
    ${LANGUAGEPACK_TEST_SOURCES}
    ${LOCALISATION_TEST_SOURCES}
    ${MULTILAUNCH_TEST_SOURCES}
    ${NETWORKLOADSAVE_TEST_SOURCES}
    ${PATHFINDING_TEST_SOURCES}
    ${PLATFORM_TEST_SOURCES}
    ${REPLAY_TEST_SOURCES}
    ${RIDE_RATINGS_TEST_SOURCES}
    ${SAWYERCODING_TEST_SOURCES}
    ${STRING_TEST_SOURCES}
    ${TILE_ELEMENT_TEST_SOURCES}
)
SET_CHECK_CXX_FLAGS(test_all)
target_link_libraries(test_all ${GTEST_LIBRARIES} libopenrct2 ${LDL} z)
target_link_platform_libraries(test_all)

# The macro is only available when using system gtest option, which pulls in gtest's cmake module
if (SYSTEM_GTEST)
    gtest_discover_tests(test_all)
else (SYSTEM_GTEST)
    add_test(NAME all COMMAND test_all)
endif (SYSTEM_GTEST)
