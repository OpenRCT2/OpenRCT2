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

#include <windows.h>
#include <string.h>
#include "addresses.h"
#include "date.h"
#include "finance.h"
#include "game.h"
#include "map.h"
#include "news_item.h"
#include "object.h"
#include "park.h"
#include "rct2.h"
#include "ride.h"
#include "sawyercoding.h"
#include "scenario.h"
#include "string_ids.h"
#include "sprite.h"
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
	rct_scenario_basic *scenario;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

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
	if ((RCT2_NUM_SCENARIOS + 1) * (int)sizeof(rct_scenario_basic) > _scenarioListSize) {
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
	rct_s6_header *s6Header = (rct_s6_header*)0x009E34E4;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	hFile = CreateFile(
		path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL
	);
	if (hFile != INVALID_HANDLE_VALUE) {
		RCT2_GLOBAL(0x009E382C, HANDLE*) = hFile;

		// Read first chunk
		sawyercoding_read_chunk(hFile, (uint8*)s6Header);
		if (s6Header->type == S6_TYPE_SCENARIO) {
			// Read second chunk
			sawyercoding_read_chunk(hFile, (uint8*)s6Info);
			CloseHandle(hFile);
			RCT2_GLOBAL(0x009AA00C, uint8) = 0;
			if (s6Info->flags != 255) {
				#ifdef _MSC_VER
				__asm {
					push ebp
					mov ebp, 0141F6F8h
					mov eax, 006A9428h
					call eax
					pop ebp
					mov _eax, eax
					jb loc_67628F
				}
				#else
				__asm__ ( "\
						push ebp 	\n\
						mov ebp, 0x0141F6F8 	\n\
						mov eax, 0x006A9428 	\n\
						call eax 	\n\
						pop ebp 	\n\
						mov %[_eax], eax 	\n\
						jb loc_67628F 	\n\
					" : [_eax] "+m" (_eax) : : "eax" );
				#endif

				int ebp = RCT2_GLOBAL(0x009ADAF8, uint32);
				format_string(s6Info->name, RCT2_GLOBAL(ebp, sint16), NULL);
				format_string(s6Info->details, RCT2_GLOBAL(ebp + 4, sint16), NULL);
				RCT2_GLOBAL(0x009AA00C, uint8) = RCT2_GLOBAL(ebp + 6, uint8);
				RCT2_CALLPROC(0x006A982D);
				#ifdef _MSC_VER
				__asm mov _eax, eax
				#else
				__asm__ ( "mov %[_eax], eax " : [_eax] "+m" (_eax) );
				#endif

				#ifdef _MSC_VER
			loc_67628F :
				#else
				__asm__ ( "loc_67628F :");
				#endif
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
	rct_s6_header *s6Header = (rct_s6_header*)0x009E34E4;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	hFile = CreateFile(
		path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL
	);
	if (hFile != INVALID_HANDLE_VALUE) {
		RCT2_GLOBAL(0x009E382C, HANDLE*) = hFile;
		if (!sawyercoding_validate_checksum(hFile)) {
			CloseHandle(hFile);
			RCT2_GLOBAL(0x009AC31B, uint8) = 255;
			RCT2_GLOBAL(0x009AC31C, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
			return;
		}

		// Read first chunk
		sawyercoding_read_chunk(hFile, (uint8*)s6Header);
		if (s6Header->type == S6_TYPE_SCENARIO) {
			// Read second chunk
			sawyercoding_read_chunk(hFile, (uint8*)s6Info);

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
			sawyercoding_read_chunk(hFile, (uint8*)RCT2_ADDRESS_CURRENT_MONTH_YEAR);

			// Read map elements
			memset((void*)RCT2_ADDRESS_MAP_ELEMENTS, 0, MAX_MAP_ELEMENTS * sizeof(rct_map_element));
			sawyercoding_read_chunk(hFile, (uint8*)RCT2_ADDRESS_MAP_ELEMENTS);

			// Read game data, including sprites
			sawyercoding_read_chunk(hFile, (uint8*)0x010E63B8);

			// Read number of guests in park and something else
			sawyercoding_read_chunk(hFile, (uint8*)RCT2_ADDRESS_GUESTS_IN_PARK);

			// Read ?
			sawyercoding_read_chunk(hFile, (uint8*)0x01357BC8);

			// Read park rating
			sawyercoding_read_chunk(hFile, (uint8*)RCT2_ADDRESS_CURRENT_PARK_RATING);

			// Read ?
			sawyercoding_read_chunk(hFile, (uint8*)0x01357CF2);

			// Read ?
			sawyercoding_read_chunk(hFile, (uint8*)0x0135832C);

			// Read ?
			sawyercoding_read_chunk(hFile, (uint8*)RCT2_ADDRESS_CURRENT_PARK_VALUE);

			// Read more game data, including research items and rides
			sawyercoding_read_chunk(hFile, (uint8*)RCT2_ADDRESS_COMPLETED_COMPANY_VALUE);

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
	RCT2_GLOBAL(0x009AC31C, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
}

/**
 * 
 *  rct2: 0x00678282
 * scenario (ebx)
 */
void scenario_load_and_play(rct_scenario_basic *scenario)
{
	rct_window *mainWindow;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	RCT2_GLOBAL(0x009AA0F0, uint32) = RCT2_GLOBAL(0x00F663B0, uint32) ^ timeGetTime();
	RCT2_GLOBAL(0x009AA0F4, uint32) = RCT2_GLOBAL(0x00F663B4, uint32) ^ timeGetTime();
	RCT2_CALLPROC_EBPSAFE(0x006CBCC3);

	subsitute_path(
		RCT2_ADDRESS(0x0141EF68, char),
		RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char),
		scenario->path
	);
	scenario_load((char*)0x0141EF68);
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_PLAYING;
	viewport_init_all();
	game_create_windows();
	mainWindow = window_get_main();

	mainWindow->var_4B0 = -1;
	mainWindow->saved_view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16);
	mainWindow->saved_view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16);

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
	mainWindow->saved_view_x -= mainWindow->viewport->view_width >> 1;
	mainWindow->saved_view_y -= mainWindow->viewport->view_height >> 1;
	window_invalidate(mainWindow);

	RCT2_CALLPROC_EBPSAFE(0x0069E9A7);
	window_new_ride_init_vars();

	RCT2_GLOBAL(0x00F663B0, sint32) = RCT2_GLOBAL(0x009AA0F0, sint32);
	RCT2_GLOBAL(0x00F663B4, sint32) = RCT2_GLOBAL(0x009AA0F4, sint32);
	RCT2_GLOBAL(0x009DEB7C, sint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, sint32) &= 0xFFFFF7FF;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, sint32) & 0x20000)
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, sint32) |= 0x800;
	RCT2_CALLPROC_EBPSAFE(0x00684AC3);
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	news_item_init_queue();
	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) != OBJECTIVE_NONE)
		window_park_objective_open();

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, sint16) = calculate_park_rating();
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, money32) = calculate_park_value();
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_COMPANY_VALUE, money32) = calculate_company_value();
	RCT2_GLOBAL(0x013587D0, money32) = RCT2_GLOBAL(0x013573DC, money32) - RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32) = ENCRYPT_MONEY(RCT2_GLOBAL(0x013573DC, sint32));
	sub_69E869(); // (loan related)

	strcpy((char*)RCT2_ADDRESS_SCENARIO_DETAILS, s6Info->details);
	strcpy((char*)RCT2_ADDRESS_SCENARIO_NAME, s6Info->name);

	if (RCT2_GLOBAL(0x009ADAE4, sint32) != -1) {
		char *ebp = RCT2_GLOBAL(0x009ADAE4, char*);

		// 
		format_string((char*)0x0141ED68, RCT2_GLOBAL(ebp + 2, uint16), 0);
		
		// Set park name
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_RENAME_PARK;
		game_do_command(1, 1, 0, *((int*)(0x0141ED68 + 0)), 33, *((int*)(0x0141ED68 + 8)), *((int*)(0x0141ED68 + 4)));
		game_do_command(2, 1, 0, *((int*)(0x0141ED68 + 12)), 33, *((int*)(0x0141ED68 + 20)), *((int*)(0x0141ED68 + 16)));
		game_do_command(0, 1, 0, *((int*)(0x0141ED68 + 24)), 33, *((int*)(0x0141ED68 + 32)), *((int*)(0x0141ED68 + 28)));

		// 
		format_string((char*)0x0141ED68, RCT2_GLOBAL(ebp + 0, uint16), 0);
		strncpy((char*)RCT2_ADDRESS_SCENARIO_NAME, (char*)0x0141ED68, 31);
		((char*)RCT2_ADDRESS_SCENARIO_NAME)[31] = '\0';

		// Set scenario details
		format_string((char*)0x0141ED68, RCT2_GLOBAL(ebp + 4, uint16), 0);
		strncpy((char*)RCT2_ADDRESS_SCENARIO_DETAILS, (char*)0x0141ED68, 255);
		((char*)RCT2_ADDRESS_SCENARIO_DETAILS)[255] = '\0';
	}

	// Set the last saved game path
	strcpy((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2, (char*)RCT2_ADDRESS_SAVED_GAMES_PATH);
	format_string((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2 + strlen((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2), RCT2_GLOBAL(0x0013573D4, uint16), (void*)0x0013573D8);
	strcat((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2, ".SV6");

	memset((void*)0x001357848, 0, 56);
	RCT2_GLOBAL(0x0135832C, uint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PROFIT, sint32) = 0;
	RCT2_GLOBAL(0x01358334, uint32) = 0;
	RCT2_GLOBAL(0x01358338, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32) = 0x80000000;
	RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_ADMISSIONS, uint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INCOME_FROM_ADMISSIONS, uint32) = 0;
	RCT2_GLOBAL(0x013587D8, uint16) = 63;
	sub_69E869(); // (loan related, called above already)
	park_reset_awards_and_history();
	reset_all_ride_build_dates();
	date_reset();
	RCT2_CALLPROC_EBPSAFE(0x00674576);
	park_calculate_size();
	RCT2_CALLPROC_EBPSAFE(0x006C1955);
	RCT2_GLOBAL(0x01358840, uint8) = 0;
	memset((void*)0x001358102, 0, 20);
	RCT2_GLOBAL(0x00135882E, uint16) = 0;

	// Open park with free entry when there is no money
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY) {
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_PARK_OPEN;
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, uint16) = 0;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_18;

	RCT2_CALLPROC_EBPSAFE(0x006837E3); // (palette related)

	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, uint16) = 0;
	RCT2_GLOBAL(0x009DEA5C, uint16) = 62000; // (doesn't appear to ever be read)
}


