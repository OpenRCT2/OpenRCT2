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

#include "addresses.h"
#include "audio/audio.h"
#include "audio/mixer.h"
#include "config.h"
#include "editor.h"
#include "game.h"
#include "hook.h"
#include "interface/chat.h"
#include "interface/themes.h"
#include "interface/window.h"
#include "interface/viewport.h"
#include "intro.h"
#include "localisation/localisation.h"
#include "network/http.h"
#include "network/network.h"
#include "object_list.h"
#include "openrct2.h"
#include "platform/crash.h"
#include "platform/platform.h"
#include "ride/ride.h"
#include "title.h"
#include "util/sawyercoding.h"
#include "util/util.h"
#include "world/mapgen.h"

#if defined(__unix__)
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif // defined(__unix__)

int gExitCode;

int gOpenRCT2StartupAction = STARTUP_ACTION_TITLE;
utf8 gOpenRCT2StartupActionPath[512] = { 0 };
utf8 gExePath[MAX_PATH];
utf8 gCustomUserDataPath[MAX_PATH] = { 0 };
utf8 gCustomOpenrctDataPath[MAX_PATH] = { 0 };
utf8 gCustomPassword[MAX_PATH] = { 0 };

// This should probably be changed later and allow a custom selection of things to initialise like SDL_INIT
bool gOpenRCT2Headless = false;

bool gOpenRCT2ShowChangelog;
bool gOpenRCT2SilentBreakpad;

/** If set, will end the OpenRCT2 game loop. Intentially private to this module so that the flag can not be set back to 0. */
int _finished;

// Used for object movement tweening
static struct { sint16 x, y, z; } _spritelocations1[MAX_SPRITES], _spritelocations2[MAX_SPRITES];

static void openrct2_loop();
static void openrct2_setup_rct2_hooks();

void openrct2_write_full_version_info(utf8 *buffer, size_t bufferSize)
{
	utf8 *ch = buffer;

	// Name and version
	strcpy(ch, OPENRCT2_NAME);
	strcat(buffer, ", v");
	strcat(buffer, OPENRCT2_VERSION);

	// Build information
	if (!str_is_null_or_empty(OPENRCT2_BRANCH)) {
		sprintf(strchr(buffer, 0), "-%s", OPENRCT2_BRANCH);
	}
	if (!str_is_null_or_empty(OPENRCT2_COMMIT_SHA1_SHORT)) {
		sprintf(strchr(buffer, 0), " build %s", OPENRCT2_COMMIT_SHA1_SHORT);
	}
	if (!str_is_null_or_empty(OPENRCT2_BUILD_SERVER)) {
		sprintf(strchr(buffer, 0), " provided by %s", OPENRCT2_BUILD_SERVER);
	}

#if DEBUG
	sprintf(strchr(buffer, 0), " (DEBUG)");
#endif
}

