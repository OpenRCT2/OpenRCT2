# CMAKE project for testpaint
cmake_minimum_required(VERSION 2.6)
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

set(OPENRCT2_SRCPATH "${ROOT_DIR}/src/openrct2")
file(GLOB_RECURSE ORCT2_RIDE_SOURCES "${OPENRCT2_SRCPATH}/ride/*/*.cpp")
file(GLOB_RECURSE ORCT2_RIDE_DEP_SOURCES
    "${OPENRCT2_SRCPATH}/Diagnostic.cpp"
    "${OPENRCT2_SRCPATH}/paint/PaintHelpers.cpp"
    "${OPENRCT2_SRCPATH}/paint/tile_element/Paint.TileElement.cpp"
    "${OPENRCT2_SRCPATH}/ride/RideData.cpp"
    "${OPENRCT2_SRCPATH}/ride/TrackData.cpp"
    "${OPENRCT2_SRCPATH}/ride/TrackPaint.cpp"
    "${OPENRCT2_SRCPATH}/core/Console.cpp"
    "${OPENRCT2_SRCPATH}/core/Diagnostics.cpp"
    "${OPENRCT2_SRCPATH}/core/Guard.cpp"
    "${OPENRCT2_SRCPATH}/core/String.cpp"
    "${OPENRCT2_SRCPATH}/Diagnostic.cpp"
    "${OPENRCT2_SRCPATH}/interface/ZoomLevel.cpp"
    "${OPENRCT2_SRCPATH}/localisation/ConversionTables.cpp"
    "${OPENRCT2_SRCPATH}/localisation/Convert.cpp"
    "${OPENRCT2_SRCPATH}/localisation/FormatCodes.cpp"
    "${OPENRCT2_SRCPATH}/localisation/UTF8.cpp"
    "${OPENRCT2_SRCPATH}/util/Util.cpp"
    "${OPENRCT2_SRCPATH}/Version.cpp"
)
file(GLOB_RECURSE ORCT2_TESTPAINT_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c"
                                          "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                                          "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_executable(testpaint ${ORCT2_RIDE_SOURCES} ${ORCT2_RIDE_DEP_SOURCES} ${ORCT2_TESTPAINT_SOURCES})
SET_CHECK_CXX_FLAGS(testpaint)
target_compile_options(testpaint PRIVATE -Wno-old-style-cast)
target_include_directories(testpaint PRIVATE "${ROOT_DIR}/src/")
target_link_libraries(testpaint z)

if (NOT MINGW AND NOT MSVC)
    # For unicode code page conversion
    find_package(ICU 59.0 REQUIRED COMPONENTS uc)
    target_link_libraries(testpaint ${ICU_LIBRARIES})
    target_include_directories(testpaint SYSTEM PUBLIC ${ICU_INCLUDE_DIRS})
endif ()

set(TESTPAINT_64BIT_FLAGS "-Wno-int-to-pointer-cast -fpermissive -Wno-error")

set_target_properties(testpaint PROPERTIES COMPILE_FLAGS "-DNO_VEHICLES -D__TESTPAINT__ -Wno-unused ${TESTPAINT_64BIT_FLAGS} -DDISABLE_HTTP")
