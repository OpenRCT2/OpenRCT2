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
#include "world/footpath.h"
#include "world/map.h"
#include "world/park.h"
#include "world/scenery.h"
#include "world/sprite.h"

static void set_all_land_owned();
static int editor_load_landscape_from_sv4(const char *path);
static int editor_load_landscape_from_sc4(const char *path);
static void editor_finalise_main_view();
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
	staff_reset_modes();
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
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_AGE, sint16) = 0;

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
		object_unload((rct_object_entry*)&object_entry_groups[OBJECT_TYPE_SCENARIO_TEXT].entries[0]);
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
	editor_finalise_main_view();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_AGE, uint16) = 0;
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
	staff_reset_modes();
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
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_AGE, sint16) = 0;
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
	staff_reset_modes();
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
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_AGE, sint16) = 0;
}

/**
 *
 *  rct2: 0x0068ABEC
 */
static void set_all_land_owned()
{
	int mapSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, sint16);

	game_do_command(64, 1, 64, 2, GAME_COMMAND_SET_LAND_OWNERSHIP, (mapSize - 2) * 32, (mapSize - 2) * 32);
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
 *  rct2: 0x006A2B02
 */
static int editor_load_landscape_from_sv4(const char *path)
{
	rct1_s4 *s4;

	s4 = malloc(sizeof(rct1_s4));
	if (!rct1_read_sv4(path, s4)) {
		free(s4);
		return 0;
	}
	rct1_import_s4(s4);
	free(s4);

	rct1_fix_landscape();
	editor_finalise_main_view();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_AGE, uint16) = 0;
	return 1;
}

static int editor_load_landscape_from_sc4(const char *path)
{
	rct1_s4 *s4;

	s4 = malloc(sizeof(rct1_s4));
	if (!rct1_read_sc4(path, s4)) {
		free(s4);
		return 0;
	}
	rct1_import_s4(s4);
	free(s4);

	rct1_fix_landscape();
	editor_finalise_main_view();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_AGE, uint16) = 0;
	return 1;
}

/**
 *
 *  rct2: 0x006758FE
 */
static int editor_read_s6(const char *path)
{
	int i, j;
	SDL_RWops* rw;
	rct_s6_header *s6Header = (rct_s6_header*)0x009E34E4;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	log_verbose("loading landscape, %s", path);

	rw = SDL_RWFromFile(path, "rb");
	if (rw != NULL) {
		if (!sawyercoding_validate_checksum(rw)) {
			SDL_RWclose(rw);
			RCT2_GLOBAL(RCT2_ADDRESS_ERROR_TYPE, uint8) = 255;
			RCT2_GLOBAL(RCT2_ADDRESS_ERROR_STRING_ID, uint16) = STR_FILE_CONTAINS_INVALID_DATA;

			log_error("failed to load scenario, invalid checksum");
			return 0;
		}

		// Read first chunk
		sawyercoding_read_chunk(rw, (uint8*)s6Header);
		if (s6Header->type == S6_TYPE_SCENARIO) {
			// Read second chunk
			sawyercoding_read_chunk(rw, (uint8*)s6Info);

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
				j += object_load_packed(rw);
			if (j > 0)
				object_list_load();
		}

		uint8 load_success = object_read_and_load_entries(rw);

		// Read flags (16 bytes). Loads:
		//	RCT2_ADDRESS_CURRENT_MONTH_YEAR
		//	RCT2_ADDRESS_CURRENT_MONTH_TICKS
		//	RCT2_ADDRESS_SCENARIO_TICKS
		sawyercoding_read_chunk(rw, (uint8*)RCT2_ADDRESS_CURRENT_MONTH_YEAR);

		// Read map elements
		memset((void*)RCT2_ADDRESS_MAP_ELEMENTS, 0, MAX_MAP_ELEMENTS * sizeof(rct_map_element));
		sawyercoding_read_chunk(rw, (uint8*)RCT2_ADDRESS_MAP_ELEMENTS);

		// Read game data, including sprites
		sawyercoding_read_chunk(rw, (uint8*)0x010E63B8);

		if (s6Header->type == S6_TYPE_SCENARIO) {
			// Read number of guests in park and something else
			sawyercoding_read_chunk(rw, (uint8*)RCT2_ADDRESS_GUESTS_IN_PARK);

			// Read ?
			sawyercoding_read_chunk(rw, (uint8*)RCT2_ADDRESS_LAST_GUESTS_IN_PARK);

			// Read park rating
			sawyercoding_read_chunk(rw, (uint8*)RCT2_ADDRESS_CURRENT_PARK_RATING);

			// Read ?
			sawyercoding_read_chunk(rw, (uint8*)RCT2_ADDRESS_ACTIVE_RESEARCH_TYPES);

			// Read ?
			sawyercoding_read_chunk(rw, (uint8*)RCT2_ADDRESS_CURRENT_EXPENDITURE);

			// Read ?
			sawyercoding_read_chunk(rw, (uint8*)RCT2_ADDRESS_CURRENT_PARK_VALUE);

			// Read more game data, including research items and rides
			sawyercoding_read_chunk(rw, (uint8*)RCT2_ADDRESS_COMPLETED_COMPANY_VALUE);
		}

		SDL_RWclose(rw);
		if (!load_success){
			log_error("failed to load all entries.");
			set_load_objects_fail_reason();
			return 0;
		}

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
		staff_reset_modes();
		RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_LAST_GUESTS_IN_PARK, uint16) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_GUEST_CHANGE_MODIFIER, uint16) = 0;
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
			finance_reset_cash_to_initial();
			finance_update_loan_hash();

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
			object_unload((rct_object_entry*)&object_entry_groups[OBJECT_TYPE_SCENARIO_TEXT].entries[0]);
			reset_loaded_objects();

			format_string(s6Info->details, STR_NO_DETAILS_YET, NULL);
			s6Info->name[0] = 0;
		}

		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_SCENARIO_EDITOR;
		viewport_init_all();
		news_item_init_queue();
		window_editor_main_open();
		editor_finalise_main_view();
		game_convert_strings_to_utf8();
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

