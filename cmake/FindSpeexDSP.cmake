include(FindPackageHandleStandardArgs)

find_library(SPEEXDSP_LIBRARY NAMES libspeexdsp.a speexdsp)
find_path(SPEEXDSP_INCLUDE_DIR speex/speex.h)

add_library(SpeexDSP::SpeexDSP IMPORTED UNKNOWN)

set_target_properties(SpeexDSP::SpeexDSP PROPERTIES
    IMPORTED_LOCATION "${SPEEXDSP_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SPEEXDSP_INCLUDE_DIR}"
    )

find_package_handle_standard_args(SpeexDSP
    "Failed to find SpeexDSP"
    SPEEXDSP_LIBRARY
    SPEEXDSP_INCLUDE_DIR
    )