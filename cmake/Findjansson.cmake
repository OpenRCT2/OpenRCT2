include(FindPackageHandleStandardArgs)

find_library(JANSSON_LIBRARY NAMES libjansson.a jansson)
find_path(JANSSON_INCLUDE_DIR jansson.h)

add_library(jansson::jansson IMPORTED UNKNOWN)

set_target_properties(jansson::jansson PROPERTIES
    IMPORTED_LOCATION "${JANSSON_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${JANSSON_INCLUDE_DIR}"
    )

find_package_handle_standard_args(jansson
    "Unable to find required jansson library"
    JANSSON_LIBRARY JANSSON_INCLUDE_DIR
    )