/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumMap.hpp"
#include "../sprites.h"

#include <map>

namespace OpenRCT2::Scripting
{
    static EnumMap<uint32_t> G2SpriteLookupTable = {
        { "empty", SPR_G2_EMPTY },
        { "placeholder", SPR_G2_PLACEHOLDER },
        { "logo", SPR_G2_LOGO },
        { "logo_text", SPR_G2_TITLE },
        { "fast_forward", SPR_G2_FASTFORWARD },
        { "game_speed_indicator", SPR_G2_SPEED_ARROW },
        { "game_speed_indicator_double", SPR_G2_HYPER_ARROW },
        { "map_gen_land", SPR_G2_TAB_LAND },
        { "zoom_in", SPR_G2_ZOOM_IN },
        { "zoom_in_background", SPR_G2_ZOOM_IN_DISABLED },
        { "zoom_out", SPR_G2_ZOOM_OUT },
        { "zoom_out_background", SPR_G2_ZOOM_OUT_DISABLED },
        { "map_gen_trees", SPR_G2_TAB_TREE },
        { "map_gen_noise", SPR_G2_TAB_PENCIL },
        { "large_scenery", SPR_G2_BUTTON_LARGE_SCENERY },
        { "small_scenery", SPR_G2_BUTTON_TREES },
        { "paths", SPR_G2_BUTTON_FOOTPATH },
        { "rct1_close_off", SPR_G2_RCT1_CLOSE_BUTTON_0 },
        { "rct1_close_off_pressed", SPR_G2_RCT1_CLOSE_BUTTON_1 },
        { "rct1_close_on", SPR_G2_RCT1_CLOSE_BUTTON_2 },
        { "rct1_close_off_pressed", SPR_G2_RCT1_CLOSE_BUTTON_3 },
        { "rct1_test_off", SPR_G2_RCT1_TEST_BUTTON_0 },
        { "rct1_test_off_pressed", SPR_G2_RCT1_TEST_BUTTON_1 },
        { "rct1_test_on", SPR_G2_RCT1_TEST_BUTTON_2 },
        { "rct1_test_off_pressed", SPR_G2_RCT1_TEST_BUTTON_3 },
        { "rct1_open_off", SPR_G2_RCT1_OPEN_BUTTON_0 },
        { "rct1_open_off_pressed", SPR_G2_RCT1_OPEN_BUTTON_1 },
        { "rct1_open_on", SPR_G2_RCT1_OPEN_BUTTON_2 },
        { "rct1_open_on_pressed", SPR_G2_RCT1_OPEN_BUTTON_3 },
        { "title_restart", SPR_G2_TITLE_RESTART },
        { "title_stop", SPR_G2_TITLE_STOP },
        { "title_play", SPR_G2_TITLE_PLAY },
        { "title_skip", SPR_G2_TITLE_SKIP },
        { "cheats", SPR_G2_SANDBOX },
        { "news_messages", SPR_G2_TAB_NEWS },
        { "server_password", SPR_G2_LOCKED },
        { "multiplayer", SPR_G2_MENU_MULTIPLAYER },
        { "sort", SPR_G2_SORT },
        { "copy", SPR_G2_COPY },
        { "paste", SPR_G2_PASTE },
        { "mute", SPR_G2_TOOLBAR_MUTE },
        { "mute_pressed", SPR_G2_TOOLBAR_MUTE_PRESSED },
        { "unmute", SPR_G2_TOOLBAR_UNMUTE },
        { "unmute_pressed", SPR_G2_TOOLBAR_UNMUTE_PRESSED },
        { "search", SPR_G2_SEARCH },
        { "eyedropper", SPR_G2_EYEDROPPER },
        { "chat", SPR_G2_CHAT },
        { "map_north", SPR_G2_MAP_NORTH },
        { "map_north_pressed", SPR_G2_MAP_NORTH_PRESSED },
        { "map_west", SPR_G2_MAP_WEST },
        { "map_west_pressed", SPR_G2_MAP_WEST_PRESSED },
        { "map_south", SPR_G2_MAP_SOUTH },
        { "map_south_pressed", SPR_G2_MAP_SOUTH_PRESSED },
        { "map_east", SPR_G2_MAP_EAST },
        { "map_east_pressed", SPR_G2_MAP_EAST_PRESSED },
        { "multiplayer_toolbar", SPR_G2_TOOLBAR_MULTIPLAYER },
        { "multiplayer_toolbar_pressed", SPR_G2_TOOLBAR_MULTIPLAYER_PRESSED },
        { "multiplayer_sync", SPR_G2_MULTIPLAYER_SYNC },
        { "multiplayer_desync", SPR_G2_MULTIPLAYER_DESYNC },
        { "simulate", SPR_G2_SIMULATE },
        { "rct1_simulate_off", SPR_G2_RCT1_SIMULATE_BUTTON_0 },
        { "rct1_simulate_off_pressed", SPR_G2_RCT1_SIMULATE_BUTTON_1 },
        { "rct1_simulate_on", SPR_G2_RCT1_SIMULATE_BUTTON_2 },
        { "rct1_simulate_on_pressed", SPR_G2_RCT1_SIMULATE_BUTTON_3 },
        { "normal_selection_6x6", SPR_G2_LAND_TOOL_SIZE_6 },
        { "mountain_tool_even", SPR_G2_MOUNTAIN_TOOL_EVEN },
        { "mountain_tool_odd", SPR_G2_MOUNTAIN_TOOL_ODD },
        { "scenery_scatter_low", SPR_G2_SCENERY_SCATTER_LOW },
        { "scenery_scatter_medium", SPR_G2_SCENERY_SCATTER_MEDIUM },
        { "scenery_scatter_high", SPR_G2_SCENERY_SCATTER_HIGH },
        { "view", SPR_G2_VIEW },
        { "path_railings", SPR_G2_PATH_RAILINGS_TAB },
        { "legacy_path", SPR_G2_LEGACY_PATH_TAB },
        { "path_surface", SPR_G2_PATH_SURFACE_TAB },
        { "ride_station", SPR_G2_RIDE_STATION_TAB },
        { "terrain_edge", SPR_G2_TERRAIN_EDGE_TAB },
        { "hide_vegitation", SPR_G2_BUTTON_HIDE_VEGETATION },
        { "hide_scenery", SPR_G2_BUTTON_HIDE_SCENERY },
        { "hide_vehicles", SPR_G2_BUTTON_HIDE_VEHICLES },
        { "hide_supports", SPR_G2_BUTTON_HIDE_SUPPORTS },
        { "hide_partial", SPR_G2_BUTTON_HIDE_PARTIAL },
        { "hide_full", SPR_G2_BUTTON_HIDE_FULL },
        { "link_chain", SPR_G2_LINK_CHAIN },
        { "sideways_tab", SPR_G2_SIDEWAYS_TAB },
        { "sideways_tab_active", SPR_G2_SIDEWAYS_TAB_ACTIVE },
        { "arrow_up", SPR_G2_ARROW_UP },
        { "arrow_down", SPR_G2_ARROW_DOWN },
        { "reload", SPR_G2_RELOAD },
        { "glassy_recolourable", SPR_G2_SURFACE_GLASSY_RECOLOURABLE },
        { "selection_edge_nw", SPR_G2_SELECTION_EDGE_NW },
        { "selection_edge_ne", SPR_G2_SELECTION_EDGE_NE },
        { "selection_edge_sw", SPR_G2_SELECTION_EDGE_SW },
        { "selection_edge_se", SPR_G2_SELECTION_EDGE_SE },
    };

