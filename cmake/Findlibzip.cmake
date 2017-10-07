include(FindPackageHandleStandardArgs)

find_library(LIBZIP_LIBRARY NAMES libzip.a zip)
find_path(LIBZIP_INCLUDE_DIR zip.h)
if(LIBZIP_INCLUDE_DIR)
    get_filename_component(zipconf_hint "${LIBZIP_INCLUDE_DIR}/../lib/libzip/include" ABSOLUTE)
endif()
find_path(LIBZIP_ZIPCONF_INCLUDE_DIR zipconf.h
    HINTS ${zipconf_hint}
    )

add_library(libzip::libzip IMPORTED UNKNOWN)

set_target_properties(libzip::libzip PROPERTIES
    IMPORTED_LOCATION "${LIBZIP_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${LIBZIP_INCLUDE_DIR};${LIBZIP_ZIPCONF_INCLUDE_DIR}"
    )

find_package_handle_standard_args(libzip
    "Unable to find required libzip library"
    LIBZIP_LIBRARY
    LIBZIP_INCLUDE_DIR
    LIBZIP_ZIPCONF_INCLUDE_DIR
    )
