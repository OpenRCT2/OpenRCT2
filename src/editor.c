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
#include "platform/platform.h"
#include "rct1.h"
#include "ride/ride.h"
#include "scenario.h"
#include "util/sawyercoding.h"
#include "util/util.h"
#include "world/banner.h"
#include "world/climate.h"
#include "world/map.h"
#include "world/park.h"
#include "world/sprite.h"

static void set_all_land_owned();
static int editor_load_landscape_from_sv4(const char *path);
static int editor_load_landscape_from_sc4(const char *path);
static int editor_read_sc4(char *src, int length);
static int editor_read_sv4(char *src, int length);
static int editor_read_s4(rct1_s4 *data);
static int editor_read_s6(const char *path);

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
	map_init(150);
	banner_init();
	reset_park_entrances();
	user_string_clear_all();
	reset_sprite_list();
	ride_init_all();
	window_guest_list_init_vars_a();
	sub_6BD3A4();
	park_init();
	finance_init();
	date_reset();
	window_guest_list_init_vars_b();
	window_staff_list_init_vars();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_SCENARIO_EDITOR;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
	window_new_ride_init_vars();
	RCT2_GLOBAL(0x0141F571, uint8) = 4;
	viewport_init_all();
	news_item_init_queue();
	window_editor_main_open();
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	load_palette();
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, sint16) = 0;

	strcpy((char*)RCT2_ADDRESS_SCENARIO_NAME, language_get_string(2749));
}

/**
 *
 *  rct2: 0x0067505F
 */
static int show_convert_saved_game_to_scenario_dialog(char *resultPath)
{
	int result;
	char title[256];
	char filename[MAX_PATH];
	char filterName[256];

	format_string(title, STR_CONVERT_SAVED_GAME_TO_SCENARIO_1038, NULL);
	strcpy(filename, RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH, char));
	format_string(filterName, STR_RCT2_SAVED_GAME, NULL);

	pause_sounds();
	result = platform_open_common_file_dialog(1, title, filename, "*.SV6", filterName);
	unpause_sounds();

	if (result)
		strcpy(resultPath, filename);
	return result;
}

/**
 *
 *  rct2: 0x00672781
 */
void editor_convert_save_to_scenario()
{
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;
	char savedGamePath[MAX_PATH];
	rct_window *w;
	rct_viewport *viewport;

	tool_cancel();
	if (!show_convert_saved_game_to_scenario_dialog(savedGamePath))
		return;

	path_set_extension(savedGamePath, ".SV6");

	// Load the saved game
	if (!game_load_save(savedGamePath))
		return;

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_NO_MONEY_SCENARIO;
	else
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) &= ~PARK_FLAGS_NO_MONEY_SCENARIO;
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_NO_MONEY;

	strcpy(s6Info->name, (const char*)RCT2_ADDRESS_SCENARIO_NAME);
	strcpy(s6Info->details, (const char*)RCT2_ADDRESS_SCENARIO_DETAILS);
	s6Info->objective_type = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8);
	s6Info->objective_arg_1 = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8);
	s6Info->objective_arg_2 = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, sint32);
	s6Info->objective_arg_3 = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, sint16);
	climate_reset(RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, uint8));

	rct_stex_entry* stex = g_stexEntries[0];
	if ((int)stex != 0xFFFFFFFF) {
		object_unload(0, &object_entry_groups[OBJECT_TYPE_SCENARIO_TEXT].entries[0]);
		//RCT2_CALLPROC_EBPSAFE(0x006A9FC0);
		reset_loaded_objects();

		format_string(s6Info->details, STR_NO_DETAILS_YET, NULL);
		s6Info->name[0] = 0;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_SCENARIO_EDITOR;
	s6Info->var_000 = 4;
	s6Info->category = SCENARIO_CATEGORY_BUILDYOUROWN;
	viewport_init_all();
	news_item_init_queue();
	window_editor_main_open();

	// Initialise main view
	w = window_get_main();
	viewport = w->viewport;

	w->viewport_target_sprite = -1;
	w->saved_view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16);
	w->saved_view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16);

	viewport->zoom = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) & 0xFF;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) >> 8;

	int cx = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, sint16) - viewport->zoom;
	if (cx != 0) {
		if (cx >= 0) {
			viewport->view_width <<= cx;
			viewport->view_height <<= cx;
		} else {
			cx = -cx;
			viewport->view_width >>= cx;
			viewport->view_height >>= cx;
		}
	}
	w->saved_view_x -= viewport->view_width >> 1;
	w->saved_view_y -= viewport->view_height >> 1;

	window_invalidate(w);
	sub_69E9A7();
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	window_new_ride_init_vars();
	RCT2_GLOBAL(0x009DEB7C, uint16) = 0;
	load_palette();
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, uint16) = 0;
}

