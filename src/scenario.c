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

#include <windows.h>
#include "addresses.h"
#include "date.h"
#include "game.h"
#include "map.h"
#include "news_item.h"
#include "object.h"
#include "park.h"
#include "rct2.h"
#include "sawyercoding.h"
#include "scenario.h"
#include "strings.h"
#include "viewport.h"

#define UNINITIALISED_SCENARIO_LIST ((rct_scenario_basic*)-1)

#define RCT2_SCENARIO_LIST RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_LIST, rct_scenario_basic*)
#define RCT2_NUM_SCENARIOS RCT2_GLOBAL(RCT2_ADDRESS_NUM_SCENARIOS, sint32)
int _scenarioListSize;

static void scenario_list_add(char *path);
static void scenario_list_sort();
static int scenario_list_sort_compare(const void* a, const void* b);
static void scenario_scores_load();
static void scenario_scores_save();
static int scenario_load_basic(char *path);

static void subsitute_path(char *dest, char *path, char *filename)
{
	while (*path != '*') {
		*dest++ = *path++;
	}
	strcpy(dest, filename);
}

static rct_scenario_basic *get_scenario_by_filename(char *filename)
{
	int i;
	for (i = 0; i < RCT2_NUM_SCENARIOS; i++)
		if (strcmp(RCT2_SCENARIO_LIST[i].path, filename) == 0)
			return &(RCT2_SCENARIO_LIST[i]);

	return NULL;
}

/**
 * 
 *  rct2: 0x006775A8
 */
void scenario_load_list()
{
	int i;
	HANDLE hFindFile;
	WIN32_FIND_DATAA findFileData;

	// Load scores
	scenario_scores_load();

	// Set all scenarios to be invisible
	for (i = 0; i < RCT2_NUM_SCENARIOS; i++)
		RCT2_SCENARIO_LIST[i].flags &= ~SCENARIO_FLAGS_VISIBLE;

	// Enumerate through each scenario in the directory
	hFindFile = FindFirstFile(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), &findFileData);
	if (hFindFile != INVALID_HANDLE_VALUE) {
		do {
			scenario_list_add(findFileData.cFileName);
		} while (FindNextFile(hFindFile, &findFileData));
		FindClose(hFindFile);
	}

	// Sort alphabetically
	scenario_list_sort();

	// Save the scores
	scenario_scores_save();
}

static void scenario_list_add(char *path)
{
	int i;
	rct_scenario_basic *scenario;
	rct_s6_info *s6Info = 0x0141F570;

	// Check if scenario already exists in list, likely if in scores
	scenario = get_scenario_by_filename(path);
	if (scenario != NULL) {
		// Set 0141EF68 to the scenario path
		subsitute_path(
			RCT2_ADDRESS(0x0141EF68, char),
			RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char),
			path
		);

		// Load the basic scenario information
		if (!scenario_load_basic(RCT2_ADDRESS(0x0141EF68, char)))
			return;

		// 
		if (s6Info->var_000 != 255)
			return;

		// Update the scenario information
		scenario->flags |= SCENARIO_FLAGS_VISIBLE;
		scenario->category = s6Info->category;
		scenario->objective_type = s6Info->objective_type;
		scenario->objective_arg_1 = s6Info->objective_arg_1;
		scenario->objective_arg_2 = s6Info->objective_arg_2;
		scenario->objective_arg_3 = s6Info->objective_arg_3;
		strcpy(scenario->name, s6Info->name);
		strcpy(scenario->details, s6Info->details);
		return;
	}

	// Check if the scenario list buffer has room for another scenario
	if ((RCT2_NUM_SCENARIOS + 1) * sizeof(rct_scenario_basic) > _scenarioListSize) {
		// Allocate more room
		_scenarioListSize += 16 * sizeof(rct_scenario_basic);
		RCT2_SCENARIO_LIST = (rct_scenario_basic*)rct2_realloc(RCT2_SCENARIO_LIST, _scenarioListSize);
	}

	// Set 0141EF68 to the scenario path
	subsitute_path(
		RCT2_ADDRESS(0x0141EF68, char),
		RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char),
		path
	);

	// Load the scenario information
	if (!scenario_load_basic(RCT2_ADDRESS(0x0141EF68, char)))
		return;

	// 
	if (s6Info->var_000 != 255)
		return;

	// Increment the number of scenarios
	scenario = &RCT2_SCENARIO_LIST[RCT2_NUM_SCENARIOS];
	RCT2_NUM_SCENARIOS++;

	// Add this new scenario to the list
	strcpy(scenario->path, path);
	scenario->flags = SCENARIO_FLAGS_VISIBLE;
	if (RCT2_GLOBAL(0x009AA00C, uint8) & 1)
		scenario->flags |= SCENARIO_FLAGS_SIXFLAGS;
	scenario->category = s6Info->category;
	scenario->objective_type = s6Info->objective_type;
	scenario->objective_arg_1 = s6Info->objective_arg_1;
	scenario->objective_arg_2 = s6Info->objective_arg_2;
	scenario->objective_arg_3 = s6Info->objective_arg_3;
	strcpy(scenario->name, s6Info->name);
	strcpy(scenario->details, s6Info->details);
}

