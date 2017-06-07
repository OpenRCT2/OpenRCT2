#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "rct2/addresses.h"
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
#include "scenario/scenario.h"
#include "util/sawyercoding.h"
#include "util/util.h"
#include "world/banner.h"
#include "world/Climate.h"
#include "world/footpath.h"
#include "world/entrance.h"
#include "world/map.h"
#include "world/park.h"
#include "world/scenery.h"
#include "world/sprite.h"
#include "network/network.h"

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

static void editor_convert_save_to_scenario_callback(sint32 result, const utf8 * path);
static void set_all_land_owned();
static sint32 editor_load_landscape_from_sv4(const char *path);
static sint32 editor_load_landscape_from_sc4(const char *path);
static void editor_finalise_main_view();
static sint32 editor_read_s6(const char *path);
static void editor_clear_map_for_editing(bool fromSave);

/**
 *
 *  rct2: 0x0066FFE1
 */
void editor_load()
{
    audio_stop_all_music_and_sounds();
    object_manager_unload_all_objects();
    object_list_load();
    game_init_all(150);
    gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
    gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
    gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
    gS6Info.category = SCENARIO_CATEGORY_OTHER;
    viewport_init_all();
    rct_window *mainWindow = window_editor_main_open();
    window_set_location(mainWindow, 2400, 2400, 112);
    load_palette();
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
    window_loadsave_set_loadsave_callback(editor_convert_save_to_scenario_callback);
}

