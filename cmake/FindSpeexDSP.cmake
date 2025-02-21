#
# - Find speexdsp libraries
#
#  SPEEXDSP_INCLUDE_DIRS - where to find speexdsp headers.
#  SPEEXDSP_LIBRARIES    - List of libraries when using speexdsp.
#  SPEEXDSP_FOUND        - True if speexdsp is found.

find_package(PkgConfig QUIET)
pkg_search_module(PC_SPEEXDSP QUIET speexdsp)

find_path(SPEEXDSP_INCLUDE_DIR
  NAMES
    speex/speex_resampler.h
  HINTS
    ${PC_SPEEXDSP_INCLUDE_DIRS}
)

find_library(SPEEXDSP_LIBRARY
  NAMES
    speexdsp
  HINTS
    ${PC_SPEEXDSP_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SpeexDSP
  REQUIRED_VARS   SPEEXDSP_LIBRARY SPEEXDSP_INCLUDE_DIR
  VERSION_VAR     PC_SPEEXDSP_VERSION)

if(SPEEXDSP_FOUND)
  set(SPEEXDSP_LIBRARIES ${SPEEXDSP_LIBRARY})
  set(SPEEXDSP_INCLUDE_DIRS ${SPEEXDSP_INCLUDE_DIR})
  set(SPEEX_INCLUDE_DIRS ${SPEEXDSP_INCLUDE_DIR})
else()
  set(SPEEXDSP_LIBRARIES)
  set(SPEEXDSP_INCLUDE_DIRS)
  set(SPEEX_INCLUDE_DIRS)
endif()

mark_as_advanced(SPEEXDSP_LIBRARIES SPEEXDSP_INCLUDE_DIRS SPEEX_INCLUDE_DIRS)
