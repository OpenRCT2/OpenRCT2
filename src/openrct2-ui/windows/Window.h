/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2-ui/input/KeyboardShortcuts.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2/common.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/windows/tile_inspector.h>
#include <string_view>

using loadsave_callback = void (*)(int32_t result, const utf8* path);
using scenarioselect_callback = void (*)(const utf8* path);
struct Peep;
struct TileElement;
struct Vehicle;
enum class GuestListFilterType : int32_t;
enum class ScatterToolDensity : uint8_t;

extern ScenerySelection gWindowSceneryTabSelections[];
extern uint8_t gWindowSceneryActiveTabIndex;
extern bool gWindowSceneryScatterEnabled;
extern uint16_t gWindowSceneryScatterSize;
extern ScatterToolDensity gWindowSceneryScatterDensity;
extern uint8_t gWindowSceneryPaintEnabled;
extern uint8_t gWindowSceneryRotation;
extern colour_t gWindowSceneryPrimaryColour;
extern colour_t gWindowScenerySecondaryColour;
extern colour_t gWindowSceneryTertiaryColour;
extern bool gWindowSceneryEyedropperEnabled;

rct_window* window_about_open();
void WindowCampaignRefreshRides();
rct_window* window_changelog_open(int personality);
rct_window* window_cheats_open();
rct_window* window_clear_scenery_open();
rct_window* custom_currency_window_open();
rct_window* window_debug_paint_open();
rct_window* window_editor_inventions_list_open();
rct_window* window_editor_main_open();
rct_window* window_editor_objective_options_open();
rct_window* window_editor_scenario_options_open();
rct_window* window_footpath_open();
rct_window* window_guest_open(Peep* peep);
rct_window* window_land_open();
rct_window* window_land_rights_open();
rct_window* window_main_open();
rct_window* window_mapgen_open();
rct_window* window_multiplayer_open();
rct_window* window_network_open();
rct_window* window_music_credits_open();
rct_window* window_news_open();
rct_window* window_news_options_open();
rct_window* window_options_open();
rct_window* window_save_prompt_open();
#ifndef DISABLE_NETWORK
rct_window* window_server_list_open();
rct_window* window_server_start_open();
#endif
rct_window* window_shortcut_change_open(OpenRCT2::Input::Shortcut shortcut, rct_string_id key_string_id);
rct_window* window_shortcut_keys_open();
rct_window* window_staff_list_open();
rct_window* window_staff_open(Peep* peep);
void WindowStaffListRefresh();
rct_window* window_themes_open();
rct_window* window_title_exit_open();
rct_window* window_title_logo_open();
rct_window* window_title_menu_open();
rct_window* window_title_options_open();
rct_window* window_viewport_open();
rct_window* window_water_open();
rct_window* window_view_clipping_open();

// WC_FINANCES
rct_window* window_finances_open();
rct_window* window_finances_research_open();

// WC_PARK_INFORMATION
rct_window* window_park_awards_open();
rct_window* window_park_entrance_open();
rct_window* window_park_guests_open();
rct_window* window_park_objective_open();
rct_window* window_park_rating_open();

rct_window* window_banner_open(rct_windownumber number);
rct_window* window_ride_demolish_prompt_open(Ride* ride);
rct_window* window_ride_refurbish_prompt_open(Ride* ride);
rct_window* window_sign_open(rct_windownumber number);
rct_window* window_sign_small_open(rct_windownumber number);
rct_window* window_player_open(uint8_t id);
rct_window* window_new_campaign_open(int16_t campaignType);

rct_window* window_install_track_open(const utf8* path);
void window_guest_list_refresh_list();
rct_window* window_guest_list_open();
rct_window* window_guest_list_open_with_filter(GuestListFilterType type, int32_t index);
rct_window* window_staff_fire_prompt_open(Peep* peep);
void window_title_editor_open(int32_t tab);
void window_title_command_editor_open(struct TitleSequence* sequence, int32_t command, bool insert);
rct_window* window_scenarioselect_open(scenarioselect_callback callback, bool titleEditor);
rct_window* window_scenarioselect_open(std::function<void(std::string_view)> callback, bool titleEditor, bool disableLocking);

rct_window* window_error_open(rct_string_id title, rct_string_id message, const class Formatter& formatter);
rct_window* window_error_open(std::string_view title, std::string_view message);
struct TrackDesign;
rct_window* window_loadsave_open(
    int32_t type, std::string_view defaultPath, std::function<void(int32_t result, std::string_view)> callback,
    TrackDesign* trackDesign);
