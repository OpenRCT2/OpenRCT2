include(FindPackageHandleStandardArgs)
include(CMakeFindDependencyMacro)

find_dependency(EXPAT)
find_dependency(PNG)

find_library(FONTCONFIG_LIBRARY NAMES libfontconfig.a fontconfig)

add_library(fontconfig::fontconfig IMPORTED UNKNOWN)

set_target_properties(fontconfig::fontconfig PROPERTIES
    IMPORTED_LOCATION "${FONTCONFIG_LIBRARY}"
    INTERFACE_LINK_LIBRARIES "EXPAT::EXPAT;PNG::PNG"
    )

find_package_handle_standard_args(fontconfig
    "Unable to find required fontconfig library"
    FONTCONFIG_LIBRARY
    )
