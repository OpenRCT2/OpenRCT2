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

enum GAME_COMMAND {
	GAME_COMMAND_SET_RIDE_APPEARANCE,
	GAME_COMMAND_1,
	GAME_COMMAND_TOGGLE_PAUSE, // 2
	GAME_COMMAND_PLACE_TRACK,
	GAME_COMMAND_REMOVE_TRACK,
	GAME_COMMAND_LOAD_OR_QUIT, // 5
	GAME_COMMAND_6,
	GAME_COMMAND_7,
	GAME_COMMAND_SET_RIDE_STATUS, // 8
	GAME_COMMAND_9,
	GAME_COMMAND_SET_RIDE_NAME,
	GAME_COMMAND_SET_RIDE_SETTING,
	GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT,
	GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT,
	GAME_COMMAND_REMOVE_SCENERY,
	GAME_COMMAND_PLACE_SCENERY,
	GAME_COMMAND_16,
	GAME_COMMAND_PLACE_PATH, // 17
	GAME_COMMAND_18,
	GAME_COMMAND_REMOVE_PATH, // 19
	GAME_COMMAND_CHANGE_SURFACE_STYLE, //20
	GAME_COMMAND_SET_RIDE_PRICE,
	GAME_COMMAND_22, //To do with text input
	GAME_COMMAND_RAISE_LAND,
	GAME_COMMAND_LOWER_LAND,
	GAME_COMMAND_EDIT_LAND_SMOOTH,
	GAME_COMMAND_RAISE_WATER,
	GAME_COMMAND_LOWER_WATER,
	GAME_COMMAND_28,
	GAME_COMMAND_HIRE_NEW_STAFF_MEMBER, // 29
	GAME_COMMAND_SET_STAFF_PATROL, //30
	GAME_COMMAND_FIRE_STAFF_MEMBER, // 31
	GAME_COMMAND_SET_STAFF_ORDER, // 32
	GAME_COMMAND_SET_PARK_NAME,
	GAME_COMMAND_SET_PARK_OPEN, // 34
	GAME_COMMAND_BUY_LAND_RIGHTS, // 35
	GAME_COMMAND_PLACE_PARK_ENTRANCE,
	GAME_COMMAND_REMOVE_PARK_ENTRANCE,
	GAME_COMMAND_38,
	GAME_COMMAND_SET_PARK_ENTRANCE_FEE, // 39
	GAME_COMMAND_SET_STAFF_COLOUR, // 40
	GAME_COMMAND_PLACE_FENCE,
	GAME_COMMAND_REMOVE_FENCE,
	GAME_COMMAND_PLACE_LARGE_SCENERY,
	GAME_COMMAND_REMOVE_LARGE_SCENERY,
	GAME_COMMAND_SET_CURRENT_LOAN, // 45
	GAME_COMMAND_SET_RESEARCH_FUNDING, // 46
	GAME_COMMAND_PLACE_TRACK_DESIGN,
	GAME_COMMAND_START_MARKETING_CAMPAIGN, // 48
	GAME_COMMAND_49,
	GAME_COMMAND_PLACE_BANNER, // New banner? (possibly scenery)
	GAME_COMMAND_REMOVE_BANNER, // Remove banner
	GAME_COMMAND_52,
	GAME_COMMAND_53,
	GAME_COMMAND_54,
	GAME_COMMAND_55,
	GAME_COMMAND_56,	// Set land owned (possibly does other things)
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
int game_load_sv6(const char *path);
int game_load_save(const char *path);
void game_pause_toggle(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void pause_toggle();
char save_game();
void rct2_exit();
void rct2_exit_reason(rct_string_id title, rct_string_id body);
void game_autosave();

#endif
