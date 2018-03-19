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

#include <openrct2-ui/interface/Window.h>
#include <openrct2/windows/tile_inspector.h>
#include <openrct2/ride/Ride.h>

using loadsave_callback       = void (*)(sint32 result, const utf8 * path);
using scenarioselect_callback = void (*)(const utf8 *path);
struct rct_peep;
struct rct_tile_element;
struct rct_vehicle;

rct_window * window_about_open();
rct_window * window_changelog_open();
rct_window * window_cheats_open();
rct_window * window_clear_scenery_open();
rct_window * custom_currency_window_open();
rct_window * window_debug_paint_open();
rct_window * window_editor_inventions_list_open();
rct_window * window_editor_main_open();
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
void window_guest_list_refresh_list();
rct_window * window_guest_list_open();
rct_window * window_guest_list_open_with_filter(sint32 type, sint32 index);
rct_window * window_staff_fire_prompt_open(rct_peep* peep);
void window_title_editor_open(sint32 tab);
void window_title_command_editor_open(struct TitleSequence * sequence, sint32 command, bool insert);
rct_window * window_scenarioselect_open(scenarioselect_callback callback, bool titleEditor);

rct_window * window_error_open(rct_string_id title, rct_string_id message);

rct_window * window_loadsave_open(sint32 type, const char *defaultName);
void window_loadsave_set_loadsave_callback(loadsave_callback cb);
rct_window * window_track_place_open(const struct track_design_file_ref *tdFileRef);
rct_window * window_track_manage_open(struct track_design_file_ref *tdFileRef);

rct_window * window_map_open();
void window_map_reset();

rct_window * window_research_open();
void window_research_development_page_paint(rct_window * w, rct_drawpixelinfo * dpi, rct_widgetindex baseWidgetIndex);
void window_research_funding_page_paint(rct_window * w, rct_drawpixelinfo * dpi, rct_widgetindex baseWidgetIndex);

rct_window * window_new_ride_open();
rct_window * window_new_ride_open_research();
void window_new_ride_init_vars();
void window_new_ride_focus(ride_list_item rideItem);

rct_window * window_ride_list_open();
void window_ride_list_refresh_list(rct_window * w);

rct_window * window_ride_main_open(sint32 rideIndex);
rct_window * window_ride_open_track(rct_tile_element * tileElement);
rct_window * window_ride_open_vehicle(rct_vehicle * vehicle);
void window_ride_measurements_design_cancel();

// rct2: 0x00F635EE
extern ride_list_item _window_track_list_item;
rct_window * window_track_list_open(ride_list_item item);

void window_map_tooltip_update_visibility();

rct_window * window_maze_construction_open();
void window_maze_construction_update_pressed_widgets();

rct_window * window_network_status_open(const char * text, close_callback onClose);
rct_window * window_network_status_open_password();
void window_network_status_close();

void window_text_input_key(rct_window * w, char keychar);
void window_text_input_open(rct_window * call_w, rct_widgetindex call_widget, rct_string_id title, rct_string_id description, rct_string_id existing_text, uintptr_t existing_args, sint32 maxLength);
void window_text_input_raw_open(rct_window * call_w, rct_widgetindex call_widget, rct_string_id title, rct_string_id description, const_utf8string existing_text, sint32 maxLength);

rct_window * window_object_load_error_open(utf8 * path, size_t numMissingObjects, const rct_object_entry * missingObjects);

rct_window * window_ride_construction_open();
void window_ride_construction_update_active_elements_impl();
void window_ride_construction_update_enabled_track_pieces();

rct_window * window_top_toolbar_open();
bool land_tool_is_active();
bool clear_scenery_tool_is_active();
bool water_tool_is_active();

rct_window * window_scenery_open();
bool window_scenery_set_selected_item(sint32 sceneryId);
void window_scenery_set_default_placement_configuration();
void window_scenery_init();
void window_scenery_reset_selected_scenery_items();

extern uint8 gToolbarDirtyFlags;
rct_window * window_game_bottom_toolbar_open();
void window_game_bottom_toolbar_invalidate_news_item();

rct_window * window_editor_bottom_toolbar_open();

rct_window * window_tile_inspector_open();
void window_tile_inspector_clear_clipboard();
void window_tile_inspector_set_page(rct_window *w, TILE_INSPECTOR_PAGE page);
void window_tile_inspector_auto_set_buttons(rct_window *w);

rct_window * window_editor_object_selection_open();

void window_tooltip_reset(sint32 x, sint32 y);
void window_tooltip_show(rct_string_id id, sint32 x, sint32 y);
void window_tooltip_open(rct_window * widgetWindow, rct_widgetindex widgetIndex, sint32 x, sint32 y);
void window_tooltip_close();
