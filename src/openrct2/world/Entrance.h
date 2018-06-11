/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#ifndef _ENTRANCE_H_
#define _ENTRANCE_H_

#include "../common.h"
#include "Location.hpp"

#pragma pack(push, 1)
struct rct_entrance_type {
    rct_string_id string_idx;   // 0x00
    uint32 image_id;            // 0x02
    uint8 scrolling_mode;       // 0x06
    uint8 text_height;          // 0x07
};
assert_struct_size(rct_entrance_type, 8);
#pragma pack(pop)

void game_command_remove_park_entrance(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);

struct rct_tile_element;

extern bool gParkEntranceGhostExists;
extern LocationXYZ16 gParkEntranceGhostPosition;
extern uint8 gParkEntranceGhostDirection;

#define MAX_PARK_ENTRANCES 4

extern CoordsXYZD gParkEntrances[MAX_PARK_ENTRANCES];

extern CoordsXYZD gRideEntranceExitGhostPosition;
extern uint8 gRideEntranceExitGhostStationIndex;

void park_entrance_remove_ghost();
money32 park_entrance_place_ghost(sint32 x, sint32 y, sint32 z, sint32 direction);
money32 place_park_entrance(sint16 x, sint16 y, sint16 z, uint8 direction);

void reset_park_entrance();
void maze_entrance_hedge_replacement(sint32 x, sint32 y, rct_tile_element *tileElement);
void maze_entrance_hedge_removal(sint32 x, sint32 y, rct_tile_element *tileElement);

void fix_park_entrance_locations();

#endif
