# Helpers for inter-procedural optimizations (IPO)

option(DISABLE_IPO "Disable IPO in supported release builds." OFF)

# Enabled IPO for a LIST of CMake build types.
# Provides IPO_BUILD_ENABLED to the parent scope.
# Make sure to supply the build types in UPPER CASE
function(ipo_enable IPO_ENABLED_BUILDS)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_LOG)

    set(IPO_BUILD_ENABLED OFF PARENT_SCOPE)
    string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "CMAKE_BUILD_TYPE not explicitly set. Not enabling IPO.")
    elseif(IPO_SUPPORTED AND ${CMAKE_BUILD_TYPE_UPPER} IN_LIST IPO_ENABLED_BUILDS)
        if(NOT DISABLE_IPO)
            message(STATUS "IPO supported and enabled in ${CMAKE_BUILD_TYPE}.")
            set(IPO_BUILD_ENABLED ON PARENT_SCOPE)
        else()
            message(STATUS "IPO explicitly disabled.")
        endif()
    elseif(NOT IPO_SUPPORTED AND ${CMAKE_BUILD_TYPE_UPPER} IN_LIST IPO_ENABLED_BUILDS)
        message(STATUS "IPO not supported: ${IPO_LOG}.")
    endif()
endfunction()

# Sets appropriate IPO properties on target _target
# if IPO is supported and enabled for the current build.
function(ipo_set_target_properties _target)
    if(IPO_BUILD_ENABLED)
        set_property(TARGET ${_target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endfunction()
