/*****************************************************************************
 * Copyright (c) 2014 Ted John, Matthias Lanzinger
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

#pragma warning(disable : 4996) // GetVersionExA deprecated

#include <string.h>
#include <setjmp.h>
#ifdef _MSC_VER
#include <time.h>
#endif
#include <windows.h>
#include <shlobj.h>
#include <SDL.h>
#include "addresses.h"
#include "audio.h"
#include "climate.h"
#include "config.h"
#include "date.h"
#include "game.h"
#include "gfx.h"
#include "intro.h"
#include "map.h"
#include "news_item.h"
#include "object.h"
#include "osinterface.h"
#include "park.h"
#include "rct2.h"
#include "ride.h"
#include "scenario.h"
#include "title.h"
#include "track.h"
#include "viewport.h"

typedef struct tm tm_t;

void print_launch_information();

void rct2_init_directories();
void rct2_startup_checks();


static void rct2_init();
static void rct2_loop();
static void rct2_update();
static void rct2_update_2();

static int _finished;
static jmp_buf _end_update_jump;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	return TRUE;
}

__declspec(dllexport) int StartOpenRCT(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	print_launch_information();

	// Begin RCT2
	RCT2_GLOBAL(0x01423A08, HINSTANCE) = hInstance;
	RCT2_GLOBAL(RCT2_ADDRESS_CMDLINE, LPSTR) = lpCmdLine;
	get_system_info();

	audio_init();
	audio_get_devices();
	RCT2_CALLPROC(0x0040502E); // get_dsound_devices()
	
	config_init();
	rct2_init();
	rct2_loop();
	osinterface_free();
	exit(0);

	return 0;
}

void print_launch_information()
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

void rct2_loop()
{
	int last_tick = 0;

	_finished = 0;
	do {
		if (SDL_GetTicks() - last_tick < 25)
			continue;
		last_tick = SDL_GetTicks();

		osinterface_process_messages();
		rct2_update();
		osinterface_draw();
	} while (!_finished);
}

void rct2_finish()
{
	_finished = 1;
}

void rct2_quit() {
	if (gGeneral_config.confirmation_prompt) {
		RCT2_GLOBAL(RCT2_ADDRESS_SAVE_PROMPT_MODE, uint16) = PM_QUIT;
		window_save_prompt_open();
	} else
		rct2_finish();
}

void rct2_init()
{
	RCT2_GLOBAL(0x00F663AC, int) = 0;
	RCT2_GLOBAL(0x009AC310, char*) = RCT2_GLOBAL(RCT2_ADDRESS_CMDLINE, char*);
	get_system_time();
	RCT2_GLOBAL(0x009DEA69, short) = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAY, short);
	RCT2_GLOBAL(0x009DEA6B, short) = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAY, short);
	rct2_init_directories();
	rct2_startup_checks();
	config_reset_shortcut_keys();
	RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) = 0;
	config_load();
	// RCT2_CALLPROC_EBPSAFE(0x00674B81); // pointless expansion pack crap
	object_list_load();
	scenario_load_list();
	track_load_list(253);
	gfx_load_g1();
	//RCT2_CALLPROC_EBPSAFE(0x006C19AC); //Load character widths
	gfx_load_character_widths();
	
	osinterface_init();
	RCT2_CALLPROC_EBPSAFE(0x006BA8E0); // init_audio();
	viewport_init_all();
	news_item_init_queue();
	get_local_time();
	reset_park_entrances();
	reset_saved_strings();
	RCT2_CALLPROC_EBPSAFE(0x0069EB13); //Sprite list reset/load
	ride_init_all();
	window_guest_list_init_vars_a();
	RCT2_CALLPROC_EBPSAFE(0x006BD3A4); //Peep?
	map_init();
	park_init();
	RCT2_CALLPROC_EBPSAFE(0x0066B5C0); // 0x0066B5C0 (part of 0x0066B3E8) screen_game_create_windows()
	date_reset();
	climate_reset(CLIMATE_COOL_AND_WET);
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	window_new_ride_init_vars();
	window_guest_list_init_vars_b();
	window_staff_init_vars();

	title_load();

	gfx_clear(RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo), 10);
	RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) = gGeneral_config.play_intro ? 8 : 255;
}

// rct2: 0x00683499
void rct2_init_directories()
{
	// check install directory
	if ( !osinterface_directory_exists(gGeneral_config.game_path) ) {
		osinterface_show_messagebox("Invalid RCT2 installation path. Please correct in config.ini.");
		exit(-1);
	}

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char), gGeneral_config.game_path);

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH_SLASH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH_SLASH, char), "\\");

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH, char), "\\Saved Games\\");

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), "\\Scenarios\\*.SC6");

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_LANDSCAPES_PATH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_LANDSCAPES_PATH, char), "\\Landscapes\\*.SC6");

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), "\\ObjData\\*.DAT");

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), "\\Tracks\\*.TD?");

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH_2, char), RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH, char));
}

void subsitute_path(char *dest, const char *path, const char *filename)
{
	while (*path != '*') {
		*dest++ = *path++;
	}
	strcpy(dest, filename);
}

// rct2: 0x00674B42
void rct2_startup_checks()
{
	// check if game is already running

	RCT2_CALLPROC_EBPSAFE(0x00674C0B);
}

void rct2_update()
{
	// Set 0x009DE564 to the value of esp
	// RCT2 sets the stack pointer to the value of this address when ending the current game tick from anywhere
	#ifdef _MSC_VER
	__asm {
		mov eax, 009DE564h
		mov [eax], esp
	}
	#else
	__asm__ ( "\
	\n\
		mov eax, 0x009DE564 	\n\
		mov [eax], esp 	\n\
	 " : : : "eax" );
	#endif

	if (!setjmp(_end_update_jump))
		rct2_update_2();
}

void check_cmdline_arg()
{
	if(RCT2_GLOBAL(0x009AC310, uint32) == 0xFFFFFFFF)
		return;

	char *arg = RCT2_GLOBAL(0x009AC310, char *);
	char processed_arg[255];
	int len, i, j;
	int quote = 0;
	int last_period = 0;

	RCT2_GLOBAL(0x009AC310, uint32) = 0xFFFFFFFF;
	len = strlen(arg);

	for(i = 0, j = 0; i < len; i ++)
	{
		if(arg[i] == '\"')
		{
			if(quote)
				quote = 0;
			else
				quote = 1;
			continue;
		}
		if(arg[i] == ' ' && !quote)
			break;
		if(arg[i] == '.')
			last_period = i;
		processed_arg[j ++] = arg[i];
	}
	processed_arg[j ++] = 0;

	if (!_stricmp(processed_arg + last_period, "sv6"))
	{
		strcpy((char*)0x00141EF68, processed_arg);
		game_load_save();
	}
	else if (!_stricmp(processed_arg + last_period, "sc6"))
	{
		//TODO: scenario install
	}
	else if (!_stricmp(processed_arg + last_period, "td6") || !_stricmp(processed_arg + last_period, "td4"))
	{
		//TODO: track design install
	}
}

void rct2_update_2()
{
	int tick, tick2;

	tick = timeGetTime();

	RCT2_GLOBAL(0x009DE588, sint16) = tick2 = tick - RCT2_GLOBAL(0x009DE580, sint32);
	if (RCT2_GLOBAL(0x009DE588, sint16) > 500)
		RCT2_GLOBAL(0x009DE588, sint16) = 500;

	RCT2_GLOBAL(0x009DE580, sint32) = tick;
	if (RCT2_GLOBAL(0x009DEA6E, uint8) == 0)
		RCT2_GLOBAL(0x009DE584, sint32) += tick2;

	if (RCT2_GLOBAL(0x009DEA6E, uint8) == 0)
		RCT2_GLOBAL(0x009DE584, sint32) += tick2;

	if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) != 0)
		RCT2_GLOBAL(0x009DE588, sint16) = 31;

	// TODO: screenshot countdown process

	check_cmdline_arg();
	// Screens
	if (RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) != 0)
		intro_update();
	else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 1)
		title_update();
	else
		game_update();
}

void rct2_endupdate()
{
	longjmp(_end_update_jump, 0);
}

/**
 * 
 *  rct2: 0x00674E6C
 */
