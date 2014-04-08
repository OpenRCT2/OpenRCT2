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
#include "rct2.h"
#include "scenario.h"
#include "strings.h"

#define UNINITIALISED_SCENARIO_LIST ((rct_scenario_basic*)-1)

#define RCT2_SCENARIO_LIST RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_LIST, rct_scenario_basic*)
#define RCT2_NUM_SCENARIOS RCT2_GLOBAL(RCT2_ADDRESS_NUM_SCENARIOS, sint32)
int _scenarioListSize;

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
	HANDLE hFindFile;
	WIN32_FIND_DATAA findFileData;
	int i;

	// Load scores
	scenario_scores_load();

	// Unset flag 1 for each scenario
	for (i = 0; i < RCT2_NUM_SCENARIOS; i++)
		RCT2_SCENARIO_LIST[i].var_0268 &= ~0x01;

	// Enumerate through each scenario in the directory
	hFindFile = FindFirstFile(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), &findFileData);
	if (hFindFile != INVALID_HANDLE_VALUE) {
		do {
			// Check if scenario already exists in list, likely if in scores
			rct_scenario_basic *scenario = get_scenario_by_filename(findFileData.cFileName);
			if (scenario != NULL) {
				// Set 0141EF68 to the scenario path
				subsitute_path(
					RCT2_ADDRESS(0x0141EF68, char),
					RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char),
					findFileData.cFileName
				);

				// Load the basic scenario information
				if (!scenario_load_basic(RCT2_ADDRESS(0x0141EF68, char)))
					continue;

				// 
				if (RCT2_GLOBAL(0x0141F570, sint8) != -1)
					continue;

				// Update the scenario information
				scenario->var_0268 |= 0x01;
				scenario->category = RCT2_GLOBAL(0x0141F571, uint8);
				scenario->var_0120 = RCT2_GLOBAL(0x0141F572, sint8);
				scenario->var_0121 = RCT2_GLOBAL(0x0141F573, sint8);
				scenario->var_0122 = RCT2_GLOBAL(0x0141F574, sint32);
				scenario->var_0126 = RCT2_GLOBAL(0x0141F578, sint16);
				strcpy(scenario->name, RCT2_ADDRESS(RCT2_ADDRESS_SCENARIO_NAME, char));
				strcpy(scenario->details, RCT2_ADDRESS(RCT2_ADDRESS_SCENARIO_DETAILS, char));
				continue;
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
				findFileData.cFileName
			);

			// Load the scenario information
			if (!scenario_load_basic(RCT2_ADDRESS(0x0141EF68, char)))
				continue;

			// 
			if (RCT2_GLOBAL(0x0141F570, sint8) != -1)
				continue;

			// Increment the number of scenarios
			i = RCT2_NUM_SCENARIOS;
			RCT2_NUM_SCENARIOS++;

			// Add this new scenario to the list
			strcpy(RCT2_SCENARIO_LIST[i].path, findFileData.cFileName);
			RCT2_SCENARIO_LIST[i].var_0268 = 0x01;
			if (RCT2_GLOBAL(0x009AA00C, uint8) & 1)
				RCT2_SCENARIO_LIST[i].var_0268 |= 0x04;
			RCT2_SCENARIO_LIST[i].category = RCT2_GLOBAL(0x0141F571, uint8);
			RCT2_SCENARIO_LIST[i].var_0120 = RCT2_GLOBAL(0x0141F572, sint8);
			RCT2_SCENARIO_LIST[i].var_0121 = RCT2_GLOBAL(0x0141F573, sint8);
			RCT2_SCENARIO_LIST[i].var_0122 = RCT2_GLOBAL(0x0141F574, sint32);
			RCT2_SCENARIO_LIST[i].var_0126 = RCT2_GLOBAL(0x0141F578, sint16);
			strcpy(RCT2_SCENARIO_LIST[i].name, RCT2_ADDRESS(RCT2_ADDRESS_SCENARIO_NAME, char));
			strcpy(RCT2_SCENARIO_LIST[i].details, RCT2_ADDRESS(RCT2_ADDRESS_SCENARIO_DETAILS, char));
		} while (FindNextFile(hFindFile, &findFileData));
		FindClose(hFindFile);
	}

	scenario_list_sort();

	// Save the scores
	scenario_scores_save();
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

	hFile = CreateFile(get_file_path(32), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
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

	hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		RCT2_GLOBAL(0x009E382C, HANDLE*) = hFile;
		RCT2_CALLPROC_X(0x0067685F, 0, 0, 0, 0, 0x009E34E4, 0, 0);
		if (RCT2_GLOBAL(0x009E34E4, uint8) == 1) {
			RCT2_CALLPROC_X(0x0067685F, 0, 0, 0, 0, 0x0141F570, 0, 0);
			CloseHandle(hFile);
			RCT2_GLOBAL(0x009AA00C, uint8) = 0;
			if (RCT2_GLOBAL(0x0141F6F8, uint8) != 255) {
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
				format_string(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIO_NAME, char), RCT2_GLOBAL(ebp, sint16), NULL);
				format_string(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIO_DETAILS, char), RCT2_GLOBAL(ebp + 4, sint16), NULL);
				RCT2_GLOBAL(0x009AA00C, uint8) = RCT2_GLOBAL(ebp + 6, uint8);
				RCT2_CALLPROC(0x006A982D);
				__asm mov _eax, eax
			loc_67628F :
				return _eax;
			}
		} else {
			CloseHandle(hFile);
		}
	}

	RCT2_GLOBAL(0x009AC31B, sint8) = -1;
	RCT2_GLOBAL(0x009AC31C, sint16) = 3011;
	return 0;
}