rct_window* window_track_place_open(const struct track_design_file_ref* tdFileRef);
rct_window* window_track_manage_open(struct track_design_file_ref* tdFileRef);

void TrackPlaceClearProvisionalTemporarily();
void TrackPlaceRestoreProvisional();

rct_window* window_map_open();
void window_map_reset();

rct_window* window_research_open();
void window_research_development_page_paint(rct_window* w, rct_drawpixelinfo* dpi, rct_widgetindex baseWidgetIndex);
void window_research_funding_page_paint(rct_window* w, rct_drawpixelinfo* dpi, rct_widgetindex baseWidgetIndex);

rct_window* window_new_ride_open();
rct_window* window_new_ride_open_research();
void window_new_ride_init_vars();
void window_new_ride_focus(RideSelection rideItem);

rct_window* window_ride_list_open();
void window_ride_list_refresh_list(rct_window* w);

rct_window* window_ride_main_open(Ride* ride);
rct_window* window_ride_open_track(TileElement* tileElement);
rct_window* window_ride_open_vehicle(Vehicle* vehicle);
void window_ride_measurements_design_cancel();

// rct2: 0x00F635EE
extern RideSelection _window_track_list_item;
rct_window* window_track_list_open(RideSelection item);

void SetMapTooltip(Formatter& ft);
const Formatter& GetMapTooltip();
void window_map_tooltip_update_visibility();

rct_window* window_maze_construction_open();
void window_maze_construction_update_pressed_widgets();

rct_window* window_network_status_open(const char* text, close_callback onClose);
rct_window* window_network_status_open_password();
void window_network_status_close();

void window_text_input_key(rct_window* w, char keychar);
void window_text_input_open(
    rct_window* call_w, rct_widgetindex call_widget, rct_string_id title, rct_string_id description,
    rct_string_id existing_text, uintptr_t existing_args, int32_t maxLength);
void window_text_input_raw_open(
    rct_window* call_w, rct_widgetindex call_widget, rct_string_id title, rct_string_id description,
    const_utf8string existing_text, int32_t maxLength);

void window_text_input_open(
    std::string_view title, std::string_view description, std::string_view initialValue, size_t maxLength,
    std::function<void(std::string_view)> okCallback, std::function<void()> cancelCallback);

rct_window* window_object_load_error_open(utf8* path, size_t numMissingObjects, const rct_object_entry* missingObjects);

rct_window* window_ride_construction_open();
void window_ride_construction_update_active_elements_impl();
void window_ride_construction_update_enabled_track_pieces();

rct_window* window_top_toolbar_open();
bool land_tool_is_active();
bool clear_scenery_tool_is_active();
bool water_tool_is_active();

rct_window* window_scenery_open();
bool window_scenery_set_selected_item(const ScenerySelection& scenery);
void window_scenery_set_default_placement_configuration();
void window_scenery_init();
void window_scenery_reset_selected_scenery_items();

extern uint8_t gToolbarDirtyFlags;
rct_window* window_game_bottom_toolbar_open();
void window_game_bottom_toolbar_invalidate_news_item();

rct_window* window_editor_bottom_toolbar_open();

rct_window* window_tile_inspector_open();
void window_tile_inspector_clear_clipboard();

rct_window* window_editor_object_selection_open();

void window_tooltip_reset(const ScreenCoordsXY& screenCoords);
void window_tooltip_show(const OpenRCT2String& message, ScreenCoordsXY screenCoords);
void window_tooltip_open(rct_window* widgetWindow, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
void window_tooltip_close();

rct_window* window_scenery_scatter_open();

// clang-format off
#define WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, CLOSE_STR) \
    { WindowWidgetType::Frame,    0,  0,          WIDTH - 1, 0, HEIGHT - 1, 0xFFFFFFFF,  STR_NONE }, \
    { WindowWidgetType::Caption,  0,  1,          WIDTH - 2, 1, 14,         TITLE,       STR_WINDOW_TITLE_TIP }, \
    { WindowWidgetType::CloseBox, 0,  WIDTH - 13, WIDTH - 3, 2, 13,         CLOSE_STR, STR_CLOSE_WINDOW_TIP }

#define WINDOW_SHIM(TITLE, WIDTH, HEIGHT) WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, STR_CLOSE_X)
#define WINDOW_SHIM_WHITE(TITLE, WIDTH, HEIGHT) WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, STR_CLOSE_X_WHITE)

// clang-format on
