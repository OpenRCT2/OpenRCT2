set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB_RECURSE OPENRCT2_CLI_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

add_executable(openrct2-cli ${OPENRCT2_CLI_SOURCES})
target_include_directories(openrct2-cli PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
ipo_set_target_properties(openrct2-cli)
if (EMSCRIPTEN)
    target_link_libraries(openrct2-cli ${ICU_DT_LIBRARY_RELEASE} ${ICU_DATA_LIBRARIES})
endif ()
target_link_libraries(openrct2-cli libopenrct2 Threads::Threads)
target_link_platform_libraries(openrct2-cli)
