# TODO: This module can be deleted with CMake 3.9, which supports import targets
# for EXPAT

include(FindPackageHandleStandardArgs)

find_library(EXPAT_LIBRARY NAMES libexpat.a expat)
find_path(EXPAT_INCLUDE_DIRS expat.h)

add_library(EXPAT::EXPAT IMPORTED UNKNOWN)

set_target_properties(EXPAT::EXPAT PROPERTIES
    IMPORTED_LOCATION "${EXPAT_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${EXPAT_INCLUDE_DIRS}"
    )

find_package_handle_standard_args(EXPAT
    "Unable to find required EXPAT library"
    EXPAT_LIBRARY EXPAT_INCLUDE_DIRS
    )
