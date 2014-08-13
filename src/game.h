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

enum GAME_COMMAND {
	GAME_COMMAND_0,
	GAME_COMMAND_1,
	GAME_COMMAND_TOGGLE_PAUSE, // 2
	GAME_COMMAND_3,
	GAME_COMMAND_4,
	GAME_COMMAND_LOAD_OR_QUIT, // 5
	GAME_COMMAND_6,
	GAME_COMMAND_7,
	GAME_COMMAND_SET_RIDE_OPEN, // 8
	GAME_COMMAND_9,
	GAME_COMMAND_10,
	GAME_COMMAND_11,
	GAME_COMMAND_12,
	GAME_COMMAND_13,
	GAME_COMMAND_14,
	GAME_COMMAND_15,
	GAME_COMMAND_16,
	GAME_COMMAND_PLACE_PATH, // 17
	GAME_COMMAND_18,
	GAME_COMMAND_REMOVE_PATH, // 19
	GAME_COMMAND_20,
	GAME_COMMAND_21,
	GAME_COMMAND_22,
	GAME_COMMAND_23,
	GAME_COMMAND_24,
	GAME_COMMAND_25,
	GAME_COMMAND_26,
	GAME_COMMAND_27,
	GAME_COMMAND_28,
	GAME_COMMAND_HIRE_NEW_STAFF_MEMBER, // 29
	GAME_COMMAND_30,
	GAME_COMMAND_31,
	GAME_COMMAND_32,
	GAME_COMMAND_33,
	GAME_COMMAND_SET_PARK_OPEN, // 34
	GAME_COMMAND_35,
	GAME_COMMAND_36,
	GAME_COMMAND_37,
	GAME_COMMAND_38,
	GAME_COMMAND_SET_PARK_ENTRANCE_FEE, // 39
	GAME_COMMAND_SET_STAFF_COLOUR, // 40
	GAME_COMMAND_41,
	GAME_COMMAND_42,
	GAME_COMMAND_43,
	GAME_COMMAND_44,
	GAME_COMMAND_SET_CURRENT_LOAN, // 45
	GAME_COMMAND_SET_RESEARCH_FUNDING, // 46
	GAME_COMMAND_47,
	GAME_COMMAND_START_MARKETING_CAMPAIGN, // 48
	GAME_COMMAND_49,
	GAME_COMMAND_50,
	GAME_COMMAND_51,
	GAME_COMMAND_52,
	GAME_COMMAND_53,
	GAME_COMMAND_54,
	GAME_COMMAND_55,
	GAME_COMMAND_56,	// Set land owned (possibly does other things)
	GAME_COMMAND_57
};

void game_create_windows();
void game_update();
void game_logic_update();
void sub_0x0069E9A7(); 
void update_rain_animation();
void update_water_animation();

int game_do_command(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);
int game_do_command_p(int command, int *eax, int *ebx, int *ecx, int *edx, int *edi, int *esi, int *ebp);

void game_increase_game_speed();
void game_reduce_game_speed();

void game_load_or_quit_no_save_prompt();
int game_load_save();
void game_pause_toggle();
char save_game();
void rct2_exit();

#endif
