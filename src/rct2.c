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
 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <setjmp.h>
#include <windows.h>
#include <SDL.h>
#include "addresses.h"
#include "climate.h"
#include "config.h"
#include "date.h"
#include "game.h"
#include "gfx.h"
#include "intro.h"
#include "map.h"
#include "news_item.h"
#include "osinterface.h"
#include "park.h"
#include "rct2.h"
#include "ride.h"
#include "scenario.h"
#include "title.h"
#include "viewport.h"

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
	RCT2_GLOBAL(0x01423A08, HINSTANCE) = hInstance;
	RCT2_GLOBAL(RCT2_ADDRESS_CMDLINE, LPSTR) = lpCmdLine;
	RCT2_CALLPROC(0x004076B1); // get_system_info()
	RCT2_CALLPROC(0x0040502E); // get_dsound_devices()

	rct2_init();
	rct2_loop();
	osinterface_free();
	exit(0);

	return 0;
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

void rct2_init()
{
	RCT2_GLOBAL(0x00F663AC, int) = 0;
	RCT2_GLOBAL(0x009AC310, char*) = RCT2_GLOBAL(RCT2_ADDRESS_CMDLINE, char*);
	get_system_time();
	RCT2_GLOBAL(0x009DEA69, short) = RCT2_GLOBAL(0x01424304, short);
	RCT2_GLOBAL(0x009DEA6B, short) = RCT2_GLOBAL(0x01424304, short);
	rct2_init_directories();
	rct2_startup_checks();
	RCT2_CALLPROC_EBPSAFE(0x06E3604); // reset_keyboard_shortcuts()
	RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) = 0;
	config_load();
	// RCT2_CALLPROC_EBPSAFE(0x00674B81); // pointless expansion pack crap
	RCT2_CALLPROC_EBPSAFE(0x006A8B40); // object_load_list()
	scenario_load_list();
	RCT2_CALLPROC_X(0x006CED50, 0, 0, 0, 253, 0, 0, 0); // track_load_list(253)
	gfx_load_g1();
	RCT2_CALLPROC_EBPSAFE(0x006C19AC);
	osinterface_init();
	RCT2_CALLPROC_EBPSAFE(0x006BA8E0); // init_audio();
	viewport_init_all();
	news_item_init_queue();
	RCT2_CALLPROC_EBPSAFE(0x006C45E7); // get local time
	RCT2_CALLPROC_EBPSAFE(0x00667104);
	RCT2_CALLPROC_EBPSAFE(0x006C4209);
	RCT2_CALLPROC_EBPSAFE(0x0069EB13);
	ride_init_all();
	RCT2_CALLPROC_EBPSAFE(0x0068F083); // window guest list init vars a
	RCT2_CALLPROC_EBPSAFE(0x006BD3A4);
	map_init();
	park_init();
	RCT2_CALLPROC_EBPSAFE(0x0066B5C0); // 0x0066B5C0 (part of 0x0066B3E8) screen_game_create_windows()
	date_reset();
	climate_reset(CLIMATE_COOL_AND_WET);
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	RCT2_CALLPROC_EBPSAFE(0x006ACA58);
	RCT2_CALLPROC_EBPSAFE(0x0068F050); // window guest list init vars b
	RCT2_CALLPROC_EBPSAFE(0x006BD39C);

	title_load();

	gfx_clear(RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo), 10);
	// RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, int) = 8;
	RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, int) = 0;
}

// rct2: 0x00683499
void rct2_init_directories()
{
	strcpy(RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char), "C:\\Program Files (x86)\\Infogrames\\RollerCoaster Tycoon 2");

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

// rct2: 0x00674B42
void rct2_startup_checks()
{
	// check if game is already running

	RCT2_CALLPROC(0x00674C0B);
}

void rct2_update()
{
	// Set 0x009DE564 to the value of esp
	// RCT2 sets the stack pointer to the value of this address when ending the current game tick from anywhere
	__asm {
		mov eax, 009DE564h
		mov [eax], esp
	}

	if (!setjmp(_end_update_jump))
		rct2_update_2();
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
char *get_file_path(int pathId)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	ebx = pathId;
	RCT2_CALLFUNC_X(0x00674E6C, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return (char*)ebx;
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