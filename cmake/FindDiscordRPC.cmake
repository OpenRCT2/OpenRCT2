include(LibFindMacros)

libfind_pkg_check_modules(DISCORDRPC_PKGCONF DISCORDRPC)

find_path(DISCORDRPC_INCLUDE_DIR
	NAMES discord_rpc.h
	PATHS
		${DISCORDRPC_PKGCONF_INCLUDE_DIRS}
		"/usr/include"
		"/usr/local/include"
)

find_library(DISCORDRPC_LIBRARY
	NAMES discord-rpc
	PATHS
		${DISCORDRPC_PKGCONF_LIBRARY_DIRS}
        "/usr/lib"
        "/usr/lib64"
        "/usr/local/lib"
        "/usr/local/lib64"
)

set(DISCORDRPC_PROCESS_INCLUDES DISCORDRPC_INCLUDE_DIR)
set(DISCORDRPC_PROCESS_LIBS DISCORDRPC_LIBRARY)
libfind_process(DISCORDRPC)
