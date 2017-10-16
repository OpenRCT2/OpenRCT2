if(NOT DEFINED CMAKE_CONFIG)
    message(FATAL_ERROR "Expected a value for CMAKE_CONFIG")
endif()

get_filename_component(build_dir "${CMAKE_CURRENT_LIST_DIR}/build" ABSOLUTE)

find_program(ninja_executable NAMES ninja-build ninja)
if(ninja_executable)
    message(STATUS "Building with ninja: ${ninja_executable}")
    set(ninja_generator -GNinja)
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND}
        ${ninja_generator}
        "-C${CMAKE_CONFIG}"
        "-H${SOURCE_DIR}"
        "-B${BUILD_DIR}"
    RESULT_VARIABLE res
    )

if(res)
    message(FATAL_ERROR "CMake configure failed [${res}]")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} --build "${BUILD_DIR}"
    RESULT_VARIABLE res
    )

if(res)
    message(FATAL_ERROR "CMake build failed [${res}]")
endif()

if(NOT DOCKER_TAG STREQUAL "mingw-arch")
    execute_process(
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -j8
        WORKING_DIRECTORY "${BUILD_DIR}"
        RESULT_VARIABLE res
        )
    if(res)
        message(FATAL_ERROR "CTest failed [${res}]")
    endif()
else()
    message(STATUS "Skipping tests because we are compiling with MinGW")
endif()

execute_process(
    COMMAND ${CMAKE_CPACK_COMMAND} -G TGZ
    WORKING_DIRECTORY "${BUILD_DIR}"
    RESULT_VARIABLE res
    )
if(res)
    message(FATAL_ERROR "Failed to run CPack [${res}]")
endif()