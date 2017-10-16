include(FindPackageHandleStandardArgs)

find_library(CURL_LIBRARY NAMES curl-gnutls curl)
find_path(CURL_INCLUDE_DIR curl/curl.h)

add_library(curl::curl IMPORTED UNKNOWN)

set_target_properties(curl::curl PROPERTIES
    IMPORTED_LOCATION "${CURL_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIR}"
    )

find_package_handle_standard_args(CURL
    "Unable to find required curl library"
    CURL_LIBRARY CURL_INCLUDE_DIR
    )