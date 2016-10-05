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
#include "object/ObjectManager.h"
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

uint8 _editorSelectedRides[128];
uint8 _editorSelectedSmallScenery[252];
uint8 _editorSelectedLargeScenery[128];
uint8 _editorSelectedWalls[128];
uint8 _editorSelectedBanners[32];
uint8 _editorSelectedFootpaths[16];
uint8 _editorSelectedFootpathAdditions[15];
uint8 _editorSelectedSceneryGroups[19];
uint8 _editorSelectedParkEntrances[1];
uint8 _editorSelectedWaters[1];
uint8 _editorSelectedStexs[1];

uint8 * gEditorSelectedObjects[OBJECT_ENTRY_GROUP_COUNT] = {
	_editorSelectedRides,
	_editorSelectedSmallScenery,
	_editorSelectedLargeScenery,
	_editorSelectedWalls,
	_editorSelectedBanners,
	_editorSelectedFootpaths,
	_editorSelectedFootpathAdditions,
	_editorSelectedSceneryGroups,
	_editorSelectedParkEntrances,
	_editorSelectedWaters,
	_editorSelectedStexs,
};

void editor_convert_save_to_scenario_callback(int result);
static void set_all_land_owned();
static int editor_load_landscape_from_sv4(const char *path);
static int editor_load_landscape_from_sc4(const char *path);
static void editor_finalise_main_view();
static int editor_read_s6(const char *path);
static void editor_clear_map_for_editing(bool fromSave);

/**
 *
 *  rct2: 0x0066FFE1
 */
void editor_load()
{
	rct_window *mainWindow;

	audio_stop_all_music_and_sounds();
	object_manager_unload_all_objects();
	object_list_load();
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
	gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
	gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
	gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
	window_new_ride_init_vars();
	gS6Info.category = SCENARIO_CATEGORY_OTHER;
	viewport_init_all();
	news_item_init_queue();
	window_editor_main_open();
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	load_palette();
	gfx_invalidate_screen();
	gScreenAge = 0;

	safe_strcpy(gScenarioName, language_get_string(STR_MY_NEW_SCENARIO), 64);
}

/**
 *
 *  rct2: 0x00672781
 */
void editor_convert_save_to_scenario()
{
	tool_cancel();
	window_loadsave_open(LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME, NULL);
	gLoadSaveCallback = editor_convert_save_to_scenario_callback;
}

void editor_convert_save_to_scenario_callback(int result)
{
	if (result != MODAL_RESULT_OK) {
		return;
	}

	if (gParkFlags & PARK_FLAGS_NO_MONEY)
		gParkFlags |= PARK_FLAGS_NO_MONEY_SCENARIO;
	else
		gParkFlags &= ~PARK_FLAGS_NO_MONEY_SCENARIO;
	gParkFlags |= PARK_FLAGS_NO_MONEY;

	safe_strcpy(gS6Info.name, gScenarioName, 64);
	safe_strcpy(gS6Info.details, gScenarioDetails, 256);
	gS6Info.objective_type = gScenarioObjectiveType;
	gS6Info.objective_arg_1 = gScenarioObjectiveYear;
	gS6Info.objective_arg_2 = gScenarioObjectiveCurrency;
	gS6Info.objective_arg_3 = gScenarioObjectiveNumGuests;
	climate_reset(gClimate);

	gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
	gS6Info.editor_step = EDITOR_STEP_OBJECTIVE_SELECTION;
	gS6Info.category = SCENARIO_CATEGORY_OTHER;
	viewport_init_all();
	news_item_init_queue();
	window_editor_main_open();
	editor_finalise_main_view();
	gScreenAge = 0;
}

/**
 *
 *  rct2: 0x00672957
 */
