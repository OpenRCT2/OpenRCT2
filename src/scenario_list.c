#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "config.h"
#include "localisation/localisation.h"
#include "object_list.h"
#include "platform/platform.h"
#include "rct2.h"
#include "scenario.h"
#include "util/util.h"

// Scenario list
int gScenarioListCount = 0;
int gScenarioListCapacity = 0;
scenario_index_entry *gScenarioList = NULL;

int gScenarioHighscoreListCount = 0;
int gScenarioHighscoreListCapacity = 0;
scenario_highscore_entry *gScenarioHighscoreList = NULL;

static void scenario_list_include(const utf8 *directory);
static void scenario_list_add(const utf8 *path, uint64 timestamp);
static void scenario_list_sort();
static int scenario_list_sort_by_category(const void *a, const void *b);
static int scenario_list_sort_by_index(const void *a, const void *b);

static bool scenario_scores_load();
static void scenario_scores_legacy_get_path(utf8 *outPath);
static bool scenario_scores_legacy_load(const utf8 *path);
static void scenario_highscore_remove(scenario_highscore_entry *higscore);
static void scenario_highscore_list_dispose();
static utf8 *io_read_string(SDL_RWops *file);
static void io_write_string(SDL_RWops *file, utf8 *source);

/**
 * Searches and grabs the metadata for all the scenarios.
 */
void scenario_load_list()
{
	utf8 directory[MAX_PATH];

	// Clear scenario list
	gScenarioListCount = 0;

	// Get scenario directory from RCT2
	safe_strcpy(directory, gRCT2AddressAppPath, sizeof(directory));
	safe_strcat_path(directory, "Scenarios", sizeof(directory));
	scenario_list_include(directory);

	// Get scenario directory from user directory
	platform_get_user_directory(directory, "scenario");
	scenario_list_include(directory);

	scenario_list_sort();
	scenario_scores_load();

	utf8 scoresPath[MAX_PATH];
	scenario_scores_legacy_get_path(scoresPath);
	scenario_scores_legacy_load(scoresPath);
	scenario_scores_legacy_load(get_file_path(PATH_ID_SCORES));
}

static void scenario_list_include(const utf8 *directory)
{
	int handle;
	file_info fileInfo;

	// Scenarios in this directory
	utf8 pattern[MAX_PATH];
	safe_strcpy(pattern, directory, sizeof(pattern));
	safe_strcat_path(pattern, "*.sc6", sizeof(pattern));

	handle = platform_enumerate_files_begin(pattern);
	while (platform_enumerate_files_next(handle, &fileInfo)) {
		utf8 path[MAX_PATH];
		safe_strcpy(path, directory, sizeof(pattern));
		safe_strcat_path(path, fileInfo.path, sizeof(pattern));
		scenario_list_add(path, fileInfo.last_modified);
	}
	platform_enumerate_files_end(handle);

	// Include sub-directories
	utf8 subDirectory[MAX_PATH];
	handle = platform_enumerate_directories_begin(directory);
	while (platform_enumerate_directories_next(handle, subDirectory)) {
		utf8 path[MAX_PATH];
		safe_strcpy(path, directory, sizeof(pattern));
		safe_strcat_path(path, subDirectory, sizeof(pattern));
		scenario_list_include(path);
	}
	platform_enumerate_directories_end(handle);
}