/**
* Sort the list of scenarios. This used to be an insertion sort which took
* place as each scenario loaded. It has now been changed to a quicksort which
* takes place after all the scenarios have been loaded in.
*  rct2: 0x00677C3B
*/
static void scenario_list_sort()
{
	qsort(
		RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_LIST, rct_scenario_basic*),
		RCT2_NUM_SCENARIOS,
		sizeof(rct_scenario_basic),
		scenario_list_sort_compare
	);
}

/**
* Basic scenario information compare function for sorting.
*  rct2: 0x00677C08
*/
static int scenario_list_sort_compare(const void* a, const void* b)
{
	return strcmp(((rct_scenario_basic*)a)->name, ((rct_scenario_basic*)b)->name);
}

/**
 * 
 *  rct2: 0x006775A8
 */
static void scenario_scores_load()
{
	HANDLE hFile;
	DWORD bytes_read;

	// Free scenario list if already allocated
	if (RCT2_SCENARIO_LIST != UNINITIALISED_SCENARIO_LIST) {
		rct2_free(RCT2_SCENARIO_LIST);
		RCT2_SCENARIO_LIST = UNINITIALISED_SCENARIO_LIST;
	}

	// Try and load the scores
	hFile = CreateFile(get_file_path(PATH_ID_SCORES), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		ReadFile(hFile, (void*)0x009A9FFC, 16, &bytes_read, NULL);
		if (bytes_read == 16) {
			_scenarioListSize = RCT2_NUM_SCENARIOS * sizeof(rct_scenario_basic);
			RCT2_SCENARIO_LIST = (rct_scenario_basic*)rct2_malloc(_scenarioListSize);
			ReadFile(hFile, RCT2_SCENARIO_LIST, _scenarioListSize, &bytes_read, NULL);
			CloseHandle(hFile);
			if (bytes_read == _scenarioListSize)
				return;
		} else {
			CloseHandle(hFile);
		}
	}

	// Unable to load scores, allocate some space for a reload
	RCT2_NUM_SCENARIOS = 0;
	_scenarioListSize = 0x4000;
	RCT2_SCENARIO_LIST = (rct_scenario_basic*)rct2_malloc(_scenarioListSize);
}

/**
 * 
 *  rct2: 0x00677B50
 */
static void scenario_scores_save()
{
	HANDLE hFile;
	DWORD bytes_written;

	hFile = CreateFile(get_file_path(PATH_ID_SCORES), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		WriteFile(hFile, (void*)0x009A9FFC, 16, &bytes_written, NULL);
		if (RCT2_NUM_SCENARIOS > 0)
			WriteFile(hFile, RCT2_SCENARIO_LIST, RCT2_NUM_SCENARIOS * sizeof(rct_scenario_basic), &bytes_written, NULL);
		CloseHandle(hFile);
	}
}

