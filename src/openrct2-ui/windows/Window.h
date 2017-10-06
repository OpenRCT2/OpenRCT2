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

#pragma once

#include <openrct2/common.h>

#include <openrct2/interface/window.h>

typedef void (*loadsave_callback)(sint32 result, const utf8 * path);
typedef void (*scenarioselect_callback)(const utf8 *path);

rct_window * window_about_open();
rct_window * window_changelog_open();
rct_window * window_cheats_open();
rct_window * window_clear_scenery_open();
rct_window * custom_currency_window_open();
rct_window * window_debug_paint_open();
rct_window * window_editor_inventions_list_open();
rct_window * window_editor_objective_options_open();
rct_window * window_editor_scenario_options_open();
rct_window * window_footpath_open();
rct_window * window_guest_open(rct_peep* peep);
rct_window * window_land_open();
rct_window * window_land_rights_open();
rct_window * window_main_open();
rct_window * window_mapgen_open();
rct_window * window_multiplayer_open();
rct_window * window_music_credits_open();
rct_window * window_news_open();
rct_window * window_news_options_open();
rct_window * window_options_open();
rct_window * window_save_prompt_open();
rct_window * window_server_list_open();
rct_window * window_server_start_open();
rct_window * window_shortcut_change_open(sint32 selected_key);
rct_window * window_shortcut_keys_open();
rct_window * window_staff_list_open();
rct_window * window_staff_open(rct_peep* peep);
rct_window * window_themes_open();
rct_window * window_title_exit_open();
rct_window * window_title_logo_open();
rct_window * window_title_menu_open();
rct_window * window_title_options_open();
rct_window * window_viewport_open();
rct_window * window_water_open();
rct_window * window_view_clipping_open();

// WC_FINANCES
rct_window * window_finances_open();
rct_window * window_finances_research_open();

// WC_PARK_INFORMATION
rct_window * window_park_awards_open();
rct_window * window_park_entrance_open();
rct_window * window_park_guests_open();
rct_window * window_park_objective_open();
rct_window * window_park_rating_open();

rct_window * window_banner_open(rct_windownumber number);
rct_window * window_ride_demolish_prompt_open(sint32 rideIndex);
rct_window * window_sign_open(rct_windownumber number);
rct_window * window_sign_small_open(rct_windownumber number);
rct_window * window_player_open(uint8 id);
rct_window * window_new_campaign_open(sint16 campaignType);

rct_window * window_install_track_open(const utf8* path);
void window_guest_list_init_vars();
rct_window * window_guest_list_open();
rct_window * window_guest_list_open_with_filter(sint32 type, sint32 index);
rct_window * window_staff_fire_prompt_open(rct_peep* peep);
void window_title_editor_open(sint32 tab);
void window_title_command_editor_open(struct TitleSequence * sequence, sint32 command, bool insert);
rct_window * window_scenarioselect_open(scenarioselect_callback callback);

rct_window * window_error_open(rct_string_id title, rct_string_id message);

rct_window * window_loadsave_open(sint32 type, const char *defaultName);
void window_loadsave_set_loadsave_callback(loadsave_callback cb);
rct_window * window_track_place_open(const struct track_design_file_ref *tdFileRef);
rct_window * window_track_manage_open(struct track_design_file_ref *tdFileRef);

rct_window * window_map_open();
void window_map_reset();
