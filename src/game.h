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

enum {
	GAME_FLAGS_PAUSED = (1 << 0),
	GAME_FLAGS_PAUSE_FOR_SAVE = (1 << 1)
};

#define GAME_IS_PAUSED() (RCT2_GLOBAL(RTC2_ADDRESS_PAUSE_FLAGS, uint8) != 0)

void game_create_windows();
void game_update();
void game_logic_update();

int game_do_command(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);

void game_load_or_quit_no_save_prompt();
int game_load_save();

#endif
