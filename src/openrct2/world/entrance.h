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

#ifndef _ENTRANCE_H_
#define _ENTRANCE_H_

#include "../common.h"

#pragma pack(push, 1)
typedef struct rct_entrance_type {
    rct_string_id string_idx;   // 0x00
    uint32 image_id;            // 0x02
    uint8 scrolling_mode;       // 0x06
    uint8 text_height;          // 0x07
} rct_entrance_type;
assert_struct_size(rct_entrance_type, 8);
#pragma pack(pop)

void game_command_remove_park_entrance(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);

typedef struct rct_xyz16 rct_xyz16;
typedef struct rct_xyzd16 rct_xyzd16;
typedef struct rct_map_element rct_map_element;

extern bool gParkEntranceGhostExists;
extern rct_xyz16 gParkEntranceGhostPosition;
extern uint8 gParkEntranceGhostDirection;

#define MAX_PARK_ENTRANCES 4

extern rct_xyzd16 gParkEntrances[MAX_PARK_ENTRANCES];

extern rct_xyzd16 gRideEntranceExitGhostPosition;
extern uint8 gRideEntranceExitGhostStationIndex;

void park_entrance_remove_ghost();
money32 park_entrance_place_ghost(sint32 x, sint32 y, sint32 z, sint32 direction);
money32 place_park_entrance(sint16 x, sint16 y, sint16 z, uint8 direction);

void reset_park_entrance();
void maze_entrance_hedge_replacement(sint32 x, sint32 y, rct_map_element *mapElement);
void maze_entrance_hedge_removal(sint32 x, sint32 y, rct_map_element *mapElement);

#endif
