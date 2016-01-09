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

#include <time.h>
#include "addresses.h"
#include "audio/audio.h"
#include "audio/mixer.h"
#include "config.h"
#include "drawing/drawing.h"
#include "editor.h"
#include "game.h"
#include "input.h"
#include "interface/chat.h"
#include "interface/console.h"
#include "interface/viewport.h"
#include "intro.h"
#include "localisation/date.h"
#include "localisation/localisation.h"
#include "management/news_item.h"
#include "network/network.h"
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
#include "util/util.h"
#include "world/map.h"
#include "world/park.h"
#include "world/climate.h"
#include "world/scenery.h"
#include "world/sprite.h"

// rct2: 0x0097F67C
const char * const RCT2FilePaths[PATH_ID_END] = {
	"Data\\g1.dat",
	"Data\\plugin.dat",
	"Data\\css1.dat",
	"Data\\css2.dat",
	"Data\\css4.dat",
	"Data\\css5.dat",
	"Data\\css6.dat",
	"Data\\css7.dat",
	"Data\\css8.dat",
	"Data\\css9.dat",
	"Data\\css11.dat",
	"Data\\css12.dat",
	"Data\\css13.dat",
	"Data\\css14.dat",
	"Data\\css15.dat",
	"Data\\css3.dat",
	"Data\\css17.dat",
	"Data\\css18.dat",
	"Data\\css19.dat",
	"Data\\css20.dat",
	"Data\\css21.dat",
	"Data\\css22.dat",
	"Saved Games\\scores.dat",
	"Data\\css23.dat",
	"Data\\css24.dat",
	"Data\\css25.dat",
	"Data\\css26.dat",
	"Data\\css27.dat",
	"Data\\css28.dat",
	"Data\\css29.dat",
	"Data\\css30.dat",
	"Data\\css31.dat",
	"Data\\css32.dat",
	"Data\\css33.dat",
	"Data\\css34.dat",
	"Data\\css35.dat",
	"Data\\css36.dat",
	"Data\\css37.dat",
	"Data\\css38.dat",
	"Data\\CUSTOM1.WAV",
	"Data\\CUSTOM2.WAV",
	"Data\\css39.dat",
	"Data\\css40.dat",
	"Data\\css41.dat",
	"Scenarios\\Six Flags Magic Mountain.SC6",
	"Data\\css42.dat",
	"Data\\css43.dat",
	"Data\\css44.dat",
	"Data\\css45.dat",
	"Data\\css46.dat",
	"Data\\css50.dat"
};

uint32 gCurrentDrawCount = 0;

typedef struct tm tm_t;

void print_launch_information();

int rct2_init_directories();
int rct2_startup_checks();

static void rct2_draw_fps();

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
	util_srand((unsigned int)time(0));
	RCT2_GLOBAL(0x009DEA69, short) = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAY, short);
	RCT2_GLOBAL(0x009DEA6B, short) = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MONTH, short);
	if (!rct2_init_directories())
		return 0;

	if (!rct2_startup_checks())
		return 0;

	config_reset_shortcut_keys();
	config_shortcut_keys_load();
	gInputPlaceObjectModifier = PLACE_OBJECT_MODIFIER_NONE;
	// config_load();

	object_list_load();
	scenario_load_list();

	ride_list_item item = { 253, 0 };
	track_load_list(item);

	gfx_load_g1();
	gfx_load_g2();
	font_sprite_initialise_characters();
	if (!gOpenRCT2Headless) {
		platform_init();
		audio_init_ride_sounds_and_info();
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

	char separator[] = {platform_get_path_separator(), 0};

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char), gConfigGeneral.game_path);

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH_SLASH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH_SLASH, char), separator);

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH, char), separator);
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH, char), "Saved Games");
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH, char), separator);

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), separator);
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), "Scenarios");
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), separator);
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), "*.SC6");

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_LANDSCAPES_PATH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_LANDSCAPES_PATH, char), separator);
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_LANDSCAPES_PATH, char), "Landscapes");
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_LANDSCAPES_PATH, char), separator);
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_LANDSCAPES_PATH, char), "*.SC6");

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), separator);
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), "ObjData");
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), separator);
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), "*.DAT");

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), separator);
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), "Tracks");
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), separator);
	strcat(RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), "*.TD?");

	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH_2, char), RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH, char));
	return 1;
}

