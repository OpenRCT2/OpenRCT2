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
#include "world/scenery.h"
#include "world/sprite.h"

static void set_all_land_owned();
static int editor_load_landscape_from_sv4(const char *path);
static int editor_load_landscape_from_sc4(const char *path);
static int editor_read_sc4(char *src, int length);
static int editor_read_sv4(char *src, int length);
static int editor_read_s4(rct1_s4 *data);
static int editor_read_s6(const char *path);

static void editor_load_rct1_default_objects();
static void editor_fix_rct1_scenery();
static void editor_rct1_reset_research();

typedef struct {
	const rct_object_entry* entries;
	int count;
} RCT1DefaultObjectsGroup;

static const RCT1DefaultObjectsGroup RCT1DefaultObjects[10];

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
	scenery_set_default_placement_configuration();
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
void sub_6BD3A4()
{
	for (int i = 0; i < 200; i++)
		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[i] = STAFF_MODE_NONE;

	for (int i = 200; i < 204; i++)
		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[i] = STAFF_MODE_WALK;

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
	editor_load_rct1_default_objects();
	reset_loaded_objects();
	RCT2_CALLPROC_EBPSAFE(0x006A2730);
	editor_fix_rct1_scenery();
	RCT2_CALLPROC_EBPSAFE(0x006A29B9);
	RCT2_CALLPROC_EBPSAFE(0x006A2A68);
	editor_rct1_reset_research();
	research_populate_list_random();
	research_remove_non_separate_vehicle_types();

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
	sub_69E869();

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
	scenery_set_default_placement_configuration();
	window_new_ride_init_vars();
	RCT2_GLOBAL(0x009DEB7C, uint16) = 0;
	load_palette();
	gfx_invalidate_screen();

	RCT2_GLOBAL(0x009DEA66, uint16) = 0;
}

/**
 *
 *  rct2: 0x0069F53D
 */
static void editor_load_rct1_default_objects()
{
	for (int i = 0; i < 9; i++) {
		rct_object_entry *entries = (rct_object_entry*)RCT1DefaultObjects[i].entries;
		for (int j = 0; j < RCT1DefaultObjects[i].count; j++) {
			if (!object_load(j, &entries[j], NULL)) {
				error_string_quit(0x99990000 + (i * 0x100) + j, -1);
				return;
			}
		}
	}

	// Water is a special case
	rct_object_entry *waterEntries = (rct_object_entry*)RCT1DefaultObjects[9].entries;
	rct_object_entry *waterEntry = &waterEntries[RCT2_GLOBAL(0x01358841, uint8) == 0 ? 0 : 1];
	if (!object_load(0, waterEntry, NULL)) {
		error_string_quit(0x99990900, -1);
		return;
	}
}

/**
 *
 *  rct2: 0x006A2956
 */
static void editor_fix_rct1_scenery()
{
	rct_map_element *element;
	map_element_iterator it;

	map_element_iterator_begin(&it);
	while (map_element_iterator_next(&it)) {
		element = it.element;

		if (map_element_get_type(element) != MAP_ELEMENT_TYPE_SCENERY)
			continue;

		switch (element->properties.scenery.type) {
		case 157:	// TGE1	(Geometric Sculpture)
		case 162:	// TGE2	(Geometric Sculpture)
		case 168:	// TGE3	(Geometric Sculpture)
		case 170:	// TGE4	(Geometric Sculpture)
		case 171:	// TGE5	(Geometric Sculpture)
			element->properties.scenery.colour_2 = 2;
			break;
		}
	}
}

/**
 *
 *  rct2: 0x0069F509
 */
