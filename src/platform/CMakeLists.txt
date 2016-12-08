file(GLOB LOCAL_OPENRCT2_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

set(LOCAL_OPENRCT2_SOURCES
    ${LOCAL_OPENRCT2_HEADERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/crash.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shared.c"
    )
if (UNIX)
    set(LOCAL_OPENRCT2_SOURCES
        ${LOCAL_OPENRCT2_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/posix.c"
        )
    if (APPLE)
        set(ORCT2_MM_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/macos.m"
            )
        set_source_files_properties(${ORCT2_MM_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c -fmodules")
        set(LOCAL_OPENRCT2_SOURCES
            ${LOCAL_OPENRCT2_SOURCES}
            ${ORCT2_MM_SOURCES}
            )
    else()
        set(LOCAL_OPENRCT2_SOURCES
            ${LOCAL_OPENRCT2_SOURCES}
            "${CMAKE_CURRENT_SOURCE_DIR}/linux.c"
            )
    endif()
elseif(WIN32)
    set(LOCAL_OPENRCT2_SOURCES
        ${LOCAL_OPENRCT2_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/windows.c"
        )
endif()
set(OPENRCT2_SOURCES
    ${OPENRCT2_SOURCES}
    ${LOCAL_OPENRCT2_SOURCES}
    PARENT_SCOPE
    )
set(OPENRCT2_INCLUDES
    ${OPENRCT2_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
    )