static void openrct2_copy_files_over(const utf8 *originalDirectory, const utf8 *newDirectory, const utf8 *extension)
{
	utf8 *ch, filter[MAX_PATH], oldPath[MAX_PATH], newPath[MAX_PATH];
	int fileEnumHandle;
	file_info fileInfo;

	if (!platform_ensure_directory_exists(newDirectory)) {
		log_error("Could not create directory %s.", newDirectory);
		return;
	}

	// Create filter path
	safe_strcpy(filter, originalDirectory, MAX_PATH);
	ch = strchr(filter, '*');
	if (ch != NULL)
		*ch = 0;
	strcat(filter, "*");
	strcat(filter, extension);

	fileEnumHandle = platform_enumerate_files_begin(filter);
	while (platform_enumerate_files_next(fileEnumHandle, &fileInfo)) {
		safe_strcpy(newPath, newDirectory, MAX_PATH);
		strcat(newPath, fileInfo.path);

		safe_strcpy(oldPath, originalDirectory, MAX_PATH);
		ch = strchr(oldPath, '*');
		if (ch != NULL)
			*ch = 0;
		strcat(oldPath, fileInfo.path);

		if (!platform_file_exists(newPath))
			platform_file_copy(oldPath, newPath, false);
	}
	platform_enumerate_files_end(fileEnumHandle);

	fileEnumHandle = platform_enumerate_directories_begin(originalDirectory);
	while (platform_enumerate_directories_next(fileEnumHandle, filter)) {
		safe_strcpy(newPath, newDirectory, MAX_PATH);
		strcat(newPath, filter);

		safe_strcpy(oldPath, originalDirectory, MAX_PATH);
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
	platform_get_exe_path(gExePath);
	log_verbose("Setting exe path to %s", gExePath);
}

/**
 * Copy saved games and landscapes to user directory
 */
static void openrct2_copy_original_user_files_over()
{
	utf8 path[MAX_PATH];

	platform_get_user_directory(path, "save");
	openrct2_copy_files_over((utf8*)gRCT2AddressSavedGamesPath, path, ".sv6");

	platform_get_user_directory(path, "landscape");
	openrct2_copy_files_over((utf8*)gRCT2AddressLandscapesPath, path, ".sc6");
}

bool openrct2_initialise()
{
	utf8 userPath[MAX_PATH];

	platform_resolve_openrct_data_path();
	platform_resolve_user_data_path();
	platform_get_user_directory(userPath, NULL);
	if (!platform_ensure_directory_exists(userPath)) {
		log_fatal("Could not create user directory (do you have write access to your documents folder?)");
		return false;
	}

	crash_init();

	if (!openrct2_setup_rct2_segment()) {
		log_fatal("Unable to load RCT2 data sector");
		return false;
	}

	openrct2_set_exe_path();

	config_set_defaults();
	if (!config_open_default()) {
		if (!config_find_or_browse_install_directory()) {
			gConfigGeneral.last_run_version = strndup(OPENRCT2_VERSION, strlen(OPENRCT2_VERSION));
			config_save_default();
			utf8 path[MAX_PATH];
			config_get_default_path(path);
			log_fatal("An RCT2 install directory must be specified! Please edit \"game_path\" in %s.", path);
			return false;
		}
	}

	gOpenRCT2ShowChangelog = true;
	if (gConfigGeneral.last_run_version != NULL && (strcmp(gConfigGeneral.last_run_version, OPENRCT2_VERSION) == 0))
		gOpenRCT2ShowChangelog = false;
	gConfigGeneral.last_run_version = strndup(OPENRCT2_VERSION, strlen(OPENRCT2_VERSION));
	config_save_default();

	// TODO add configuration option to allow multiple instances
	// if (!gOpenRCT2Headless && !platform_lock_single_instance()) {
	// 	log_fatal("OpenRCT2 is already running.");
	// 	return false;
	// }

	if (!gOpenRCT2Headless) {
		audio_init();
		audio_populate_devices();
	}

	object_list_init();

	if (!language_open(gConfigGeneral.language)) {
		log_error("Failed to open configured language...");

		if (!language_open(LANGUAGE_ENGLISH_UK)) {
			log_fatal("Failed to open fallback language...");
			return false;
		}
	}
	http_init();

	theme_manager_initialise();
	title_sequences_set_default();
	title_sequences_load_presets();

	openrct2_setup_rct2_hooks();

	if (!rct2_init())
		return false;

	chat_init();

	openrct2_copy_original_user_files_over();

	// TODO move to audio initialise function
	if (str_is_null_or_empty(gConfigSound.device)) {
		Mixer_Init(NULL);
		gAudioCurrentDevice = 0;
	} else {
		Mixer_Init(gConfigSound.device);
		for (int i = 0; i < gAudioDeviceCount; i++) {
			if (strcmp(gAudioDevices[i].name, gConfigSound.device) == 0) {
				gAudioCurrentDevice = i;
			}
		}
	}

	return true;
}

/**
 * Launches the game, after command line arguments have been parsed and processed.
 */
void openrct2_launch()
{
	if (openrct2_initialise()) {
		gIntroState = INTRO_STATE_NONE;
		if((gOpenRCT2StartupAction == STARTUP_ACTION_TITLE) && gConfigGeneral.play_intro)
			gOpenRCT2StartupAction = STARTUP_ACTION_INTRO;

		switch (gOpenRCT2StartupAction) {
		case STARTUP_ACTION_INTRO:
			gIntroState = INTRO_STATE_PUBLISHER_BEGIN;
			break;
		case STARTUP_ACTION_TITLE:
			gScreenFlags = SCREEN_FLAGS_TITLE_DEMO;
			break;
		case STARTUP_ACTION_OPEN:
			assert(gOpenRCT2StartupActionPath != NULL);
			if (!rct2_open_file(gOpenRCT2StartupActionPath)) {
				break;
			}

			gScreenFlags = SCREEN_FLAGS_PLAYING;

#ifndef DISABLE_NETWORK
			if (gNetworkStart == NETWORK_MODE_SERVER) {
				if (gNetworkStartPort == 0) {
					gNetworkStartPort = gConfigNetwork.default_port;
				}

				if (str_is_null_or_empty(gCustomPassword)) {
					network_set_password(gConfigNetwork.default_password);
				}
				else {
					network_set_password(gCustomPassword);
				}
				network_begin_server(gNetworkStartPort);
			}
#endif // DISABLE_NETWORK
			break;
		case STARTUP_ACTION_EDIT:
			if (strlen(gOpenRCT2StartupActionPath) == 0) {
				editor_load();
			} else {
				editor_load_landscape(gOpenRCT2StartupActionPath);
			}
			break;
		}

#ifndef DISABLE_NETWORK
		if (gNetworkStart == NETWORK_MODE_CLIENT) {
			if (gNetworkStartPort == 0) {
				gNetworkStartPort = gConfigNetwork.default_port;
			}

			network_begin_client(gNetworkStartHost, gNetworkStartPort);
		}
#endif // DISABLE_NETWORK

		openrct2_loop();
	}
	openrct2_dispose();

	// HACK Some threads are still running which causes the game to not terminate. Investigation required!
	exit(gExitCode);
}

void openrct2_dispose()
{
	network_close();
	http_dispose();
	language_close_all();
	rct2_dispose();
	config_release();
	platform_free();
}

/**
 * Determines whether its worth tweening a sprite or not when frame smoothing is on.
 */
static bool sprite_should_tween(rct_sprite *sprite)
{
	switch (sprite->unknown.linked_list_type_offset >> 1) {
	case SPRITE_LIST_VEHICLE:
	case SPRITE_LIST_PEEP:
	case SPRITE_LIST_UNKNOWN:
		return true;
	}
	return false;
}

/**
 * Run the main game loop until the finished flag is set at 40fps (25ms interval).
 */
static void openrct2_loop()
{
	uint32 currentTick, ticksElapsed, lastTick = 0;
	static uint32 uncapTick = 0;
	static int fps = 0;
	static uint32 secondTick = 0;

	log_verbose("begin openrct2 loop");

	_finished = 0;
	do {
		bool is_minimised = (SDL_GetWindowFlags(gWindow) & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN)) != 0;
		if (gConfigGeneral.uncap_fps && gGameSpeed <= 4 && !gOpenRCT2Headless && !is_minimised) {
			currentTick = SDL_GetTicks();
			if (uncapTick == 0) {
				// Reset sprite locations
				uncapTick = SDL_GetTicks();
				openrct2_reset_object_tween_locations();
			}

			// Limit number of updates per loop (any long pauses or debugging can make this update for a very long time)
			if (currentTick - uncapTick > 25 * 60) {
				uncapTick = currentTick - 25 - 1;
			}

			platform_process_messages();

			while (uncapTick <= currentTick && currentTick - uncapTick > 25) {
				// Get the original position of each sprite
				for (uint16 i = 0; i < MAX_SPRITES; i++) {
					_spritelocations1[i].x = g_sprite_list[i].unknown.x;
					_spritelocations1[i].y = g_sprite_list[i].unknown.y;
					_spritelocations1[i].z = g_sprite_list[i].unknown.z;
				}

				// Update the game so the sprite positions update
				rct2_update();

				// Get the next position of each sprite
				for (uint16 i = 0; i < MAX_SPRITES; i++) {
					_spritelocations2[i].x = g_sprite_list[i].unknown.x;
					_spritelocations2[i].y = g_sprite_list[i].unknown.y;
					_spritelocations2[i].z = g_sprite_list[i].unknown.z;
				}

				uncapTick += 25;
			}

			// Tween the position of each sprite from the last position to the new position based on the time between the last
			// tick and the next tick.
			float nudge = 1 - ((float)(currentTick - uncapTick) / 25);
			for (uint16 i = 0; i < MAX_SPRITES; i++) {
				if (!sprite_should_tween(&g_sprite_list[i]))
					continue;

				sprite_move(
					_spritelocations2[i].x + (sint16)((_spritelocations1[i].x - _spritelocations2[i].x) * nudge),
					_spritelocations2[i].y + (sint16)((_spritelocations1[i].y - _spritelocations2[i].y) * nudge),
					_spritelocations2[i].z + (sint16)((_spritelocations1[i].z - _spritelocations2[i].z) * nudge),
					&g_sprite_list[i]
				);
				invalidate_sprite_2(&g_sprite_list[i]);
			}

			platform_draw();

			fps++;
			if (SDL_GetTicks() - secondTick >= 1000) {
				fps = 0;
				secondTick = SDL_GetTicks();
			}

			// Restore the real positions of the sprites so they aren't left at the mid-tween positions
			for (uint16 i = 0; i < MAX_SPRITES; i++) {
				if (!sprite_should_tween(&g_sprite_list[i]))
					continue;

				invalidate_sprite_2(&g_sprite_list[i]);
				sprite_move(_spritelocations2[i].x, _spritelocations2[i].y, _spritelocations2[i].z, &g_sprite_list[i]);
			}
			network_update();
		} else {
			uncapTick = 0;
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

			if (!is_minimised) {
				platform_draw();
			}
		}
	} while (!_finished);
}

