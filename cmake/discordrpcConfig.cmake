# This file is modified from duktapeConfig.cmake

# - Try to find discordrpc
# Once done this will define
#
#  DISCORDRPC_FOUND - system has discordrpc
#  DISCORDRPC_INCLUDE_DIRS - the discordrpc include directory
#  DISCORDRPC_LIBRARIES - Link these to use discordrpc
#  DISCORDRPC_DEFINITIONS - Compiler switches required for using discordrpc
#

PKG_CHECK_MODULES(PC_DISCORD_RPC QUIET discord-rpc libdiscord-rpc)

find_path(DISCORDRPC_INCLUDE_DIR discord_rpc.h
    HINTS ${PC_DISCORD_RPC_INCLUDEDIR} ${PC_DISCORD_RPC_INCLUDE_DIRS}
    PATH_SUFFIXES discord-rpc)

find_library(DISCORDRPC_LIBRARY
    NAMES discord-rpc libdiscord-rpc
    HINTS ${PC_DISCORD_RPC_LIBDIR} ${PC_DISCORD_RPC_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(discordrpc
    REQUIRED_VARS DISCORDRPC_LIBRARY DISCORDRPC_INCLUDE_DIR)

if (DISCORDRPC_FOUND)
    set (DISCORDRPC_LIBRARIES ${DISCORDRPC_LIBRARY})
    set (DISCORDRPC_INCLUDE_DIRS ${DISCORDRPC_INCLUDE_DIR} )
endif ()

MARK_AS_ADVANCED(
    DISCORDRPC_INCLUDE_DIR
    DISCORDRPC_LIBRARY
)