void scenario_end()
{
	rct_window* w;
	window_close_by_id(WC_DROPDOWN, 0);
	
	for (w = RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_LIST, rct_window); w < RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); w++){
		if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)))
			window_close(w);
	}
	window_park_objective_open();
}

/*
* rct2: 0x0066A752
**/
void scenario_failure()
{
	RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32) = 0x80000001;
	scenario_end();
}

/*
 * rct2: 0x0066A75E
 **/
void scenario_success()
{
	int i;
	rct_scenario_basic* scenario;
	uint32 current_val = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_COMPANY_VALUE, uint32);
	
	RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32) = current_val;
	RCT2_CALLPROC_EBPSAFE(0x0069BE9B); // celebration

	for (i = 0; i < RCT2_GLOBAL(RCT2_ADDRESS_NUM_SCENARIOS, sint32); i++) {
		char *cur_scenario_name = RCT2_ADDRESS(0x135936C, char);
		scenario = &(RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_LIST, rct_scenario_basic*)[i]);

		if (0 == strncmp(cur_scenario_name, scenario->path, 256)){
			if (scenario->flags & SCENARIO_FLAGS_COMPLETED && scenario->company_value < current_val)
				break; // not a new high score -> no glory

			// bts game_flags, 1 happens here but I don't know what for
			scenario->company_value = current_val;
			scenario->flags |= SCENARIO_FLAGS_COMPLETED;
			scenario->completed_by[0] = 0;
			RCT2_GLOBAL(0x013587C0, uint32) = current_val; // value used in window for score?
			scenario_scores_save();
			break;
		}
	}
	scenario_end();
}

