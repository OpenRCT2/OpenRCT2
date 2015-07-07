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
#include "../object.h"

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

enum {
	FOOTPATH_SEARCH_SUCCESS,
	FOOTPATH_SEARCH_NOT_FOUND,
	FOOTPATH_SEARCH_INCOMPLETE,
	FOOTPATH_SEARCH_TOO_COMPLEX
};

#define g_pathTypeEntries ((rct_path_type**)object_entry_groups[OBJECT_TYPE_PATHS].chunks)

extern const rct_xy16 word_981D6C[4];

void game_command_place_footpath(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_remove_footpath(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
money32 footpath_place(int type, int x, int y, int z, int slope, int flags);
void footpath_remove(int x, int y, int z, int flags);
money32 footpath_provisional_set(int type, int x, int y, int z, int slope);
void footpath_provisional_remove();
void footpath_provisional_update();
void footpath_get_coordinates_from_pos(int screenX, int screenY, int *x, int *y, int *direction, rct_map_element **mapElement);
void footpath_bridge_get_info_from_pos(int screenX, int screenY, int *x, int *y, int *direction, rct_map_element **mapElement);
void footpath_remove_litter(int x, int y, int z);
void sub_6A6C66(int x, int y, rct_map_element *mapElement, int flags);
void sub_6A759F();
void footpath_chain_ride_queue(int rideIndex, int entranceIndex, int x, int y, rct_map_element *mapElement, int direction);

void footpath_bridge_get_info_from_pos(int screenX, int screenY, int *x, int *y, int *direction, rct_map_element **mapElement);

int footpath_is_connected_to_map_edge(int x, int y, int z, int direction, int flags);
bool footpath_element_is_sloped(rct_map_element *mapElement);
int footpath_element_get_slope_direction(rct_map_element *mapElement);
bool footpath_element_is_queue(rct_map_element *mapElement);
void footpath_remove_edges_at(int x, int y, rct_map_element *mapElement);

#endif