const char *get_file_path(int pathId)
{
	static char path[MAX_PATH]; // get_file_path_buffer @ 0x009E3605

	// The original implementation has a check for 0x009AA0B1 here. That flag is set
	// by check_file_path if the file cannot be found in its default location, but this
	// only seems to be the case for versions that require a CD-ROM. Therefore it has
	// been removed.
	strcpy(path, gGeneral_config.game_path);

	// Make sure base path is terminated with a slash
	if (strlen(path) == 0 || path[strlen(path) - 1] != '\\')
	{
		if (strlen(path) >= MAX_PATH - 1)
		{
			RCT2_ERROR("Path for %s too long", file_paths[pathId]);
			path[0] = '\0';
			return path;
		}

		strcat(path, "\\");
	}

	// Concatenate file path
	if (strlen(path) + strlen(file_paths[pathId]) > MAX_PATH)
	{
		RCT2_ERROR("Path for %s too long", file_paths[pathId]);
		path[0] = '\0';
		return path;
	}

	strcat(path, file_paths[pathId]);

	return path;
}

/**
 *  Obtains basic system versions and capabilities.
 *  rct2: 0x004076B1
 */
void get_system_info()
{
	OSVERSIONINFO versionInfo;
	SYSTEM_INFO sysInfo;
	MEMORYSTATUS memInfo;

	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&versionInfo)) {
		RCT2_GLOBAL(RCT2_ADDRESS_OS_PLATFORM_ID, uint32) = versionInfo.dwPlatformId;
		RCT2_GLOBAL(RCT2_ADDRESS_OS_MAJOR_VERSION, uint32) = versionInfo.dwMajorVersion;
		RCT2_GLOBAL(RCT2_ADDRESS_OS_MINOR_VERSION, uint32) = versionInfo.dwMinorVersion;
		RCT2_GLOBAL(RCT2_ADDRESS_OS_BUILD_NUMBER, uint32) = versionInfo.dwBuildNumber;
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_OS_PLATFORM_ID, uint32) = -1;
		RCT2_GLOBAL(RCT2_ADDRESS_OS_MAJOR_VERSION, uint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_OS_MINOR_VERSION, uint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_OS_BUILD_NUMBER, uint32) = 0;
	}

	GetSystemInfo(&sysInfo);
	// RCT2 only has 2 bytes reserved for OEM_ID even though it should be a DWORD
	RCT2_GLOBAL(RCT2_ADDRESS_SYS_OEM_ID, uint16) = (uint16)sysInfo.dwOemId;
	RCT2_GLOBAL(RCT2_ADDRESS_SYS_CPU_LEVEL, uint16) = sysInfo.wProcessorLevel;
	RCT2_GLOBAL(RCT2_ADDRESS_SYS_CPU_REVISION, uint16) = sysInfo.wProcessorRevision;
	RCT2_GLOBAL(RCT2_ADDRESS_SYS_CPU_NUMBER, uint32) = sysInfo.dwNumberOfProcessors;

	GlobalMemoryStatus(&memInfo);
	RCT2_GLOBAL(RCT2_ADDRESS_MEM_TOTAL_PHYSICAL, uint32) = memInfo.dwTotalPhys;
	RCT2_GLOBAL(RCT2_ADDRESS_MEM_TOTAL_PAGEFILE, uint32) = memInfo.dwTotalPageFile;
	RCT2_GLOBAL(RCT2_ADDRESS_MEM_TOTAL_VIRTUAL, uint32) = memInfo.dwTotalVirtual;

	DWORD size = 80;
	GetUserName((char*)RCT2_ADDRESS_OS_USER_NAME, &size);
	size = 80;
	GetComputerName((char*)RCT2_ADDRESS_OS_COMPUTER_NAME, &size);

	// Screen Display Width/Height but RCT_ADDRESS_SCREEN_HEIGHT/WIDTH already taken?
	RCT2_GLOBAL(0x01423C08, sint32) = GetSystemMetrics(SM_CXSCREEN);
	RCT2_GLOBAL(0x01423C0C, sint32) = GetSystemMetrics(SM_CYSCREEN);

	HDC screenHandle = GetDC(NULL);
	if (screenHandle) {
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_CAP_BPP, sint32) = GetDeviceCaps(screenHandle, BITSPIXEL);
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_CAP_RASTER_STRETCH, sint32) = GetDeviceCaps(screenHandle, RASTERCAPS) >> 8; 
		ReleaseDC(NULL, screenHandle);
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_CAP_BPP, sint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_CAP_RASTER_STRETCH, sint32) = 0;
	}

	RCT2_GLOBAL(0x01423C1C, uint32) = (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_CAP_BPP, sint32) >= 8);
	if (RCT2_GLOBAL(RCT2_ADDRESS_OS_MAJOR_VERSION, uint32) < 4 || RCT2_GLOBAL(0x1423C10, sint32) < 4)
		RCT2_GLOBAL(0x1423C18, sint32) = 0;
	else
		RCT2_GLOBAL(0x1423C18, sint32) = 1;

	RCT2_GLOBAL(0x01423C20, uint32) = (SDL_HasMMX() == SDL_TRUE);
}