/**
 * Loads only the basic information from a scenario.
 *  rct2: 0x006761D6
 */
static int scenario_load_basic(char *path)
{
	HANDLE hFile;
	int _eax;
	rct_s6_header *s6Header = 0x009E34E4;
	rct_s6_info *s6Info = 0x0141F570;

	hFile = CreateFile(
		path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL
	);
	if (hFile != INVALID_HANDLE_VALUE) {
		RCT2_GLOBAL(0x009E382C, HANDLE*) = hFile;

		// Read first chunk
		sawyercoding_read_chunk(hFile, s6Header);
		if (s6Header->type == S6_TYPE_SCENARIO) {
			// Read second chunk
			sawyercoding_read_chunk(hFile, s6Info);
			CloseHandle(hFile);
			RCT2_GLOBAL(0x009AA00C, uint8) = 0;
			if (s6Info->flags != 255) {
				__asm {
					push ebp
					mov ebp, 0141F6F8h
					mov eax, 006A9428h
					call eax
					pop ebp
					mov _eax, eax
					jb loc_67628F
				}

				int ebp = RCT2_GLOBAL(0x009ADAF8, uint32);
				format_string(s6Info->name, RCT2_GLOBAL(ebp, sint16), NULL);
				format_string(s6Info->details, RCT2_GLOBAL(ebp + 4, sint16), NULL);
				RCT2_GLOBAL(0x009AA00C, uint8) = RCT2_GLOBAL(ebp + 6, uint8);
				RCT2_CALLPROC(0x006A982D);
				__asm mov _eax, eax
			loc_67628F :
				return _eax;
			}
			return 1;
		}
		CloseHandle(hFile);
	}

	RCT2_GLOBAL(0x009AC31B, sint8) = -1;
	RCT2_GLOBAL(0x009AC31C, sint16) = 3011;
	return 0;
}

/**
 * 
 *  rct2: 0x00676053
 * scenario (ebx)
 */
void scenario_load(char *path)
{
	HANDLE hFile;
	int i, j;
	rct_s6_header *s6Header = 0x009E34E4;
	rct_s6_info *s6Info = 0x0141F570;

	hFile = CreateFile(
		path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL
	);
	if (hFile != INVALID_HANDLE_VALUE) {
		RCT2_GLOBAL(0x009E382C, HANDLE*) = hFile;

		// Read first chunk
		sawyercoding_read_chunk(hFile, s6Header);
		if (s6Header->type == S6_TYPE_SCENARIO) {
			// Read second chunk
			sawyercoding_read_chunk(hFile, s6Info);

			// Read packed objects
			if (s6Header->num_packed_objects > 0) {
				j = 0;
				for (i = 0; i < s6Header->num_packed_objects; i++)
					j += object_load_packed();
				if (j > 0)
					object_load_list();
			}

			object_read_and_load_entries(hFile);

			// Read flags (16 bytes)
			sawyercoding_read_chunk(hFile, RCT2_ADDRESS_CURRENT_MONTH_YEAR);

			// Read map elements
			memset(RCT2_ADDRESS_MAP_ELEMENTS, 0, MAX_MAP_ELEMENTS * sizeof(rct_map_element));
			sawyercoding_read_chunk(hFile, RCT2_ADDRESS_MAP_ELEMENTS);

			// Read game data, including sprites
			sawyercoding_read_chunk(hFile, 0x010E63B8);

			// Read number of guests in park and something else
			sawyercoding_read_chunk(hFile, RCT2_ADDRESS_GUESTS_IN_PARK);

			// Read ?
			sawyercoding_read_chunk(hFile, 0x01357BC8);

			// Read park rating
			sawyercoding_read_chunk(hFile, RCT2_ADDRESS_CURRENT_PARK_RATING);

			// Read ?
			sawyercoding_read_chunk(hFile, 0x01357CF2);

			// Read ?
			sawyercoding_read_chunk(hFile, 0x0135832C);

			// Read ?
			sawyercoding_read_chunk(hFile, RCT2_ADDRESS_CURRENT_PARK_VALUE);

			// Read more game data, including research items and rides
			sawyercoding_read_chunk(hFile, 0x01358740);

			CloseHandle(hFile);

			// Check expansion pack
			// RCT2_CALLPROC_EBPSAFE(0x006757E6);

			RCT2_CALLPROC_EBPSAFE(0x006A9FC0);
			map_update_tile_pointers();
			RCT2_CALLPROC_EBPSAFE(0x0069EBE4);
			return;
		}

		CloseHandle(hFile);
	}

	RCT2_GLOBAL(0x009AC31B, uint8) = 255;
	RCT2_GLOBAL(0x009AC31C, uint16) = 3011;
}

