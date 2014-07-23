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
#include <time.h>
#include "addresses.h"
#include "config.h"
#include "climate.h"
#include "date.h"
#include "game.h"
#include "gfx.h"
#include "intro.h"
#include "map.h"
#include "news_item.h"
#include "park.h"
#include "rct2.h"
#include "ride.h"
#include "scenario.h"
#include "sprite.h"
#include "string_ids.h"
#include "viewport.h"
#include "editor.h"

static const int gOldMusic = 0;
static const int gRandomShowcase = 0;

#pragma region Showcase script

enum {
	TITLE_SCRIPT_WAIT,
	TITLE_SCRIPT_LOAD,
	TITLE_SCRIPT_LOCATION,
	TITLE_SCRIPT_ROTATE,
	TITLE_SCRIPT_RESTART,
};

#define WAIT(t)				TITLE_SCRIPT_WAIT, t
#define LOAD()				TITLE_SCRIPT_LOAD
#define LOCATION(x, y)		TITLE_SCRIPT_LOCATION, x, y
#define ROTATE(n)			TITLE_SCRIPT_ROTATE, n
#define RESTART()			TITLE_SCRIPT_RESTART

static const uint8 _magicMountainScript[] = {
	LOAD(),
	LOCATION(210, 112), WAIT(13),
	ROTATE(1), LOCATION(210, 112), WAIT(14),
	ROTATE(3), LOCATION(167, 180), WAIT(12),
	ROTATE(1), LOCATION(155, 189), WAIT(12),
	LOCATION(106, 39), WAIT(12),
	LOCATION(182, 50), WAIT(12),
	ROTATE(3), LOCATION(209, 47), WAIT(12),
	ROTATE(1), LOCATION(159, 93), WAIT(12),
	RESTART(),
};

static const uint8* _currentScript;
static int _scriptWaitCounter;

static void title_init_showcase();
static void title_update_showcase();
static void title_play_music();

static uint8 *generate_random_script();

#pragma endregion

static void title_create_windows();

/**
 *
 *  rct2: 0x0068E8DA
 */
void title_load()
{
	if (RCT2_GLOBAL(0x009DEA6E, uint8) & 1)
		RCT2_CALLPROC_X(0x00667C15, 0, 1, 0, 0, 0, 0, 0);//Game pause toggle

	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TITLE_DEMO;

	reset_park_entrances();
	reset_saved_strings();
	reset_sprite_list();
	ride_init_all();
	window_guest_list_init_vars_a();
	sub_6BD3A4(); // RCT2_CALLPROC_EBPSAFE(0x006BD3A4);
	map_init();
	park_init();
	date_reset();
	climate_reset(CLIMATE_COOL_AND_WET);
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	window_new_ride_init_vars();
	window_guest_list_init_vars_b();
	window_staff_init_vars();
	map_update_tile_pointers(); //RCT2_CALLPROC_EBPSAFE(0x0068AFFD);
	reset_0x69EBE4();// RCT2_CALLPROC_EBPSAFE(0x0069EBE4);
	viewport_init_all();
	news_item_init_queue();
	title_create_windows();
	title_init_showcase();
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, uint16) = 0;
}

/**
 * Creates the windows shown on the title screen; New game, load game,
 * tutorial, toolbox and exit.
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
static void title_create_windows()
{
	// RCT2_CALLPROC_EBPSAFE(0x0066B3E8);

	window_main_open();
	window_title_menu_open();
	window_title_exit_open();
	window_title_logo_open();
	RCT2_CALLPROC_EBPSAFE(0x0066B905);
}

/**
 *
 *  rct2: 0x00678680
 */
static void title_init_showcase()
{
	_currentScript = _magicMountainScript;
	if (gRandomShowcase)
		_currentScript = generate_random_script();
	_scriptWaitCounter = 0;
	title_update_showcase();
}

/**
 *
 *  rct2: 0x00678761
 */
static void title_update_showcase()
{
	rct_window* w;
	uint8 script_opcode, script_operand;
	short x, y, z;
	int i;

	if (_scriptWaitCounter <= 0) {
		do {
			script_opcode = *_currentScript++;
			switch (script_opcode) {
			case TITLE_SCRIPT_WAIT:
				_scriptWaitCounter = (*_currentScript++) * 32;
				break;
			case TITLE_SCRIPT_LOAD:
				scenario_load(get_file_path(PATH_ID_SIXFLAGS_MAGICMOUNTAIN));

				w = window_get_main();
				w->viewport_target_sprite = -1;
				w->saved_view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16);
				w->saved_view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16);

				{
					char _cl = (RCT2_GLOBAL(0x0138869E, sint16) & 0xFF) - w->viewport->zoom;
					w->viewport->zoom = RCT2_GLOBAL(0x0138869E, sint16) & 0xFF;
					*((char*)(&RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, sint32))) = RCT2_GLOBAL(0x0138869E, sint16) >> 8;
					if (_cl != 0) {
						if (_cl < 0) {
							_cl = -_cl;
							w->viewport->view_width >>= _cl;
							w->viewport->view_height >>= _cl;
						} else {
							w->viewport->view_width <<= _cl;
							w->viewport->view_height <<= _cl;
						}
					}
					w->saved_view_x -= w->viewport->view_width >> 1;
					w->saved_view_y -= w->viewport->view_height >> 1;
				}

				window_invalidate(w);
				sub_0x0069E9A7();// RCT2_CALLPROC_EBPSAFE(0x0069E9A7);
				window_new_ride_init_vars();
				RCT2_CALLPROC_EBPSAFE(0x00684AC3);
				RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
				news_item_init_queue();
				gfx_invalidate_screen();
				RCT2_GLOBAL(0x009DEA66, sint16) = 0;
				RCT2_GLOBAL(0x009DEA5C, sint16) = 0x0D6D8;
				break;
			case TITLE_SCRIPT_LOCATION:
				x = (*_currentScript++) * 32 + 16;
				y = (*_currentScript++) * 32 + 16;
				z = map_element_height(x, y);

				// Update viewport
				w = window_get_main();
				if (w != NULL) {
					window_scroll_to_location(w, x, y, z);
					w->flags &= ~0x08;
					viewport_update_position(w);
				}
				break;
			case TITLE_SCRIPT_ROTATE:
				script_operand = (*_currentScript++);
				w = window_get_main();
				if (w != NULL)
					for (i = 0; i < script_operand; i++)
						window_rotate_camera(w);
				break;
			case TITLE_SCRIPT_RESTART:
				_currentScript = _magicMountainScript;
				if (gRandomShowcase) {
					if (_currentScript != NULL)
						free((uint8*)_currentScript);
					_currentScript = generate_random_script();
				}
				break;
			}
		} while (_scriptWaitCounter == 0);
	}

	_scriptWaitCounter--;
}