    // Maps legacy G2 icons sprite indices to new sprite indices

    static std::map<uint32_t, uint32_t> LegacyIconMap{
        { 32248, 29357 }, { 29363, 29358 }, { 29357, 29359 }, { 29358, 29360 }, { 29359, 29361 }, { 29360, 29362 },
        { 29361, 29363 }, { 29362, 29364 }, { 29364, 29365 }, { 29365, 29366 }, { 29366, 29367 }, { 29367, 29368 },
        { 29368, 29369 }, { 29369, 29370 }, { 29370, 29371 }, { 29371, 29372 }, { 29372, 29373 }, { 29373, 29374 },
        { 29374, 29375 }, { 29375, 29376 }, { 29376, 29377 }, { 29377, 29378 }, { 29378, 29379 }, { 29379, 29380 },
        { 29380, 29381 }, { 29381, 29382 }, { 29382, 29383 }, { 29383, 29384 }, { 29384, 29385 }, { 29385, 29386 },
        { 29386, 29387 }, { 29387, 29388 }, { 29388, 29389 }, { 29389, 29390 }, { 29414, 29391 }, { 29415, 29392 },
        { 29416, 29393 }, { 29433, 29394 }, { 29434, 29395 }, { 29435, 29396 }, { 29442, 29397 }, { 29442, 29398 },
        { 29444, 29399 }, { 29445, 29400 }, { 29461, 29401 }, { 29467, 29402 }, { 29468, 29403 }, { 29469, 29404 },
        { 29470, 29405 }, { 29471, 29406 }, { 29472, 29407 }, { 29473, 29408 }, { 29474, 29409 }, { 29475, 29410 },
        { 29476, 29411 }, { 29477, 29412 }, { 29478, 29413 }, { 29479, 29414 }, { 29480, 29415 }, { 29481, 29416 },
        { 29482, 29417 }, { 29483, 29418 }, { 29484, 29419 }, { 29485, 29420 }, { 29486, 29421 }, { 29487, 29422 },
        { 29488, 29423 }, { 29489, 29424 }, { 29490, 29425 }, { 29491, 29426 }, { 29494, 29427 }, { 29495, 29428 },
        { 29496, 29429 }, { 29497, 29430 }, { 29498, 29431 }, { 29499, 29432 }, { 29500, 29433 }, { 29501, 29434 },
        { 29502, 29435 }, { 29503, 29436 }, { 29504, 29439 }, { 29505, 29438 }, { 29506, 29439 }, { 29507, 29440 },
        { 29508, 29441 }, { 29509, 29442 }, { 29510, 29443 }, { 29511, 29444 },
    };

    inline uint32_t GetIconByName(const std::string& input)
    {
        auto result = G2SpriteLookupTable.find(input);
        if (result != G2SpriteLookupTable.end())
            return result->second;
        return SPR_G2_EMPTY;
    }

    inline uint32_t FilterLegacyIconSprites(const uint32_t& input)
    {
        auto result = LegacyIconMap.find(input);
        if (result != LegacyIconMap.end())
            return result->second;
        return input;
    }

} // namespace OpenRCT2::Scripting