/**
* Checks if there are 10 rollercoasters of different subtype with
* excitement >= 600 .
* rct2:
**/
void scenario_objective5_check()
{
	int rcs = 0;
	uint8 type_already_counted[256];
	rct_ride* ride;

	memset(type_already_counted, 0, 256);

	for (int i = 0; i < MAX_RIDES; i++) {
		uint8 subtype_id;
		uint32 subtype_p;
		ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);
		if (ride->type == RIDE_TYPE_NULL)
			continue;
		subtype_id = (uint8)ride->subtype;
		subtype_p = RCT2_GLOBAL(0x009ACFA4 + subtype_id * 4, uint32);

		if ((RCT2_GLOBAL(subtype_p + 0x1BE, sint8) == 2 ||
			RCT2_GLOBAL(subtype_p + 0x1BF, sint8) == 2) &&
			ride->status == RIDE_STATUS_OPEN &&
			ride->excitement >= 600 && type_already_counted[subtype_id] == 0){
			type_already_counted[subtype_id]++;
			rcs++;
		}
	}

	if (rcs >= 10)
		scenario_success();
}

/**
 * Checks if there are 10 rollercoasters of different subtype with
 * excitement > 700 and a minimum length;
 * rct2: 0x0066A6B5
 **/
void scenario_objective8_check()
{
	int rcs = 0;
	uint8 type_already_counted[256];
	rct_ride* ride;
	sint16 objective_length = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16);

	memset(type_already_counted, 0, 256);

	for (int i = 0; i < MAX_RIDES; i++) {
		uint8 subtype_id;
		uint32 subtype_p;
		ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);
		if (ride->type == RIDE_TYPE_NULL)
			continue;
		subtype_id = (uint8)ride->subtype;
		subtype_p = RCT2_GLOBAL(0x009ACFA4 + subtype_id * 4, uint32);

		if ((RCT2_GLOBAL(subtype_p + 0x1BE, sint8) == 2 ||
			RCT2_GLOBAL(subtype_p + 0x1BF, sint8) == 2) &&
			ride->status == RIDE_STATUS_OPEN &&
			ride->excitement >= 600 && type_already_counted[subtype_id] == 0){

			// this calculates the length, no idea why it's done so complicated though.
			uint8 limit = ride->pad_088[63];
			uint32 sum = 0;
			for (int j = 0; j < limit; ++j) {
				sum += ((uint32*)&ride->pad_088[92])[j];
			}
			if ((sum >> 16) > (uint32)objective_length) {
				type_already_counted[subtype_id]++;
				rcs++;
			}
		}
	}

	if (rcs >= 10)
		scenario_success();
}