void substitute_path(char *dest, const char *path, const char *filename)
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

	return 1;
}

void rct2_draw()
{
	redraw_rain();
	window_update_all();
	gfx_invalidate_pickedup_peep();
	gfx_draw_pickedup_peep();
	update_rain_animation();
	update_palette_effects();

	chat_draw();
	console_draw(RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo));

	if (RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) != 0) {
		//intro
	} else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TITLE_DEMO) {
		//title
		DrawOpenRCT2(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 20);
	} else {
		//game
	}

	if (gConfigGeneral.show_fps) {
		rct2_draw_fps();
	}

	gCurrentDrawCount++;
}

static uint32 _lastFPSUpdateTicks;
static uint32 _lastFPSTicks;
static float _currentFPS;

static float rct2_measure_fps()
{
	uint32 currentTicks = SDL_GetTicks();
	if (currentTicks - _lastFPSUpdateTicks > 500) {
		_lastFPSUpdateTicks = currentTicks;

		uint32 frameDelta = currentTicks - _lastFPSTicks;
		_currentFPS = 1000.0f / frameDelta;
	}
	_lastFPSTicks = currentTicks;
	return _currentFPS;
}

static void rct2_draw_fps()
{
	rct_drawpixelinfo *dpi = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	int x = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) / 2;
	int y = 2;

	// Measure FPS
	float fps = rct2_measure_fps();

	// Format string
	utf8 buffer[64];
	utf8 *ch = buffer;
	ch = utf8_write_codepoint(ch, FORMAT_MEDIUMFONT);
	ch = utf8_write_codepoint(ch, FORMAT_OUTLINE);
	ch = utf8_write_codepoint(ch, FORMAT_WHITE);

	const char *formatString = (_currentFPS >= 100.0f ? "%.0f" : "%.1f");
	sprintf(ch, formatString, _currentFPS);

	// Draw Text
	int stringWidth = gfx_get_string_width(buffer);
	x = x - (stringWidth / 2);
	gfx_draw_string(dpi, buffer, 0, x, y);

	// Make area dirty so the text doesn't get drawn over the last
	gfx_set_dirty_blocks(x - 16, y - 4, gLastDrawStringX + 16, 16);
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
	for (int pathId = 0; pathId < PATH_ID_END; pathId++) {
		if (!check_file_path(pathId)) {
			return 0;
		}
	}
	return 1;
}

/**
 *
 *  rct2: 0x00674CA5
 */
int check_file_path(int pathId)
{
	const utf8* path = get_file_path(pathId);
	SDL_RWops *file = SDL_RWFromFile(path, "rb");

	switch (pathId) {
	case PATH_ID_G1:
		if (file == NULL) {
			log_fatal("Could not find file %s", path);
			return 0;
		}
		break;

	case PATH_ID_CUSTOM1:
		if (file != NULL) {
			// Store file size in music_custom1_size @ 0x009AF164
			gRideMusicInfoList[36]->length = (uint32)SDL_RWsize(file);
		}
		break;

	case PATH_ID_CUSTOM2:
		if (file != NULL) {
			// Store file size in music_custom2_size @ 0x009AF16E
			gRideMusicInfoList[37]->length = (uint32)SDL_RWsize(file);
		}
		break;
	}

	if (file != NULL) {
		SDL_RWclose(file);
	}

	return 1;
}

void rct2_update()
{
	int tick, tick2;

	tick = SDL_GetTicks();

	tick2 = tick - RCT2_GLOBAL(RCT2_ADDRESS_LAST_TICK_COUNT, sint32);
	RCT2_GLOBAL(RCT2_ADDRESS_TICKS_SINCE_LAST_UPDATE, sint16) = tick2 = min(tick2, 500);

	RCT2_GLOBAL(RCT2_ADDRESS_LAST_TICK_COUNT, sint32) = tick;
	if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) == 0)
		RCT2_GLOBAL(RCT2_ADDRESS_PALETTE_EFFECT_FRAME_NO, sint32) += tick2;

	// TODO: screenshot countdown process

	network_update();

	// check_cmdline_arg();
	// Screens
	if (RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) != 0)
		intro_update();
	else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TITLE_DEMO)
		title_update();
	else
		game_update();

	//stop_completed_sounds(); // removes other sounds that are no longer playing in directsound

	twitch_update();
	chat_update();
	console_update();
}