/**
 * 
 *  rct2: 0x00678282
 * scenario (ebx)
 */
void scenario_load_and_play(rct_scenario_basic *scenario)
{
	rct_window *mainWindow;
	rct_s6_info *s6Info = 0x0141F570;

	RCT2_GLOBAL(0x009AA0F0, uint32) = RCT2_GLOBAL(0x00F663B0, uint32) ^ timeGetTime();
	RCT2_GLOBAL(0x009AA0F4, uint32) = RCT2_GLOBAL(0x00F663B4, uint32) ^ timeGetTime();
	RCT2_CALLPROC_EBPSAFE(0x006CBCC3);

	subsitute_path(
		RCT2_ADDRESS(0x0141EF68, char),
		RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char),
		scenario->path
	);
	scenario_load(0x0141EF68);
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_PLAYING;
	viewport_init_all();
	game_create_windows();
	mainWindow = window_get_main();

	mainWindow->var_4B0 = -1;
	mainWindow->var_4B2 = RCT2_GLOBAL(0x0138869A, sint16);
	mainWindow->var_4B4 = RCT2_GLOBAL(0x0138869C, sint16);

	uint8 _cl = (RCT2_GLOBAL(0x0138869E, sint16) & 0xFF) - mainWindow->viewport->zoom;
	mainWindow->viewport->zoom = RCT2_GLOBAL(0x0138869E, sint16) & 0xFF;
	*((char*)(&RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, sint32))) = RCT2_GLOBAL(0x0138869E, sint16) >> 8;
	if (_cl != 0) {
		if (_cl < 0) {
			_cl = -_cl;
			mainWindow->viewport->view_width >>= _cl;
			mainWindow->viewport->view_height >>= _cl;
		} else {
			mainWindow->viewport->view_width <<= _cl;
			mainWindow->viewport->view_height <<= _cl;
		}
	}
	mainWindow->var_4B2 -= mainWindow->viewport->view_width >> 1;
	mainWindow->var_4B4 -= mainWindow->viewport->view_height >> 1;
	window_invalidate(mainWindow);

	RCT2_CALLPROC_EBPSAFE(0x0069E9A7);
	RCT2_CALLPROC_EBPSAFE(0x006ACA58);

	RCT2_GLOBAL(0x00F663B0, sint32) = RCT2_GLOBAL(0x009AA0F0, sint32);
	RCT2_GLOBAL(0x00F663B4, sint32) = RCT2_GLOBAL(0x009AA0F4, sint32);
	RCT2_GLOBAL(0x009DEB7C, sint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_FLAGS, sint32) &= 0xFFFFF7FF;
	if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_FLAGS, sint32) & 0x20000)
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_FLAGS, sint32) |= 0x800;
	RCT2_CALLPROC_EBPSAFE(0x00684AC3);
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	news_item_init_queue();
	if (RCT2_ADDRESS(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) != OBJECTIVE_NONE)
		window_park_objective_open();

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, sint16) = calculate_park_rating();
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, sint16) = calculate_park_value();
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_COMPANY_VALUE, sint16) = calculate_company_value();
	RCT2_GLOBAL(0x013587D0, sint16) = RCT2_GLOBAL(0x013573DC, sint16) - RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, sint16);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint16) = ENCRYPT_MONEY(RCT2_GLOBAL(0x013573DC, sint32));
	RCT2_CALLPROC_EBPSAFE(0x0069E869);

	strcpy(0x0135924A, s6Info->details);
	strcpy(0x0135920A, s6Info->name);

	RCT2_CALLPROC_EBPSAFE(0x00678461);
}



