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
#include "date.h"
#include "editor.h"
#include "game.h"
#include "gfx.h"
#include "map.h"
#include "news_item.h"
#include "park.h"
#include "ride.h"
#include "window.h"
#include "viewport.h"

static void set_all_land_owned();

/**
 *
 *  rct2: 0x0066FFE1
 */
void editor_load()
{
	rct_window *mainWindow;

	pause_sounds();
	unpause_sounds();
	RCT2_CALLPROC_EBPSAFE(0x006A9CE8);
	map_init();
	RCT2_CALLPROC_EBPSAFE(0x006B9CB0);
	reset_park_entrances();
	reset_saved_strings();
	RCT2_CALLPROC_EBPSAFE(0x0069EB13); // sprites_init
	ride_init_all();
	RCT2_CALLPROC_EBPSAFE(0x0068F083); // window_guest_list_init_vars_a
	RCT2_CALLPROC_EBPSAFE(0x006BD3A4);
	park_init();
	finance_init();
	date_reset();
	RCT2_CALLPROC_EBPSAFE(0x0068F050); // window_guest_list_init_vars_b
	RCT2_CALLPROC_EBPSAFE(0x006BD39C);
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_SCENARIO_EDITOR;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
	RCT2_CALLPROC_EBPSAFE(0x006ACA58);
	RCT2_GLOBAL(0x0141F571, uint8) = 4;
	viewport_init_all();
	news_item_init_queue();
	RCT2_CALLPROC_EBPSAFE(0x0066EF38); // window_main_editor_create
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_3;
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

	RCT2_CALLPROC_EBPSAFE(0x006A9CE8);
	map_init();
	set_all_land_owned();
	RCT2_CALLPROC_EBPSAFE(0x006B9CB0);
	reset_park_entrances();
	reset_saved_strings();
	RCT2_CALLPROC_EBPSAFE(0x0069EB13); // reset_sprites
	ride_init_all();
	RCT2_CALLPROC_EBPSAFE(0x0068F083); // window_guest_list_init_vars_a
	RCT2_CALLPROC_EBPSAFE(0x006BD3A4);
	park_init();
	finance_init();
	date_reset();
	RCT2_CALLPROC_EBPSAFE(0x0068F050); // window_guest_list_init_vars_b
	RCT2_CALLPROC_EBPSAFE(0x006BD39C);
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TRACK_DESIGNER;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	RCT2_CALLPROC_EBPSAFE(0x006ACA58);
	viewport_init_all();
	news_item_init_queue();
	RCT2_CALLPROC_EBPSAFE(0x0066EF38); // window_main_editor_create
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_3;
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

	RCT2_CALLPROC_EBPSAFE(0x006A9CE8);
	map_init();
	set_all_land_owned();
	RCT2_CALLPROC_EBPSAFE(0x006B9CB0);
	reset_park_entrances();
	reset_saved_strings();
	RCT2_CALLPROC_EBPSAFE(0x0069EB13); // reset_sprites
	ride_init_all();
	RCT2_CALLPROC_EBPSAFE(0x0068F083); // window_guest_list_init_vars_a
	RCT2_CALLPROC_EBPSAFE(0x006BD3A4);
	park_init();
	finance_init();
	date_reset();
	RCT2_CALLPROC_EBPSAFE(0x0068F050); // window_guest_list_init_vars_b
	RCT2_CALLPROC_EBPSAFE(0x006BD39C);
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TRACK_MANAGER;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	RCT2_CALLPROC_EBPSAFE(0x006ACA58);
	viewport_init_all();
	news_item_init_queue();
	RCT2_CALLPROC_EBPSAFE(0x0066EF38); // window_main_editor_create
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_3;
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

	game_do_command(64, 1, 64, 2, 56, (mapSize - 2) * 32, (mapSize - 2) * 32);
}

/**
*
*  rct2: 0x00667104
*/
void reset_park_entrances() {
	RCT2_GLOBAL(0x013573D4, uint16) = 0;

	for (short i = 0; i < 4; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] = 0x8000;
	}

	RCT2_GLOBAL(0x013573F2, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x013573F8, uint16) = 0xFFFF;
}

/**
*
*  rct2: 0x006C4209
*/
void reset_saved_strings() {
	for (int i = 0; i < 1024; i++) {
		RCT2_ADDRESS(0x135A8F4, uint8)[i * 32] = 0;
	}
}

/**
*
*  rct2: 0x006BABB4
*/
void pause_sounds() {
	if (++RCT2_GLOBAL(0x009AF59C, uint8) == 1) {
		RCT2_CALLPROC_EBPSAFE(0x006BCAE5);
		RCT2_CALLPROC_EBPSAFE(0x006BABDF);
		RCT2_CALLPROC_EBPSAFE(0x006BCA9F);
		RCT2_CALLPROC_EBPSAFE(0x006BD07F);
	}
}

/**
*
*  rct2: 0x006BABD8
*/
void unpause_sounds() {
	RCT2_GLOBAL(0x009AF59C, uint8)--;
}

/**
*
*  rct2: 0x0069DEFB
*/
void finance_init() {

	for (short i = 0; i < 56; i++) {
		RCT2_ADDRESS(0x01357848, uint32)[i] = 0;
	}

	RCT2_GLOBAL(0x0135832C, uint32) = 0;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PROFIT, uint32) = 0;

	RCT2_GLOBAL(0x01358334, uint32) = 0;
	RCT2_GLOBAL(0x01358338, uint16) = 0;

	RCT2_GLOBAL(0x013573DC, uint32) = 100000;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32) = ENCRYPT_MONEY(100000);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, sint32) = 100000;
	RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, uint32) = 200000;

	RCT2_GLOBAL(0x013587D0, uint32) = 0;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_INTEREST_RATE, uint8) = 10;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, sint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_COMPANY_VALUE, sint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, sint32) = 0x80000000;
	RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_ADMISSIONS, uint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INCOME_FROM_ADMISSIONS, uint32) = 0;

	RCT2_GLOBAL(0x013587D8, uint16) = 0x3F;

	RCT2_CALLPROC_EBPSAFE(0x0069E869);
}