/**
 *
 *  rct2: 0x00674E6C
 */
const utf8 *get_file_path(int pathId)
{
	static utf8 path[MAX_PATH];
	strcpy(path, gConfigGeneral.game_path);

	// Make sure base path is terminated with a slash
	if (strlen(path) == 0 || path[strlen(path) - 1] != platform_get_path_separator()) {
		if (strlen(path) >= MAX_PATH - 1) {
			log_error("Path for %s too long", RCT2FilePaths[pathId]);
			path[0] = '\0';
			return path;
		}

		char separator[] = {platform_get_path_separator(), 0};
		strcat(path, separator);
	}

	// Concatenate file path
	if (strlen(path) + strlen(RCT2FilePaths[pathId]) > MAX_PATH) {
		log_error("Path for %s too long", RCT2FilePaths[pathId]);
		path[0] = '\0';
		return path;
	}

	char *pathp = path + strnlen(path, sizeof(path));
	strcat(path, RCT2FilePaths[pathId]);
	while (*pathp) {
		if (*pathp == '\\') *pathp = platform_get_path_separator();
		pathp++;
	}

	return path;
}

/**
 * Obtains basic system versions and capabilities.
 *  rct2: 0x004076B1
 */
void get_system_info()
{
#ifdef _WIN32
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
#endif // _WIN32
		RCT2_GLOBAL(RCT2_ADDRESS_OS_PLATFORM_ID, uint32) = -1;
		RCT2_GLOBAL(RCT2_ADDRESS_OS_MAJOR_VERSION, uint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_OS_MINOR_VERSION, uint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_OS_BUILD_NUMBER, uint32) = 0;
#ifdef _WIN32
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
#else
	STUB();
#endif // _WIN32
}


/**
 * Obtains os system time (day, month, year and day of the week).
 *  rct2: 0x00407671
 */
void get_system_time()
{
	rct2_date date;
	platform_get_date(&date);
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAY, sint16) = date.day;
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MONTH, sint16) = date.month;
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_YEAR, sint16) = date.year;
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAYOFWEEK, sint16) = date.day_of_week;
}

/**
 * Obtains os local time (hour and minute)
 *  rct2: 0x006C45E7;
 */
void get_local_time()
{
	rct2_time t;
	platform_get_time(&t);
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_HOUR, sint16) = t.hour;
	RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MINUTE, sint16) = t.minute;
}

/**
 * RCT2 and this DLL can not free each other's allocated memory blocks. Use this to allocate memory if RCT2 is still able to
 * free it.
 *  rct2: 0x004068B2
 */
void *rct2_malloc(size_t numBytes)
{
	#ifdef _WIN32
	return RCT2_CALLFUNC_1(0x004068B2, void*, size_t, numBytes);
	#else
	//log_warning("call rct's function");
	return malloc(numBytes);
	#endif // _WIN32
}

/**
 * RCT2 and this DLL can not free each other's allocated memory blocks. Use this to reallocate memory if RCT2 is still able to
 * free it.
 *  rct2: 0x004068BD
 */
void *rct2_realloc(void *block, size_t numBytes)
{
	#ifdef _WIN32
	return RCT2_CALLFUNC_2(0x004068BD, void*, void*, size_t, block, numBytes);
	#else
	//log_warning("call rct's function");
	return realloc(block, numBytes);
	#endif // _WIN32
}

/**
 * RCT2 and this DLL can not free each other's allocated memory blocks. Use this to free memory that was allocated by RCT2.
 *  rct2: 0x004068CD
 */
void rct2_free(void *block)
{
	#ifdef _WIN32
	RCT2_CALLPROC_1(0x004068CD, void*, block);
	#else
	//log_warning("call rct's function");
	free(block);
	#endif // _WIN32
}