static void scenario_list_add(const utf8 *path, uint64 timestamp)
{
	// Load the basic scenario information
	rct_s6_header s6Header;
	rct_s6_info s6Info;
	if (!scenario_load_basic(path, &s6Header, &s6Info)) {
		return;
	}

	scenario_index_entry *newEntry = NULL;

	const utf8 *filename = path_get_filename(path);
	scenario_index_entry *existingEntry = scenario_list_find_by_filename(filename);
	if (existingEntry != NULL) {
		bool bail = false;
		const utf8 *conflictPath;
		if (existingEntry->timestamp > timestamp) {
			// Existing entry is more recent
			conflictPath = existingEntry->path;

			// Overwrite existing entry with this one
			newEntry = existingEntry;
		} else {
			// This entry is more recent
			conflictPath = path;
			bail = true;
		}
		printf("Scenario conflict: '%s' ignored because it is newer.\n", conflictPath);
		if (bail) {
			return;
		}
	}

	if (newEntry == NULL) {
		// Increase list size
		if (gScenarioListCount == gScenarioListCapacity) {
			gScenarioListCapacity = max(8, gScenarioListCapacity * 2);
			gScenarioList = (scenario_index_entry*)realloc(gScenarioList, gScenarioListCapacity * sizeof(scenario_index_entry));
		}
		newEntry = &gScenarioList[gScenarioListCount];
		gScenarioListCount++;
	}

	// Set new entry
	safe_strcpy(newEntry->path, path, sizeof(newEntry->path));
	newEntry->timestamp = timestamp;
	newEntry->category = s6Info.category;
	newEntry->objective_type = s6Info.objective_type;
	newEntry->objective_arg_1 = s6Info.objective_arg_1;
	newEntry->objective_arg_2 = s6Info.objective_arg_2;
	newEntry->objective_arg_3 = s6Info.objective_arg_3;
	newEntry->highscore = NULL;
	safe_strcpy(newEntry->name, s6Info.name, sizeof(newEntry->name));
	safe_strcpy(newEntry->details, s6Info.details, sizeof(newEntry->details));

	// Normalise the name to make the scenario as recognisable as possible.
	scenario_normalise_name(newEntry->name);

	// Look up and store information regarding the origins of this scenario.
	source_desc desc;
	if (scenario_get_source_desc(newEntry->name, &desc)) {
		newEntry->sc_id = desc.id;
		newEntry->source_index = desc.index;
		newEntry->source_game = desc.source;
		newEntry->category = desc.category;
	} else {
		newEntry->sc_id = SC_UNIDENTIFIED;
		newEntry->source_index = -1;
		if (newEntry->category == SCENARIO_CATEGORY_REAL) {
			newEntry->source_game = SCENARIO_SOURCE_REAL;
		} else {
			newEntry->source_game = SCENARIO_SOURCE_OTHER;
		}
	}

	scenario_translate(newEntry, &s6Info.entry);
}

void scenario_list_dispose()
{
	gScenarioListCapacity = 0;
	gScenarioListCount = 0;
	SafeFree(gScenarioList);
}

static void scenario_list_sort()
{
	int(*compareFunc)(void const*, void const*);

	compareFunc = gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN ?
		scenario_list_sort_by_index :
		scenario_list_sort_by_category;

	qsort(gScenarioList, gScenarioListCount, sizeof(scenario_index_entry), compareFunc);
}

static int scenario_list_category_compare(int categoryA, int categoryB)
{
	if (categoryA == categoryB) return 0;
	if (categoryA == SCENARIO_CATEGORY_DLC) return -1;
	if (categoryB == SCENARIO_CATEGORY_DLC) return 1;
	if (categoryA == SCENARIO_CATEGORY_BUILD_YOUR_OWN) return -1;
	if (categoryB == SCENARIO_CATEGORY_BUILD_YOUR_OWN) return 1;
	return sgn(categoryA - categoryB);
}

static int scenario_list_sort_by_category(const void *a, const void *b)
{
	const scenario_index_entry *entryA = (const scenario_index_entry*)a;
	const scenario_index_entry *entryB = (const scenario_index_entry*)b;

	// Order by category
	if (entryA->category != entryB->category) {
		return scenario_list_category_compare(entryA->category, entryB->category);
	}

	// Then by source game / name
	switch (entryA->category) {
	default:
		if (entryA->source_game != entryB->source_game) {
			return entryA->source_game - entryB->source_game;
		}
		return strcmp(entryA->name, entryB->name);
	case SCENARIO_CATEGORY_REAL:
	case SCENARIO_CATEGORY_OTHER:
		return strcmp(entryA->name, entryB->name);
	}
}

static int scenario_list_sort_by_index(const void *a, const void *b)
{
	const scenario_index_entry *entryA = (const scenario_index_entry*)a;
	const scenario_index_entry *entryB = (const scenario_index_entry*)b;

	// Order by source game
	if (entryA->source_game != entryB->source_game) {
		return entryA->source_game - entryB->source_game;
	}

	// Then by index / category / name
	uint8 sourceGame = entryA->source_game;
	switch (sourceGame) {
	default:
		if (entryA->source_index == -1 && entryB->source_index == -1) {
			if (entryA->category == entryB->category) {
				return scenario_list_sort_by_category(a, b);
			} else {
				return scenario_list_category_compare(entryA->category, entryB->category);
			}
		} else if (entryA->source_index == -1) {
			return 1;
		} else if (entryB->source_index == -1) {
			return -1;
		} else {
			return entryA->source_index - entryB->source_index;
		}
	case SCENARIO_SOURCE_REAL:
		return scenario_list_sort_by_category(a, b);
	}
}