/**
 *
 *  rct2: 0x00672957
 */
void trackdesigner_load()
{
	rct_window *mainWindow;

	object_unload_all();
	map_init(150);
	set_all_land_owned();
	banner_init();
	reset_park_entrances();
	user_string_clear_all();
	reset_sprite_list(); 
	ride_init_all();
	window_guest_list_init_vars_a();
	sub_6BD3A4();
	park_init();
	finance_init();
	date_reset();
	window_guest_list_init_vars_b();
	window_staff_list_init_vars();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TRACK_DESIGNER;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	window_new_ride_init_vars();
	viewport_init_all();
	news_item_init_queue();
	window_editor_main_open();
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	load_palette();
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, sint16) = 0;
}

/**
 *
 *  rct2: 0x006729FD
 */
void trackmanager_load()
{
	rct_window *mainWindow;

	object_unload_all();
	map_init(150);
	set_all_land_owned();
	banner_init();
	reset_park_entrances();
	user_string_clear_all();
	reset_sprite_list();
	ride_init_all();
	window_guest_list_init_vars_a();
	sub_6BD3A4();
	park_init();
	finance_init();
	date_reset();
	window_guest_list_init_vars_b();
	window_staff_list_init_vars();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TRACK_MANAGER;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	window_new_ride_init_vars();
	viewport_init_all();
	news_item_init_queue();
	window_editor_main_open();
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	load_palette();
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, sint16) = 0;
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

/**
 *
 *  rct2: 0x006758C0
 */
void editor_load_landscape(const char *path)
{
	window_close_construction_windows();
	
	char *extension = strrchr(path, '.');
	if (extension != NULL) {
		if (_stricmp(extension, ".sv4") == 0) {
			editor_load_landscape_from_sv4(path);
			return;
		} else if (_stricmp(extension, ".sc4") == 0) {
			editor_load_landscape_from_sc4(path);
			return;
		}
	}

	// Load SC6 / SV6
	editor_read_s6(path);
}

/**
 *
 *  rct2: 0x00666DFD
 */
static void sub_666DFD()
{
	int x, y;
	rct_map_element *mapElement;

	x = RCT2_GLOBAL(0x013573EA, uint16);
	y = RCT2_GLOBAL(0x013573EC, uint16);
	if (x == 0x8000)
		return;

	x /= 32;
	y /= 32;
	mapElement = map_get_first_element_at(x, y);
	do {
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE) {
			if (mapElement->properties.entrance.type == ENTRANCE_TYPE_PARK_ENTRANCE) {
				mapElement->properties.entrance.path_type = 0;
				break;
			}
		}
	} while (!map_element_is_last_for_tile(mapElement++));
}

/**
 *
 *  rct2: 0x0069F06A
 */
