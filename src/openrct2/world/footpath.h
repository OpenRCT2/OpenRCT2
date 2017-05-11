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
	uint8 support_type;			// 0x0A
	uint8 flags;				// 0x0B
	uint8 scrolling_mode;		// 0x0C
} rct_footpath_entry;
assert_struct_size(rct_footpath_entry, 13);
#pragma pack(pop)

enum {
	FOOTPATH_ENTRY_SUPPORT_TYPE_POLE = 0,
	FOOTPATH_ENTRY_SUPPORT_TYPE_BOX = 1,
	FOOTPATH_ENTRY_SUPPORT_TYPE_COUNT
};

enum {
	FOOTPATH_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE = (1 << 0),
	FOOTPATH_ENTRY_FLAG_HAS_PATH_BASE_SPRITE = (1 << 1), // When elevated
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

money32 footpath_remove_real(sint32 x, sint32 y, sint32 z, sint32 flags);
void game_command_place_footpath(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_place_footpath_from_track(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_remove_footpath(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
money32 footpath_place(sint32 type, sint32 x, sint32 y, sint32 z, sint32 slope, sint32 flags);
void footpath_remove(sint32 x, sint32 y, sint32 z, sint32 flags);
money32 footpath_provisional_set(sint32 type, sint32 x, sint32 y, sint32 z, sint32 slope);
void footpath_provisional_remove();
void footpath_provisional_update();
void footpath_get_coordinates_from_pos(sint32 screenX, sint32 screenY, sint32 *x, sint32 *y, sint32 *direction, rct_map_element **mapElement);
void footpath_bridge_get_info_from_pos(sint32 screenX, sint32 screenY, sint32 *x, sint32 *y, sint32 *direction, rct_map_element **mapElement);
void footpath_remove_litter(sint32 x, sint32 y, sint32 z);
void footpath_connect_edges(sint32 x, sint32 y, rct_map_element *mapElement, sint32 flags);
void sub_6A759F();
bool fence_in_the_way(sint32 x, sint32 y, sint32 z0, sint32 z1, sint32 direction);
void footpath_chain_ride_queue(sint32 rideIndex, sint32 entranceIndex, sint32 x, sint32 y, rct_map_element *mapElement, sint32 direction);
void footpath_update_path_wide_flags(sint32 x, sint32 y);

sint32 footpath_is_connected_to_map_edge(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 flags);
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
void footpath_remove_edges_at(sint32 x, sint32 y, rct_map_element *mapElement);
sint32 entrance_get_directions(rct_map_element *mapElement);

rct_footpath_entry *get_footpath_entry(sint32 entryIndex);

void footpath_queue_chain_reset();
void footpath_queue_chain_push(uint8 rideIndex);

#endif