scenario_index_entry *scenario_list_find_by_filename(const utf8 *filename)
{
	for (int i = 0; i < gScenarioListCount; i++) {
		const utf8 *scenarioFilename = path_get_filename(gScenarioList[i].path);
		if (_strcmpi(filename, scenarioFilename) == 0) {
			return &gScenarioList[i];
		}
	}
	return NULL;
}

scenario_index_entry *scenario_list_find_by_path(const utf8 *path)
{
	for (int i = 0; i < gScenarioListCount; i++) {
		if (_strcmpi(path, gScenarioList[i].path) == 0) {
			return &gScenarioList[i];
		}
	}
	return NULL;
}

/**
 * Gets the path for the scenario scores path.
 */
static void scenario_scores_get_path(utf8 *outPath)
{
	platform_get_user_directory(outPath, NULL);
	strcat(outPath, "highscores.dat");
}

/**
 * Gets the path for the scenario scores path.
 */
static void scenario_scores_legacy_get_path(utf8 *outPath)
{
	platform_get_user_directory(outPath, NULL);
	strcat(outPath, "scores.dat");
}

/**
 * Loads the original scores.dat file and replaces any highscores that
 * are better for matching scenarios.
 */
static bool scenario_scores_legacy_load(const utf8 *path)
{
	// First check user folder and then fallback to install directory
	SDL_RWops *file = SDL_RWFromFile(path, "rb");
	if (file == NULL) {
		return false;
	}

	Sint64 fileSize = SDL_RWsize(file);
	if (fileSize <= 4) {
		// Initial value of scores for RCT2, just ignore
		return false;
	}

	// Load header
	rct_scenario_scores_header header;
	if (SDL_RWread(file, &header, 16, 1) != 1) {
		SDL_RWclose(file);
		log_error("Invalid header in legacy scenario scores file.");
		return false;
	}

	// Read scenarios
	bool highscoresDirty = false;
	for (uint32 i = 0; i < header.scenario_count; i++) {
		// Read legacy entry
		rct_scenario_basic scBasic;
		if (SDL_RWread(file, &scBasic, sizeof(rct_scenario_basic), 1) != 1) {
			break;
		}

		// Ignore non-completed scenarios
		if (!(scBasic.flags & SCENARIO_FLAGS_COMPLETED)) {
			continue;
		}

		// Find matching scenario entry
		scenario_index_entry *scenarioIndexEntry = scenario_list_find_by_filename(scBasic.path);
		if (scenarioIndexEntry != NULL) {
			// Check if legacy highscore is better
			scenario_highscore_entry *highscore = scenarioIndexEntry->highscore;
			if (highscore == NULL) {
				highscore = scenario_highscore_insert();
				scenarioIndexEntry->highscore = highscore;
			} else if (highscore->company_value < (money32)scBasic.company_value) {
				scenario_highscore_free(highscore);
				// Re-use highscore entry
			} else {
				highscore = NULL;
			}

			// Set new highscore
			if (highscore != NULL) {
				highscore->fileName = _strdup(scBasic.path);
				highscore->name = win1252_to_utf8_alloc(scBasic.completed_by);
				highscore->company_value = (money32)scBasic.company_value;
				highscore->timestamp = DATETIME64_MIN;
				highscoresDirty = true;
			}
		}
	}
	SDL_RWclose(file);

	if (highscoresDirty) {
		scenario_scores_save();
	}
	return true;
}

static bool scenario_scores_load()
{
	utf8 scoresPath[MAX_PATH];
	scenario_scores_get_path(scoresPath);

	// Load scores file
	SDL_RWops *file = SDL_RWFromFile(scoresPath, "rb");
	if (file == NULL) {
		return false;
	}

	// Check file version
	uint32 fileVersion;
	SDL_RWread(file, &fileVersion, sizeof(fileVersion), 1);
	if (fileVersion != 1) {
		log_error("Invalid or incompatible highscores file.");
		return false;
	}

	// Read and allocate the highscore list
	scenario_highscore_list_dispose();
	SDL_RWread(file, &gScenarioHighscoreListCount, sizeof(gScenarioHighscoreListCount), 1);
	gScenarioHighscoreListCapacity = gScenarioHighscoreListCount;
	gScenarioHighscoreList = malloc(gScenarioHighscoreListCapacity * sizeof(scenario_highscore_entry));

	// Read highscores
	for (int i = 0; i < gScenarioHighscoreListCount; i++) {
		scenario_highscore_entry *highscore = &gScenarioHighscoreList[i];
		highscore->fileName = io_read_string(file);
		highscore->name = io_read_string(file);
		SDL_RWread(file, &highscore->company_value, sizeof(highscore->company_value), 1);
		SDL_RWread(file, &highscore->timestamp, sizeof(highscore->timestamp), 1);

		// Attach highscore to correct scenario entry
		if (highscore->fileName == NULL) {
			continue;
		}
		scenario_index_entry *scenarioIndexEntry = scenario_list_find_by_filename(highscore->fileName);
		if (scenarioIndexEntry != NULL) {
			scenarioIndexEntry->highscore = highscore;
		}
	}

	SDL_RWclose(file);
	return true;
}