static void sub_69F06A()
{
	RCT2_CALLPROC_EBPSAFE(0x0069F06A); return;

	// TODO, bug with the following code
	RCT2_GLOBAL(0x013CE770, uint32) |= (1 << 0) | (1 << 1) | (1 << 14) | (1 << 2) | (1 << 3);
	if (!(RCT2_GLOBAL(0x013CE770, uint32) & (1 << 4))) {
		RCT2_GLOBAL(0x013CE770, uint32) |= (1 << 4);
		banner_init(); // 6B9CB0
	}
	if (!(RCT2_GLOBAL(0x013CE770, uint32) & (1 << 6))) {
		RCT2_GLOBAL(0x013CE770, uint32) |= (1 << 6);
		RCT2_CALLPROC_EBPSAFE(0x0069E891);
	}
	RCT2_GLOBAL(0x013CE770, uint32) |= (1 << 7);
	if (!(RCT2_GLOBAL(0x013CE770, uint32) & (1 << 8))) {
		RCT2_GLOBAL(0x013CE770, uint32) |= (1 << 8);
		sub_666DFD();
	}
	if (!(RCT2_GLOBAL(0x013CE770, uint32) & (1 << 9))) {
		RCT2_GLOBAL(0x013CE770, uint32) |= (1 << 9);
		RCT2_CALLPROC_EBPSAFE(0x0069E89B);
	}
	if (!(RCT2_GLOBAL(0x013CE770, uint32) & (1 << 13))) {
		RCT2_GLOBAL(0x013CE770, uint32) |= (1 << 13);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) = 127 * 32;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_MAXIMUM_X_Y, uint16) = 4350;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) = 128;
		RCT2_GLOBAL(0x01358836, uint16) = 4095;
	}
	if (!(RCT2_GLOBAL(0x013CE770, uint32) & (1 << 15))) {
		RCT2_GLOBAL(0x013CE770, uint32) |= (1 << 15);
		RCT2_GLOBAL(0x01358838, uint32) = 0;
	}
	RCT2_GLOBAL(0x013CE770, uint32) |= (1 << 16) | (1 << 18) | (1 << 19);
}

/**
 *
 *  rct2: 0x006A2B62
 */
