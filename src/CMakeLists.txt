# This var is used in a few places
set(OPENRCT2_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Each component gets its own subdirectory
# Core:
add_subdirectory(openrct2)
# CLI exe
add_subdirectory(openrct2-cli)
# UI library
add_subdirectory(openrct2-ui)
if(WIN32)
    # Windows runtime
    add_subdirectory(openrct2-dll)
    # Windows Main
    add_subdirectory(openrct2-win)
elseif(UNIX)
    # Linux Main
    add_subdirectory(openrct2-nix)
endif()