void scenario_update()
{
	uint8 screen_flags = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8);
	uint32 month_tick = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16),
		next_month_tick = month_tick + 4;
	uint8 month = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16) & 7,
		current_days_in_month = days_in_month[month],
		objective_type = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8);

	if (screen_flags & (~SCREEN_FLAGS_PLAYING)) // only in normal play mode
		return;

	if ((current_days_in_month * next_month_tick) >> 16 != (current_days_in_month * month_tick) >> 16) {
		// daily checks

		RCT2_CALLPROC_EBPSAFE(0x0069E79A); // objective_finance_mystery1
		RCT2_CALLPROC_EBPSAFE(0x0069C35E); // some kind of peeps update loop
		RCT2_CALLPROC_EBPSAFE(0x006C45E7); // get local time
		RCT2_CALLPROC_EBPSAFE(0x0066A13C); // check_objective_6
		if (objective_type == 10 || objective_type == 9 || objective_type == 8 ||
			objective_type == 6 || objective_type == 5) {
			RCT2_CALLPROC_EBPSAFE(0x0066A4B2); // objective daily checks
		}
	}

	
	//if ( (unsigned int)((4 * current_day) & 0xFFFF) >= 0xFFEFu) {
	if ( next_month_tick % 0x4000 == 0) {
		// weekly checks
		RCT2_CALLPROC_EBPSAFE(0x006C18A9);
		RCT2_CALLPROC_EBPSAFE(0x00684DA5);
		RCT2_CALLPROC_EBPSAFE(0x0069E092);
		RCT2_CALLPROC_EBPSAFE(0x0069E0C1);
		RCT2_CALLPROC_EBPSAFE(0x0069BF41);
		RCT2_CALLPROC_EBPSAFE(0x006B7A5E);
		RCT2_CALLPROC_EBPSAFE(0x006AC916);

		if (month <= 1 && RCT2_GLOBAL(0x009ADAE0, sint32) != -1 && RCT2_GLOBAL(0x009ADAE0 + 14, uint16) & 1) {
			for (int i = 0; i < 100; ++i) {
				int carry;
				RCT2_CALLPROC_EBPSAFE(0x006744A9); // clears carry flag on failure -.-
				__asm mov carry, 0;
				__asm adc carry, 0;
				if (!carry)
					break;
			}
		}
		RCT2_CALLPROC_EBPSAFE(0x0066A231);
		RCT2_CALLPROC_EBPSAFE(0x0066A348);
	}

	//if ( (unsigned int)((2 * current_day) & 0xFFFF) >= 0xFFF8) {
	if (next_month_tick % 0x8000 == 0) {
		// biweekly checks
		RCT2_CALLPROC_EBPSAFE(0x006AC885);
	}

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16) = next_month_tick;
	if (next_month_tick > 0x10000) {
		// month ends actions
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16)++;
		RCT2_GLOBAL(0x009A9804, uint32) |= 2;
		RCT2_CALLPROC_EBPSAFE(0x0069DEAD);
		RCT2_CALLPROC_EBPSAFE(0x0066A4B2); // objective daily checks
		RCT2_CALLPROC_EBPSAFE(0x0066A80E);
		RCT2_CALLPROC_EBPSAFE(0x0066A86C);
	}
	
}

