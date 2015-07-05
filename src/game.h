/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 * 
 * This file is part of OpenRCT2.
 * 
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/
 
#ifndef _GAME_H_
#define _GAME_H_

#include "common.h"
#include "platform/platform.h"

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
	GAME_COMMAND_18,
	GAME_COMMAND_REMOVE_PATH,
	GAME_COMMAND_CHANGE_SURFACE_STYLE,
	GAME_COMMAND_SET_RIDE_PRICE,
	GAME_COMMAND_SET_PEEP_NAME,
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
	GAME_COMMAND_PLACE_FENCE,
	GAME_COMMAND_REMOVE_FENCE,
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
	GAME_COMMAND_SET_FENCE_COLOUR,
	GAME_COMMAND_SET_LARGE_SCENERY_COLOUR,
	GAME_COMMAND_SET_BANNER_COLOUR,
	GAME_COMMAND_SET_LAND_OWNERSHIP,
	GAME_COMMAND_CLEAR_SCENERY
};

// If this flag is set, the command is applied, otherwise only the cost is retrieved
#define GAME_COMMAND_FLAG_APPLY (1 << 0)

typedef void (GAME_COMMAND_POINTER)(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);

extern int gGameSpeed;

void game_increase_game_speed();
void game_reduce_game_speed();

void game_create_windows();
void game_update();
void game_logic_update();
void reset_all_sprite_quadrant_placements();
void update_palette_effects();

int game_do_command(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);
int game_do_command_p(int command, int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);

void game_increase_game_speed();
void game_reduce_game_speed();

void game_load_or_quit_no_save_prompt();
int game_load_sv6(SDL_RWops* rw);
int game_load_save(const char *path);
void game_load_init();
void game_pause_toggle(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void pause_toggle();
int save_game();
void rct2_exit();
void rct2_exit_reason(rct_string_id title, rct_string_id body);
void game_autosave();

#endif
