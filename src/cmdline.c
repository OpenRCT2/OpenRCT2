/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <time.h>
#include <argparse/argparse.h>
#include "addresses.h"
#include "cmdline.h"
#include "interface/screenshot.h"
#include "network/network.h"
#include "openrct2.h"
#include "platform/platform.h"
#include "util/util.h"

#ifdef ENABLE_TESTS
	#include "../test/tests.h"
#else
	static int cmdline_for_test_error(const char **argv, int argc)
	{
		printf("OpenRCT2 has not been built with the test suite.\n");
		return -1;
	}
#endif

typedef struct tm tm_t;
typedef struct argparse_option argparse_option_t;
typedef struct argparse argparse_t;

typedef int (*cmdline_action)(const char **argv, int argc);

int gExitCode = 0;

#ifndef DISABLE_NETWORK
int gNetworkStart = NETWORK_MODE_NONE;
char gNetworkStartHost[128];
int gNetworkStartPort = NETWORK_DEFAULT_PORT;
#endif // DISABLE_NETWORK

static void print_launch_information();
static void print_version();
static int cmdline_call_action(const char **argv, int argc);

static const char *const usage[] = {
	"openrct2 <command> [options] [<args>]",
	"openrct2 <path> [options]",
	"openrct2 intro [options]",
	"openrct2 edit [path] [options]",
	NULL
};

/**
 * A shared entry point to OpenRCT2. The command lines must be parsed before any further action is done. Invalid command lines
 * will then terminate before any initialisation has even been done.
 * @returns 1 if the game should run, otherwise 0.
 */
int cmdline_run(const char **argv, int argc)
{
	//
	int version = 0, headless = 0, verbose = 0, width = 0, height = 0, port = 0;
	char *server = NULL;
	char *userDataPath = NULL;

	argparse_option_t options[] = {
		OPT_HELP(),
		OPT_BOOLEAN('v', "version", &version, "show version information and exit"),
		OPT_BOOLEAN(0, "headless", &headless, "run OpenRCT2 headless"),
		OPT_BOOLEAN(0, "verbose", &verbose, "log verbose messages"),
		OPT_INTEGER('m', "mode", &sprite_mode, "the type of sprite conversion. 0 = default, 1 = simple closest pixel match, 2 = dithering"),
		OPT_STRING(0, "server", &server, "server to connect to"),
		OPT_INTEGER(0, "port", &port, "port"),
		OPT_STRING(0, "user-data-path", &userDataPath, "path to the user data directory (containing config.ini)"),
		OPT_END()
	};

	argparse_t argparse;
	argparse_init(&argparse, options, usage, 0);
	argc = argparse_parse(&argparse, argc, argv);

	if (version) {
		print_version();
		return 0;
	}

	if (headless)
		gOpenRCT2Headless = true;

	if (verbose)
		_log_levels[DIAGNOSTIC_LEVEL_VERBOSE] = 1;

	if (userDataPath != NULL) {
		safe_strncpy(gCustomUserDataPath, userDataPath, sizeof(gCustomUserDataPath));
	}

#ifndef DISABLE_NETWORK
	if (port != 0) {
		gNetworkStart = NETWORK_MODE_SERVER;
		gNetworkStartPort = port;
	}

	if (server != NULL) {
		gNetworkStart = NETWORK_MODE_CLIENT;
		safe_strncpy(gNetworkStartHost, server, sizeof(gNetworkStartHost));
	}
#endif // DISABLE_NETWORK

	if (argc != 0) {
		// see comment next to cmdline_call_action for expected return codes
		gExitCode = cmdline_call_action(argv, argc);
		if (gExitCode < 0) {
			// action failed, don't change exit code
			// and don't start the game
			return 0;
		} else if (gExitCode > 0) {
			// action successful, but don't start the game
			// change exit code to success
			gExitCode = 0;
			return 0;
		}
		// start the game, so far exit code means success.
	}

	// Headless mode requires a park to open
	if (gOpenRCT2Headless) {
		if (str_is_null_or_empty(gOpenRCT2StartupActionPath)) {
			printf("You must specify a park to open in headless mode.\n");
			gExitCode = -1;
			return 0;
		}
	}

	if (verbose) {
		print_launch_information();
	}
	return 1;
}

static void print_launch_information()
{
	char buffer[256];
	time_t timer;
	tm_t* tmInfo;

	// Print name and version information
	openrct2_write_full_version_info(buffer, sizeof(buffer));
	printf("%s\n", buffer);
	printf("%s (%s)\n", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
	printf("@ %s\n\n", OPENRCT2_TIMESTAMP);

	// Print current time
	time(&timer);
	tmInfo = localtime(&timer);
	strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", tmInfo);
	printf("VERBOSE: time is %s\n", buffer);

	// TODO Print other potential information (e.g. user, hardware)
}

static void print_version()
{
	char buffer[256];
	openrct2_write_full_version_info(buffer, sizeof(buffer));
	printf("%s\n", buffer);
	printf("%s (%s)\n", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
}

static int cmdline_for_intro(const char **argv, int argc)
{
	gOpenRCT2StartupAction = STARTUP_ACTION_INTRO;
	return 0;
}

static int cmdline_for_edit(const char **argv, int argc)
{
	gOpenRCT2StartupAction = STARTUP_ACTION_EDIT;
	if (argc >= 1)
		safe_strncpy(gOpenRCT2StartupActionPath, argv[0], 512);

	return 0;
}

static int cmdline_for_none(const char **argv, int argc)
{
	assert(argc >= 1);

	if (platform_file_exists(argv[0])) {
		gOpenRCT2StartupAction = STARTUP_ACTION_OPEN;
		safe_strncpy(gOpenRCT2StartupActionPath, argv[0], 512);
		return 0;
	} else {
		fprintf(stderr, "error: %s does not exist\n", argv[0]);
		return -1;
	}
}

// see comment next to cmdline_call_action for expected return codes
struct { const char *firstArg; cmdline_action action; } cmdline_table[] = {
	{ "intro", cmdline_for_intro },
	{ "edit", cmdline_for_edit },
	{ "sprite", cmdline_for_sprite },
	{ "screenshot", cmdline_for_screenshot },

#ifdef ENABLE_TESTS
	{ "test", cmdline_for_test },
#else
	{ "test", cmdline_for_test_error },
#endif
};

/**
 * This function delegates starting the game to different handlers, if found.
 * 
 * Three cases of return values are supported:
 * - result < 0 means failure, will exit with error code
 *   This case is useful when user provided wrong arguments or the requested
 *   action failed
 * - result > 0 means success, won't start game, will exit program with success code
 *   This case is useful when you want to do some batch action and signalize
 *   success to the user.
 * - result == 0 means success, will launch the game.
 *   This is default when ran with no arguments.
 */
static int cmdline_call_action(const char **argv, int argc)
{
	for (int i = 0; i < countof(cmdline_table); i++) {
		if (_stricmp(cmdline_table[i].firstArg, argv[0]) != 0)
			continue;

		return cmdline_table[i].action(argv + 1, argc - 1);
	}

	return cmdline_for_none(argv, argc);
}