/**
 * Causes the OpenRCT2 game loop to finish.
 */
void openrct2_finish()
{
	_finished = 1;
}

void openrct2_reset_object_tween_locations()
{
	for (uint16 i = 0; i < MAX_SPRITES; i++) {
		_spritelocations1[i].x = _spritelocations2[i].x = g_sprite_list[i].unknown.x;
		_spritelocations1[i].y = _spritelocations2[i].y = g_sprite_list[i].unknown.y;
		_spritelocations1[i].z = _spritelocations2[i].z = g_sprite_list[i].unknown.z;
	}
}

/**
 * Loads RCT2's data model and remaps the addresses.
 * @returns true if the data integrity check succeeded, otherwise false.
 */
bool openrct2_setup_rct2_segment()
{
	// OpenRCT2 on Linux and macOS is wired to have the original Windows PE sections loaded
	// necessary. Windows does not need to do this as OpenRCT2 runs as a DLL loaded from the Windows PE.
#if defined(__unix__)
	#define RDATA_OFFSET 0x004A4000
	#define DATASEG_OFFSET 0x005E2000

	// Using PE-bear I was able to figure out all the needed addresses to be filled.
	// There are three sections to be loaded: .rdata, .data and .text, plus another
	// one to be mapped: DATASEG.
	// Out of the three, two can simply be mmapped into memory, while the third one,
	// .data has a virtual size which is much completely different to its file size
	// (even when taking page-alignment into consideration)
	//
	// The sections are as follows (dump from gdb)
	// [0]     0x401000->0x6f7000 at 0x00001000: .text ALLOC LOAD READONLY CODE HAS_CONTENTS
	// [1]     0x6f7000->0x8a325d at 0x002f7000: CODESEG ALLOC LOAD READONLY CODE HAS_CONTENTS
	// [2]     0x8a4000->0x9a5894 at 0x004a4000: .rdata ALLOC LOAD DATA HAS_CONTENTS
	// [3]     0x9a6000->0x9e2000 at 0x005a6000: .data ALLOC LOAD DATA HAS_CONTENTS
	// [4]     0x1428000->0x14282bc at 0x005e2000: DATASEG ALLOC LOAD DATA HAS_CONTENTS
	// [5]     0x1429000->0x1452000 at 0x005e3000: .cms_t ALLOC LOAD READONLY CODE HAS_CONTENTS
	// [6]     0x1452000->0x14aaf3e at 0x0060c000: .cms_d ALLOC LOAD DATA HAS_CONTENTS
	// [7]     0x14ab000->0x14ac58a at 0x00665000: .idata ALLOC LOAD READONLY DATA HAS_CONTENTS
	// [8]     0x14ad000->0x14b512f at 0x00667000: .rsrc ALLOC LOAD DATA HAS_CONTENTS
	//
	// .data section, however, has virtual size of 0xA81C3C, and so
	// 0x9a6000 + 0xA81C3C = 0x1427C3C, which after alignment to page size becomes
	// 0x1428000, which can be seen as next section, DATASEG
	//
	// The data is now loaded into memory with a linker script, which proves to
	// be more reliable, as mallocs that happen before we reach segment setup
	// could have already taken the space we need.

	// TODO: UGLY, UGLY HACK!
	//off_t file_size = 6750208;

	int len = 0x01429000 - 0x8a4000; // 0xB85000, 12079104 bytes or around 11.5MB
	int pageSize = getpagesize();
	int numPages = (len + pageSize - 1) / pageSize;
	unsigned char *dummy = malloc(numPages);
	int err = mincore((void *)0x8a4000, len, dummy);
	bool pagesMissing = false;
	if (err != 0)
	{
		err = errno;
#ifdef __LINUX__
		// On Linux ENOMEM means all requested range is unmapped
		if (err != ENOMEM)
		{
			pagesMissing = true;
			perror("mincore");
		}
#else
		pagesMissing = true;
		perror("mincore");
#endif // __LINUX__
	} else {
		for (int i = 0; i < numPages; i++)
		{
			if (dummy[i] != 1)
			{
				pagesMissing = true;
				void *start = (void *)0x8a4000 + i * pageSize;
				void *end = (void *)0x8a4000 + (i + 1) * pageSize - 1;
				log_warning("required page %p - %p is not in memory!", start, end);
			}
		}
	}
	free(dummy);
	if (pagesMissing)
	{
		log_error("At least one of required pages was not found in memory. This can cause segfaults later on.");
	}
	// section: text
	err = mprotect((void *)0x401000, 0x8a4000 - 0x401000, PROT_READ | PROT_EXEC);
	if (err != 0)
	{
		perror("mprotect");
	}
	// section: rw data
	err = mprotect((void *)0x8a4000, 0x01429000 - 0x8a4000, PROT_READ | PROT_WRITE);
	if (err != 0)
	{
		perror("mprotect");
	}
#endif // defined(__unix__)

#if !defined(NO_RCT2) || !defined(__WINDOWS__)
	// Check that the expected data is at various addresses.
	// Start at 0x9a6000, which is start of .data, to skip the region containing addresses to DLL
	// calls, which can be changed by windows/wine loader.
	const uint32 c1 = sawyercoding_calculate_checksum((void *)0x009A6000, 0x009E0000 - 0x009A6000);
	const uint32 c2 = sawyercoding_calculate_checksum((void *)0x01428000, 0x014282BC - 0x01428000);
	const uint32 exp_c1 = 10114815;
	const uint32 exp_c2 = 23564;
	if (c1 != exp_c1 || c2 != exp_c2) {
		log_warning("c1 = %u, expected %u, match %d", c1, exp_c1, c1 == exp_c1);
		log_warning("c2 = %u, expected %u, match %d", c2, exp_c2, c2 == exp_c2);
		return false;
	}
#endif

	return true;
}

/**
 * Setup hooks to allow RCT2 to call OpenRCT2 functions instead.
 */
static void openrct2_setup_rct2_hooks()
{
	// None for now
}

#if _MSC_VER >= 1900
/**
 * Temporary fix for libraries not compiled with VS2015
 */
FILE **__iob_func()
{
	static FILE* streams[3];
	streams[0] = stdin;
	streams[1] = stdout;
	streams[2] = stderr;
	return streams;
}
#endif
