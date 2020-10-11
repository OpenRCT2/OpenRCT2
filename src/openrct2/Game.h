/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"

#include <string>

struct ParkLoadResult;
struct rct_s6_data;

enum GAME_COMMAND
{
    GAME_COMMAND_SET_RIDE_APPEARANCE,          // GA
    GAME_COMMAND_SET_LAND_HEIGHT,              // GA
    GAME_COMMAND_TOGGLE_PAUSE,                 // GA
    GAME_COMMAND_PLACE_TRACK,                  // GA
    GAME_COMMAND_REMOVE_TRACK,                 // GA
    GAME_COMMAND_LOAD_OR_QUIT,                 // GA
    GAME_COMMAND_CREATE_RIDE,                  // GA
    GAME_COMMAND_DEMOLISH_RIDE,                // GA
    GAME_COMMAND_SET_RIDE_STATUS,              // GA
    GAME_COMMAND_SET_RIDE_VEHICLES,            // GA
    GAME_COMMAND_SET_RIDE_NAME,                // GA
    GAME_COMMAND_SET_RIDE_SETTING,             // GA
    GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT,  // GA
    GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT, // GA
    GAME_COMMAND_REMOVE_SCENERY,               // GA
    GAME_COMMAND_PLACE_SCENERY,                // GA
    GAME_COMMAND_SET_WATER_HEIGHT,             // GA
    GAME_COMMAND_PLACE_PATH,                   // GA
    GAME_COMMAND_PLACE_PATH_FROM_TRACK,        // GA
    GAME_COMMAND_REMOVE_PATH,                  // GA
    GAME_COMMAND_CHANGE_SURFACE_STYLE,         // GA
    GAME_COMMAND_SET_RIDE_PRICE,               // GA
    GAME_COMMAND_SET_GUEST_NAME,               // GA
    GAME_COMMAND_SET_STAFF_NAME,               // GA
    GAME_COMMAND_RAISE_LAND,                   // GA
    GAME_COMMAND_LOWER_LAND,                   // GA
    GAME_COMMAND_EDIT_LAND_SMOOTH,             // GA
    GAME_COMMAND_RAISE_WATER,                  // GA
    GAME_COMMAND_LOWER_WATER,                  // GA
    GAME_COMMAND_SET_BRAKES_SPEED,             // GA
    GAME_COMMAND_HIRE_NEW_STAFF_MEMBER,        // GA
    GAME_COMMAND_SET_STAFF_PATROL,             // GA
    GAME_COMMAND_FIRE_STAFF_MEMBER,            // GA
    GAME_COMMAND_SET_STAFF_ORDERS,             // GA
    GAME_COMMAND_SET_PARK_NAME,                // GA
    GAME_COMMAND_SET_PARK_OPEN,                // GA
    GAME_COMMAND_BUY_LAND_RIGHTS,              // GA
    GAME_COMMAND_PLACE_PARK_ENTRANCE,          // GA
    GAME_COMMAND_REMOVE_PARK_ENTRANCE,         // GA
    GAME_COMMAND_SET_MAZE_TRACK,               // GA
    GAME_COMMAND_SET_PARK_ENTRANCE_FEE,        // GA
    GAME_COMMAND_SET_STAFF_COLOUR,             // GA
    GAME_COMMAND_PLACE_WALL,                   // GA
    GAME_COMMAND_REMOVE_WALL,                  // GA
    GAME_COMMAND_PLACE_LARGE_SCENERY,          // GA
    GAME_COMMAND_REMOVE_LARGE_SCENERY,         // GA
    GAME_COMMAND_SET_CURRENT_LOAN,             // GA
    GAME_COMMAND_SET_RESEARCH_FUNDING,         // GA
    GAME_COMMAND_PLACE_TRACK_DESIGN,           // GA
    GAME_COMMAND_START_MARKETING_CAMPAIGN,     // GA
    GAME_COMMAND_PLACE_MAZE_DESIGN,            // GA
    GAME_COMMAND_PLACE_BANNER,                 // GA
    GAME_COMMAND_REMOVE_BANNER,                // GA
    GAME_COMMAND_SET_SCENERY_COLOUR,           // GA
    GAME_COMMAND_SET_WALL_COLOUR,              // GA
    GAME_COMMAND_SET_LARGE_SCENERY_COLOUR,     // GA
    GAME_COMMAND_SET_BANNER_COLOUR,            // GA
    GAME_COMMAND_SET_LAND_OWNERSHIP,           // GA
    GAME_COMMAND_CLEAR_SCENERY,                // GA
    GAME_COMMAND_SET_BANNER_NAME,              // GA
    GAME_COMMAND_SET_SIGN_NAME,                // GA
    GAME_COMMAND_SET_BANNER_STYLE,             // GA
    GAME_COMMAND_SET_SIGN_STYLE,               // GA
    GAME_COMMAND_SET_PLAYER_GROUP,             // GA
    GAME_COMMAND_MODIFY_GROUPS,                // GA
    GAME_COMMAND_KICK_PLAYER,                  // GA
    GAME_COMMAND_CHEAT,                        // GA
    GAME_COMMAND_PICKUP_GUEST,                 // GA
    GAME_COMMAND_PICKUP_STAFF,                 // GA
    GAME_COMMAND_BALLOON_PRESS,                // GA
    GAME_COMMAND_MODIFY_TILE,                  // GA
    GAME_COMMAND_EDIT_SCENARIO_OPTIONS,        // GA
    GAME_COMMAND_PLACE_PEEP_SPAWN,             // GA
    GAME_COMMAND_SET_CLIMATE,                  // GA
    GAME_COMMAND_SET_COLOUR_SCHEME,            // GA
    GAME_COMMAND_SET_STAFF_COSTUME,            // GA
    GAME_COMMAND_PLACE_FOOTPATH_SCENERY,       // GA
    GAME_COMMAND_REMOVE_FOOTPATH_SCENERY,      // GA
    GAME_COMMAND_GUEST_SET_FLAGS,              // GA
    GAME_COMMAND_SET_DATE,                     // GA
    GAME_COMMAND_CUSTOM,                       // GA
    GAME_COMMAND_COUNT,
};