static void sub_6A2B62()
{
	int i;
	rct_sprite *sprite;
	rct_ride *ride;
	map_element_iterator it;

	RCT2_CALLPROC_EBPSAFE(0x0069F007);

	// Free sprite user strings
	for (i = 0; i < MAX_SPRITES; i++) {
		sprite = &g_sprite_list[i];
		if (sprite->unknown.sprite_identifier != 255)
			user_string_free(sprite->unknown.name_string_idx);
	}

	reset_sprite_list();

	// Free ride user strings
	FOR_ALL_RIDES(i, ride)
		user_string_free(ride->name);

	ride_init_all();
	RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16) = 0;
	RCT2_GLOBAL(0x01357BC8, uint16) = 0;
	RCT2_GLOBAL(0x013573FE, uint8) = 0;
	RCT2_CALLPROC_EBPSAFE(0x0069F44B);
	sub_69F06A();
	RCT2_CALLPROC_EBPSAFE(0x0069F143);
	RCT2_CALLPROC_EBPSAFE(0x0069F2D0);
	RCT2_CALLPROC_EBPSAFE(0x0069F3AB);

	// Fix paths and remove all ride track / entrance / exit
	map_element_iterator_begin(&it);
	do {
		switch (map_element_get_type(it.element)) {
		case MAP_ELEMENT_TYPE_PATH:
			if (it.element->type & 1) {
				it.element->properties.path.type &= 0xF7;
				it.element->properties.path.addition_status = 255;
			}
			break;

		case MAP_ELEMENT_TYPE_TRACK:
			RCT2_CALLPROC_EBPSAFE(0x006A7594);
			sub_6A6AA7(it.x * 32, it.y * 32, it.element);
			map_element_remove(it.element);
			map_element_iterator_restart_for_tile(&it);
			break;

		case MAP_ELEMENT_TYPE_ENTRANCE:
			if (it.element->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE) {
				RCT2_CALLPROC_EBPSAFE(0x006A7594);
				sub_6A6AA7(it.x * 32, it.y * 32, it.element);
				map_element_remove(it.element);
				map_element_iterator_restart_for_tile(&it);
			}
			break;
		}
	} while (map_element_iterator_next(&it));

	object_unload_all();

	RCT2_CALLPROC_EBPSAFE(0x0069F53D);
	reset_loaded_objects();
	RCT2_CALLPROC_EBPSAFE(0x006A2730);
	RCT2_CALLPROC_EBPSAFE(0x006A2956);
	RCT2_CALLPROC_EBPSAFE(0x006A29B9);
	RCT2_CALLPROC_EBPSAFE(0x006A2A68);
	RCT2_CALLPROC_EBPSAFE(0x0069F509);
	RCT2_CALLPROC_EBPSAFE(0x00685675);
	RCT2_CALLPROC_EBPSAFE(0x0068585B);

	climate_reset(RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, uint8));
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_SCENARIO_EDITOR;
	viewport_init_all();
	news_item_init_queue();
	window_editor_main_open();

	rct_s6_header *s6Header = (rct_s6_header*)0x009E34E4;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	s6Info->var_000 = 1;
	s6Info->category = 4;
	format_string(s6Info->details, STR_NO_DETAILS_YET, NULL);
	s6Info->name[0] = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint8) & PARK_FLAGS_NO_MONEY) {
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint8) |= PARK_FLAGS_NO_MONEY_SCENARIO;
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint8) &= PARK_FLAGS_NO_MONEY_SCENARIO;
	}
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16) == MONEY_FREE) {
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint8) |= PARK_FLAGS_PARK_FREE_ENTRY;
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint8) &= ~PARK_FLAGS_PARK_FREE_ENTRY;
	}
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint8) &= ~PARK_FLAGS_18;
	RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_CASH, money16) = clamp(
		MONEY(10,00),
		RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_CASH, money16),
		MONEY(100,00)
	);
	RCT2_GLOBAL(RCT2_ADDRESS_INITIAL_CASH, money32) = min(
		MONEY(10000,00),
		RCT2_GLOBAL(RCT2_ADDRESS_INITIAL_CASH, money32)
	);
	RCT2_CALLPROC_EBPSAFE(0x0069E89B);
	sub_69E869();//RCT2_CALLPROC_EBPSAFE(0x0069E869);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32) = clamp(
		MONEY(0,00),
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32),
		MONEY(5000000,00)
	);

	RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, money32) = clamp(
		MONEY(0,00),
		RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, money32),
		MONEY(5000000,00)
	);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_INTEREST_RATE, uint8) = clamp(
		5,
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_INTEREST_RATE, uint8),
		80
	);

	if (
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) == OBJECTIVE_NONE ||
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) == OBJECTIVE_HAVE_FUN ||
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) == OBJECTIVE_BUILD_THE_BEST
	) {
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) = OBJECTIVE_GUESTS_BY;
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8) = 4;
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) = 1000;
	}

	RCT2_GLOBAL(0x01358774, uint16) = 0;

	// Initialise main view
	rct_window *w = window_get_main();
	rct_viewport *viewport = w->viewport;

	w->viewport_target_sprite = -1;
	w->saved_view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16);
	w->saved_view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16);

	viewport->zoom = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) & 0xFF;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) >> 8;

	int cx = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, sint16) - viewport->zoom;
	if (cx != 0) {
		if (cx >= 0) {
			viewport->view_width <<= cx;
			viewport->view_height <<= cx;
		} else {
			cx = -cx;
			viewport->view_width >>= cx;
			viewport->view_height >>= cx;
		}
	}
	w->saved_view_x -= viewport->view_width >> 1;
	w->saved_view_y -= viewport->view_height >> 1;

	window_invalidate(w);
	sub_69E9A7();
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	window_new_ride_init_vars();
	RCT2_GLOBAL(0x009DEB7C, uint16) = 0;
	load_palette();
	gfx_invalidate_screen();

	RCT2_GLOBAL(0x009DEA66, uint16) = 0;
}

/**
 *
 *  rct2: 0x006A2B02
 */
static int editor_load_landscape_from_sv4(const char *path)
{
	FILE *fp;
	long fpLength;
	char *fpBuffer;

	// Open file
	fp = fopen(path, "rb");
	if (fp == NULL) {
		RCT2_GLOBAL(0x009AC31B, uint8) = 255;
		RCT2_GLOBAL(0x009AC31C, uint16) = 3011;
		return 0;
	}

	// Read whole file into a buffer
	fpLength = fsize(fp);
	fpBuffer = malloc(fpLength);
	fread(fpBuffer, fpLength, 1, fp);
	fclose(fp);

	editor_read_sv4(fpBuffer, fpLength);
	free(fpBuffer);

	sub_6A2B62();
	return 1;
}

