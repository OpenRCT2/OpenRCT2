# Options
option(DISABLE_OPENGL "Disable OpenGL support." OFF)

if (PORTABLE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif ()

# Sources
file(GLOB_RECURSE OPENRCT2_UI_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

if (APPLE)
    file(GLOB_RECURSE OPENRCT2_UI_M_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.m")
    set_source_files_properties(${OPENRCT2_UI_M_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c -fmodules")

    file(GLOB_RECURSE OPENRCT2_UI_MM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    set_source_files_properties(${OPENRCT2_UI_MM_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c++ -fmodules")
endif ()

# Outputs
add_library(openrct2-ui STATIC ${OPENRCT2_UI_SOURCES} ${OPENRCT2_UI_M_SOURCES} ${OPENRCT2_UI_MM_SOURCES})
add_library(openrct2::ui ALIAS openrct2-ui)

target_link_libraries(openrct2-ui PUBLIC "libopenrct2"
                                 ${SDL2_LDFLAGS}
                                 ${SPEEX_LDFLAGS})

if (APPLE)
    target_link_libraries(openrct2-ui "-framework Cocoa")
endif ()

# Includes
target_include_directories(openrct2-ui PUBLIC "${CMAKE_CURRENT_LIST_DIR}/.."
                                              ${SPEEX_INCLUDE_DIRS})
target_include_directories(openrct2-ui SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})

# Compiler flags
if (WIN32)
    # mingw complains about "%zu" not being a valid format specifier for printf, unless we
    # tell it that it is
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__USE_MINGW_ANSI_STDIO=1")
endif ()

# Defines
if (DISABLE_OPENGL)
    target_compile_definitions(openrct2-ui PUBLIC DISABLE_OPENGL)
else ()
    # Makes OpenGL function get queried in run-time rather than linked-in
    add_definitions(-DOPENGL_NO_LINK)
endif ()
