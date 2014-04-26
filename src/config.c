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

#include <SDL_keycode.h>
#include <windows.h>
#include "addresses.h"
#include "config.h"
#include "rct2.h"

// Current keyboard shortcuts
uint16 gShortcutKeys[SHORTCUT_COUNT];

// Magic number for original game cfg file
static const int MagicNumber = 0x0003113A;

// Default keyboard shortcuts
static const uint16 _defaultShortcutKeys[SHORTCUT_COUNT] = {
	SDL_SCANCODE_BACKSPACE,				// SHORTCUT_CLOSE_TOP_MOST_WINDOW
	0x0100 | SDL_SCANCODE_BACKSPACE,	// SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS
	SDL_SCANCODE_ESCAPE,				// SHORTCUT_CANCEL_CONSTRUCTION_MODE
	SDL_SCANCODE_PAUSE,					// SHORTCUT_PAUSE_GAME
	SDL_SCANCODE_PAGEUP,				// SHORTCUT_ZOOM_VIEW_OUT
	SDL_SCANCODE_PAGEDOWN,				// SHORTCUT_ZOOM_VIEW_IN
	SDL_SCANCODE_RETURN,				// SHORTCUT_ROTATE_VIEW
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
	0x0200 | SDL_SCANCODE_S				// SHORTCUT_SCREENSHOT
};

/**
 * 
 *  rct2: 0x006E3604
 */
void config_reset_shortcut_keys()
{
	memcpy(gShortcutKeys, _defaultShortcutKeys, sizeof(gShortcutKeys));
}

/**
 * 
 *  rct2: 0x006752D5
 */
void config_load()
{
	HANDLE hFile;
	DWORD bytesRead;

	hFile = CreateFile(get_file_path(PATH_ID_GAMECFG), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		// Read and check magic number
		ReadFile(hFile, RCT2_ADDRESS(0x013CE928, void), 4, &bytesRead, NULL);
		if (RCT2_GLOBAL(0x013CE928, int) == MagicNumber) {
			// Read options
			ReadFile(hFile, (void*)0x009AAC5C, 2155, &bytesRead, NULL);
			CloseHandle(hFile);
			if (RCT2_GLOBAL(0x009AB4C6, sint8) == 1)
				return;
			RCT2_GLOBAL(0x009AB4C6, sint8) = 1;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FAHRENHEIT, sint8) = 1;
			RCT2_GLOBAL(0x009AACBB, sint8) = 1;
			RCT2_GLOBAL(0x009AACBD, sint16) = 0;
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
				RCT2_GLOBAL(0x009AACBD, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
			RCT2_GLOBAL(0x009AA00D, sint8) = 1;
		}
	}

	RCT2_GLOBAL(0x009AAC77, sint8) = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_OS_TOTALPHYS, uint32) > 0x4000000) {
		RCT2_GLOBAL(0x009AAC77, sint8) = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_OS_TOTALPHYS, uint32) > 0x8000000)
			RCT2_GLOBAL(0x009AAC77, sint8) = 2;
	}

	RCT2_GLOBAL(0x009AAC75, sint8) = RCT2_ADDRESS(0x009AF601, sint8)[RCT2_GLOBAL(0x009AAC77, sint8)];
	RCT2_GLOBAL(0x009AAC76, sint8) = RCT2_ADDRESS(0x009AF604, sint8)[RCT2_GLOBAL(0x009AAC77, sint8)];
	RCT2_GLOBAL(0x009AACBD, sint16) = 0;
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
		RCT2_GLOBAL(0x009AACBD, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
	RCT2_GLOBAL(0x009AA00D, sint8) = 1;
}

/**
 * 
 *  rct2: 0x00675487
 */
void config_save()
{
	HANDLE hFile;
	DWORD bytesWritten;

	hFile = CreateFile(get_file_path(PATH_ID_GAMECFG), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		WriteFile(hFile, &MagicNumber, 4, &bytesWritten, NULL);
		WriteFile(hFile, 0x009AAC5C, 2155, &bytesWritten, NULL);
		CloseHandle(hFile);
	}
}