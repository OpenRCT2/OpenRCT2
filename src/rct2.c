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

#include <setjmp.h>
#include <time.h>
#include "addresses.h"
#include "audio/audio.h"
#include "audio/mixer.h"
#include "config.h"
#include "drawing/drawing.h"
#include "editor.h"
#include "game.h"
#include "interface/console.h"
#include "interface/viewport.h"
#include "intro.h"
#include "localisation/date.h"
#include "localisation/localisation.h"
#include "management/news_item.h"
#include "network/twitch.h"
#include "object.h"
#include "openrct2.h"
#include "peep/staff.h"
#include "platform/platform.h"
#include "rct1.h"
#include "ride/ride.h"
#include "ride/track.h"
#include "scenario.h"
#include "title.h"
#include "world/map.h"
#include "world/park.h"
#include "world/climate.h"
#include "world/scenery.h"
#include "world/sprite.h"

typedef struct tm tm_t;

void print_launch_information();

int rct2_init_directories();
int rct2_startup_checks();

static void rct2_update_2();

static jmp_buf _end_update_jump;

void rct2_quit()
{
	RCT2_GLOBAL(RCT2_ADDRESS_SAVE_PROMPT_MODE, uint16) = PM_QUIT;
	window_save_prompt_open();
}

int rct2_init()
{
	log_verbose("initialising game");

	RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32) = 0;
	RCT2_GLOBAL(0x009AC310, char*) = RCT2_GLOBAL(RCT2_ADDRESS_CMDLINE, char*);
	get_system_time();
	srand((unsigned int)time(0));
	RCT2_GLOBAL(0x009DEA69, short) = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAY, short);
	RCT2_GLOBAL(0x009DEA6B, short) = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MONTH, short);
	if (!rct2_init_directories())
		return 0;

	if (!rct2_startup_checks())
		return 0;

	config_reset_shortcut_keys();
	config_shortcut_keys_load();
	RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) = 0;
	// config_load();

	object_list_load();
	scenario_load_list();

	ride_list_item item = { 253, 0 };
	track_load_list(item);

	gfx_load_g1();
	gfx_load_g2();
	gfx_load_character_widths();
	if (!gOpenRCT2Headless) {
		platform_init();
		audio_init1();
	}
	viewport_init_all();
	news_item_init_queue();
	get_local_time();
	reset_park_entrances();
	user_string_clear_all();
	reset_sprite_list();
	ride_init_all();
	window_guest_list_init_vars_a();
	staff_reset_modes();
	map_init(150);
	park_init();
	if (!gOpenRCT2Headless)
		window_title_menu_open();
	date_reset();
	climate_reset(CLIMATE_COOL_AND_WET);
	scenery_set_default_placement_configuration();
	window_new_ride_init_vars();
	window_guest_list_init_vars_b();
	window_staff_list_init_vars();

	if (!gOpenRCT2Headless) {
		title_load();

		gfx_clear(RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo), 10);
	}

	log_verbose("initialising game finished");
	return 1;
}

/**
 * 
 *  rct2: 0x00683499
 */
int rct2_init_directories()
{
	// windows_get_registry_install_info((rct2_install_info*)0x009AA10C, "RollerCoaster Tycoon 2 Setup", "MS Sans Serif", 0);

	// check install directory
	if (!platform_original_game_data_exists(gConfigGeneral.game_path)) {
		log_verbose("install directory does not exist or invalid directory selected, %s", gConfigGeneral.game_path);
		if (!config_find_or_browse_install_directory()) {
			log_fatal("Invalid RCT2 installation path. Please correct in config.ini.");
			return 0;
	}
	}

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char), gConfigGeneral.game_path);

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
	return 1;
}

void subsitute_path(char *dest, const char *path, const char *filename)
{
	while (*path != '*') {
		*dest++ = *path++;
	}
	strcpy(dest, filename);
}

/**
 * 
 *  rct2: 0x00674B42
 */
int rct2_startup_checks()
{
	if (!check_file_paths())
		return 0;

	if (!check_files_integrity())
		return 0;
	
	return 1;
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

void rct2_draw()
{
	redraw_rain();
	window_update_all();
	gfx_invalidate_pickedup_peep();
	gfx_draw_pickedup_peep();
	update_rain_animation();
	update_palette_effects();

	console_draw(RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo));

	if (RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) != 0) {
		//intro
	} else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TITLE_DEMO) {
		//title
		DrawOpenRCT2(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 20);
	} else {
		//game
	}
}