static void editor_convert_save_to_scenario_callback(sint32 result, const utf8 * path)
{
    if (result != MODAL_RESULT_OK) {
        return;
    }

    if (!game_load_save_or_scenario(path)) {
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
    audio_stop_all_music_and_sounds();
    gScreenFlags = SCREEN_FLAGS_TRACK_DESIGNER;
    gScreenAge = 0;

    object_manager_unload_all_objects();
    object_list_load();
    game_init_all(150);
    set_all_land_owned();
    gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
    viewport_init_all();
    rct_window * mainWindow = window_editor_main_open();
    window_set_location(mainWindow, 2400, 2400, 112);
    load_palette();
}

/**
 *
 *  rct2: 0x006729FD
 */
void trackmanager_load()
{
    audio_stop_all_music_and_sounds();
    gScreenFlags = SCREEN_FLAGS_TRACK_MANAGER;
    gScreenAge = 0;

    object_manager_unload_all_objects();
    object_list_load();
    game_init_all(150);
    set_all_land_owned();
    gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
    viewport_init_all();
    rct_window *mainWindow = window_editor_main_open();
    window_set_location(mainWindow, 2400, 2400, 112);
    load_palette();
}

/**
 *
 *  rct2: 0x0068ABEC
 */
static void set_all_land_owned()
{
    sint32 mapSize = gMapSize;

    game_do_command(64, 1, 64, 2, GAME_COMMAND_SET_LAND_OWNERSHIP, (mapSize - 3) * 32, (mapSize - 3) * 32);
}

/**
 *
 *  rct2: 0x006758C0
 */
bool editor_load_landscape(const utf8 *path)
{
    // #4996: Make sure the object selection window closes here to prevent unload objects
    //        after we have loaded a new park.
    window_close_all();

    uint32 extension = get_file_extension_type(path);
    switch (extension) {
        case FILE_EXTENSION_SC6:
        case FILE_EXTENSION_SV6:
            return editor_read_s6(path);
        case FILE_EXTENSION_SC4:
            return editor_load_landscape_from_sc4(path);
        case FILE_EXTENSION_SV4:
            return editor_load_landscape_from_sv4(path);
        default:
            return 0;
    }
}

/**
 *
 *  rct2: 0x006A2B02
 */
static sint32 editor_load_landscape_from_sv4(const char *path)
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

static sint32 editor_load_landscape_from_sc4(const char *path)
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
static sint32 editor_read_s6(const char *path)
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
    for (sint32 i = 0; i < MAX_BANNERS; i++) {
        if (gBanners[i].type == 255) {
            gBanners[i].flags &= ~BANNER_FLAG_LINKED_TO_RIDE;
        }
    }

    //
    {
        sint32 i;
        rct_ride *ride;
        FOR_ALL_RIDES(i, ride) {
            user_string_free(ride->name);
        }
    }

    ride_init_all();

    //
    for (sint32 i = 0; i < MAX_SPRITES; i++) {
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

        gParkFlags &= ~PARK_FLAGS_SPRITES_INITIALISED;

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

    sint32 zoom_difference = gSavedViewZoom - viewport->zoom;
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
    window_tile_inspector_clear_clipboard();
}

/**
 *
 *  rct2: 0x006AB9B8
 */
sint32 editor_check_object_selection()
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
    sint32 parkSize = park_calculate_size();
    if (parkSize == 0) {
        gGameCommandErrorText = STR_PARK_MUST_OWN_SOME_LAND;
        return false;
    }

    for (sint32 i = 0; i < MAX_PARK_ENTRANCES; i++) {
        if (gParkEntrances[i].x != MAP_LOCATION_NULL)
            break;

        if (i == MAX_PARK_ENTRANCES - 1) {
            gGameCommandErrorText = STR_NO_PARK_ENTRANCES;
            return false;
        }
    }

    for (sint32 i = 0; i < MAX_PARK_ENTRANCES; i++) {
        if (gParkEntrances[i].x == MAP_LOCATION_NULL)
            continue;

        sint32 x = gParkEntrances[i].x;
        sint32 y = gParkEntrances[i].y;
        sint32 z = gParkEntrances[i].z / 8;
        sint32 direction = gParkEntrances[i].direction ^ 2;

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

    for (sint32 i = 0; i < MAX_PEEP_SPAWNS; i++) {
        if (gPeepSpawns[i].x != PEEP_SPAWN_UNDEFINED)
            break;

        if (i == MAX_PEEP_SPAWNS - 1) {
            gGameCommandErrorText = STR_PEEP_SPAWNS_NOT_SET;
            return false;
        }
    }

    return true;
}

void game_command_edit_scenario_options(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
{
    if (!(*ebx & GAME_COMMAND_FLAG_APPLY)) {
        *ebx = 0;
        return;
    }

    switch (*ecx) {
    case EDIT_SCENARIOOPTIONS_SETNOMONEY:
        if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) {
            if (*edx != 0) {
                gParkFlags |= PARK_FLAGS_NO_MONEY_SCENARIO;
            }
            else {
                gParkFlags &= PARK_FLAGS_NO_MONEY_SCENARIO;
            }
        }
        else {
            if (*edx != 0) {
                gParkFlags |= PARK_FLAGS_NO_MONEY;
            }
            else {
                gParkFlags &= ~PARK_FLAGS_NO_MONEY;
            }
            // Invalidate all windows that have anything to do with finance
            window_invalidate_by_class(WC_RIDE);
            window_invalidate_by_class(WC_PEEP);
            window_invalidate_by_class(WC_PARK_INFORMATION);
            window_invalidate_by_class(WC_FINANCES);
            window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
            window_invalidate_by_class(WC_TOP_TOOLBAR);
        }
        break;
    case EDIT_SCENARIOOPTIONS_SETINITIALCASH:
        gInitialCash = clamp(MONEY(0, 00), *edx, MONEY(1000000, 00));
        gCashEncrypted = ENCRYPT_MONEY(gInitialCash);
        window_invalidate_by_class(WC_FINANCES);
        window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
        break;
    case EDIT_SCENARIOOPTIONS_SETINITIALLOAN:
        gBankLoan = clamp(MONEY(0,00), *edx, MONEY(5000000,00));
        gMaxBankLoan = max(gBankLoan, gMaxBankLoan);
        window_invalidate_by_class(WC_FINANCES);
        break;
    case EDIT_SCENARIOOPTIONS_SETMAXIMUMLOANSIZE:
        gMaxBankLoan = clamp(MONEY(0,00), *edx, MONEY(5000000,00));
        gBankLoan = min(gBankLoan, gMaxBankLoan);
        window_invalidate_by_class(WC_FINANCES);
        break;
    case EDIT_SCENARIOOPTIONS_SETANNUALINTERESTRATE:
        gBankLoanInterestRate = clamp(0, *edx, 80);
        window_invalidate_by_class(WC_FINANCES);
        break;
    case EDIT_SCENARIOOPTIONS_SETFORBIDMARKETINGCAMPAIGNS:
        if (*edx != 0) {
            gParkFlags |= PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
        }
        else {
            gParkFlags &= ~PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
        }
        break;
    case EDIT_SCENARIOOPTIONS_SETAVERAGECASHPERGUEST:
        gGuestInitialCash = clamp(MONEY(0, 00), *edx, MONEY(1000, 00));
        break;
    case EDIT_SCENARIOOPTIONS_SETGUESTINITIALHAPPINESS:
        gGuestInitialHappiness = clamp(40, *edx, 250);
        break;
    case EDIT_SCENARIOOPTIONS_SETGUESTINITIALHUNGER:
        gGuestInitialHunger = clamp(40, *edx, 250);
        break;
    case EDIT_SCENARIOOPTIONS_SETGUESTINITIALTHIRST:
        gGuestInitialThirst = clamp(40, *edx, 250);
        break;
    case EDIT_SCENARIOOPTIONS_SETGUESTSPREFERLESSINTENSERIDES:
        if (*edx != 0) {
            gParkFlags |= PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
        }
        else {
            gParkFlags &= ~PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
        }
        break;
    case EDIT_SCENARIOOPTIONS_SETGUESTSPREFERMOREINTENSERIDES:
        if (*edx != 0) {
            gParkFlags |= PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
        }
        else {
            gParkFlags &= ~PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
        }
        break;
    case EDIT_SCENARIOOPTIONS_SETCOSTTOBUYLAND:
        gLandPrice = clamp(MONEY(5, 00), *edx, MONEY(200, 00));
        break;
    case EDIT_SCENARIOOPTIONS_SETCOSTTOBUYCONSTRUCTIONRIGHTS:
        gLandRightsCost = clamp(MONEY(5,00), *edx, MONEY(200,00));
        break;
    case EDIT_SCENARIOOPTIONS_SETPARKCHARGEMETHOD:
        if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) {
            if (*edx == 0) {
                if (!(gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY)) {
                    gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                    gParkEntranceFee = MONEY(0, 00);
                }
            }
            else {
                if (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) {
                    gParkFlags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                    gParkEntranceFee = MONEY(10, 00);
                }
            }
        }
        else {
            if (*edx == 0) {
                if (!(gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY)) {
                    gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                }
            }
            else {
                if (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) {
                    gParkFlags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                }
            }
            window_invalidate_by_class(WC_PARK_INFORMATION);
            window_invalidate_by_class(WC_RIDE);
        }
        break;
    case EDIT_SCENARIOOPTIONS_SETPARKCHARGEENTRYFEE:
        gParkEntranceFee = clamp(MONEY(0, 00), *edx, MONEY(100, 00));
        window_invalidate_by_class(WC_PARK_INFORMATION);
        break;
    case EDIT_SCENARIOOPTIONS_SETFORBIDTREEREMOVAL:
        if (*edx != 0) {
            gParkFlags |= PARK_FLAGS_FORBID_TREE_REMOVAL;
        }
        else {
            gParkFlags &= ~PARK_FLAGS_FORBID_TREE_REMOVAL;
        }
        break;
    case EDIT_SCENARIOOPTIONS_SETFORBIDLANDSCAPECHANGES:
        if (*edx != 0) {
            gParkFlags |= PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
        }
        else {
            gParkFlags &= ~PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
        }
        break;
    case EDIT_SCENARIOOPTIONS_SETFORBIDHIGHCONSTRUCTION:
        if (*edx != 0) {
            gParkFlags |= PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
        }
        else {
            gParkFlags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
        }
        break;
    case EDIT_SCENARIOOPTIONS_SETPARKRATINGHIGHERDIFFICULTLEVEL:
        if (*edx != 0) {
            gParkFlags |= PARK_FLAGS_DIFFICULT_PARK_RATING;
        }
        else {
            gParkFlags &= ~PARK_FLAGS_DIFFICULT_PARK_RATING;
        }
        break;
    case EDIT_SCENARIOOPTIONS_SETGUESTGENERATIONHIGHERDIFFICULTLEVEL:
        if (*edx != 0) {
            gParkFlags |= PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
        }
        else {
            gParkFlags &= ~PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
        }
        break;
    }
    window_invalidate_by_class(WC_EDITOR_SCENARIO_OPTIONS);
    *ebx = 0;
}
