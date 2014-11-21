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

#ifdef _MSC_VER
#include <time.h>
#endif
#include <argparse/argparse.h>
#include "addresses.h"
#include "cmdline.h"
#include "openrct2.h"
#include "platform/platform.h"

typedef struct tm tm_t;
typedef struct argparse_option argparse_option_t;
typedef struct argparse argparse_t;

int gExitCode = 0;

static void print_launch_information();

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
int cmdline_run(char *argv[], int argc)
{
	// For argparse's sake, add virtual first argument process path
	argc++;
	argv--;

	// 
	int version = 0, verbose = 0, width = 0, height = 0;

	argparse_option_t options[] = {
		OPT_HELP(),
		OPT_BOOLEAN('v', "version", &version, "show version information and exit"),
		OPT_BOOLEAN(0, "verbose", &verbose, "log verbose messages"),
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

	if (argc != 0) {
		if (_stricmp(argv[0], "intro") == 0) {
			gOpenRCT2StartupAction = STARTUP_ACTION_INTRO;
		} else if (_stricmp(argv[0], "edit") == 0) {
			gOpenRCT2StartupAction = STARTUP_ACTION_EDIT;
			if (argc >= 2)
				strcpy(gOpenRCT2StartupActionPath, argv[1]);
		} else {
			if (platform_file_exists(argv[0])) {
				gOpenRCT2StartupAction = STARTUP_ACTION_OPEN;
				strcpy(gOpenRCT2StartupActionPath, argv[0]);
			} else {
				fprintf(stderr, "error: %s does not exist\n", argv[0]);
				return 0;
			}
		}
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