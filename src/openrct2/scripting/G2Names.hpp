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
#include <string>

namespace OpenRCT2::Scripting
{
    // When adding to this map, also add to IconName in openrct2.d.ts

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
        { "rct1_close_on_pressed", SPR_G2_RCT1_CLOSE_BUTTON_3 },
        { "rct1_test_off", SPR_G2_RCT1_TEST_BUTTON_0 },
        { "rct1_test_off_pressed", SPR_G2_RCT1_TEST_BUTTON_1 },
        { "rct1_test_on", SPR_G2_RCT1_TEST_BUTTON_2 },
        { "rct1_test_on_pressed", SPR_G2_RCT1_TEST_BUTTON_3 },
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
        { "legacy_paths", SPR_G2_LEGACY_PATH_TAB },
        { "path_surface", SPR_G2_PATH_SURFACE_TAB },
        { "ride_station", SPR_G2_RIDE_STATION_TAB },
        { "terrain_edge", SPR_G2_TERRAIN_EDGE_TAB },
        { "hide_vegetation", SPR_G2_BUTTON_HIDE_VEGETATION },
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
        { 32248, SPR_G2_EMPTY },
        { 29363, SPR_G2_PLACEHOLDER },
        { 29357, SPR_G2_LOGO },
        { 29358, SPR_G2_TITLE },
        { 29359, SPR_G2_FASTFORWARD },
        { 29360, SPR_G2_SPEED_ARROW },
        { 29361, SPR_G2_HYPER_ARROW },
        { 29362, SPR_G2_TAB_LAND },
        { 29364, SPR_G2_ZOOM_IN },
        { 29365, SPR_G2_ZOOM_IN_DISABLED },
        { 29366, SPR_G2_ZOOM_OUT },
        { 29367, SPR_G2_ZOOM_OUT_DISABLED },
        { 29368, SPR_G2_TAB_TREE },
        { 29369, SPR_G2_TAB_PENCIL },
        { 29370, SPR_G2_BUTTON_LARGE_SCENERY },
        { 29371, SPR_G2_BUTTON_TREES },
        { 29372, SPR_G2_BUTTON_FOOTPATH },
        { 29373, SPR_G2_RCT1_CLOSE_BUTTON_0 },
        { 29374, SPR_G2_RCT1_CLOSE_BUTTON_1 },
        { 29375, SPR_G2_RCT1_CLOSE_BUTTON_2 },
        { 29376, SPR_G2_RCT1_CLOSE_BUTTON_3 },
        { 29377, SPR_G2_RCT1_TEST_BUTTON_0 },
        { 29378, SPR_G2_RCT1_TEST_BUTTON_1 },
        { 29379, SPR_G2_RCT1_TEST_BUTTON_2 },
        { 29380, SPR_G2_RCT1_TEST_BUTTON_3 },
        { 29381, SPR_G2_RCT1_OPEN_BUTTON_0 },
        { 29382, SPR_G2_RCT1_OPEN_BUTTON_1 },
        { 29383, SPR_G2_RCT1_OPEN_BUTTON_2 },
        { 29384, SPR_G2_RCT1_OPEN_BUTTON_3 },
        { 29385, SPR_G2_TITLE_RESTART },
        { 29386, SPR_G2_TITLE_STOP },
        { 29387, SPR_G2_TITLE_PLAY },
        { 29388, SPR_G2_TITLE_SKIP },
        { 29389, SPR_G2_SANDBOX },
        { 29414, SPR_G2_TAB_NEWS },
        { 29415, SPR_G2_LOCKED },
        { 29416, SPR_G2_MENU_MULTIPLAYER },
        { 29433, SPR_G2_SORT },
        { 29434, SPR_G2_COPY },
        { 29435, SPR_G2_PASTE },
        { 29442, SPR_G2_TOOLBAR_MUTE },
        { 29443, SPR_G2_TOOLBAR_MUTE_PRESSED },
        { 29444, SPR_G2_TOOLBAR_UNMUTE },
        { 29445, SPR_G2_TOOLBAR_UNMUTE_PRESSED },
        { 29461, SPR_G2_SEARCH },
        { 29467, SPR_G2_EYEDROPPER },
        { 29468, SPR_G2_CHAT },
        { 29469, SPR_G2_MAP_NORTH },
        { 29470, SPR_G2_MAP_NORTH_PRESSED },
        { 29471, SPR_G2_MAP_WEST },
        { 29472, SPR_G2_MAP_WEST_PRESSED },
        { 29473, SPR_G2_MAP_SOUTH },
        { 29474, SPR_G2_MAP_SOUTH_PRESSED },
        { 29475, SPR_G2_MAP_EAST },
        { 29476, SPR_G2_MAP_EAST_PRESSED },
        { 29477, SPR_G2_TOOLBAR_MULTIPLAYER },
        { 29478, SPR_G2_TOOLBAR_MULTIPLAYER_PRESSED },
        { 29479, SPR_G2_MULTIPLAYER_SYNC },
        { 29480, SPR_G2_MULTIPLAYER_DESYNC },
        { 29481, SPR_G2_SIMULATE },
        { 29482, SPR_G2_RCT1_SIMULATE_BUTTON_0 },
        { 29483, SPR_G2_RCT1_SIMULATE_BUTTON_1 },
        { 29484, SPR_G2_RCT1_SIMULATE_BUTTON_2 },
        { 29485, SPR_G2_RCT1_SIMULATE_BUTTON_3 },
        { 29486, SPR_G2_LAND_TOOL_SIZE_6 },
        { 29487, SPR_G2_MOUNTAIN_TOOL_EVEN },
        { 29488, SPR_G2_MOUNTAIN_TOOL_ODD },
        { 29489, SPR_G2_SCENERY_SCATTER_LOW },
        { 29490, SPR_G2_SCENERY_SCATTER_MEDIUM },
        { 29491, SPR_G2_SCENERY_SCATTER_HIGH },
        { 29494, SPR_G2_VIEW },
        { 29495, SPR_G2_PATH_RAILINGS_TAB },
        { 29496, SPR_G2_LEGACY_PATH_TAB },
        { 29497, SPR_G2_PATH_SURFACE_TAB },
        { 29498, SPR_G2_RIDE_STATION_TAB },
        { 29499, SPR_G2_TERRAIN_EDGE_TAB },
        { 29500, SPR_G2_BUTTON_HIDE_VEGETATION },
        { 29501, SPR_G2_BUTTON_HIDE_SCENERY },
        { 29502, SPR_G2_BUTTON_HIDE_VEHICLES },
        { 29503, SPR_G2_BUTTON_HIDE_SUPPORTS },
        { 29504, SPR_G2_BUTTON_HIDE_PARTIAL },
        { 29505, SPR_G2_BUTTON_HIDE_FULL },
        { 29506, SPR_G2_LINK_CHAIN },
        { 29507, SPR_G2_SIDEWAYS_TAB },
        { 29508, SPR_G2_SIDEWAYS_TAB_ACTIVE },
        { 29509, SPR_G2_ARROW_UP },
        { 29510, SPR_G2_ARROW_DOWN },
        { 29511, SPR_G2_RELOAD },
    };

    inline uint32_t GetIconByName(const std::string& input)
    {
        auto result = G2SpriteLookupTable.find(input);
        if (result != G2SpriteLookupTable.end())
            return result->second;
        return SPR_G2_EMPTY;
    }

    inline uint32_t NewIconIndex(const uint32_t& input)
    {
        auto result = LegacyIconMap.find(input);
        if (result != LegacyIconMap.end())
            return result->second;
        return input;
    }

    inline uint32_t LegacyIconIndex(const uint32_t& input)
    {
        uint32_t result = input;
        for (auto& i : LegacyIconMap)
        {
            if (i.second == input)
            {
                result = i.first;
                break;
            }
        }
        return result;
    }

} // namespace OpenRCT2::Scripting
