cmake_minimum_required(VERSION 3.9)

include_directories(${ROOT_DIR}/src)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIR})

function(add_gtest_default name sources)
    add_executable(test_${name} ${sources})
    set_check_cxx_flags(test_${name})
    target_link_libraries(test_${name} ${GTEST_LIBRARIES} test-common libopenrct2 z)
    target_link_platform_libraries(test_${name})
    add_test(NAME ${name} COMMAND test_${name})
endfunction()

# Some most common files required in tests
set(COMMON_TEST_SOURCES
    "${ROOT_DIR}/src/openrct2/core/Console.cpp"
    "${ROOT_DIR}/src/openrct2/core/Diagnostics.cpp"
    "${ROOT_DIR}/src/openrct2/core/Guard.cpp"
    "${ROOT_DIR}/src/openrct2/core/String.cpp"
    "${ROOT_DIR}/src/openrct2/Diagnostic.cpp"
    "${ROOT_DIR}/src/openrct2/localisation/ConversionTables.cpp"
    "${ROOT_DIR}/src/openrct2/localisation/Convert.cpp"
    "${ROOT_DIR}/src/openrct2/localisation/FormatCodes.cpp"
    "${ROOT_DIR}/src/openrct2/localisation/UTF8.cpp"
    "${ROOT_DIR}/src/openrct2/util/Util.cpp"
    "${ROOT_DIR}/src/openrct2/Version.cpp")

# Create a re-usable library to save some compilation time
add_library(test-common STATIC "${COMMON_TEST_SOURCES}")

# Setup testdata. It should be fine here, as the only way to reach here is by explicitly requesting tests.
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_LIST_DIR}/testdata" "${CMAKE_CURRENT_BINARY_DIR}/testdata")
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"\$ENV{DESTDIR}${CMAKE_INSTALL_FULL_DATADIR}/openrct2\" \"${CMAKE_CURRENT_BINARY_DIR}/data\")")

if (NOT MINGW AND NOT MSVC)
    # For unicode code page conversion (required for ini and string tests)
    find_package(ICU 59.0 REQUIRED COMPONENTS uc)
    target_link_libraries(test-common ${ICU_LIBRARIES})
    target_include_directories(test-common SYSTEM PUBLIC ${ICU_INCLUDE_DIRS})
endif ()

# Start of OpenRCT2 tests

set(SAWYERCODING_TEST_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/sawyercoding_test.cpp"
        "${ROOT_DIR}/src/openrct2/core/IStream.cpp"
        "${ROOT_DIR}/src/openrct2/core/MemoryStream.cpp"
        "${ROOT_DIR}/src/openrct2/rct12/SawyerChunk.cpp"
        "${ROOT_DIR}/src/openrct2/rct12/SawyerChunkReader.cpp"
        "${ROOT_DIR}/src/openrct2/util/SawyerCoding.cpp")
add_gtest_default(sawyercoding "${SAWYERCODING_TEST_SOURCES}")

set(LANGUAGEPACK_TEST_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/LanguagePackTest.cpp"
        "${ROOT_DIR}/src/openrct2/localisation/LanguagePack.cpp")
add_gtest_default(languagepack "${LANGUAGEPACK_TEST_SOURCES}")
if (UNIX AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "BSD")
    # Include libdl for dlopen
    target_link_libraries(test_languagepack dl)
endif ()

set(INI_TEST_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/IniWriterTest.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/IniReaderTest.cpp"
        "${ROOT_DIR}/src/openrct2/config/IniReader.cpp"
        "${ROOT_DIR}/src/openrct2/config/IniWriter.cpp"
        "${ROOT_DIR}/src/openrct2/core/IStream.cpp"
        "${ROOT_DIR}/src/openrct2/core/MemoryStream.cpp")
add_gtest_default(ini "${INI_TEST_SOURCES}")

set(STRING_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/StringTest.cpp")
add_gtest_default(string "${STRING_TEST_SOURCES}")

set(LOCALISATION_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/Localisation.cpp")
add_gtest_default(localisation "${LOCALISATION_TEST_SOURCES}")

if (NOT DISABLE_NETWORK)
    set(CRYPT_TEST_SOURCES  "${CMAKE_CURRENT_LIST_DIR}/CryptTests.cpp"
                            "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp")
    add_gtest_default(crypt "${CRYPT_TEST_SOURCES}")
endif ()

set(IMAGEIMPORTER_TEST_SOURCES  "${CMAKE_CURRENT_LIST_DIR}/ImageImporterTests.cpp"
                                "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp")
add_gtest_default(imageimporter "${IMAGEIMPORTER_TEST_SOURCES}")

set(RIDE_RATINGS_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/RideRatings.cpp"
                              "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp")
add_gtest_default(ride_ratings "${RIDE_RATINGS_TEST_SOURCES}")

set(MULTILAUNCH_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/MultiLaunch.cpp"
                             "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp")
add_gtest_default(multilaunch "${MULTILAUNCH_TEST_SOURCES}")

set(TILE_ELEMENT_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/TileElements.cpp"
                              "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp")
add_gtest_default(tile_elements "${TILE_ELEMENT_TEST_SOURCES}")

if (NOT DISABLE_NETWORK)
    set(REPLAY_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/ReplayTests.cpp"
                                  "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp")
    add_gtest_default(replays "${REPLAY_TEST_SOURCES}")
endif ()

set(PATHFINDING_TEST_SOURCES  "${CMAKE_CURRENT_LIST_DIR}/Pathfinding.cpp"
                              "${CMAKE_CURRENT_LIST_DIR}/TestData.cpp")
add_gtest_default(pathfinding "${PATHFINDING_TEST_SOURCES}")
