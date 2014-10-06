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
#include "drawing/drawing.h"
#include "editor.h"
#include "game.h"
#include "interface/window.h"
#include "interface/viewport.h"
#include "localisation/date.h"
#include "localisation/localisation.h"
#include "management/finance.h"
#include "management/news_item.h"
#include "object.h"
#include "peep/staff.h"
#include "ride/ride.h"
#include "world/map.h"
#include "world/park.h"
#include "world/sprite.h"

static void set_all_land_owned();

/*Syntax error blah blah blat *&2)*/

/**
 *
 *  rct2: 0x0066FFE1
 */
void editor_load()
{
	rct_window *mainWindow;

	pause_sounds();
	unpause_sounds();
	object_unload_all();
	map_init();
	RCT2_CALLPROC_EBPSAFE(0x006B9CB0);
	reset_park_entrances();
	reset_saved_strings();
	reset_sprite_list();
	ride_init_all();
	window_guest_list_init_vars_a();
	sub_6BD3A4();
	park_init();
	finance_init();
	date_reset();
	window_guest_list_init_vars_b();
	window_staff_init_vars();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_SCENARIO_EDITOR;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
	window_new_ride_init_vars();
	RCT2_GLOBAL(0x0141F571, uint8) = 4;
	viewport_init_all();
	news_item_init_queue();
	RCT2_CALLPROC_EBPSAFE(0x0066EF38); // window_main_editor_create
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	RCT2_CALLPROC_EBPSAFE(0x006837E3);
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, sint16) = 0;
	rct2_endupdate();
}

/**
 *
 *  rct2: 0x00672781
 */
void editor_convert_save_to_scenario()
{
	RCT2_CALLPROC_EBPSAFE(0x00672781);
}

/**
 *
 *  rct2: 0x00672957
 */
void trackdesigner_load()
{
	rct_window *mainWindow;

	object_unload_all();
	map_init();
	set_all_land_owned();
	RCT2_CALLPROC_EBPSAFE(0x006B9CB0);
	reset_park_entrances();
	reset_saved_strings();
	reset_sprite_list(); 
	ride_init_all();
	window_guest_list_init_vars_a();
	sub_6BD3A4();
	park_init();
	finance_init();
	date_reset();
	window_guest_list_init_vars_b();
	window_staff_init_vars();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TRACK_DESIGNER;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	window_new_ride_init_vars();
	viewport_init_all();
	news_item_init_queue();
	RCT2_CALLPROC_EBPSAFE(0x0066EF38); // window_main_editor_create
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	RCT2_CALLPROC_EBPSAFE(0x006837E3);
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, sint16) = 0;
	rct2_endupdate();
}

/**
 *
 *  rct2: 0x006729FD
 */
void trackmanager_load()
{
	rct_window *mainWindow;

	object_unload_all();
	map_init();
	set_all_land_owned();
	RCT2_CALLPROC_EBPSAFE(0x006B9CB0);
	reset_park_entrances();
	reset_saved_strings();
	reset_sprite_list();
	ride_init_all();
	window_guest_list_init_vars_a();
	sub_6BD3A4();
	park_init();
	finance_init();
	date_reset();
	window_guest_list_init_vars_b();
	window_staff_init_vars();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TRACK_MANAGER;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	window_new_ride_init_vars();
	viewport_init_all();
	news_item_init_queue();
	RCT2_CALLPROC_EBPSAFE(0x0066EF38); // window_main_editor_create
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	RCT2_CALLPROC_EBPSAFE(0x006837E3);
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, sint16) = 0;
	rct2_endupdate();
}

/**
 *
 *  rct2: 0x0068ABEC
 */
static void set_all_land_owned()
{
	int mapSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, sint16);

	game_do_command(64, 1, 64, 2, GAME_COMMAND_56, (mapSize - 2) * 32, (mapSize - 2) * 32);
}

/**
*
*  rct2: 0x006BD3A4
*/
void sub_6BD3A4() {
	for (short i = 0; i < 200; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[i] = STAFF_MODE_NONE;
	}
	for (short i = 200; i < 204; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[i] = STAFF_MODE_WALK;
	}
	//RCT2_CALLPROC_EBPSAFE(0x006C0C3F);
	sub_6C0C3F();
}
