option(ENABLE_RCT2_TESTS "Enable tests that require RollerCoaster Tycoon 2 assets." ON)
option(SYSTEM_GTEST "Use the googletest library provided by the system." OFF)

if(SYSTEM_GTEST)
    find_package(GTest REQUIRED)
    set(GTEST_LIBRARIES ${GTEST_BOTH_LIBRARIES})
    message(WARNING "Gtest strongly advices against using a system installation, see https://github.com/google/googletest/blob/master/googletest/docs/FAQ.md#why-is-it-not-recommended-to-install-a-pre-compiled-copy-of-google-test-for-example-into-usrlocal for detailed information. If errors occur please double-check without the SYSTEM_GTEST flag.")
else()
    include(get-gtest.cmake)
endif()

include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})

if(NOT "$ENV{CI}" STREQUAL "")
    message("Detected CI environment. Disabling ride rating test.")
    set(ENABLE_RCT2_TESTS OFF)
endif()

function(add_orct2_test name)
    add_executable(test-${name} ${ARGN})
    target_link_libraries(test-${name} PRIVATE openrct2::core ${GTEST_LIBRARIES})
    target_compile_definitions(test-${name} PRIVATE GTEST_LANG_CXX11)
    add_test(NAME ${name} COMMAND test-${name})
endfunction()

# Define those tests!

add_orct2_test(SawyerCoding sawyercoding_test.cpp)
add_orct2_test(LanguagePack LanguagePackTest.cpp)
add_orct2_test(INI IniWriterTest.cpp IniReaderTest.cpp)
add_orct2_test(String StringTest.cpp)

if(ENABLE_RCT2_TESTS)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_LIST_DIR}/testdata" "${CMAKE_CURRENT_BINARY_DIR}/testdata")
    add_orct2_test(RideRatings RideRatings.cpp TestData.cpp)
    add_orct2_test(MultiLaunch MultiLaunch.cpp TestData.cpp)
endif()
