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

#ifndef _GAME_H_
#define _GAME_H_

#include "rct2/addresses.h"
#include "common.h"
#include "scenario/scenario.h"

enum GAME_COMMAND {
    GAME_COMMAND_SET_RIDE_APPEARANCE,
    GAME_COMMAND_SET_LAND_HEIGHT,
    GAME_COMMAND_TOGGLE_PAUSE,
    GAME_COMMAND_PLACE_TRACK,
    GAME_COMMAND_REMOVE_TRACK,
    GAME_COMMAND_LOAD_OR_QUIT,
    GAME_COMMAND_CREATE_RIDE,
    GAME_COMMAND_DEMOLISH_RIDE,
    GAME_COMMAND_SET_RIDE_STATUS,
    GAME_COMMAND_SET_RIDE_VEHICLES,
    GAME_COMMAND_SET_RIDE_NAME,
    GAME_COMMAND_SET_RIDE_SETTING,
    GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT,
    GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT,
    GAME_COMMAND_REMOVE_SCENERY,
    GAME_COMMAND_PLACE_SCENERY,
    GAME_COMMAND_SET_WATER_HEIGHT,
    GAME_COMMAND_PLACE_PATH,
    GAME_COMMAND_PLACE_PATH_FROM_TRACK,
    GAME_COMMAND_REMOVE_PATH,
    GAME_COMMAND_CHANGE_SURFACE_STYLE,
    GAME_COMMAND_SET_RIDE_PRICE,
    GAME_COMMAND_SET_GUEST_NAME,
    GAME_COMMAND_SET_STAFF_NAME,
    GAME_COMMAND_RAISE_LAND,
    GAME_COMMAND_LOWER_LAND,
    GAME_COMMAND_EDIT_LAND_SMOOTH,
    GAME_COMMAND_RAISE_WATER,
    GAME_COMMAND_LOWER_WATER,
    GAME_COMMAND_SET_BRAKES_SPEED,
    GAME_COMMAND_HIRE_NEW_STAFF_MEMBER,
    GAME_COMMAND_SET_STAFF_PATROL,
    GAME_COMMAND_FIRE_STAFF_MEMBER,
    GAME_COMMAND_SET_STAFF_ORDER,
    GAME_COMMAND_SET_PARK_NAME,
    GAME_COMMAND_SET_PARK_OPEN,
    GAME_COMMAND_BUY_LAND_RIGHTS,
    GAME_COMMAND_PLACE_PARK_ENTRANCE,
    GAME_COMMAND_REMOVE_PARK_ENTRANCE,
    GAME_COMMAND_SET_MAZE_TRACK,
    GAME_COMMAND_SET_PARK_ENTRANCE_FEE,
    GAME_COMMAND_SET_STAFF_COLOUR,
    GAME_COMMAND_PLACE_WALL,
    GAME_COMMAND_REMOVE_WALL,
    GAME_COMMAND_PLACE_LARGE_SCENERY,
    GAME_COMMAND_REMOVE_LARGE_SCENERY,
    GAME_COMMAND_SET_CURRENT_LOAN,
    GAME_COMMAND_SET_RESEARCH_FUNDING,
    GAME_COMMAND_PLACE_TRACK_DESIGN,
    GAME_COMMAND_START_MARKETING_CAMPAIGN,
    GAME_COMMAND_PLACE_MAZE_DESIGN,
    GAME_COMMAND_PLACE_BANNER,
    GAME_COMMAND_REMOVE_BANNER,
    GAME_COMMAND_SET_SCENERY_COLOUR,
    GAME_COMMAND_SET_WALL_COLOUR,
    GAME_COMMAND_SET_LARGE_SCENERY_COLOUR,
    GAME_COMMAND_SET_BANNER_COLOUR,
    GAME_COMMAND_SET_LAND_OWNERSHIP,
    GAME_COMMAND_CLEAR_SCENERY,
    GAME_COMMAND_SET_BANNER_NAME,
    GAME_COMMAND_SET_SIGN_NAME,
    GAME_COMMAND_SET_BANNER_STYLE,
    GAME_COMMAND_SET_SIGN_STYLE,
    GAME_COMMAND_SET_PLAYER_GROUP,
    GAME_COMMAND_MODIFY_GROUPS,
    GAME_COMMAND_KICK_PLAYER,
    GAME_COMMAND_CHEAT,
    GAME_COMMAND_PICKUP_GUEST,
    GAME_COMMAND_PICKUP_STAFF,
    GAME_COMMAND_BALLOON_PRESS,
    GAME_COMMAND_MODIFY_TILE,
    GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
    GAME_COMMAND_COUNT
};

