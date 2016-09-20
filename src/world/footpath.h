#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#ifndef _WORLD_FOOTPATH_H_
#define _WORLD_FOOTPATH_H_

#include "../common.h"
#include "../interface/viewport.h"
#include "../object.h"

enum {
	PROVISIONAL_PATH_FLAG_SHOW_ARROW = (1 << 0),
	PROVISIONAL_PATH_FLAG_1 = (1 << 1),
	PROVISIONAL_PATH_FLAG_2 = (1 << 2),
};

#pragma pack(push, 1)
typedef struct rct_footpath_entry {
	rct_string_id string_idx;	// 0x00
	uint32 image;				// 0x02
	uint32 bridge_image;		// 0x06
	uint8 var_0A;
	uint8 flags;				// 0x0B
	uint8 scrolling_mode;		// 0x0C
} rct_footpath_entry;
assert_struct_size(rct_footpath_entry, 13);
#pragma pack(pop)

enum {
	FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR = (1 << 2),
};

enum {
	FOOTPATH_SEARCH_SUCCESS,
	FOOTPATH_SEARCH_NOT_FOUND,
	FOOTPATH_SEARCH_INCOMPLETE,
	FOOTPATH_SEARCH_TOO_COMPLEX
};

extern uint8 gFootpathProvisionalFlags;
extern rct_xyz16 gFootpathProvisionalPosition;
extern uint8 gFootpathProvisionalType;
extern uint8 gFootpathProvisionalSlope;
extern uint8 gFootpathConstructionMode;
extern uint16 gFootpathSelectedId;
extern uint8 gFootpathSelectedType;
extern rct_xyz16 gFootpathConstructFromPosition;
extern uint8 gFootpathConstructDirection;
extern uint8 gFootpathConstructSlope;
extern uint8 gFootpathConstructValidDirections;
extern money32 gFootpathPrice;
extern uint8 gFootpathGroundFlags;

extern const rct_xy16 word_981D6C[4];

money32 footpath_remove_real(int x, int y, int z, int flags);
void game_command_place_footpath(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_place_footpath_from_track(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_remove_footpath(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
money32 footpath_place(int type, int x, int y, int z, int slope, int flags);
void footpath_remove(int x, int y, int z, int flags);
money32 footpath_provisional_set(int type, int x, int y, int z, int slope);
void footpath_provisional_remove();
void footpath_provisional_update();
void footpath_get_coordinates_from_pos(int screenX, int screenY, int *x, int *y, int *direction, rct_map_element **mapElement);
void footpath_bridge_get_info_from_pos(int screenX, int screenY, int *x, int *y, int *direction, rct_map_element **mapElement);
void footpath_remove_litter(int x, int y, int z);
void footpath_connect_edges(int x, int y, rct_map_element *mapElement, int flags);
void sub_6A759F();
bool fence_in_the_way(int x, int y, int z0, int z1, int direction);
void footpath_chain_ride_queue(int rideIndex, int entranceIndex, int x, int y, rct_map_element *mapElement, int direction);
void footpath_update_path_wide_flags(int x, int y);

void footpath_bridge_get_info_from_pos(int screenX, int screenY, int *x, int *y, int *direction, rct_map_element **mapElement);

int footpath_is_connected_to_map_edge(int x, int y, int z, int direction, int flags);
bool footpath_element_is_sloped(rct_map_element *mapElement);
uint8 footpath_element_get_slope_direction(rct_map_element *mapElement);
bool footpath_element_is_queue(rct_map_element *mapElement);
bool footpath_element_is_wide(rct_map_element *mapElement);
uint8 footpath_element_get_type(rct_map_element *mapElement);
bool footpath_element_has_path_scenery(rct_map_element *mapElement);
uint8 footpath_element_get_path_scenery(rct_map_element *mapElement);
void footpath_element_set_path_scenery(rct_map_element *mapElement, uint8 pathSceneryType);
uint8 footpath_element_get_path_scenery_index(rct_map_element *mapElement);
bool footpath_element_path_scenery_is_ghost(rct_map_element *mapElement);
void footpath_scenery_set_is_ghost(rct_map_element *mapElement, bool isGhost);
void footpath_remove_edges_at(int x, int y, rct_map_element *mapElement);
int entrance_get_directions(rct_map_element *mapElement);

rct_footpath_entry *get_footpath_entry(int entryIndex);

void footpath_queue_chain_reset();
void footpath_queue_chain_push(uint8 rideIndex);

#endif
