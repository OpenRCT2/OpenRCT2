# Sources
file(GLOB_RECURSE OPENRCT2_UI_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

if(APPLE)
    file(GLOB_RECURSE OPENRCT2_UI_M_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.m")
    file(GLOB_RECURSE OPENRCT2_UI_MM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    set_source_files_properties(${OPENRCT2_UI_M_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c -fmodules")
    set_source_files_properties(${OPENRCT2_UI_MM_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c++ -fmodules")
endif()

# Generate a static library with the UI code that can be linkined into a DLL or executable
add_library(openrct2-ui STATIC ${OPENRCT2_UI_SOURCES} ${OPENRCT2_UI_M_SOURCES} ${OPENRCT2_UI_MM_SOURCES})
add_library(openrct2::ui ALIAS openrct2-ui)
target_link_libraries(openrct2-ui PUBLIC openrct2::core)

if(APPLE)
    target_link_libraries(openrct2-ui "-framework Cocoa")
endif()

# Compiler flags
if(WIN32)
    # mingw complains about "%zu" not being a valid format specifier for printf, unless we
    # tell it that it is
    target_compile_definitions(openrct2-ui PUBLIC __USE_MINGW_ANSI_STDIO=1)
endif()

# Defines
if(NOT ENABLE_OPENGL)
    target_compile_definitions(openrct2-ui PUBLIC DISABLE_OPENGL)
endif()