static int editor_load_landscape_from_sc4(const char *path)
{
	FILE *fp;
	long fpLength;
	char *fpBuffer;

	// Open file
	fp = fopen(path, "rb");
	if (fp == NULL) {
		RCT2_GLOBAL(0x009AC31B, uint8) = 255;
		RCT2_GLOBAL(0x009AC31C, uint16) = 3011;
		return 0;
	}

	// Get length
	fseek(fp, 0, SEEK_END);
	fpLength = ftell(fp);
	rewind(fp);

	// Read whole file into a buffer
	fpBuffer = malloc(fpLength);
	fread(fpBuffer, fpLength, 1, fp);
	fclose(fp);

	editor_read_sc4(fpBuffer, fpLength);
	free(fpBuffer);

	sub_6A2B62();
	return 1;
}

static int editor_read_sc4(char *src, int length)
{
	int decodedLength;
	rct1_s4 *data;

	int fileType = sawyercoding_detect_file_type(src, length);

	data = malloc(sizeof(rct1_s4));
	decodedLength = (fileType & FILE_VERSION_MASK) == FILE_VERSION_RCT1 ?
		sawyercoding_decode_sv4(src, (char*)data, length) :
		sawyercoding_decode_sc4(src, (char*)data, length);
	if (decodedLength != sizeof(rct1_s4)) {
		free(data);
		return 0;
	}

	editor_read_s4(data);
	free(data);
	return 1;
}

static int editor_read_sv4(char *src, int length)
{
	int decodedLength;
	rct1_s4 *data;

	data = malloc(sizeof(rct1_s4));
	decodedLength = sawyercoding_decode_sv4(src, (char*)data, length);
	if (decodedLength != sizeof(rct1_s4)) {
		free(data);
		return 0;
	}

	editor_read_s4(data);
	free(data);
	return 1;
}

static void read(void *dst, void *src, int length)
{
	memcpy(dst, src, length);
}

/**
 *
 *  rct2: 0x0069EEA0
 */
static int editor_read_s4(rct1_s4 *src)
{
	int i;
	rct_banner *banner;

	read((void*)RCT2_ADDRESS_CURRENT_MONTH_YEAR, &src->month, 16);
	memset((void*)RCT2_ADDRESS_MAP_ELEMENTS, 0, 0x30000 * sizeof(rct_map_element));
	read((void*)RCT2_ADDRESS_MAP_ELEMENTS, src->map_elements, sizeof(src->map_elements));
	read((void*)0x010E63B8, &src->unk_counter, 4 + sizeof(src->sprites));

	for (i = 0; i < MAX_BANNERS; i++)
		gBanners[i].type = 255;

	read((void*)0x013573BC, &src->next_sprite_index, 12424);

	for (i = 0; i < MAX_BANNERS; i++) {
		banner = &gBanners[i];
		if (banner->type != 255 && banner->string_idx != 3458)
			banner->string_idx = 778;
	}

	read((void*)0x0135A8F4, &src->string_table, 0x2F51C);
	memset((void*)0x013CA672, 0, 204);
	read((void*)0x0138B580, &src->animated_objects, 0x258F2);
	read((void*)0x013C6A72, &src->patrol_areas, sizeof(src->patrol_areas));

	char *esi = (char*)0x13C6A72;
	char *edi = (char*)0x13B0E72;
	int ebx, edx = 116;
	do {
		ebx = 32;
		do {
			memcpy(edi, esi, 4); esi += 4; edi += 4;
			memset(edi, 0, 4); edi += 4;
		} while (--ebx > 0);
		memset(edi, 0, 64); edi += 64;
	} while (--edx > 0);
	edi += 0xA800;
	
	edx = 4;
	do {
		ebx = 32;
		do {
			memcpy(edi, esi, 4); esi += 4; edi += 4;
			memset(edi, 0, 4); edi += 4;
		} while (--ebx);
		memset(edi, 0, 64); edi += 64;
	} while (--edx);

	read((void*)0x013CA672, &src->unk_1F42AA, 116);
	read((void*)0x013CA73A, &src->unk_1F431E, 4);
	read((void*)0x013CA73E, &src->unk_1F4322, 0x41EA);
	return 1;
}

/**
 *
 *  rct2: 0x006758FE
 */
