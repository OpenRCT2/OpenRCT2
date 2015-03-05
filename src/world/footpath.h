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

#ifndef _WORLD_FOOTPATH_H_
#define _WORLD_FOOTPATH_H_

#include "../common.h"
#include "../interface/viewport.h"

enum {
	PROVISIONAL_PATH_FLAG_SHOW_ARROW = (1 << 0)
};

typedef struct {
	rct_string_id string_idx;	// 0x00
	uint32 image;				// 0x02
	uint32 bridge_image;		// 0x06
	uint8 var_0A;
	uint8 flags;				// 0x0B
} rct_path_type;

void game_command_place_footpath(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_remove_footpath(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
money32 footpath_place(int type, int x, int y, int z, int slope, int flags);
void footpath_remove(int x, int y, int z, int flags);
money32 footpath_provisional_set(int type, int x, int y, int z, int slope);
void footpath_provisional_remove();
void sub_6A7831();
void sub_68A0C9(int screenX, int screenY, int *x, int *y, int *direction, rct_map_element **mapElement);

#endif