/*
 * Checks the win/lose conditions of the current objective.
 * rct2: 0x0066A4B2
 **/
void scenario_objectives_check()
{
	uint8 objective_type = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8),
		objective_year = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8);
	sint16 park_rating = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, sint16),
		guests_in_park = RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16),
		objective_guests = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16),
		cur_month_year = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16);
	uint32 scenario_completed_company_value = RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32);
	sint32 objective_currency = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, sint32),
		park_value = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, sint32);


	if ( scenario_completed_company_value != 0x80000000)
		return;

	switch (objective_type) {
	case OBJECTIVE_GUESTS_BY://1

		if (cur_month_year == 8 * objective_year){
			if (park_rating < 600 || guests_in_park < objective_guests)
				scenario_failure();
			else
				scenario_success();
		}
		break;

	case OBJECTIVE_PARK_VALUE_BY://2

		if (cur_month_year == 8 * objective_year) {
			if (park_value < objective_currency)
				scenario_failure();
			else
				scenario_success();
		}
		break;

	case OBJECTIVE_10_ROLLERCOASTERS://5

		scenario_objective5_check();
		break;

	case OBJECTIVE_GUESTS_AND_RATING://6

		if (park_rating >= 700 && guests_in_park > objective_guests)
			scenario_success();
		break;

	case OBJECTIVE_MONTHLY_RIDE_INCOME://7
	{
		sint32 monthly_ride_income = RCT2_GLOBAL(RCT2_ADDRESS_MONTHLY_RIDE_INCOME, sint32);
		if (monthly_ride_income > objective_currency)
			scenario_success();
		break;
	}
	case OBJECTIVE_10_ROLLERCOASTERS_LENGTH://8

		scenario_objective8_check();
		break;

	case OBJECTIVE_FINISH_5_ROLLERCOASTERS://9
	{
		rct_ride* ride;
		int rcs = 0;
		for (int i = 0; i < MAX_RIDES; i++) {
			ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);
			if (ride->status && ride->excitement > objective_currency)
				rcs++;
		}
		if (rcs >= 5)
			scenario_success();
		break;
	}
	case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE://A
	{
		sint32 current_loan = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, sint32);
		if (current_loan <= 0 && park_value > objective_currency)
			scenario_success();
		break;
	}
	case OBJECTIVE_MONTHLY_FOOD_INCOME://B
	{
		sint32 income_sum = RCT2_GLOBAL(0x013578A4, sint32) + RCT2_GLOBAL(0x013578A0, sint32) +
						   RCT2_GLOBAL(0x0135789C, sint32) + RCT2_GLOBAL(0x01357898, sint32);
		if (income_sum > objective_currency)
			scenario_success();
		break;
	}
	default:
		return;
	}
}