static int editor_read_s6(const char *path)
{
	int i, j;
	FILE *file;
	rct_s6_header *s6Header = (rct_s6_header*)0x009E34E4;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	// strcpy((char *)0x0141EF68, path);
	// RCT2_CALLPROC_EBPSAFE(0x006758FE);

	log_verbose("loading landscape, %s", path);

	file = fopen(path, "rb");
	if (file != NULL) {
		if (!sawyercoding_validate_checksum(file)) {
			fclose(file);
			RCT2_GLOBAL(0x009AC31B, uint8) = 255;
			RCT2_GLOBAL(0x009AC31C, uint16) = STR_FILE_CONTAINS_INVALID_DATA;

			log_error("failed to load scenario, invalid checksum");
			return 0;
		}

		// Read first chunk
		sawyercoding_read_chunk(file, (uint8*)s6Header);
		if (s6Header->type == S6_TYPE_SCENARIO) {
			// Read second chunk
			sawyercoding_read_chunk(file, (uint8*)s6Info);

			if (s6Info->var_000 == 255)
				s6Info->var_000 = 1;
		} else {
			s6Info->var_000 = 1;
			s6Info->category = SCENARIO_CATEGORY_BUILDYOUROWN;
			format_string(s6Info->details, STR_NO_DETAILS_YET, NULL);
		}

		// Read packed objects
		if (s6Header->num_packed_objects > 0) {
			j = 0;
			for (i = 0; i < s6Header->num_packed_objects; i++)
				j += object_load_packed(file);
			if (j > 0)
				object_list_load();
		}

		uint8 load_success = object_read_and_load_entries(file);

		// Read flags (16 bytes). Loads:
		//	RCT2_ADDRESS_CURRENT_MONTH_YEAR
		//	RCT2_ADDRESS_CURRENT_MONTH_TICKS
		//	RCT2_ADDRESS_SCENARIO_TICKS
		sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_CURRENT_MONTH_YEAR);

		// Read map elements
		memset((void*)RCT2_ADDRESS_MAP_ELEMENTS, 0, MAX_MAP_ELEMENTS * sizeof(rct_map_element));
		sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_MAP_ELEMENTS);

		// Read game data, including sprites
		sawyercoding_read_chunk(file, (uint8*)0x010E63B8);

		if (s6Header->type == S6_TYPE_SCENARIO) {
			// Read number of guests in park and something else
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_GUESTS_IN_PARK);

			// Read ?
			sawyercoding_read_chunk(file, (uint8*)0x01357BC8);

			// Read park rating
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_CURRENT_PARK_RATING);

			// Read ?
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_ACTIVE_RESEARCH_TYPES);

			// Read ?
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_CURRENT_EXPENDITURE);

			// Read ?
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_CURRENT_PARK_VALUE);

			// Read more game data, including research items and rides
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_COMPLETED_COMPANY_VALUE);
		}

		fclose(file);
		if (!load_success){
			log_error("failed to load all entries.");
			set_load_objects_fail_reason();
			return 0;
		}
		// Check expansion pack
		// RCT2_CALLPROC_EBPSAFE(0x006757E6);

		reset_loaded_objects();//RCT2_CALLPROC_EBPSAFE(0x006A9FC0);
		map_update_tile_pointers();
		map_remove_all_rides();

		// 
		for (i = 0; i < MAX_BANNERS; i++)
			if (gBanners[i].type == 255)
				gBanners[i].flags &= ~BANNER_FLAG_2;

		// 
		rct_ride *ride;
		FOR_ALL_RIDES(i, ride)
			user_string_free(ride->name);

		ride_init_all();

		// 
		for (i = 0; i < MAX_SPRITES; i++) {
			rct_sprite *sprite = &g_sprite_list[i];
			user_string_free(sprite->unknown.name_string_idx);
		}

		reset_sprite_list();
		sub_6BD3A4();
		RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16) = 0;
		RCT2_GLOBAL(0x01357BC8, uint16) = 0;
		RCT2_GLOBAL(0x013573FE, uint16) = 0;
		if (s6Header->type != S6_TYPE_SCENARIO) {
			RCT2_CALLPROC_EBPSAFE(0x00685675);
			RCT2_CALLPROC_EBPSAFE(0x0068585B);

			if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)
				RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_NO_MONEY_SCENARIO;
			else
				RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) &= ~PARK_FLAGS_NO_MONEY_SCENARIO;
			RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_NO_MONEY;

			if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16) == 0)
				RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_PARK_FREE_ENTRY;
			else
				RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) &= ~PARK_FLAGS_PARK_FREE_ENTRY;

			RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) &= ~PARK_FLAGS_18;

			RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_CASH, money16) = clamp(
				MONEY(10,00),
				RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_CASH, money16),
				MONEY(100,00)
			);

			RCT2_GLOBAL(0x013573DC, uint32) = min(RCT2_GLOBAL(0x013573DC, uint32), 100000);
			RCT2_CALLPROC_EBPSAFE(0x0069E89B);
			RCT2_CALLPROC_EBPSAFE(0x0069E869);

			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32) = clamp(
				MONEY(0,00),
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32),
				MONEY(5000000,00)
			);

			RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, money32) = clamp(
				MONEY(0,00),
				RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, money32),
				MONEY(5000000,00)
			);

			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_INTEREST_RATE, uint8) = clamp(
				5,
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_INTEREST_RATE, uint8),
				80
			);
		}

		climate_reset(RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, uint8));

		rct_stex_entry* stex = g_stexEntries[0];
		if ((int)stex != 0xFFFFFFFF) {
			object_unload(0, &object_entry_groups[OBJECT_TYPE_SCENARIO_TEXT].entries[0]);
			reset_loaded_objects();//RCT2_CALLPROC_EBPSAFE(0x006A9FC0);

			format_string(s6Info->details, STR_NO_DETAILS_YET, NULL);
			s6Info->name[0] = 0;
		}

		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_SCENARIO_EDITOR;
		viewport_init_all();
		news_item_init_queue();
		window_editor_main_open();

		// Initialise main view
		rct_window *w = window_get_main();
		rct_viewport *viewport = w->viewport;

		w->viewport_target_sprite = -1;
		w->saved_view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16);
		w->saved_view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16);

		int zoom_difference = (RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, sint16) & 0xFF) - viewport->zoom;
		viewport->zoom = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) & 0xFF;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) >> 8;

		if (zoom_difference != 0) {
			if (zoom_difference >= 0) {
				viewport->view_width <<= zoom_difference;
				viewport->view_height <<= zoom_difference;
			} else {
				zoom_difference = -zoom_difference;
				viewport->view_width >>= zoom_difference;
				viewport->view_height >>= zoom_difference;
			}
		}
		w->saved_view_x -= viewport->view_width >> 1;
		w->saved_view_y -= viewport->view_height >> 1;

		window_invalidate(w);
		sub_69E9A7();
		RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
		window_new_ride_init_vars();
		RCT2_GLOBAL(0x009DEB7C, uint16) = 0;
		load_palette(); 
		gfx_invalidate_screen();

		return 1;
	}

	log_error("failed to find scenario file.");
	RCT2_GLOBAL(0x009AC31B, uint8) = 255;
	RCT2_GLOBAL(0x009AC31C, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
	return 0;
}

/**
 *
 *  rct2: 0x0067009A
 */
void editor_open_windows_for_current_step()
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_EDITOR))
		return;

	switch (g_editor_step) {
	case EDITOR_STEP_OBJECT_SELECTION:
		if (window_find_by_class(WC_EDITOR_OBJECT_SELECTION))
			return;

		if (window_find_by_class(49))
			return;

		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
			object_unload_all();
		}

		window_editor_object_selection_open();
		break;
	case EDITOR_STEP_INVENTIONS_LIST_SET_UP:
		if (window_find_by_class(WC_EDITOR_INVENTION_LIST))
			return;

		window_editor_inventions_list_open();
		break;
	case EDITOR_STEP_OPTIONS_SELECTION:
		if (window_find_by_class(WC_EDITOR_SCENARIO_OPTIONS))
			return;

		window_editor_scenario_options_open();
		break;
	case EDITOR_STEP_OBJECTIVE_SELECTION:
		if (window_find_by_class(WC_EDTIOR_OBJECTIVE_OPTIONS))
			return;

		window_editor_objective_options_open();
		break;
	}
}
