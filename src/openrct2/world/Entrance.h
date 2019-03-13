/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _ENTRANCE_H_
#define _ENTRANCE_H_

#include "../common.h"
#include "Location.hpp"

#include <vector>

#pragma pack(push, 1)
struct rct_entrance_type
{
    rct_string_id string_idx; // 0x00
    uint32_t image_id;        // 0x02
    uint8_t scrolling_mode;   // 0x06
    uint8_t text_height;      // 0x07
};
assert_struct_size(rct_entrance_type, 8);
#pragma pack(pop)

void game_command_remove_park_entrance(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);

struct TileElement;

extern bool gParkEntranceGhostExists;
extern LocationXYZ16 gParkEntranceGhostPosition;
extern uint8_t gParkEntranceGhostDirection;

#define MAX_PARK_ENTRANCES 4

constexpr int32_t MaxRideEntranceOrExitHeight = 244;

extern std::vector<CoordsXYZD> gParkEntrances;

extern CoordsXYZD gRideEntranceExitGhostPosition;
extern uint8_t gRideEntranceExitGhostStationIndex;

void park_entrance_remove_ghost();
money32 park_entrance_place_ghost(int32_t x, int32_t y, int32_t z, int32_t direction);
money32 place_park_entrance(int16_t x, int16_t y, int16_t z, uint8_t direction);

void reset_park_entrance();
void maze_entrance_hedge_replacement(int32_t x, int32_t y, TileElement* tileElement);
void maze_entrance_hedge_removal(int32_t x, int32_t y, TileElement* tileElement);

void fix_park_entrance_locations();

#endif
