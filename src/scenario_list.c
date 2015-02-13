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

#include "addresses.h"
#include "platform/platform.h"
#include "scenario.h"

// Scenario list
int gScenarioListCount = 0;
int gScenarioListCapacity = 0;
rct_scenario_basic *gScenarioList = NULL;

static void scenario_list_add(const char *path);
static void scenario_list_sort();
static int scenario_list_sort_compare(const void *a, const void *b);
static int scenario_scores_load();

static rct_scenario_basic *get_scenario_by_filename(const char *filename)
{
	int i;
	for (i = 0; i < gScenarioListCount; i++)
		if (strcmp(gScenarioList[i].path, filename) == 0)
			return &gScenarioList[i];

	return NULL;
}

/**
 * 
 *  rct2: 0x006775A8
 */
void scenario_load_list()
{
	int i, enumFileHandle;
	file_info enumFileInfo;

	// Load scores
	scenario_scores_load();

	// Set all scenarios to be invisible
	for (i = 0; i < gScenarioListCount; i++)
		gScenarioList[i].flags &= ~SCENARIO_FLAGS_VISIBLE;

	// Enumerate through each scenario in the directory
	enumFileHandle = platform_enumerate_files_begin(RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char));
	if (enumFileHandle != INVALID_HANDLE) {
		while (platform_enumerate_files_next(enumFileHandle, &enumFileInfo)) {
			scenario_list_add(enumFileInfo.path);
		}
		platform_enumerate_files_end(enumFileHandle);
	}

	// Sort alphabetically
	scenario_list_sort();

	// Save the scores
	scenario_scores_save();
}

static void scenario_list_add(const char *path)
{
	char scenarioPath[MAX_PATH];
	rct_scenario_basic *scenario;
	rct_s6_header s6Header;
	rct_s6_info s6Info;

	// Get absolute path
	subsitute_path(scenarioPath, RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), path);

	// Load the basic scenario information
	if (!scenario_load_basic(scenarioPath, &s6Header, &s6Info))
		return;

	// Ignore scenarios where first header byte is not 255
	if (s6Info.var_000 != 255)
		return;

	// Check if scenario already exists in list, likely if in scores
	scenario = get_scenario_by_filename(path);
	if (scenario != NULL) {
		// Update the scenario information
		scenario->flags |= SCENARIO_FLAGS_VISIBLE;
		scenario->category = s6Info.category;
		scenario->objective_type = s6Info.objective_type;
		scenario->objective_arg_1 = s6Info.objective_arg_1;
		scenario->objective_arg_2 = s6Info.objective_arg_2;
		scenario->objective_arg_3 = s6Info.objective_arg_3;
		strcpy(scenario->name, s6Info.name);
		strcpy(scenario->details, s6Info.details);
	} else {
		// Check if the scenario list buffer has room for another scenario
		if (gScenarioListCount >= gScenarioListCapacity) {
			// Allocate more room
			gScenarioListCapacity += 16;
			gScenarioList = realloc(gScenarioList, gScenarioListCapacity * sizeof(rct_scenario_basic));
		}

		// Increment the number of scenarios
		scenario = &gScenarioList[gScenarioListCount];
		gScenarioListCount++;

		// Add this new scenario to the list
		strcpy(scenario->path, path);
		scenario->flags = SCENARIO_FLAGS_VISIBLE;
		if (RCT2_GLOBAL(0x009AA00C, uint8) & 1)
			scenario->flags |= SCENARIO_FLAGS_SIXFLAGS;
		scenario->category = s6Info.category;
		scenario->objective_type = s6Info.objective_type;
		scenario->objective_arg_1 = s6Info.objective_arg_1;
		scenario->objective_arg_2 = s6Info.objective_arg_2;
		scenario->objective_arg_3 = s6Info.objective_arg_3;
		strcpy(scenario->name, s6Info.name);
		strcpy(scenario->details, s6Info.details);
	}
}

/**
* Sort the list of scenarios. This used to be an insertion sort which took
* place as each scenario loaded. It has now been changed to a quicksort which
* takes place after all the scenarios have been loaded in.
*  rct2: 0x00677C3B
*/
static void scenario_list_sort()
{
	qsort(gScenarioList, gScenarioListCount, sizeof(rct_scenario_basic), scenario_list_sort_compare);
}

/**
 * Basic scenario information compare function for sorting.
 *  rct2: 0x00677C08
 */
static int scenario_list_sort_compare(const void *a, const void *b)
{
	return strcmp(((rct_scenario_basic*)a)->name, ((rct_scenario_basic*)b)->name);
}

/**
 * Gets the path for the scenario scores path.
 */
static void scenario_scores_get_path(char *outPath)
{
	char *homePath = platform_get_orct2_homefolder();
	sprintf(outPath, "%s%c%s", homePath, platform_get_path_separator(), "scores.dat");
	free(homePath);
}

/**
 * 
 *  rct2: 0x006775A8
 */
static int scenario_scores_load()
{
	FILE *file;
	char scoresPath[MAX_PATH];

	scenario_scores_get_path(scoresPath);

	// Free scenario list if already allocated
	if (gScenarioList != NULL) {
		free(gScenarioList);
		gScenarioList = NULL;
	}

	// Try and load the scores file

	// First check user folder and then fallback to install directory
	file = fopen(scoresPath, "rb");
	if (file == NULL) {
		file = fopen(get_file_path(PATH_ID_SCORES), "rb");
		if (file == NULL) {
			RCT2_ERROR("Unable to load scenario scores.");
			return 0;
		}
	}

	// Load header
	rct_scenario_scores_header header;
	if (fread(&header, 16, 1, file) != 1) {
		fclose(file);
		RCT2_ERROR("Invalid header in scenario scores file.");
		return 0;
	}
	gScenarioListCount = header.scenario_count;

	// Load scenario information with scores
	int scenarioListBufferSize = gScenarioListCount * sizeof(rct_scenario_basic);
	gScenarioListCapacity = gScenarioListCount;
	gScenarioList = malloc(scenarioListBufferSize);
	if (fread(gScenarioList, scenarioListBufferSize, 1, file) == 1) {
		fclose(file);
		return 1;
	}

	// Unable to load scores, free scenario list
	fclose(file);
	gScenarioListCount = 0;
	gScenarioListCapacity = 0;
	free(gScenarioList);
	gScenarioList = NULL;
	return 0;
}

/**
 * 
 *  rct2: 0x00677B50
 */
int scenario_scores_save()
{
	FILE *file;
	char scoresPath[MAX_PATH];

	scenario_scores_get_path(scoresPath);

	file = fopen(scoresPath, "wb");
	if (file == NULL) {
		RCT2_ERROR("Unable to save scenario scores.");
		return 0;
	}

	rct_scenario_scores_header header;
	header.scenario_count = gScenarioListCount;

	fwrite(&header, sizeof(header), 1, file);
	if (gScenarioListCount > 0)
		fwrite(gScenarioList, gScenarioListCount * sizeof(rct_scenario_basic), 1, file);

	fclose(file);
	return 1;
}