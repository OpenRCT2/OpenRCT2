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

int gNetworkStart = NETWORK_MODE_NONE;
char gNetworkStartHost[128];
int gNetworkStartPort = NETWORK_DEFAULT_PORT;

static void print_launch_information();
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
	int version = 0, verbose = 0, width = 0, height = 0, port = 0;
	char *server = NULL;

	argparse_option_t options[] = {
		OPT_HELP(),
		OPT_BOOLEAN('v', "version", &version, "show version information and exit"),
		OPT_BOOLEAN(0, "verbose", &verbose, "log verbose messages"),
		OPT_INTEGER('m', "mode", &sprite_mode, "the type of sprite conversion. 0 = default, 1 = simple closest pixel match, 2 = dithering"),
		OPT_STRING(0, "server", &server, "server to connect to"),
		OPT_INTEGER(0, "port", &port, "port"),
		OPT_END()
	};

	argparse_t argparse;
	argparse_init(&argparse, options, usage, 0);
	argc = argparse_parse(&argparse, argc, argv);

	if (version) {
		printf("%s v%s\n", OPENRCT2_NAME, OPENRCT2_VERSION);
		printf("%s (%s)\n", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
		printf("%s\n", OPENRCT2_TIMESTAMP);
		return 0;
	}

	if (verbose)
		_log_levels[DIAGNOSTIC_LEVEL_VERBOSE] = 1;

	if (port != 0) {
		gNetworkStart = NETWORK_MODE_SERVER;
		gNetworkStartPort = port;
	}

	if (server != NULL) {
		gNetworkStart = NETWORK_MODE_CLIENT;
		strncpy(gNetworkStartHost, server, sizeof(gNetworkStartHost));
	}

	if (argc != 0) {
		gExitCode = cmdline_call_action(argv, argc);
		if (gExitCode != 0)
			return 0;
	}

	print_launch_information();
	return 1;
}

static void print_launch_information()
{
	char buffer[32];
	time_t timer;
	tm_t* tmInfo;

	// Print version information
	printf("Starting %s v%s\n", OPENRCT2_NAME, OPENRCT2_VERSION);
	printf("  %s (%s)\n", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
	printf("  %s\n\n", OPENRCT2_TIMESTAMP);

	// Print current time
	time(&timer);
	tmInfo = localtime(&timer);
	strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", tmInfo);
	printf("Time: %s\n", buffer);

	// TODO Print other potential information (e.g. user, hardware)
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
		strcpy(gOpenRCT2StartupActionPath, argv[0]);

	return 0;
}

static int cmdline_for_none(const char **argv, int argc)
{
	assert(argc >= 1);

	if (platform_file_exists(argv[0])) {
		gOpenRCT2StartupAction = STARTUP_ACTION_OPEN;
		strcpy(gOpenRCT2StartupActionPath, argv[0]);
		return 0;
	} else {
		fprintf(stderr, "error: %s does not exist\n", argv[0]);
		return -1;
	}
}

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

static int cmdline_call_action(const char **argv, int argc)
{
	for (int i = 0; i < countof(cmdline_table); i++) {
		if (_stricmp(cmdline_table[i].firstArg, argv[0]) != 0)
			continue;

		return cmdline_table[i].action(argv + 1, argc - 1);
	}

	return cmdline_for_none(argv, argc);
}