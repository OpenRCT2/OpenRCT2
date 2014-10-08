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

#include <string.h>
#ifdef _MSC_VER
#include <time.h>
#endif
#include "cmdline.h"
#include "openrct2.h"

typedef struct tm tm_t;

int gExitCode = 0;

static void print_launch_information();

/**
 * A shared entry point to OpenRCT2. The command lines must be parsed before any further action is done. Invalid command lines
 * will then terminate before any initialisation has even been done.
 * @returns 1 if the game should run, otherwise 0.
 */
int cmdline_run(char *argv[], int argc)
{
	print_launch_information();

	if (argc > 0) {
		if (_stricmp(argv[0], "edit") == 0) {
			gOpenRCT2StartupAction = STARTUP_ACTION_EDIT;
			if (argc >= 2)
				strcpy(gOpenRCT2StartupActionPath, argv[1]);
		} else {
			gOpenRCT2StartupAction = STARTUP_ACTION_OPEN;
			strcpy(gOpenRCT2StartupActionPath, argv[0]);
		}
	}

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

//void check_cmdline_arg()
//{
//	int argc;
//	char **argv;
//	char *args;
//
//	args = RCT2_GLOBAL(0x009AC310, char*);
//	if (args == (char*)0xFFFFFFFF)
//		return;
//	RCT2_GLOBAL(0x009AC310, char*) = (char*)0xFFFFFFFF;
//
//	argv = CommandLineToArgvA(args, &argc);
//	if (argc > 0) {
//		if (_stricmp(argv[0], "edit") == 0) {
//			if (argc >= 1)
//				editor_load_landscape(argv[1]);
//		} else {
//			rct2_open_file(argv[0]);
//		}
//	}
//
//	LocalFree(argv);
//}