enum {
    GAME_COMMAND_FLAG_APPLY = (1 << 0), // If this flag is set, the command is applied, otherwise only the cost is retrieved
    GAME_COMMAND_FLAG_2 = (1 << 2),
    GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED = (1 << 3), // Allow while paused
    GAME_COMMAND_FLAG_4 = (1 << 4),
    GAME_COMMAND_FLAG_5 = (1 << 5),
    GAME_COMMAND_FLAG_GHOST = (1 << 6),
    GAME_COMMAND_FLAG_PATH_SCENERY = (1 << 7),
    GAME_COMMAND_FLAG_NETWORKED = (1u << 31) // Game command is coming from network
};

enum {
    GAME_PAUSED_NORMAL          = 1 << 0,
    GAME_PAUSED_MODAL           = 1 << 1,
    GAME_PAUSED_SAVING_TRACK    = 1 << 2,
};

enum {
    ERROR_TYPE_NONE = 0,
    ERROR_TYPE_GENERIC = 254,
    ERROR_TYPE_FILE_LOAD = 255
};

typedef void (GAME_COMMAND_POINTER)(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);

typedef void (GAME_COMMAND_CALLBACK_POINTER)(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);

extern GAME_COMMAND_CALLBACK_POINTER* game_command_callback;
sint32 game_command_callback_get_index(GAME_COMMAND_CALLBACK_POINTER* callback);
GAME_COMMAND_CALLBACK_POINTER* game_command_callback_get_callback(sint32 index);
extern sint32 game_command_playerid;

extern rct_string_id gGameCommandErrorTitle;
extern rct_string_id gGameCommandErrorText;
extern uint8 gErrorType;
extern rct_string_id gErrorStringId;

extern GAME_COMMAND_POINTER* new_game_command_table[GAME_COMMAND_COUNT];

#ifndef NO_RCT2
#define gCurrentTicks               RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32)
#else
extern uint32 gCurrentTicks;
#endif

extern uint16 gTicksSinceLastUpdate;
extern uint8 gGamePaused;
extern sint32 gGameSpeed;
extern float gDayNightCycle;
extern bool gInUpdateCode;
extern bool gInMapInitCode;
extern sint32 gGameCommandNestLevel;
extern bool gGameCommandIsNetworked;

extern uint8 gUnk13CA740;
extern uint8 gUnk141F568;

void game_increase_game_speed();
void game_reduce_game_speed();

void game_create_windows();
void game_update();
bool game_logic_begin();
void game_logic_update();
void game_logic_finish();
void reset_all_sprite_quadrant_placements();
void update_palette_effects();

sint32 game_do_command(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);
sint32 game_do_command_p(sint32 command, sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);

void game_log_multiplayer_command(int command, int *eax, int* ebx, int* ecx, int* edx, int* edi, int* ebp);

void game_load_or_quit_no_save_prompt();
ParkLoadResult * game_load_sv6_path(const char * path);
bool game_load_save(const utf8 *path);
void game_load_init();
void game_pause_toggle(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void pause_toggle();
bool game_is_paused();
bool game_is_not_paused();
void save_game();
void save_game_as();
void handle_park_load_failure_with_title_opt(const ParkLoadResult * result, const utf8 * path, bool loadTitleFirst);
void handle_park_load_failure(const ParkLoadResult * result, const utf8 * path);
void rct2_exit();
void rct2_exit_reason(rct_string_id title, rct_string_id body);
void game_autosave();
void game_convert_strings_to_utf8();
void game_convert_news_items_to_utf8();
void game_convert_strings_to_rct2(rct_s6_data *s6);
void game_fix_save_vars();
bool game_load_save_or_scenario(const utf8 * path);
void game_init_all(sint32 mapSize);

#endif
