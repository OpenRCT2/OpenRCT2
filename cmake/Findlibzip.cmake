include(FindPackageHandleStandardArgs)

find_library(LIBZIP_LIBRARY NAMES libzip.a zip)
find_path(LIBZIP_INCLUDE_DIR zip.h)

add_library(libzip::libzip IMPORTED UNKNOWN)

set_target_properties(libzip::libzip PROPERTIES
    IMPORTED_LOCATION "${LIBZIP_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${LIBZIP_INCLUDE_DIR}"
    )

find_package_handle_standard_args(libzip
    "Unable to find required libzip library"
    LIBZIP_LIBRARY LIBZIP_INCLUDE_DIR
    )
