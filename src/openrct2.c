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

#include "audio/audio.h"
#include "audio/mixer.h"
#include "config.h"
#include "editor.h"
#include "game.h"
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
#include "rct2/interop.h"
#include "ride/ride.h"
#include "title.h"
#include "util/util.h"
#include "version.h"
#include "world/mapgen.h"

#define UPDATE_TIME_MS		25		// (1000 / 40fps) = 25ms

int gExitCode;

int gOpenRCT2StartupAction = STARTUP_ACTION_TITLE;
utf8 gOpenRCT2StartupActionPath[512] = { 0 };
utf8 gExePath[MAX_PATH];
utf8 gCustomUserDataPath[MAX_PATH] = { 0 };
utf8 gCustomOpenrctDataPath[MAX_PATH] = { 0 };
utf8 gCustomRCT2DataPath[MAX_PATH] = { 0 };
utf8 gCustomPassword[MAX_PATH] = { 0 };

// This should probably be changed later and allow a custom selection of things to initialise like SDL_INIT
bool gOpenRCT2Headless = false;

bool gOpenRCT2ShowChangelog;
bool gOpenRCT2SilentBreakpad;

#ifndef DISABLE_NETWORK
// OpenSSL's message digest context used for calculating sprite checksums
EVP_MD_CTX *gHashCTX = NULL;
#endif // DISABLE_NETWORK

/** If set, will end the OpenRCT2 game loop. Intentially private to this module so that the flag can not be set back to 0. */
int _finished;

static void openrct2_loop();

void openrct2_write_full_version_info(utf8 *buffer, size_t bufferSize)
{
	utf8 *ch = buffer;

	// Name and version
	safe_strcpy(ch, OPENRCT2_NAME ", v" OPENRCT2_VERSION, bufferSize - (ch - buffer));
	ch = strchr(ch, '\0');

	// Build information
	if (!str_is_null_or_empty(gGitBranch)) {
		snprintf(ch, bufferSize - (ch - buffer),  "-%s", gGitBranch);
		ch = strchr(ch, '\0');
	}
	if (!str_is_null_or_empty(gCommitSha1Short)) {
		snprintf(ch, bufferSize - (ch - buffer), " build %s", gCommitSha1Short);
		ch = strchr(ch, '\0');
	}
	if (!str_is_null_or_empty(gBuildServer)) {
		snprintf(ch, bufferSize - (ch - buffer), " provided by %s", gBuildServer);
		ch = strchr(ch, '\0');
	}

#if DEBUG
	snprintf(ch, bufferSize - (ch - buffer), " (DEBUG)");
#endif
}

static void openrct2_set_exe_path()
{
	platform_get_exe_path(gExePath, sizeof(gExePath));
	log_verbose("Setting exe path to %s", gExePath);
}

bool openrct2_initialise()
{
	utf8 userPath[MAX_PATH];

#ifndef DISABLE_NETWORK
	gHashCTX = EVP_MD_CTX_create();
	assert(gHashCTX != NULL);
#endif // DISABLE_NETWORK

	platform_resolve_openrct_data_path();
	platform_resolve_user_data_path();
	platform_get_user_directory(userPath, NULL, sizeof(userPath));
	if (!platform_ensure_directory_exists(userPath)) {
		log_fatal("Could not create user directory (do you have write access to your documents folder?)");
		return false;
	}

	crash_init();

	if (!rct2_interop_setup_segment()) {
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
			config_get_default_path(path, sizeof(path));
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

	if (!rct2_init_directories()) {
		return false;
	}
	if (!rct2_startup_checks()) {
		return false;
	}

	if (!gOpenRCT2Headless) {
		audio_init();
		audio_populate_devices();
	}

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

	rct2_interop_setup_hooks();

	if (!rct2_init())
		return false;

	chat_init();

	rct2_copy_original_user_files_over();
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
			title_load();
			break;
		case STARTUP_ACTION_TITLE:
			title_load();
			break;
		case STARTUP_ACTION_OPEN:
			assert(gOpenRCT2StartupActionPath != NULL);
			if (!rct2_open_file(gOpenRCT2StartupActionPath)) {
				fprintf(stderr, "Failed to load '%s'", gOpenRCT2StartupActionPath);
				title_load();
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
				if (!editor_load_landscape(gOpenRCT2StartupActionPath)) {
					title_load();
				}
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
#ifndef DISABLE_NETWORK
	EVP_MD_CTX_destroy(gHashCTX);
#endif // DISABLE_NETWORK
	rct2_interop_dispose();
	platform_free();
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
				sprite_position_tween_reset();
			}

			// Limit number of updates per loop (any long pauses or debugging can make this update for a very long time)
			if (currentTick - uncapTick > UPDATE_TIME_MS * 60) {
				uncapTick = currentTick - UPDATE_TIME_MS - 1;
			}

			platform_process_messages();

			while (uncapTick <= currentTick && currentTick - uncapTick > UPDATE_TIME_MS) {
				// Get the original position of each sprite
				sprite_position_tween_store_a();

				// Update the game so the sprite positions update
				rct2_update();

				// Get the next position of each sprite
				sprite_position_tween_store_b();

				uncapTick += UPDATE_TIME_MS;
			}

			// Tween the position of each sprite from the last position to the new position based on the time between the last
			// tick and the next tick.
			float nudge = 1 - ((float)(currentTick - uncapTick) / UPDATE_TIME_MS);
			sprite_position_tween_all(nudge);

			platform_draw();

			fps++;
			if (SDL_GetTicks() - secondTick >= 1000) {
				fps = 0;
				secondTick = SDL_GetTicks();
			}

			sprite_position_tween_restore();
		} else {
			uncapTick = 0;
			currentTick = SDL_GetTicks();
			ticksElapsed = currentTick - lastTick;
			if (ticksElapsed < UPDATE_TIME_MS) {
				SDL_Delay(UPDATE_TIME_MS - ticksElapsed);
				lastTick += UPDATE_TIME_MS;
			} else {
				lastTick = currentTick;
			}

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
