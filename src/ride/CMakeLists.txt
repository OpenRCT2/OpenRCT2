add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/coaster")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gentle")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/shops")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thrill")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/water")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/transport")

file(GLOB LOCAL_OPENRCT2_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

set(OPENRCT2_SOURCES
    ${OPENRCT2_SOURCES}
    ${LOCAL_OPENRCT2_HEADERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/cable_lift.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ride.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ride_data.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ride_ratings.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/station.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/track.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/track_data.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/track_data_old.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/track_design.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/TrackDesignRepository.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/track_design_save.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/track_paint.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vehicle.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vehicle_data.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vehicle_paint.c"
    PARENT_SCOPE
    )
set(OPENRCT2_INCLUDES
    ${OPENRCT2_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
    )