/*
 * Send a warning when entrance price is too high.
 * rct2: 0x0066A80E
 **/
void scenario_entrance_fee_too_high_check()
{
	uint16 x, y;
	uint16 magic = RCT2_GLOBAL(0x013580EE, uint16),
		park_entrance_fee = RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, uint16);
	int max_fee = magic + (magic / 2);
	uint32 game_flags = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), packed_xy;

	if ((game_flags & PARK_FLAGS_PARK_OPEN) && park_entrance_fee > max_fee) {
		for (int i = 0; RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] != SPRITE_LOCATION_NULL; ++i) {
			x = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] + 16;
			y = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, uint16)[i] + 16;
		}

		packed_xy = (y << 16) | x;
		news_item_add_to_queue(NEWS_ITEM_BLANK, STR_ENTRANCE_FEE_TOO_HI, packed_xy);
	}
}


/*
 * Update status of marketing campaigns and send a message when they are done.
 * rct2: 0x0069E0C1
 **/
void scenario_marketing_update()
{
	for (int i = 0; i < 6; ++i) {
		uint8 campaign_weeks_left = RCT2_ADDRESS(0x01358102, uint8)[i];
		int campaign_item = 0;

		if (!campaign_weeks_left)
			continue;
		window_invalidate_by_id(WC_FINANCES, 0);

		// high bit marks the campaign as inactive, on first check the campaign is set actice
		// this makes campaigns run a full x weeks even when started in the middle of a week
		RCT2_ADDRESS(0x01358102, uint8)[i] &= ~(1 << 7);
		if (campaign_weeks_left & (1 << 7))
			continue;
		
		RCT2_ADDRESS(0x01358102, uint8)[i]--;
		if (campaign_weeks_left - 1 != 0)
			continue;

		campaign_item = RCT2_ADDRESS(0x01358116, uint8)[i];

		// this sets the string parameters for the marketing types that have an argument.
		if (i == 1 || i == 5) { // free RIDES oh yea
			RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL(0x01362942 + 304 * campaign_item, uint16);;
			RCT2_GLOBAL(0x013CE954, uint32) = RCT2_GLOBAL(0x01362944 + 152 * campaign_item, uint32);
		} else if (i == 3) { // free food/merch
			campaign_item += 2016;
			if (campaign_item >= 2048)
				campaign_item += 96;
			RCT2_GLOBAL(0x013CE952, uint16) = campaign_item;
		}

		news_item_add_to_queue(NEWS_ITEM_MONEY, STR_MARKETING_FINISHED_BASE + i, 0);
	}
}