void trackdesigner_load()
{
	rct_window *mainWindow;

	audio_stop_all_music_and_sounds();
	gScreenFlags = SCREEN_FLAGS_TRACK_DESIGNER;
	gScreenAge = 0;

	object_manager_unload_all_objects();
	object_list_load();
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
	gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
	window_new_ride_init_vars();
	viewport_init_all();
	news_item_init_queue();
	window_editor_main_open();
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	load_palette();
	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x006729FD
 */
void trackmanager_load()
{
	rct_window *mainWindow;

	audio_stop_all_music_and_sounds();
	gScreenFlags = SCREEN_FLAGS_TRACK_MANAGER;
	gScreenAge = 0;

	object_manager_unload_all_objects();
	object_list_load();
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
	gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
	window_new_ride_init_vars();
	viewport_init_all();
	news_item_init_queue();
	window_editor_main_open();
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	load_palette();
	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x0068ABEC
 */
static void set_all_land_owned()
{
	int mapSize = gMapSize;

	game_do_command(64, 1, 64, 2, GAME_COMMAND_SET_LAND_OWNERSHIP, (mapSize - 3) * 32, (mapSize - 3) * 32);
}

/**
 *
 *  rct2: 0x006758C0
 */
bool editor_load_landscape(const utf8 *path)
{
	window_close_construction_windows();

	char *extension = strrchr(path, '.');
	if (extension != NULL) {
		if (_stricmp(extension, ".sv4") == 0) {
			return editor_load_landscape_from_sv4(path);
		} else if (_stricmp(extension, ".sc4") == 0) {
			return editor_load_landscape_from_sc4(path);
		}
	}

	// Load SC6 / SV6
	return editor_read_s6(path);
}

/**
 *
 *  rct2: 0x006A2B02
 */
static int editor_load_landscape_from_sv4(const char *path)
{
	rct1_load_saved_game(path);
	editor_clear_map_for_editing(true);

	gS6Info.editor_step = EDITOR_STEP_LANDSCAPE_EDITOR;
	gScreenAge = 0;
	gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
	viewport_init_all();
	window_editor_main_open();
	editor_finalise_main_view();
	return 1;
}

static int editor_load_landscape_from_sc4(const char *path)
{
	rct1_load_scenario(path);
	editor_clear_map_for_editing(false);

	gS6Info.editor_step = EDITOR_STEP_LANDSCAPE_EDITOR;
	gScreenAge = 0;
	gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
	viewport_init_all();
	window_editor_main_open();
	editor_finalise_main_view();
	return 1;
}

/**
 *
 *  rct2: 0x006758FE
 */
static int editor_read_s6(const char *path)
{
	bool loadResult = false;
	const char *extension = path_get_extension(path);
	if (_stricmp(extension, ".sc6") == 0) {
		loadResult = scenario_load(path);
	} else if (_stricmp(extension, ".sv6") == 0) {
		loadResult = game_load_sv6_path(path);
	}
	if (!loadResult) {
		return 0;
	}

	editor_clear_map_for_editing(true);

	gS6Info.editor_step = EDITOR_STEP_LANDSCAPE_EDITOR;
	gScreenAge = 0;
	gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
	viewport_init_all();
	window_editor_main_open();
	editor_finalise_main_view();
	return 1;
}

static void editor_clear_map_for_editing(bool fromSave)
{
	map_remove_all_rides();

	//
	for (int i = 0; i < MAX_BANNERS; i++) {
		if (gBanners[i].type == 255) {
			gBanners[i].flags &= ~BANNER_FLAG_2;
		}
	}

	//
	{
		int i;
		rct_ride *ride;
		FOR_ALL_RIDES(i, ride) {
			user_string_free(ride->name);
		}
	}

	ride_init_all();

	//
	for (int i = 0; i < MAX_SPRITES; i++) {
		rct_sprite *sprite = get_sprite(i);
		user_string_free(sprite->unknown.name_string_idx);
	}

	reset_sprite_list();
	staff_reset_modes();
	gNumGuestsInPark = 0;
	gNumGuestsHeadingForPark = 0;
	gNumGuestsInParkLastWeek = 0;
	gGuestChangeModifier = 0;
	if (fromSave) {
		research_populate_list_random();
		research_remove_non_separate_vehicle_types();

		if (gParkFlags & PARK_FLAGS_NO_MONEY)
			gParkFlags |= PARK_FLAGS_NO_MONEY_SCENARIO;
		else
			gParkFlags &= ~PARK_FLAGS_NO_MONEY_SCENARIO;
		gParkFlags |= PARK_FLAGS_NO_MONEY;

		if (gParkEntranceFee == 0)
			gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
		else
			gParkFlags &= ~PARK_FLAGS_PARK_FREE_ENTRY;

		gParkFlags &= ~PARK_FLAGS_18;

		gGuestInitialCash = clamp(MONEY(10,00), gGuestInitialCash, MONEY(100,00));

		gInitialCash = min(gInitialCash, 100000);
		finance_reset_cash_to_initial();

		gBankLoan = clamp(
			MONEY(0,00),
			gBankLoan,
			MONEY(5000000,00)
		);

		gMaxBankLoan = clamp(
			MONEY(0,00),
			gMaxBankLoan,
			MONEY(5000000,00)
		);

		gBankLoanInterestRate = clamp(5, gBankLoanInterestRate, 80);
	}

	climate_reset(gClimate);

	news_item_init_queue();
}

/**
 *
 *  rct2: 0x0067009A
 */
void editor_open_windows_for_current_step()
{
	if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
		return;

	switch (gS6Info.editor_step) {
	case EDITOR_STEP_OBJECT_SELECTION:
		if (window_find_by_class(WC_EDITOR_OBJECT_SELECTION))
			return;

		if (window_find_by_class(49))
			return;

		if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
			object_manager_unload_all_objects();
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
	w->saved_view_x = gSavedViewX;
	w->saved_view_y = gSavedViewY;
	gCurrentRotation = gSavedViewRotation;

	int zoom_difference = gSavedViewZoom - viewport->zoom;
	viewport->zoom = gSavedViewZoom;
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
	gWindowUpdateTicks = 0;
	load_palette();
	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x006AB9B8
 */
int editor_check_object_selection()
{
	bool isTrackDesignerManager =
		gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER);

	if (!isTrackDesignerManager) {
		if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_PATHS)) {
			gGameCommandErrorText = STR_AT_LEAST_ONE_PATH_OBJECT_MUST_BE_SELECTED;
			return OBJECT_TYPE_PATHS;
		}
	}

	if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_RIDE)) {
		gGameCommandErrorText = STR_AT_LEAST_ONE_RIDE_OBJECT_MUST_BE_SELECTED;
		return OBJECT_TYPE_RIDE;
	}

	if (!isTrackDesignerManager) {
		if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_PARK_ENTRANCE)) {
			gGameCommandErrorText = STR_PARK_ENTRANCE_TYPE_MUST_BE_SELECTED;
			return OBJECT_TYPE_PARK_ENTRANCE;
		}

		if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_WATER)) {
			gGameCommandErrorText = STR_WATER_TYPE_MUST_BE_SELECTED;
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
		gGameCommandErrorText = STR_PARK_MUST_OWN_SOME_LAND;
		return false;
	}

	for (int i = 0; i < 4; i++) {
		if (gParkEntranceX[i] != (sint16)0x8000)
			break;

		if (i == 3) {
			gGameCommandErrorText = STR_NO_PARK_ENTRANCES;
			return false;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (gParkEntranceX[i] == (sint16)0x8000)
			continue;

		int x = gParkEntranceX[i];
		int y = gParkEntranceY[i];
		int z = gParkEntranceZ[i] / 8;
		int direction = gParkEntranceDirection[i] ^ 2;

		switch (footpath_is_connected_to_map_edge(x, y, z, direction, 0)) {
		case FOOTPATH_SEARCH_NOT_FOUND:
			gGameCommandErrorText = STR_PARK_ENTRANCE_WRONG_DIRECTION_OR_NO_PATH;
			return false;
		case FOOTPATH_SEARCH_INCOMPLETE:
		case FOOTPATH_SEARCH_TOO_COMPLEX:
			gGameCommandErrorText = STR_PARK_ENTRANCE_PATH_INCOMPLETE_OR_COMPLEX;
			return false;
		case FOOTPATH_SEARCH_SUCCESS:
			// Run the search again and unown the path
			footpath_is_connected_to_map_edge(x, y, z, direction, (1 << 5));
			break;
		}
	}

	if (gPeepSpawns[0].x == 0xFFFF && gPeepSpawns[1].x == 0xFFFF) {
		gGameCommandErrorText = STR_PEEP_SPAWNS_NOT_SET;
		return false;
	}

	return true;
}
