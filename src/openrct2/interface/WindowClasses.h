/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

enum rct_windowclass : uint8_t
{
    WC_MAIN_WINDOW = 0,
    WC_TOP_TOOLBAR = 1,
    WC_BOTTOM_TOOLBAR = 2,
    WC_TOOLTIP = 5,
    WC_DROPDOWN = 6,
    WC_ABOUT = 8,
    // WC_PUBLISHER_CREDITS = 9,
    // WC_MUSIC_CREDITS = 10,
    WC_ERROR = 11,
    WC_RIDE = 12,
    WC_RIDE_CONSTRUCTION = 13,
    WC_SAVE_PROMPT = 14,
    WC_RIDE_LIST = 15,
    WC_CONSTRUCT_RIDE = 16,
    WC_DEMOLISH_RIDE_PROMPT = 17,
    WC_SCENERY = 18,
    WC_OPTIONS = 19,
    WC_FOOTPATH = 20,
    WC_LAND = 21,
    WC_WATER = 22,
    WC_PEEP = 23,
    WC_GUEST_LIST = 24,
    WC_STAFF_LIST = 25,
    WC_FIRE_PROMPT = 26,
    WC_PARK_INFORMATION = 27,
    WC_FINANCES = 28,
    WC_TITLE_MENU = 29,
    WC_TITLE_EXIT = 30,
    WC_RECENT_NEWS = 31,
    WC_SCENARIO_SELECT = 32,
    WC_TRACK_DESIGN_LIST = 33,
    WC_TRACK_DESIGN_PLACE = 34,
    WC_NEW_CAMPAIGN = 35,
    WC_KEYBOARD_SHORTCUT_LIST = 36,
    WC_CHANGE_KEYBOARD_SHORTCUT = 37,
    WC_MAP = 38,
    WC_TITLE_LOGO = 39,
    WC_BANNER = 40,
    WC_MAP_TOOLTIP = 41,
    WC_EDITOR_OBJECT_SELECTION = 42,
    WC_EDITOR_INVENTION_LIST = 43,
    WC_EDITOR_INVENTION_LIST_DRAG = 44,
    WC_EDITOR_SCENARIO_OPTIONS = 45,
    WC_EDITOR_OBJECTIVE_OPTIONS = 46,
    WC_MANAGE_TRACK_DESIGN = 47,
    WC_TRACK_DELETE_PROMPT = 48,
    WC_INSTALL_TRACK = 49,
    WC_CLEAR_SCENERY = 50,
    WC_SCENERY_SCATTER = 51,
    WC_NOTIFICATION_OPTIONS = 109,
    WC_CHEATS = 110,
    WC_RESEARCH = 111,
    WC_VIEWPORT = 112,
    WC_TEXTINPUT = 113,
    WC_MAPGEN = 114,
    WC_LOADSAVE = 115,
    WC_LOADSAVE_OVERWRITE_PROMPT = 116,
    WC_TITLE_OPTIONS = 117,
    WC_LAND_RIGHTS = 118,
    WC_THEMES = 119,
    WC_TILE_INSPECTOR = 120,
    WC_CHANGELOG = 121,
    WC_MULTIPLAYER = 124,
    WC_PLAYER = 125,
    WC_NETWORK_STATUS = 126,
    WC_SERVER_LIST = 127,
    WC_SERVER_START = 128,
    WC_CUSTOM_CURRENCY_CONFIG = 129,
    WC_DEBUG_PAINT = 130,
    WC_VIEW_CLIPPING = 131,
    WC_OBJECT_LOAD_ERROR = 132,
    WC_PATROL_AREA = 133,
    WC_TRANSPARENCY = 134,

    // Only used for colour schemes
    WC_STAFF = 220,
    WC_EDITOR_TRACK_BOTTOM_TOOLBAR = 221,
    WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR = 222,
    WC_CHAT = 223,
    WC_CONSOLE = 224,
    WC_CUSTOM = 225,

    WC_NULL = 255,
};
