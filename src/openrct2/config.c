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
#include "interface/keyboard_shortcut.h"
#include "interface/themes.h"
#include "interface/viewport.h"
#include "localisation/language.h"
#include "localisation/localisation.h"
#include "network/network.h"
#include "OpenRCT2.h"
#include "util/util.h"

#pragma region Shortcuts

// Current keyboard shortcuts
uint16 gShortcutKeys[SHORTCUT_COUNT];

// Default keyboard shortcuts
static const uint16 _defaultShortcutKeys[SHORTCUT_COUNT] = {
	SDL_SCANCODE_BACKSPACE,				// SHORTCUT_CLOSE_TOP_MOST_WINDOW
	SHIFT | SDL_SCANCODE_BACKSPACE,		// SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS
	SDL_SCANCODE_ESCAPE,				// SHORTCUT_CANCEL_CONSTRUCTION_MODE
	SDL_SCANCODE_PAUSE,					// SHORTCUT_PAUSE_GAME
	SDL_SCANCODE_PAGEUP,				// SHORTCUT_ZOOM_VIEW_OUT
	SDL_SCANCODE_PAGEDOWN,				// SHORTCUT_ZOOM_VIEW_IN
	SDL_SCANCODE_RETURN,				// SHORTCUT_ROTATE_VIEW_CLOCKWISE
	SHIFT | SDL_SCANCODE_RETURN,		// SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE
	SDL_SCANCODE_Z,						// SHORTCUT_ROTATE_CONSTRUCTION_OBJECT
	SDL_SCANCODE_1,						// SHORTCUT_UNDERGROUND_VIEW_TOGGLE
	SDL_SCANCODE_H,						// SHORTCUT_REMOVE_BASE_LAND_TOGGLE
	SDL_SCANCODE_V,						// SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE
	SDL_SCANCODE_3,						// SHORTCUT_SEE_THROUGH_RIDES_TOGGLE
	SDL_SCANCODE_4,						// SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE
	SDL_SCANCODE_5,						// SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE
	SDL_SCANCODE_6,						// SHORTCUT_INVISIBLE_PEOPLE_TOGGLE
	SDL_SCANCODE_8,						// SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE
	SDL_SCANCODE_9,						// SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE
	SDL_SCANCODE_0,						// SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE
	SDL_SCANCODE_F1,					// SHORTCUT_ADJUST_LAND
	SDL_SCANCODE_F2,					// SHORTCUT_ADJUST_WATER
	SDL_SCANCODE_F3,					// SHORTCUT_BUILD_SCENERY
	SDL_SCANCODE_F4,					// SHORTCUT_BUILD_PATHS
	SDL_SCANCODE_F5,					// SHORTCUT_BUILD_NEW_RIDE
	SDL_SCANCODE_F,						// SHORTCUT_SHOW_FINANCIAL_INFORMATION
	SDL_SCANCODE_D,						// SHORTCUT_SHOW_RESEARCH_INFORMATION
	SDL_SCANCODE_R,						// SHORTCUT_SHOW_RIDES_LIST
	SDL_SCANCODE_P,						// SHORTCUT_SHOW_PARK_INFORMATION
	SDL_SCANCODE_G,						// SHORTCUT_SHOW_GUEST_LIST
	SDL_SCANCODE_S,						// SHORTCUT_SHOW_STAFF_LIST
	SDL_SCANCODE_M,						// SHORTCUT_SHOW_RECENT_MESSAGES
	SDL_SCANCODE_TAB,					// SHORTCUT_SHOW_MAP
	PLATFORM_MODIFIER | SDL_SCANCODE_S,	// SHORTCUT_SCREENSHOT

	// New
	SDL_SCANCODE_MINUS,					// SHORTCUT_REDUCE_GAME_SPEED,
	SDL_SCANCODE_EQUALS,				// SHORTCUT_INCREASE_GAME_SPEED,
	PLATFORM_MODIFIER | ALT | SDL_SCANCODE_C,	// SHORTCUT_OPEN_CHEAT_WINDOW,
	SDL_SCANCODE_T,						// SHORTCUT_REMOVE_TOP_BOTTOM_TOOLBAR_TOGGLE,
	SDL_SCANCODE_UP,					// SHORTCUT_SCROLL_MAP_UP
	SDL_SCANCODE_LEFT,					// SHORTCUT_SCROLL_MAP_LEFT
	SDL_SCANCODE_DOWN,					// SHORTCUT_SCROLL_MAP_DOWN
	SDL_SCANCODE_RIGHT,					// SHORTCUT_SCROLL_MAP_RIGHT
	SDL_SCANCODE_C,						// SHORTCUT_OPEN_CHAT_WINDOW
	PLATFORM_MODIFIER | SDL_SCANCODE_F10,	// SHORTCUT_QUICK_SAVE_GAME

	SHORTCUT_UNDEFINED,					// SHORTCUT_SHOW_OPTIONS
	SHORTCUT_UNDEFINED,					// SHORTCUT_MUTE_SOUND
	ALT | SDL_SCANCODE_RETURN,			// SHORTCUT_WINDOWED_MODE_TOGGLE
	SHORTCUT_UNDEFINED,					// SHORTCUT_SHOW_MULTIPLAYER
	SHORTCUT_UNDEFINED,					// SHORTCUT_PAINT_ORIGINAL_TOGGLE
	SHORTCUT_UNDEFINED,					// SHORTCUT_DEBUG_PAINT_TOGGLE
	SHORTCUT_UNDEFINED,					// SHORTCUT_SEE_THROUGH_PATHS_TOGGLE
};

#define SHORTCUT_FILE_VERSION 1

/**
 *
 *  rct2: 0x006E3604
 */
void config_reset_shortcut_keys()
{
	memcpy(gShortcutKeys, _defaultShortcutKeys, sizeof(gShortcutKeys));
}

static void config_shortcut_keys_get_path(utf8 *outPath, size_t size)
{
	platform_get_user_directory(outPath, NULL, size);
	safe_strcat_path(outPath, "hotkeys.cfg", size);
}

bool config_shortcut_keys_load()
{
	utf8 path[MAX_PATH];
	SDL_RWops *file;
	bool result;
	uint16 version;

	config_shortcut_keys_get_path(path, sizeof(path));

	file = SDL_RWFromFile(path, "rb");
	if (file != NULL) {
		result = SDL_RWread(file, &version, sizeof(version), 1) == 1;
		if (result && version == SHORTCUT_FILE_VERSION) {
			for (sint32 i = 0; i < SHORTCUT_COUNT; i++) {
				if (SDL_RWread(file, &gShortcutKeys[i], sizeof(uint16), 1) != 1) {
					break;
				}
			}
		} else {
			result = false;
		}
		SDL_RWclose(file);
	} else {
		result = false;
	}

	return result;
}

bool config_shortcut_keys_save()
{
	const uint16 version = SHORTCUT_FILE_VERSION;

	utf8 path[MAX_PATH];
	SDL_RWops *file;
	bool result;

	config_shortcut_keys_get_path(path, sizeof(path));

	file = SDL_RWFromFile(path, "wb");
	if (file != NULL) {
		result = SDL_RWwrite(file, &version, sizeof(version), 1) == 1;
		if (result) {
			result = SDL_RWwrite(file, gShortcutKeys, sizeof(gShortcutKeys), 1) == 1;
		}
		SDL_RWclose(file);
	} else {
		result = false;
	}

	return result;
}

#pragma endregion