/*
 * Scenario and finance related update iteration.
 * rct2: 0x006C44B1
 **/
void scenario_update()
{
	uint8 screen_flags = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8);
	uint32 month_tick = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16),
		next_month_tick = month_tick + 4;
	uint8 month = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16) & 7,
		current_days_in_month = (uint8)days_in_month[month],
		objective_type = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8);

	if (screen_flags & (~SCREEN_FLAGS_PLAYING)) // only in normal play mode
		return;

	if ((current_days_in_month * next_month_tick) >> 16 != (current_days_in_month * month_tick) >> 16) {
		// daily checks
		RCT2_CALLPROC_EBPSAFE(0x0069E79A); // daily profit update
		RCT2_CALLPROC_EBPSAFE(0x0069C35E); // some kind of peeps days_visited update loop
		get_local_time();
		RCT2_CALLPROC_EBPSAFE(0x0066A13C); // objective 6 dragging 
		if (objective_type == 10 || objective_type == 9 || objective_type == 8 ||
			objective_type == 6 || objective_type == 5) {
			scenario_objectives_check();
		}
	}

	
	//if ( (unsigned int)((4 * current_day) & 0xFFFF) >= 0xFFEFu) {
	if ( next_month_tick % 0x4000 == 0) {
		// weekly checks
		finance_pay_wages();
		finance_pay_research();
		finance_pay_interest();
		scenario_marketing_update();
		peep_problem_warnings_update();
		RCT2_CALLPROC_EBPSAFE(0x006B7A5E); // check ride reachability
		ride_update_favourited_stat();

		if (month <= 1 && RCT2_GLOBAL(0x009ADAE0, sint32) != -1 && RCT2_GLOBAL(0x009ADAE0 + 14, uint16) & 1) {
			for (int i = 0; i < 100; ++i) {
				int carry;
				RCT2_CALLPROC_EBPSAFE(0x006744A9); // clears carry flag on failure -.-
				#ifdef _MSC_VER
				__asm mov carry, 0;
				__asm adc carry, 0;
				#else
				__asm__ ( "mov %[carry], 0; " : [carry] "+m" (carry) );
				__asm__ ( "adc %[carry], 0; " : [carry] "+m" (carry) );
				#endif

				if (!carry)
					break;
			}
		}
		RCT2_CALLPROC_EBPSAFE(0x0066A231); // update histories (finance, ratings, etc)
		park_calculate_size();
	}

	//if ( (unsigned int)((2 * current_day) & 0xFFFF) >= 0xFFF8) {
	if (next_month_tick % 0x8000 == 0) {
		// fortnightly 
		finance_pay_ride_upkeep();
	}

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16) = (uint16)next_month_tick;
	if (next_month_tick >= 0x10000) {
		// month ends actions
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16)++;
		RCT2_GLOBAL(0x009A9804, uint32) |= 2;
		RCT2_CALLPROC_EBPSAFE(0x0069DEAD);
		scenario_objectives_check();
		scenario_entrance_fee_too_high_check();
		RCT2_CALLPROC_EBPSAFE(0x0066A86C); // award checks
	}
	
}

