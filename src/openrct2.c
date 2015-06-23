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
#include "audio/audio.h"
#include "audio/mixer.h"
#include "cmdline.h"
#include "config.h"
#include "editor.h"
#include "localisation/localisation.h"
#include "network/http.h"
#include "openrct2.h"
#include "platform/platform.h"
#include "util/sawyercoding.h"
#include "world/mapgen.h"
#include "title.h"

int gOpenRCT2StartupAction = STARTUP_ACTION_TITLE;
char gOpenRCT2StartupActionPath[512] = { 0 };
char gExePath[MAX_PATH];

// This should probably be changed later and allow a custom selection of things to initialise like SDL_INIT
bool gOpenRCT2Headless = false;

bool gOpenRCT2ShowChangelog;

/** If set, will end the OpenRCT2 game loop. Intentially private to this module so that the flag can not be set back to 0. */
int _finished;

static void openrct2_loop();

static void openrct2_copy_files_over(const char *originalDirectory, const char *newDirectory, const char *extension)
{
	char *ch, filter[MAX_PATH], oldPath[MAX_PATH], newPath[MAX_PATH];
	int fileEnumHandle;
	file_info fileInfo;
	
	if (!platform_ensure_directory_exists(newDirectory)) {
		log_error("Could not create directory %s.", newDirectory);
		return;
	}

	// Create filter path
	strcpy(filter, originalDirectory);
	ch = strchr(filter, '*');
	if (ch != NULL)
		*ch = 0;
	strcat(filter, "*");
	strcat(filter, extension);

	fileEnumHandle = platform_enumerate_files_begin(filter);
	while (platform_enumerate_files_next(fileEnumHandle, &fileInfo)) {
		strcpy(newPath, newDirectory);
		strcat(newPath, fileInfo.path);
		
		strcpy(oldPath, originalDirectory);
		ch = strchr(oldPath, '*');
		if (ch != NULL)
			*ch = 0;
		strcat(oldPath, fileInfo.path);

		if (!platform_file_exists(newPath))
			platform_file_copy(oldPath, newPath);
	}
	platform_enumerate_files_end(fileEnumHandle);

	fileEnumHandle = platform_enumerate_directories_begin(originalDirectory);
	while (platform_enumerate_directories_next(fileEnumHandle, filter)) {
		strcpy(newPath, newDirectory);
		strcat(newPath, filter);

		strcpy(oldPath, originalDirectory);
		ch = strchr(oldPath, '*');
		if (ch != NULL)
			*ch = 0;
		strcat(oldPath, filter);

		if (!platform_ensure_directory_exists(newPath)) {
			log_error("Could not create directory %s.", newPath);
			return;
		}
		openrct2_copy_files_over(oldPath, newPath, extension);
	}
	platform_enumerate_directories_end(fileEnumHandle);
}

static void openrct2_set_exe_path()
{
	char exePath[MAX_PATH];
	char tempPath[MAX_PATH];
	char *exeDelimiter;
	int pathEnd;
	int exeDelimiterIndex;

	GetModuleFileName(NULL, exePath, MAX_PATH);
	exeDelimiter = strrchr(exePath, platform_get_path_separator());
	exeDelimiterIndex = (int)(exeDelimiter - exePath);
	pathEnd = strlen(exePath) - (strlen(exePath) - exeDelimiterIndex);
	strncpy(tempPath, exePath, pathEnd);
	tempPath[pathEnd] = '\0';
	_fullpath(gExePath, tempPath, MAX_PATH);
}

/**
 * Copy saved games and landscapes to user directory
 */
static void openrct2_copy_original_user_files_over()
{
	char path[MAX_PATH];

	platform_get_user_directory(path, "save");
	openrct2_copy_files_over((char*)RCT2_ADDRESS_SAVED_GAMES_PATH, path, ".sv6");

	platform_get_user_directory(path, "landscape");
	openrct2_copy_files_over((char*)RCT2_ADDRESS_LANDSCAPES_PATH, path, ".sc6");
}

bool openrct2_initialise()
{
	char userPath[MAX_PATH];

	platform_get_user_directory(userPath, NULL);
	if (!platform_ensure_directory_exists(userPath)) {
		log_fatal("Could not create user directory (do you have write access to your documents folder?)");
		return false;
	}

	openrct2_set_exe_path();

	config_set_defaults();
	if (!config_open_default()) {
		if (!config_find_or_browse_install_directory()) {
			log_fatal("An RCT2 install directory must be specified!");
			return false;
		}
	}

	gOpenRCT2ShowChangelog = true;
	if (gConfigGeneral.last_run_version != NULL && (strcmp(gConfigGeneral.last_run_version, OPENRCT2_VERSION) == 0))
		gOpenRCT2ShowChangelog = false;
	gConfigGeneral.last_run_version = OPENRCT2_VERSION;
	config_save_default();

	// TODO add configuration option to allow multiple instances
	if (!gOpenRCT2Headless && !platform_lock_single_instance()) {
		log_fatal("OpenRCT2 is already running.");
		return false;
	}

	get_system_info();
	if (!gOpenRCT2Headless) {
		audio_init();
		audio_get_devices();
		get_dsound_devices();
	}
	language_open(gConfigGeneral.language);
	http_init();

	themes_set_default();
	themes_load_presets();
	title_sequences_set_default();
	title_sequences_load_presets();

	if (!rct2_init())
		return false;

	openrct2_copy_original_user_files_over();

	Mixer_Init(NULL);
	return true;
}

/**
 * Launches the game, after command line arguments have been parsed and processed.
 */
void openrct2_launch()
{
	if (openrct2_initialise()) {
		RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) = 0;
		switch (gOpenRCT2StartupAction) {
		case STARTUP_ACTION_INTRO:
			RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) = 8;
			break;
		case STARTUP_ACTION_TITLE:
			RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TITLE_DEMO;
			break;
		case STARTUP_ACTION_OPEN:
			assert(gOpenRCT2StartupActionPath != NULL);
			rct2_open_file(gOpenRCT2StartupActionPath);

			RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_PLAYING;
			break;
		case STARTUP_ACTION_EDIT:
			if (strlen(gOpenRCT2StartupActionPath) == 0) {
				editor_load();
			} else {
				editor_load_landscape(gOpenRCT2StartupActionPath);
			}
			break;
    	}
		openrct2_loop();
	}
	openrct2_dispose();

	// HACK Some threads are still running which causes the game to not terminate. Investigation required!
	exit(gExitCode);
}

void openrct2_dispose()
{
	http_dispose();
	language_close_all();
	platform_free();
}

/**
 * Run the main game loop until the finished flag is set at 40fps (25ms interval).
 */
static void openrct2_loop()
{
	uint32 currentTick, ticksElapsed, lastTick = 0;

	log_verbose("begin openrct2 loop");

	_finished = 0;
	do {
		currentTick = SDL_GetTicks();
		ticksElapsed = currentTick - lastTick;
		if (ticksElapsed < 25) {
			if (ticksElapsed < 15)
				SDL_Delay(15 - ticksElapsed);
			continue;
		}

		lastTick = currentTick;

		platform_process_messages();
		rct2_update();
		platform_draw();
	} while (!_finished);
}

/**
 * Causes the OpenRCT2 game loop to finish.
 */
void openrct2_finish()
{
	_finished = 1;
}