int rct2_open_file(const char *path)
{
	char *extension = strrchr(path, '.');
	if (extension == NULL)
		return 0;
	extension++;

	if (_stricmp(extension, "sv6") == 0) {
		strcpy((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2, path);
		game_load_save(path);
		return 1;
	} else if (_stricmp(extension, "sc6") == 0) {
		// TODO scenario install
		rct_scenario_basic scenarioBasic;
		strcpy(scenarioBasic.path, path);
		scenario_load_and_play_from_path(scenarioBasic.path);
	} else if (_stricmp(extension, "td6") == 0 || _stricmp(extension, "td4") == 0) {
		return 1;
	} else if (!_stricmp(extension, "td6") || !_stricmp(extension, "td4")) {
		// TODO track design install
		return 1;
	}

	return 0;
}

/**
 * 
 *  rct2: 0x00674C95
 */
int check_file_paths()
	{
	for (int pathId = 0; pathId < PATH_ID_END; pathId++)
		if (!check_file_path(pathId))
	return 0;

	return 1;
}

/**
 * 
 *  rct2: 0x00674CA5
 */
int check_file_path(int pathId)
{
	const char * path = get_file_path(pathId);
	HANDLE file = CreateFile(path, FILE_GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	switch (pathId) {
	case PATH_ID_G1:
		if (file == INVALID_HANDLE_VALUE) {
			// A data file is missing from the installation directory. The original implementation
			// asks for a CD-ROM path at this point and stores it in cdrom_path @ 0x9AA318.
			// The file_on_cdrom[pathId] @ 0x009AA0B flag is set to 1 as well.
			// For PATH_ID_SIXFLAGS_MAGICMOUNTAIN and PATH_ID_SIXFLAGS_BUILDYOUROWN,
			// the original implementation always assumes they are stored on CD-ROM.
			// This has been removed for now for the sake of simplicity and could be added
			// later in a more convenient way using the INI file.
			log_fatal("Could not find file %s", path);
			return 0;
		}
		break;

	case PATH_ID_CUSTOM1:
		if (file != INVALID_HANDLE_VALUE)
			ride_music_info_list[36]->length = SetFilePointer(file, 0, 0, FILE_END); // Store file size in music_custom1_size @ 0x009AF164
		break;

	case PATH_ID_CUSTOM2:
		if (file != INVALID_HANDLE_VALUE)
			ride_music_info_list[37]->length = SetFilePointer(file, 0, 0, FILE_END); // Store file size in music_custom2_size @ 0x009AF16E
		break;
	}

	if (file != INVALID_HANDLE_VALUE)
		CloseHandle(file);

	return 1;
}

/**
 * 
 *  rct2: 0x00674C0B
 */
int check_files_integrity()
	{
	int i;
	const char *path;
	HANDLE file;
		WIN32_FIND_DATA find_data;

	for (i = 0; files_to_check[i].pathId != PATH_ID_END; i++) {
		path = get_file_path(files_to_check[i].pathId);
		file = FindFirstFile(path, &find_data);

		if (file == INVALID_HANDLE_VALUE || find_data.nFileSizeLow != files_to_check[i].fileSize) {
			if (file != INVALID_HANDLE_VALUE)
				FindClose(file);

			log_fatal("Integrity check failed for %s", path);
			return 0;
		}

		FindClose(file);
	}

	return 1;
}

void rct2_update_2()
{
	int tick, tick2;

	tick = timeGetTime();

	tick2 = tick - RCT2_GLOBAL(0x009DE580, sint32);
	RCT2_GLOBAL(0x009DE588, sint16) = tick2 = min(tick2, 500);

	RCT2_GLOBAL(0x009DE580, sint32) = tick;
	if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) == 0)
		RCT2_GLOBAL(RCT2_ADDRESS_PALETTE_EFFECT_FRAME_NO, sint32) += tick2;

	if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) != 0)
		RCT2_GLOBAL(0x009DE588, sint16) = 31;

	// TODO: screenshot countdown process

	// check_cmdline_arg();
	// Screens
	if (RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) != 0)
		intro_update();
	else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TITLE_DEMO)
		title_update();
	else
		game_update();

	stop_completed_sounds(); // removes other sounds that are no longer playing in directsound

	twitch_update();
	console_update();
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

	// The original implementation checks if the file is on CD-ROM here (file_on_cdrom[pathId] @ 0x009AA0B1).
	// If so, the CD-ROM path (cdrom_path @ 0x9AA318) is used instead. This has been removed for now for
	// the sake of simplicity.
	strcpy(path, gConfigGeneral.game_path);

	// Make sure base path is terminated with a slash
	if (strlen(path) == 0 || path[strlen(path) - 1] != '\\')
	{
		if (strlen(path) >= MAX_PATH - 1)
		{
			log_error("Path for %s too long", file_paths[pathId]);
			path[0] = '\0';
			return path;
		}

		strcat(path, "\\");
	}

	// Concatenate file path
	if (strlen(path) + strlen(file_paths[pathId]) > MAX_PATH) {
		log_error("Path for %s too long", file_paths[pathId]);
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
 *  rct2: 0x004068CD
 */
void rct2_free(void *block)
{
	RCT2_CALLPROC_1(0x004068CD, void*, block);
}
