/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
    GAME_COMMAND_SET_WATER_HEIGHT,
    GAME_COMMAND_PLACE_PATH,
    GAME_COMMAND_PLACE_PATH_FROM_TRACK,
    GAME_COMMAND_REMOVE_PATH,
    GAME_COMMAND_CHANGE_SURFACE_STYLE,
    GAME_COMMAND_SET_RIDE_PRICE, // GA
    GAME_COMMAND_SET_GUEST_NAME, // GA
    GAME_COMMAND_SET_STAFF_NAME, // GA
    GAME_COMMAND_RAISE_LAND,
    GAME_COMMAND_LOWER_LAND,
    GAME_COMMAND_EDIT_LAND_SMOOTH,
    GAME_COMMAND_RAISE_WATER,
    GAME_COMMAND_LOWER_WATER,
    GAME_COMMAND_SET_BRAKES_SPEED,
    GAME_COMMAND_HIRE_NEW_STAFF_MEMBER,
    GAME_COMMAND_SET_STAFF_PATROL,
    GAME_COMMAND_FIRE_STAFF_MEMBER,
    GAME_COMMAND_SET_STAFF_ORDERS, // GA
    GAME_COMMAND_SET_PARK_NAME,    // GA
    GAME_COMMAND_SET_PARK_OPEN,
    GAME_COMMAND_BUY_LAND_RIGHTS,
    GAME_COMMAND_PLACE_PARK_ENTRANCE, // GA
    GAME_COMMAND_REMOVE_PARK_ENTRANCE,
    GAME_COMMAND_SET_MAZE_TRACK,
    GAME_COMMAND_SET_PARK_ENTRANCE_FEE, // GA
    GAME_COMMAND_SET_STAFF_COLOUR,      // GA
    GAME_COMMAND_PLACE_WALL,
    GAME_COMMAND_REMOVE_WALL, // GA
    GAME_COMMAND_PLACE_LARGE_SCENERY,
    GAME_COMMAND_REMOVE_LARGE_SCENERY, // GA
    GAME_COMMAND_SET_CURRENT_LOAN,     // GA
    GAME_COMMAND_SET_RESEARCH_FUNDING, // GA
    GAME_COMMAND_PLACE_TRACK_DESIGN,
    GAME_COMMAND_START_MARKETING_CAMPAIGN, // GA
    GAME_COMMAND_PLACE_MAZE_DESIGN,
    GAME_COMMAND_PLACE_BANNER,
    GAME_COMMAND_REMOVE_BANNER,
    GAME_COMMAND_SET_SCENERY_COLOUR,
    GAME_COMMAND_SET_WALL_COLOUR,
    GAME_COMMAND_SET_LARGE_SCENERY_COLOUR,
    GAME_COMMAND_SET_BANNER_COLOUR,
    GAME_COMMAND_SET_LAND_OWNERSHIP,
    GAME_COMMAND_CLEAR_SCENERY,   // GA
    GAME_COMMAND_SET_BANNER_NAME, // GA
    GAME_COMMAND_SET_SIGN_NAME,   // GA
    GAME_COMMAND_SET_BANNER_STYLE,
    GAME_COMMAND_SET_SIGN_STYLE, // GA
    GAME_COMMAND_SET_PLAYER_GROUP,
    GAME_COMMAND_MODIFY_GROUPS,
    GAME_COMMAND_KICK_PLAYER,
    GAME_COMMAND_CHEAT,
    GAME_COMMAND_PICKUP_GUEST,
    GAME_COMMAND_PICKUP_STAFF,
    GAME_COMMAND_BALLOON_PRESS,
    GAME_COMMAND_MODIFY_TILE,
    GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
    GAME_COMMAND_PLACE_PEEP_SPAWN,  // GA, TODO: refactor to separate array for just game actions
    GAME_COMMAND_SET_CLIMATE,       // GA
    GAME_COMMAND_SET_COLOUR_SCHEME, // GA
    GAME_COMMAND_SET_STAFF_COSTUME, // GA
    GAME_COMMAND_COUNT,
};

enum : uint32_t
{
    GAME_COMMAND_FLAG_APPLY = (1 << 0),  // If this flag is set, the command is applied, otherwise only the cost is retrieved
    GAME_COMMAND_FLAG_REPLAY = (1 << 1), // Command was issued from replay manager.
    GAME_COMMAND_FLAG_2 = (1 << 2),
    GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED = (1 << 3), // Allow while paused
    GAME_COMMAND_FLAG_4 = (1 << 4),
    GAME_COMMAND_FLAG_5 = (1 << 5),
    GAME_COMMAND_FLAG_GHOST = (1 << 6),
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

using GAME_COMMAND_POINTER = void(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
using GAME_COMMAND_CALLBACK_POINTER = void(
    int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi, int32_t ebp);

extern GAME_COMMAND_CALLBACK_POINTER* game_command_callback;
int32_t game_command_callback_get_index(GAME_COMMAND_CALLBACK_POINTER* callback);
GAME_COMMAND_CALLBACK_POINTER* game_command_callback_get_callback(uint32_t index);
extern int32_t game_command_playerid;

extern rct_string_id gGameCommandErrorTitle;
extern rct_string_id gGameCommandErrorText;
extern uint8_t gErrorType;
extern rct_string_id gErrorStringId;

extern GAME_COMMAND_POINTER* new_game_command_table[GAME_COMMAND_COUNT];

extern uint32_t gCurrentTicks;
extern uint32_t gCurrentRealTimeTicks;

extern uint16_t gCurrentDeltaTime;
extern uint8_t gGamePaused;
extern int32_t gGameSpeed;
extern bool gDoSingleUpdate;
extern float gDayNightCycle;
extern bool gInUpdateCode;
extern bool gInMapInitCode;
extern int32_t gGameCommandNestLevel;
extern bool gGameCommandIsNetworked;
extern std::string gCurrentLoadedPath;

extern bool gLoadKeepWindowsOpen;

extern uint8_t gUnk13CA740;
extern uint8_t gUnk141F568;

void game_increase_game_speed();
void game_reduce_game_speed();

void game_create_windows();
void reset_all_sprite_quadrant_placements();
void update_palette_effects();

int32_t game_do_command(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi, int32_t ebp);
int32_t game_do_command_p(
    uint32_t command, int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);

void game_log_multiplayer_command(int command, const int* eax, const int* ebx, const int* ecx, int* edx, int* edi, int* ebp);

void game_load_or_quit_no_save_prompt();
void load_from_sv6(const char* path);
void game_load_init();
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
