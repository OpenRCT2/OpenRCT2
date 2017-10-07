# Gets GTest from a remote
include(ExternalProject)

ExternalProject_Add(
    googletest-distribution
    URL https://github.com/google/googletest/archive/release-1.8.0.tar.gz
    URL_HASH SHA1=e7e646a6204638fe8e87e165292b8dd9cd4c36ed
    TIMEOUT 10
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

# Specify include dir
ExternalProject_Get_Property(googletest-distribution SOURCE_DIR)
set(GOOGLETEST_DISTRIB_SOURCE_DIR "${SOURCE_DIR}")

ExternalProject_Add(
    googletest
    DEPENDS googletest-distribution
    DOWNLOAD_COMMAND ""
    SOURCE_DIR "${GOOGLETEST_DISTRIB_SOURCE_DIR}/googletest"
    CMAKE_ARGS
        "-DCMAKE_CXX_FLAGS=${TARGET_M}"
        "-Dgtest_force_shared_crt=TRUE"
    BUILD_BYPRODUCTS "googletest-prefix/src/googletest-build/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
    BUILD_BYPRODUCTS "googletest-prefix/src/googletest-build/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
    # Disable install step
    INSTALL_COMMAND ""
    # Wrap download, configure and build steps in a script to log output
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)


# Specify include dir
set(GTEST_INCLUDE_DIRS ${GOOGLETEST_DISTRIB_SOURCE_DIR}/googletest/include)

# Library
ExternalProject_Get_Property(googletest BINARY_DIR)
set(GOOGLETEST_BINARY_DIR "${BINARY_DIR}")
set(GTEST_LIBRARY_PATH ${GOOGLETEST_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})
set(GTEST_MAIN_LIBRARY_PATH ${GOOGLETEST_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX})
set(GTEST_LIBRARY gtest)
set(GTEST_MAIN_LIBRARY gtest_main)
add_library(${GTEST_LIBRARY} STATIC IMPORTED)
add_library(${GTEST_MAIN_LIBRARY} STATIC IMPORTED)
set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION ${GTEST_LIBRARY_PATH})
set_property(TARGET ${GTEST_MAIN_LIBRARY} PROPERTY IMPORTED_LOCATION ${GTEST_MAIN_LIBRARY_PATH})
add_dependencies(${GTEST_LIBRARY} googletest)
add_dependencies(${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY})

set(GTEST_LIBRARIES gtest gtest_main Threads::Threads)