static void editor_rct1_reset_research()
{
	rct_research_item *researchItem;

	researchItem = gResearchItems;
	researchItem->entryIndex = RESEARCHED_ITEMS_SEPERATOR;
	researchItem++;
	researchItem->entryIndex = RESEARCHED_ITEMS_END;
	researchItem++;
	researchItem->entryIndex = RESEARCHED_ITEMS_END_2;
	RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = 0;
	RCT2_GLOBAL(0x01357CF4, sint32) = -1;
	news_item_init_queue();
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
		RCT2_GLOBAL(RCT2_ADDRESS_ERROR_TYPE, uint8) = 255;
		RCT2_GLOBAL(RCT2_ADDRESS_ERROR_STRING_ID, uint16) = 3011;
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
		RCT2_GLOBAL(RCT2_ADDRESS_ERROR_TYPE, uint8) = 255;
		RCT2_GLOBAL(RCT2_ADDRESS_ERROR_STRING_ID, uint16) = 3011;
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
	read((void*)0x0138B580, &src->map_animations, 0x258F2);
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
			RCT2_GLOBAL(RCT2_ADDRESS_ERROR_TYPE, uint8) = 255;
			RCT2_GLOBAL(RCT2_ADDRESS_ERROR_STRING_ID, uint16) = STR_FILE_CONTAINS_INVALID_DATA;

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

		reset_loaded_objects();
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
			research_populate_list_random();
			research_remove_non_separate_vehicle_types();

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
			reset_loaded_objects();

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
		scenery_set_default_placement_configuration();
		window_new_ride_init_vars();
		RCT2_GLOBAL(0x009DEB7C, uint16) = 0;
		load_palette(); 
		gfx_invalidate_screen();

		return 1;
	}

	log_error("failed to find scenario file.");
	RCT2_GLOBAL(RCT2_ADDRESS_ERROR_TYPE, uint8) = 255;
	RCT2_GLOBAL(RCT2_ADDRESS_ERROR_STRING_ID, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
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

#pragma region RCT1 Default Objects

// rct2: 0x0098BD0E
static const rct_object_entry RCT1DefaultObjectsSmallScenery[] = {
	{ 0x00000081, { "TL0     " }, 0 },
	{ 0x00000081, { "TL1     " }, 0 },
	{ 0x00000081, { "TL2     " }, 0 },
	{ 0x00000081, { "TL3     " }, 0 },
	{ 0x00000081, { "TM0     " }, 0 },
	{ 0x00000081, { "TM1     " }, 0 },
	{ 0x00000081, { "TM2     " }, 0 },
	{ 0x00000081, { "TM3     " }, 0 },
	{ 0x00000081, { "TS0     " }, 0 },
	{ 0x00000081, { "TS1     " }, 0 },
	{ 0x00000081, { "TS2     " }, 0 },
	{ 0x00000081, { "TS3     " }, 0 },
	{ 0x00000081, { "TS4     " }, 0 },
	{ 0x00000081, { "TS5     " }, 0 },
	{ 0x00000081, { "TS6     " }, 0 },
	{ 0x00000081, { "TIC     " }, 0 },
	{ 0x00000081, { "TLC     " }, 0 },
	{ 0x00000081, { "TMC     " }, 0 },
	{ 0x00000081, { "TMP     " }, 0 },
	{ 0x00000081, { "TITC    " }, 0 },
	{ 0x00000081, { "TGHC    " }, 0 },
	{ 0x00000081, { "TAC     " }, 0 },
	{ 0x00000081, { "TGHC2   " }, 0 },
	{ 0x00000081, { "TCJ     " }, 0 },
	{ 0x00000081, { "TMBJ    " }, 0 },
	{ 0x00000081, { "TCF     " }, 0 },
	{ 0x00000081, { "TCL     " }, 0 },
	{ 0x00000081, { "TRF     " }, 0 },
	{ 0x00000081, { "TRF2    " }, 0 },
	{ 0x00000081, { "TEL     " }, 0 },
	{ 0x00000081, { "TAP     " }, 0 },
	{ 0x00000081, { "TSP     " }, 0 },
	{ 0x00000081, { "TMZP    " }, 0 },
	{ 0x00000081, { "TCRP    " }, 0 },
	{ 0x00000081, { "TBP     " }, 0 },
	{ 0x00000081, { "TLP     " }, 0 },
	{ 0x00000081, { "TWP     " }, 0 },
	{ 0x00000081, { "TAS     " }, 0 },
	{ 0x00000081, { "TMG     " }, 0 },
	{ 0x00000081, { "TWW     " }, 0 },
	{ 0x00000081, { "TSB     " }, 0 },
	{ 0x00000081, { "TVL     " }, 0 },
	{ 0x00000081, { "TCT     " }, 0 },
	{ 0x00000081, { "TEF     " }, 0 },
	{ 0x00000081, { "TAL     " }, 0 },
	{ 0x00000081, { "TSQ     " }, 0 },
	{ 0x00000081, { "THT     " }, 0 },
	{ 0x00000081, { "TCB     " }, 0 },
	{ 0x00000081, { "TDM     " }, 0 },
	{ 0x00000081, { "TSD     " }, 0 },
	{ 0x00000081, { "TGS     " }, 0 },
	{ 0x00000081, { "TUS     " }, 0 },
	{ 0x00000081, { "TH1     " }, 0 },
	{ 0x00000081, { "TBC     " }, 0 },
	{ 0x00000081, { "TH2     " }, 0 },
	{ 0x00000081, { "TPM     " }, 0 },
	{ 0x00000081, { "TSC     " }, 0 },
	{ 0x00000081, { "TG1     " }, 0 },
	{ 0x00000081, { "TWF     " }, 0 },
	{ 0x00000081, { "TSH0    " }, 0 },
	{ 0x00000081, { "TSH1    " }, 0 },
	{ 0x00000081, { "TSH2    " }, 0 },
	{ 0x00000081, { "TSH3    " }, 0 },
	{ 0x00000081, { "TSH4    " }, 0 },
	{ 0x00000081, { "TSH5    " }, 0 },
	{ 0x00000081, { "TG2     " }, 0 },
	{ 0x00000081, { "TG3     " }, 0 },
	{ 0x00000081, { "TG4     " }, 0 },
	{ 0x00000081, { "TG5     " }, 0 },
	{ 0x00000081, { "TG6     " }, 0 },
	{ 0x00000081, { "TG7     " }, 0 },
	{ 0x00000081, { "TG8     " }, 0 },
	{ 0x00000081, { "TG9     " }, 0 },
	{ 0x00000081, { "TG10    " }, 0 },
	{ 0x00000081, { "TG11    " }, 0 },
	{ 0x00000081, { "TG12    " }, 0 },
	{ 0x00000081, { "TG13    " }, 0 },
	{ 0x00000081, { "TG14    " }, 0 },
	{ 0x00000081, { "TT1     " }, 0 },
	{ 0x00000081, { "TDF     " }, 0 },
	{ 0x00000081, { "TSH     " }, 0 },
	{ 0x00000081, { "THRS    " }, 0 },
	{ 0x00000081, { "TSTD    " }, 0 },
	{ 0x00000081, { "TRMS    " }, 0 },
	{ 0x00000081, { "TRWS    " }, 0 },
	{ 0x00000081, { "TRC     " }, 0 },
	{ 0x00000081, { "TQF     " }, 0 },
	{ 0x00000081, { "TES1    " }, 0 },
	{ 0x00000081, { "TEN     " }, 0 },
	{ 0x00000081, { "TERS    " }, 0 },
	{ 0x00000081, { "TERB    " }, 0 },
	{ 0x00000081, { "TEP     " }, 0 },
	{ 0x00000081, { "TST1    " }, 0 },
	{ 0x00000081, { "TST2    " }, 0 },
	{ 0x00000081, { "TMS1    " }, 0 },
	{ 0x00000081, { "TAS1    " }, 0 },
	{ 0x00000081, { "TAS2    " }, 0 },
	{ 0x00000081, { "TAS3    " }, 0 },
	{ 0x00000081, { "TST3    " }, 0 },
	{ 0x00000081, { "TST4    " }, 0 },
	{ 0x00000081, { "TST5    " }, 0 },
	{ 0x00000081, { "TAS4    " }, 0 },
	{ 0x00000081, { "TCY     " }, 0 },
	{ 0x00000081, { "TBW     " }, 0 },
	{ 0x00000081, { "TBR1    " }, 0 },
	{ 0x00000081, { "TBR2    " }, 0 },
	{ 0x00000081, { "TML     " }, 0 },
	{ 0x00000081, { "TMW     " }, 0 },
	{ 0x00000081, { "TBR3    " }, 0 },
	{ 0x00000081, { "TBR4    " }, 0 },
	{ 0x00000081, { "TMJ     " }, 0 },
	{ 0x00000081, { "TBR     " }, 0 },
	{ 0x00000081, { "TMO1    " }, 0 },
	{ 0x00000081, { "TMO2    " }, 0 },
	{ 0x00000081, { "TMO3    " }, 0 },
	{ 0x00000081, { "TMO4    " }, 0 },
	{ 0x00000081, { "TMO5    " }, 0 },
	{ 0x00000081, { "TWH1    " }, 0 },
	{ 0x00000081, { "TWH2    " }, 0 },
	{ 0x00000081, { "TNS     " }, 0 },
	{ 0x00000081, { "TP1     " }, 0 },
	{ 0x00000081, { "TP2     " }, 0 },
	{ 0x00000081, { "TK1     " }, 0 },
	{ 0x00000081, { "TK2     " }, 0 },
	{ 0x00000081, { "TR1     " }, 0 },
	{ 0x00000081, { "TR2     " }, 0 },
	{ 0x00000081, { "TQ1     " }, 0 },
	{ 0x00000081, { "TQ2     " }, 0 },
	{ 0x00000081, { "TWN     " }, 0 },
	{ 0x00000081, { "TCE     " }, 0 },
	{ 0x00000081, { "TCO     " }, 0 },
	{ 0x00000081, { "THL     " }, 0 },
	{ 0x00000081, { "TCC     " }, 0 },
	{ 0x00000081, { "TB1     " }, 0 },
	{ 0x00000081, { "TB2     " }, 0 },
	{ 0x00000081, { "TK3     " }, 0 },
	{ 0x00000081, { "TK4     " }, 0 },
	{ 0x00000081, { "TBN     " }, 0 },
	{ 0x00000081, { "TBN1    " }, 0 },
	{ 0x00000081, { "TDT1    " }, 0 },
	{ 0x00000081, { "TDT2    " }, 0 },
	{ 0x00000081, { "TDT3    " }, 0 },
	{ 0x00000081, { "TMM1    " }, 0 },
	{ 0x00000081, { "TMM2    " }, 0 },
	{ 0x00000081, { "TMM3    " }, 0 },
	{ 0x00000081, { "TGS1    " }, 0 },
	{ 0x00000081, { "TGS2    " }, 0 },
	{ 0x00000081, { "TGS3    " }, 0 },
	{ 0x00000081, { "TGS4    " }, 0 },
	{ 0x00000081, { "TDN4    " }, 0 },
	{ 0x00000081, { "TDN5    " }, 0 },
	{ 0x00000081, { "TJT1    " }, 0 },
	{ 0x00000081, { "TJT2    " }, 0 },
	{ 0x00000081, { "TJB1    " }, 0 },
	{ 0x00000081, { "TTF     " }, 0 },
	{ 0x00000081, { "TF1     " }, 0 },
	{ 0x00000081, { "TF2     " }, 0 },
	{ 0x00000081, { "TGE1    " }, 0 },
	{ 0x00000081, { "TJT3    " }, 0 },
	{ 0x00000081, { "TJT4    " }, 0 },
	{ 0x00000081, { "TJP1    " }, 0 },
	{ 0x00000081, { "TJB2    " }, 0 },
	{ 0x00000081, { "TGE2    " }, 0 },
	{ 0x00000081, { "TJT5    " }, 0 },
	{ 0x00000081, { "TJB3    " }, 0 },
	{ 0x00000081, { "TJB4    " }, 0 },
	{ 0x00000081, { "TJT6    " }, 0 },
	{ 0x00000081, { "TJP2    " }, 0 },
	{ 0x00000081, { "TGE3    " }, 0 },
	{ 0x00000081, { "TCK     " }, 0 },
	{ 0x00000081, { "TGE4    " }, 0 },
	{ 0x00000081, { "TGE5    " }, 0 },
	{ 0x00000081, { "TG15    " }, 0 },
	{ 0x00000081, { "TG16    " }, 0 },
	{ 0x00000081, { "TG17    " }, 0 },
	{ 0x00000081, { "TG18    " }, 0 },
	{ 0x00000081, { "TG19    " }, 0 },
	{ 0x00000081, { "TG20    " }, 0 },
	{ 0x00000081, { "TG21    " }, 0 },
	{ 0x00000081, { "TSM     " }, 0 },
	{ 0x00000081, { "TIG     " }, 0 },
	{ 0x00000081, { "TCFS    " }, 0 },
	{ 0x00000081, { "TRFS    " }, 0 },
	{ 0x00000081, { "TRF3    " }, 0 },
	{ 0x00000081, { "TNSS    " }, 0 },
	{ 0x00000081, { "TCT1    " }, 0 },
	{ 0x00000081, { "TCT2    " }, 0 },
	{ 0x00000081, { "TSF1    " }, 0 },
	{ 0x00000081, { "TSF2    " }, 0 },
	{ 0x00000081, { "TSF3    " }, 0 },
	{ 0x00000081, { "TCN     " }, 0 },
	{ 0x00000081, { "TTG     " }, 0 },
	{ 0x00000081, { "TSNC    " }, 0 },
	{ 0x00000081, { "TSNB    " }, 0 },
	{ 0x00000081, { "TSCP    " }, 0 },
	{ 0x00000081, { "TCD     " }, 0 },
	{ 0x00000081, { "TSG     " }, 0 },
	{ 0x00000081, { "TSK     " }, 0 },
	{ 0x00000081, { "TGH1    " }, 0 },
	{ 0x00000081, { "TGH2    " }, 0 },
	{ 0x00000081, { "TSMP    " }, 0 },
	{ 0x00000081, { "TJF     " }, 0 },
	{ 0x00000081, { "TLY     " }, 0 },
	{ 0x00000081, { "TGC1    " }, 0 },
	{ 0x00000081, { "TGC2    " }, 0 },
	{ 0x00000081, { "TGG     " }, 0 },
	{ 0x00000081, { "TSPH    " }, 0 },
	{ 0x00000081, { "TOH1    " }, 0 },
	{ 0x00000081, { "TOH2    " }, 0 },
	{ 0x00000081, { "TOT1    " }, 0 },
	{ 0x00000081, { "TOT2    " }, 0 },
	{ 0x00000081, { "TOS     " }, 0 },
	{ 0x00000081, { "TOT3    " }, 0 },
	{ 0x00000081, { "TOT4    " }, 0 },
	{ 0x00000081, { "TSC2    " }, 0 },
	{ 0x00000081, { "TSP1    " }, 0 },
	{ 0x00000081, { "TOH3    " }, 0 },
	{ 0x00000081, { "TSP2    " }, 0 },
	{ 0x00000081, { "ROMROOF1" }, 0 },
	{ 0x00000081, { "GEOROOF1" }, 0 },
	{ 0x00000081, { "TNTROOF1" }, 0 },
	{ 0x00000081, { "JNGROOF1" }, 0 },
	{ 0x00000081, { "MINROOF1" }, 0 },
	{ 0x00000081, { "ROMROOF2" }, 0 },
	{ 0x00000081, { "GEOROOF2" }, 0 },
	{ 0x00000081, { "PAGROOF1" }, 0 },
	{ 0x00000081, { "SPCROOF1" }, 0 },
	{ 0x00000081, { "ROOF1   " }, 0 },
	{ 0x00000081, { "ROOF2   " }, 0 },
	{ 0x00000081, { "ROOF3   " }, 0 },
	{ 0x00000081, { "ROOF4   " }, 0 },
	{ 0x00000081, { "ROOF5   " }, 0 },
	{ 0x00000081, { "ROOF6   " }, 0 },
	{ 0x00000081, { "ROOF7   " }, 0 },
	{ 0x00000081, { "ROOF8   " }, 0 },
	{ 0x00000081, { "ROOF9   " }, 0 },
	{ 0x00000081, { "ROOF10  " }, 0 },
	{ 0x00000081, { "ROOF11  " }, 0 },
	{ 0x00000081, { "ROOF12  " }, 0 },
	{ 0x00000081, { "ROOF13  " }, 0 },
	{ 0x00000081, { "ROOF14  " }, 0 },
	{ 0x00000081, { "IGROOF  " }, 0 },
	{ 0x00000081, { "CORROOF " }, 0 },
	{ 0x00000081, { "CORROOF2" }, 0 }
};

static const rct_object_entry RCT1DefaultObjectsLargeScenery[] = {
	{ 0x00000082, { "SCOL    " }, 0 },
	{ 0x00000082, { "SHS1    " }, 0 },
	{ 0x00000082, { "SSPX    " }, 0 },
	{ 0x00000082, { "SHS2    " }, 0 },
	{ 0x00000082, { "SCLN    " }, 0 },
	{ 0x00000082, { "SMH1    " }, 0 },
	{ 0x00000082, { "SMH2    " }, 0 },
	{ 0x00000082, { "SVLC    " }, 0 },
	{ 0x00000082, { "SPYR    " }, 0 },
	{ 0x00000082, { "SMN1    " }, 0 },
	{ 0x00000082, { "SMB     " }, 0 },
	{ 0x00000082, { "SSK1    " }, 0 },
	{ 0x00000082, { "SDN1    " }, 0 },
	{ 0x00000082, { "SDN2    " }, 0 },
	{ 0x00000082, { "SDN3    " }, 0 },
	{ 0x00000082, { "SIP     " }, 0 },
	{ 0x00000082, { "STB1    " }, 0 },
	{ 0x00000082, { "STB2    " }, 0 },
	{ 0x00000082, { "STG1    " }, 0 },
	{ 0x00000082, { "STG2    " }, 0 },
	{ 0x00000082, { "SCT     " }, 0 },
	{ 0x00000082, { "SOH1    " }, 0 },
	{ 0x00000082, { "SOH2    " }, 0 },
	{ 0x00000082, { "SOH3    " }, 0 },
	{ 0x00000082, { "SGP     " }, 0 },
	{ 0x00000082, { "SSR     " }, 0 },
	{ 0x00000082, { "STH     " }, 0 },
	{ 0x00000082, { "SAH     " }, 0 },
	{ 0x00000082, { "SPS     " }, 0 },
	{ 0x00000082, { "SPG     " }, 0 },
	{ 0x00000082, { "SOB     " }, 0 },
	{ 0x00000082, { "SAH2    " }, 0 },
	{ 0x00000082, { "SST     " }, 0 },
	{ 0x00000082, { "SSH     " }, 0 },
	{ 0x00000082, { "SAH3    " }, 0 },
	{ 0x00000082, { "SSIG1   " }, 0 },
	{ 0x00000082, { "SSIG2   " }, 0 },
	{ 0x00000082, { "SSIG3   " }, 0 },
	{ 0x00000082, { "SSIG4   " }, 0 }
};

static const rct_object_entry RCT1DefaultObjectsWall[] = {
	{ 0x00000083, { "WMF     " }, 0 },
	{ 0x00000083, { "WMFG    " }, 0 },
	{ 0x00000083, { "WRW     " }, 0 },
	{ 0x00000083, { "WEW     " }, 0 },
	{ 0x00000083, { "WHG     " }, 0 },
	{ 0x00000083, { "WHGG    " }, 0 },
	{ 0x00000083, { "WCW1    " }, 0 },
	{ 0x00000083, { "WCW2    " }, 0 },
	{ 0x00000083, { "WSW     " }, 0 },
	{ 0x00000083, { "WSWG    " }, 0 },
	{ 0x00000083, { "WMW     " }, 0 },
	{ 0x00000083, { "WFW1    " }, 0 },
	{ 0x00000083, { "WFWG    " }, 0 },
	{ 0x00000083, { "WPW1    " }, 0 },
	{ 0x00000083, { "WPW2    " }, 0 },
	{ 0x00000083, { "WPF     " }, 0 },
	{ 0x00000083, { "WPFG    " }, 0 },
	{ 0x00000083, { "WWTW    " }, 0 },
	{ 0x00000083, { "WMWW    " }, 0 },
	{ 0x00000083, { "WSW1    " }, 0 },
	{ 0x00000083, { "WSW2    " }, 0 },
	{ 0x00000083, { "WGW2    " }, 0 },
	{ 0x00000083, { "WBW     " }, 0 },
	{ 0x00000083, { "WBR1    " }, 0 },
	{ 0x00000083, { "WBRG    " }, 0 },
	{ 0x00000083, { "WBR2    " }, 0 },
	{ 0x00000083, { "WBR3    " }, 0 },
	{ 0x00000083, { "WPW3    " }, 0 },
	{ 0x00000083, { "WJF     " }, 0 },
	{ 0x00000083, { "WCH     " }, 0 },
	{ 0x00000083, { "WCHG    " }, 0 },
	{ 0x00000083, { "WC1     " }, 0 },
	{ 0x00000083, { "WC2     " }, 0 },
	{ 0x00000083, { "WC3     " }, 0 },
	{ 0x00000083, { "WC4     " }, 0 },
	{ 0x00000083, { "WC5     " }, 0 },
	{ 0x00000083, { "WC6     " }, 0 },
	{ 0x00000083, { "WC7     " }, 0 },
	{ 0x00000083, { "WC8     " }, 0 },
	{ 0x00000083, { "WC9     " }, 0 },
	{ 0x00000083, { "WC10    " }, 0 },
	{ 0x00000083, { "WC11    " }, 0 },
	{ 0x00000083, { "WC12    " }, 0 },
	{ 0x00000083, { "WC13    " }, 0 },
	{ 0x00000083, { "WC14    " }, 0 },
	{ 0x00000083, { "WC15    " }, 0 },
	{ 0x00000083, { "WC16    " }, 0 },
	{ 0x00000083, { "WC17    " }, 0 },
	{ 0x00000083, { "WC18    " }, 0 },
	{ 0x00000083, { "WALLBRDR" }, 0 },
	{ 0x00000083, { "WALLBR32" }, 0 },
	{ 0x00000083, { "WALLBR16" }, 0 },
	{ 0x00000083, { "WALLBR8 " }, 0 },
	{ 0x00000083, { "WALLCF8 " }, 0 },
	{ 0x00000083, { "WALLCF16" }, 0 },
	{ 0x00000083, { "WALLCF32" }, 0 },
	{ 0x00000083, { "WALLBB8 " }, 0 },
	{ 0x00000083, { "WALLBB16" }, 0 },
	{ 0x00000083, { "WALLBB32" }, 0 },
	{ 0x00000083, { "WALLRS8 " }, 0 },
	{ 0x00000083, { "WALLRS16" }, 0 },
	{ 0x00000083, { "WALLRS32" }, 0 },
	{ 0x00000083, { "WALLCB8 " }, 0 },
	{ 0x00000083, { "WALLCB16" }, 0 },
	{ 0x00000083, { "WALLCB32" }, 0 },
	{ 0x00000083, { "WALLGL8 " }, 0 },
	{ 0x00000083, { "WALLGL16" }, 0 },
	{ 0x00000083, { "WALLGL32" }, 0 },
	{ 0x00000083, { "WALLWD8 " }, 0 },
	{ 0x00000083, { "WALLWD16" }, 0 },
	{ 0x00000083, { "WALLWD32" }, 0 },
	{ 0x00000083, { "WALLTN32" }, 0 },
	{ 0x00000083, { "WALLJN32" }, 0 },
	{ 0x00000083, { "WALLMN32" }, 0 },
	{ 0x00000083, { "WALLSP32" }, 0 },
	{ 0x00000083, { "WALLPG32" }, 0 },
	{ 0x00000083, { "WALLU132" }, 0 },
	{ 0x00000083, { "WALLU232" }, 0 },
	{ 0x00000083, { "WALLCZ32" }, 0 },
	{ 0x00000083, { "WALLCW32" }, 0 },
	{ 0x00000083, { "WALLCY32" }, 0 },
	{ 0x00000083, { "WALLCX32" }, 0 },
	{ 0x00000083, { "WBR1A   " }, 0 },
	{ 0x00000083, { "WBR2A   " }, 0 },
	{ 0x00000083, { "WRWA    " }, 0 },
	{ 0x00000083, { "WWTWA   " }, 0 },
	{ 0x00000083, { "WALLIG16" }, 0 },
	{ 0x00000083, { "WALLIG24" }, 0 },
	{ 0x00000083, { "WALLCO16" }, 0 },
	{ 0x00000083, { "WALLCFDR" }, 0 },
	{ 0x00000083, { "WALLCBDR" }, 0 },
	{ 0x00000083, { "WALLBRWN" }, 0 },
	{ 0x00000083, { "WALLCFWN" }, 0 },
	{ 0x00000083, { "WALLCBWN" }, 0 }
};

static const rct_object_entry RCT1DefaultObjectsBanner[] = {
	{ 0x00000084, { "BN1     " }, 0 },
	{ 0x00000084, { "BN2     " }, 0 },
	{ 0x00000084, { "BN3     " }, 0 },
	{ 0x00000084, { "BN4     " }, 0 },
	{ 0x00000084, { "BN5     " }, 0 },
	{ 0x00000084, { "BN6     " }, 0 },
	{ 0x00000084, { "BN7     " }, 0 },
	{ 0x00000084, { "BN8     " }, 0 },
	{ 0x00000084, { "BN9     " }, 0 }
};

static const rct_object_entry RCT1DefaultObjectsPath[] = {
	{ 0x00000085, { "TARMAC  " }, 0 },
	{ 0x00000085, { "TARMACB " }, 0 },
	{ 0x00000085, { "PATHSPCE" }, 0 },
	{ 0x00000085, { "PATHDIRT" }, 0 },
	{ 0x00000085, { "ROAD    " }, 0 },
	{ 0x00000085, { "PATHCRZY" }, 0 },
	{ 0x00000085, { "PATHASH " }, 0 }
};

static const rct_object_entry RCT1DefaultObjectsPathBits[] = {
	{ 0x00000086, { "LAMP1   " }, 0 },
	{ 0x00000086, { "LAMP2   " }, 0 },
	{ 0x00000086, { "LITTER1 " }, 0 },
	{ 0x00000086, { "BENCH1  " }, 0 },
	{ 0x00000086, { "JUMPFNT1" }, 0 },
	{ 0x00000086, { "LAMP3   " }, 0 },
	{ 0x00000086, { "LAMP4   " }, 0 },
	{ 0x00000086, { "JUMPSNW1" }, 0 }
};

static const rct_object_entry RCT1DefaultObjectsSceneryGroup[] = {
	{ 0x00000087, { "SCGTREES" }, 0 },
	{ 0x00000087, { "SCGSHRUB" }, 0 },
	{ 0x00000087, { "SCGGARDN" }, 0 },
	{ 0x00000087, { "SCGPATHX" }, 0 },
	{ 0x00000087, { "SCGFENCE" }, 0 },
	{ 0x00000087, { "SCGMART " }, 0 },
	{ 0x00000087, { "SCGWOND " }, 0 },
	{ 0x00000087, { "SCGSNOW " }, 0 },
	{ 0x00000087, { "SCGWALLS" }, 0 }
};

static const rct_object_entry RCT1DefaultObjectsParkEntrance[] = {
	{ 0x00000088, { "PKENT1  " }, 0 }
};

static const rct_object_entry RCT1DefaultObjectsWater[] = {
	{ 0x00000089, { "WTRCYAN " }, 0 },
	{ 0x00000089, { "WTRORNG " }, 0 }
};

static const RCT1DefaultObjectsGroup RCT1DefaultObjects[10] = {
	{ NULL,								0											},
	{ RCT1DefaultObjectsSmallScenery,	countof(RCT1DefaultObjectsSmallScenery)		},
	{ RCT1DefaultObjectsLargeScenery,	countof(RCT1DefaultObjectsLargeScenery)		},
	{ RCT1DefaultObjectsWall,			countof(RCT1DefaultObjectsWall)				},
	{ RCT1DefaultObjectsBanner,			countof(RCT1DefaultObjectsBanner)			},
	{ RCT1DefaultObjectsPath,			countof(RCT1DefaultObjectsPath)				},
	{ RCT1DefaultObjectsPathBits,		countof(RCT1DefaultObjectsPathBits)			},
	{ RCT1DefaultObjectsSceneryGroup,	countof(RCT1DefaultObjectsSceneryGroup)		},
	{ RCT1DefaultObjectsParkEntrance,	countof(RCT1DefaultObjectsParkEntrance)		},
	{ RCT1DefaultObjectsWater,			countof(RCT1DefaultObjectsWater)			}
};

#pragma endregion