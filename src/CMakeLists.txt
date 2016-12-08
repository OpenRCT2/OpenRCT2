add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/audio")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/cmdline")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/core")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/drawing")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/interface")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/localisation")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/management")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/network")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/object")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/paint")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/peep")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/platform")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/rct1")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/rct2")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ride")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/util")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/windows")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/world")

file(GLOB LOCAL_OPENRCT2_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

set(OPENRCT2_SOURCES
    ${OPENRCT2_SOURCES}
    ${LOCAL_OPENRCT2_HEADERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/cheats.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmdline_sprite.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/config.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/diagnostic.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/game.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/image_io.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/input.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/intro.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/object_list.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/openrct2.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/rct1.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/rct2.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/scenario.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ScenarioRepository.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ScenarioSources.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/title.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/version.c"
    PARENT_SCOPE)

if (NOT DISABLE_RCT2)
    set(OPENRCT2_SOURCES
        ${OPENRCT2_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/addresses.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/hook.c"
        PARENT_SCOPE
        )
endif ()
set(OPENRCT2_INCLUDES
    ${OPENRCT2_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
    )