/**
 * Obtains os system time (day, month, year and day of the week).
 *  rct2: 0x00407671
 */
void get_system_time()
{
	SYSTEMTIME systime;

	GetSystemTime(&systime);
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAY, sint16) = systime.wDay;
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MONTH, sint16) = systime.wMonth;
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_YEAR, sint16) = systime.wYear;
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAYOFWEEK, sint16) = systime.wDayOfWeek;
}

/**
 * Obtains os local time (hour and minute)
 *  rct2: 0x006C45E7;
 */
void get_local_time()
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_HOUR, sint16) = systime.wHour;
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MINUTE, sint16) = systime.wMinute;
}

/**
 * RCT2 and this DLL can not free each other's allocated memory blocks. Use this to allocate memory if RCT2 is still able to
 * free it.
 *  rct2: 0x004068B2
 */
void *rct2_malloc(size_t numBytes)
{
	return RCT2_CALLFUNC_1(0x004068B2, void*, size_t, numBytes);
}

/**
 * RCT2 and this DLL can not free each other's allocated memory blocks. Use this to reallocate memory if RCT2 is still able to
 * free it.
 *  rct2: 0x004068BD
 */
void *rct2_realloc(void *block, size_t numBytes)
{
	return RCT2_CALLFUNC_2(0x004068BD, void*, void*, size_t, block, numBytes);
}

/**
 * RCT2 and this DLL can not free each other's allocated memory blocks. Use this to free memory that was allocated by RCT2.
 *  rct2: 0x004068DE
 */
void rct2_free(void *block)
{
	RCT2_CALLPROC_1(0x004068DE, void*, block);
}