/**
 *
 *  rct2: 0x00677B50
 */
bool scenario_scores_save()
{
	utf8 scoresPath[MAX_PATH];
	scenario_scores_get_path(scoresPath);

	SDL_RWops *file = SDL_RWFromFile(scoresPath, "wb");
	if (file == NULL) {
		log_error("Unable to save scenario scores.");
		return false;
	}

	const uint32 fileVersion = 1;

	SDL_RWwrite(file, &fileVersion, sizeof(fileVersion), 1);
	SDL_RWwrite(file, &gScenarioHighscoreListCount, sizeof(gScenarioHighscoreListCount), 1);
	for (int i = 0; i < gScenarioHighscoreListCount; i++) {
		scenario_highscore_entry *highscore = &gScenarioHighscoreList[i];
		io_write_string(file, highscore->fileName);
		io_write_string(file, highscore->name);
		SDL_RWwrite(file, &highscore->company_value, sizeof(highscore->company_value), 1);
		SDL_RWwrite(file, &highscore->timestamp, sizeof(highscore->timestamp), 1);
	}
	SDL_RWclose(file);

	return true;
}

scenario_highscore_entry *scenario_highscore_insert()
{
	if (gScenarioHighscoreListCount >= gScenarioHighscoreListCapacity) {
		gScenarioHighscoreListCapacity = max(8, gScenarioHighscoreListCapacity * 2);
		gScenarioHighscoreList = realloc(gScenarioHighscoreList, gScenarioHighscoreListCapacity * sizeof(scenario_highscore_entry));
	}
	return &gScenarioHighscoreList[gScenarioHighscoreListCount++];
}

static void scenario_highscore_remove(scenario_highscore_entry *highscore)
{
	for (int i = 0; i < gScenarioHighscoreListCount; i++) {
		if (&gScenarioHighscoreList[i] == highscore) {
			size_t moveSize = (gScenarioHighscoreListCount - i - 1) * sizeof(scenario_highscore_entry);
			if (moveSize > 0) {
				memmove(&gScenarioHighscoreList[i], &gScenarioHighscoreList[i + 1], moveSize);
			}
			return;
		}
	}
}

void scenario_highscore_free(scenario_highscore_entry *highscore)
{
	SafeFree(highscore->fileName);
	SafeFree(highscore->name);
}

static void scenario_highscore_list_dispose()
{
	for (int i = 0; i < gScenarioHighscoreListCount; i++) {
		scenario_highscore_free(&gScenarioHighscoreList[i]);
	}
	gScenarioHighscoreListCapacity = 0;
	gScenarioHighscoreListCount = 0;
	SafeFree(gScenarioHighscoreList);
}

static utf8 *io_read_string(SDL_RWops *file)
{
	size_t bufferCount = 0;
	size_t bufferCapacity = 0;
	utf8 *buffer = NULL;

	utf8 ch;
	do {
		SDL_RWread(file, &ch, sizeof(ch), 1);
		if (ch == '\0' && buffer == NULL) {
			break;
		}

		if (bufferCount >= bufferCapacity) {
			bufferCapacity = max(32, bufferCapacity * 2);
			buffer = realloc(buffer, bufferCapacity * sizeof(uint8));
		}

		buffer[bufferCount] = ch;
		bufferCount++;
	} while (ch != '\0');

	if (bufferCount < bufferCapacity) {
		buffer = realloc(buffer, bufferCount);
	}
	return buffer;
}

static void io_write_string(SDL_RWops *file, utf8 *source)
{
	if (source == NULL) {
		utf8 empty = 0;
		SDL_RWwrite(file, &empty, sizeof(utf8), 1);
	} else {
		SDL_RWwrite(file, source, strlen(source) + 1, 1);
	}
}