static void DrawOpenRCT2(int x, int y)
{
	char buffer[256];
	rct_drawpixelinfo *dpi = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	
	// Draw background
	gfx_fill_rect_inset(dpi, x, y, x + 128, y + 20, 0x80 | 12, 0x8);

	// Format text (name and version)
	sprintf(buffer, "%c%c%c%s, v%s", FORMAT_MEDIUMFONT, FORMAT_OUTLINE, FORMAT_WHITE, OPENRCT2_NAME, OPENRCT2_VERSION);

	// Draw Text
	gfx_draw_string(dpi, buffer, 0, x + 5, y + 5);
}


void game_handle_input();
void title_update()
{
	int tmp;

	if (RCT2_GLOBAL(0x009DEA6E, uint8) == 0) {
		title_update_showcase();
		game_logic_update();
		title_play_music();
	}

	RCT2_GLOBAL(0x009DE518, uint32) &= ~0x80;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x8000;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x02;
	tmp = RCT2_GLOBAL(0x009AC861, uint16) & 0x01;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x01;
	if (!tmp)
		RCT2_GLOBAL(0x009AC861, uint16) |= 0x02;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x08;
	tmp = RCT2_GLOBAL(0x009AC861, uint16) & 0x04;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x04;
	if (!tmp)
		RCT2_GLOBAL(0x009AC861, uint16) |= 0x04;

	RCT2_CALLPROC_EBPSAFE(0x006EE77A);

	window_update_all();
	DrawOpenRCT2(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 20);

	RCT2_GLOBAL(0x01388698, uint16)++;

	// Input
	// RCT2_CALLPROC_X(0x00667919, 1, 0, 0, 0, 0, 0, 0); // read_input(1)
	// RCT2_CALLPROC_EBPSAFE(0x006EA627); // window_manager_handle_input();
	game_handle_input();

	update_water_animation();
	update_rain_animation();

	if (RCT2_GLOBAL(0x009AAC73, uint8) != 255) {
		RCT2_GLOBAL(0x009AAC73, uint8)++;
		if (RCT2_GLOBAL(0x009AAC73, uint8) == 255)
			config_save();
	}
}

/**
 *
 *  rct2: 0x00678761
 */
static void title_play_music()
{
	// RCT2_CALLPROC_EBPSAFE(0x006BD0F8); // play title screen music

	if (!(RCT2_GLOBAL(0x009AF284, uint32) & 1) || !(RCT2_GLOBAL(0x009AF59D, uint8) & 1)) {
		if (RCT2_GLOBAL(0x009AF600, uint8) != 0)
			RCT2_CALLPROC_EBPSAFE(0x006BD0BD); // stop music
		return;
	}

	if (RCT2_GLOBAL(0x009AF600, uint8) != 0)
		return;

	// Play old title music
	char musicPath[_MAX_PATH];
	strcpy(musicPath, get_file_path(PATH_ID_CSS17));
	if (gOldMusic) {
		strcpy(musicPath, RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH, char));
		strcat(musicPath, "\\data\\css50.dat");
	}

	if (RCT2_CALLFUNC_3(0x0040194E, int, int, char*, int, 3, musicPath, 0)) // play music
		RCT2_CALLPROC_5(0x00401999, int, int, int, int, int, 3, 1, 0, 0, 0);

	RCT2_GLOBAL(0x009AF600, uint8) = 1;
}

static uint8 *generate_random_script()
{
	int i, j;
	const int views = 16;

	srand((unsigned int)time(NULL));

	uint8 *script = malloc(views * 8 + 2);
	i = 0;
	script[i++] = TITLE_SCRIPT_LOAD;
	for (j = 0; j < views; j++) {
		script[i++] = TITLE_SCRIPT_LOCATION;
		script[i++] = 64 + (rand() % 128);
		script[i++] = 64 + (rand() % 128);

		int rotationCount = rand() % 4;
		if (rotationCount > 0) {
			script[i++] = TITLE_SCRIPT_ROTATE;
			script[i++] = rotationCount;
		}

		script[i++] = TITLE_SCRIPT_WAIT;
		script[i++] = 8 + (rand() % 6);
	}
	script[i] = TITLE_SCRIPT_RESTART;

	return script;
}