enum : uint32_t
{
    GAME_COMMAND_FLAG_APPLY = (1 << 0),  // If this flag is set, the command is applied, otherwise only the cost is retrieved
    GAME_COMMAND_FLAG_REPLAY = (1 << 1), // Command was issued from replay manager.
    GAME_COMMAND_FLAG_2 = (1 << 2),      // Unused
    GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED = (1 << 3), // Allow while paused
    GAME_COMMAND_FLAG_4 = (1 << 4),                   // Unused
    GAME_COMMAND_FLAG_NO_SPEND = (1 << 5),            // Game command is not networked
    GAME_COMMAND_FLAG_GHOST = (1 << 6),               // Game command is not networked
    GAME_COMMAND_FLAG_PATH_SCENERY = (1 << 7),
    GAME_COMMAND_FLAG_NETWORKED = (1u << 31) // Game command is coming from network
};

enum
{
    GAME_PAUSED_NORMAL = 1 << 0,
    GAME_PAUSED_MODAL = 1 << 1,
    GAME_PAUSED_SAVING_TRACK = 1 << 2,
};

enum
{
    ERROR_TYPE_NONE = 0,
    ERROR_TYPE_GENERIC = 254,
    ERROR_TYPE_FILE_LOAD = 255
};

extern rct_string_id gGameCommandErrorTitle;
extern rct_string_id gGameCommandErrorText;

extern uint32_t gCurrentTicks;
extern uint32_t gCurrentRealTimeTicks;

extern uint16_t gCurrentDeltaTime;
extern uint8_t gGamePaused;
extern int32_t gGameSpeed;
extern bool gDoSingleUpdate;
extern float gDayNightCycle;
extern bool gInUpdateCode;
extern bool gInMapInitCode;
extern std::string gCurrentLoadedPath;

extern bool gLoadKeepWindowsOpen;

void game_reset_speed();
void game_increase_game_speed();
void game_reduce_game_speed();

void game_create_windows();
void reset_all_sprite_quadrant_placements();
void update_palette_effects();

void game_load_or_quit_no_save_prompt();
void load_from_sv6(const char* path);
void game_load_init();
void game_load_scripts();
void game_unload_scripts();
void pause_toggle();
bool game_is_paused();
bool game_is_not_paused();
void save_game();
void* create_save_game_as_intent();
void save_game_as();
void save_game_cmd(const utf8* name = nullptr);
void save_game_with_name(const utf8* name);
void game_autosave();
void game_convert_strings_to_utf8();
void game_convert_news_items_to_utf8();
void game_convert_strings_to_rct2(rct_s6_data* s6);
void utf8_to_rct2_self(char* buffer, size_t length);
void rct2_to_utf8_self(char* buffer, size_t length);
void game_fix_save_vars();
void start_silent_record();
bool stop_silent_record();
