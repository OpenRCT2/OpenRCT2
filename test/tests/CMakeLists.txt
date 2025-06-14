cmake_minimum_required(VERSION 3.20)
include(GoogleTest)

find_package(GTest REQUIRED)

file(CREATE_LINK "${CMAKE_CURRENT_LIST_DIR}/testdata" "${CMAKE_BINARY_DIR}/testdata" SYMBOLIC)
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"\$ENV{DESTDIR}${CMAKE_INSTALL_FULL_DATADIR}/openrct2\" \"${CMAKE_BINARY_DIR}/data\")")

set(test_files
   "${CMAKE_CURRENT_SOURCE_DIR}/AssertHelpers.hpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/BitSetTests.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/CircularBuffer.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/CLITests.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/CryptTests.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/Endianness.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/EnumMapTest.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/FormattingTests.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/ImageImporterTests.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/IniReaderTest.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/IniWriterTest.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/LanguagePackTest.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/LocalisationTest.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/MultiLaunch.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/Pathfinding.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/Platform.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/PlayTests.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/ReplayTests.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/RideRatings.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/S6ImportExportTests.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/SawyerCodingTest.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/ScenarioPatcherTests.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/StringTest.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/TestData.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/TestData.h"
   "${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/TileElements.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/TileElementsView.cpp")

add_executable(OpenRCT2Tests ${test_files})
add_executable(OpenRCT2::OpenRCT2Tests ALIAS OpenRCT2Tests)

target_link_libraries(OpenRCT2Tests GTest::gtest GTest::gtest_main libopenrct2)
target_include_directories(OpenRCT2Tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
set_target_properties(OpenRCT2Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
gtest_discover_tests(OpenRCT2Tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DISCOVERY_MODE PRE_TEST)

