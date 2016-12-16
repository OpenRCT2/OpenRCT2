file(GLOB LOCAL_OPENRCT2_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

set(OPENRCT2_SOURCES
    ${OPENRCT2_SOURCES}
    ${LOCAL_OPENRCT2_HEADERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/3d_cinema.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/enterprise.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/go_karts.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/launched_freefall.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/magic_carpet.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/motion_simulator.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/pirate_ship.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/roto_drop.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/swinging_inverter_ship.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/top_spin.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/twist.c"
    PARENT_SCOPE
    )
set(OPENRCT2_INCLUDES
    ${OPENRCT2_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
    )
