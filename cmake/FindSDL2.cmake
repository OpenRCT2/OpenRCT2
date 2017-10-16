include(FindPackageHandleStandardArgs)

find_library(SDL2_LIBRARY NAMES SDL2)
find_path(SDL2_INCLUDE_BASE SDL2/SDL.h)
set(SDL2_INCLUDE_DIR "${SDL2_INCLUDE_BASE}/SDL2" CACHE INTERNAL "Path to SDL2 headers")

add_library(SDL2::SDL IMPORTED UNKNOWN)

set_target_properties(SDL2::SDL PROPERTIES
    IMPORTED_LOCATION "${SDL2_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
    INTERFACE_COMPILE_DEFINITIONS "_REENTRANT"
    )

find_package_handle_standard_args(SDL2
    "Unable to find required SDL2 library"
    SDL2_LIBRARY SDL2_INCLUDE_DIR
    )
