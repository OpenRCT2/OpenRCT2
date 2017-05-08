# CMAKE project for testpaint
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

set(OPENRCT2_SRCPATH "${ROOT_DIR}/src/openrct2")
file(GLOB_RECURSE ORCT2_RIDE_SOURCES "${OPENRCT2_SRCPATH}/ride/*/*.c")
file(GLOB_RECURSE ORCT2_RIDE_DEP_SOURCES "${OPENRCT2_SRCPATH}/ride/ride_data.c"
                                         "${OPENRCT2_SRCPATH}/ride/track_data.c"
                                         "${OPENRCT2_SRCPATH}/ride/track_data_old.c"
                                         "${OPENRCT2_SRCPATH}/ride/track_paint.c"
                                         "${OPENRCT2_SRCPATH}/rct2/addresses.c"
                                         "${OPENRCT2_SRCPATH}/diagnostic.c"
                                         "${OPENRCT2_SRCPATH}/rct2/hook.c"
                                         "${OPENRCT2_SRCPATH}/paint/map_element/map_element.c"
                                         "${OPENRCT2_SRCPATH}/paint/paint_helpers.c")
file(GLOB_RECURSE ORCT2_TESTPAINT_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c"
                                          "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                                          "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_executable(testpaint EXCLUDE_FROM_ALL ${ORCT2_RIDE_SOURCES} ${ORCT2_RIDE_DEP_SOURCES} ${ORCT2_TESTPAINT_SOURCES} ${RCT2_SECTIONS})
target_include_directories(testpaint PRIVATE "${ROOT_DIR}/src/")
set_target_properties(testpaint PROPERTIES COMPILE_FLAGS "-DNO_VEHICLES -D__TESTPAINT__ -Wno-unused")
set_target_properties(testpaint PROPERTIES LINK_FLAGS ${RCT2_SEGMENT_LINKER_FLAGS})
add_dependencies(testpaint segfiles)