static void editor_finalise_main_view()
{
	rct_window *w = window_get_main();
	rct_viewport *viewport = w->viewport;

	w->viewport_target_sprite = -1;
	w->saved_view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16);
	w->saved_view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16);

	viewport->zoom = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) & 0xFF;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) >> 8;

	int zoom_difference = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, sint16) - viewport->zoom;
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
	reset_all_sprite_quadrant_placements();
	scenery_set_default_placement_configuration();
	window_new_ride_init_vars();
	RCT2_GLOBAL(0x009DEB7C, uint16) = 0;
	load_palette();
	gfx_invalidate_screen();
}

static bool editor_check_object_group_at_least_one_selected(int objectType)
{
	uint32 numObjects = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32);
	rct_object_entry *entry = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	uint8 *objectFlag = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	for (uint32 i = 0; i < numObjects; i++) {
		if ((entry->flags & 0x0F) == objectType && (*objectFlag & 1)) {
			return true;
		}
		entry = object_get_next(entry);
		objectFlag++;
	}
	return false;
}

/**
 *
 *  rct2: 0x006AB9B8
 */
int editor_check_object_selection()
{
	bool isTrackDesignerManager =
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER);

	if (!isTrackDesignerManager) {
		if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_PATHS)) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_AT_LEAST_ONE_PATH_OBJECT_MUST_BE_SELECTED;
			return OBJECT_TYPE_PATHS;
		}
	}

	if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_RIDE)) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_AT_LEAST_ONE_RIDE_OBJECT_MUST_BE_SELECTED;
		return OBJECT_TYPE_RIDE;
	}

	if (!isTrackDesignerManager) {
		if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_PARK_ENTRANCE)) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_PARK_ENTRANCE_TYPE_MUST_BE_SELECTED;
			return OBJECT_TYPE_PARK_ENTRANCE;
		}

		if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_WATER)) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_WATER_TYPE_MUST_BE_SELECTED;
			return OBJECT_TYPE_WATER;
		}
	}

	return -1;
}

/**
 *
 *  rct2: 0x0066FEAC
 */
bool editor_check_park()
{
	int parkSize = park_calculate_size();
	if (parkSize == 0) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_PARK_MUST_OWN_SOME_LAND;
		return false;
	}

	for (int i = 0; i < 4; i++) {
		if (RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] != 0x8000)
			break;

		if (i == 3) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_NO_PARK_ENTRANCES;
			return false;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] == 0x8000)
			continue;

		int x = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i];
		int y = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, uint16)[i];
		int z = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Z, uint16)[i] / 8;
		int direction = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_DIRECTION, uint8)[i] ^ 2;

		switch (footpath_is_connected_to_map_edge(x, y, z, direction, 0)) {
		case FOOTPATH_SEARCH_NOT_FOUND:
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_PARK_ENTRANCE_WRONG_DIRECTION_OR_NO_PATH;
			return false;
		case FOOTPATH_SEARCH_INCOMPLETE:
		case FOOTPATH_SEARCH_TOO_COMPLEX:
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_PARK_ENTRANCE_PATH_INCOMPLETE_OR_COMPLEX;
			return false;
		case FOOTPATH_SEARCH_SUCCESS:
			// Run the search again and unown the path
			footpath_is_connected_to_map_edge(x, y, z, direction, 0x20);
			break;
		}
	}

	if (gPeepSpawns[0].x == 0xFFFF && gPeepSpawns[1].x == 0xFFFF) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_PEEP_SPAWNS_NOT_SET;
		return false;
	}

	return true;
}
