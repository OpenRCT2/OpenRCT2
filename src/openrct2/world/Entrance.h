/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "Map.h"

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

struct TileElement;

constexpr const uint8_t ParkEntranceHeight = 12 * COORDS_Z_STEP;
constexpr const uint8_t RideEntranceHeight = 7 * COORDS_Z_STEP;
constexpr const uint8_t RideExitHeight = 5 * COORDS_Z_STEP;

extern bool gParkEntranceGhostExists;
extern CoordsXYZD gParkEntranceGhostPosition;

#define MAX_PARK_ENTRANCES 4

constexpr int32_t MaxRideEntranceOrExitHeight = 244 * COORDS_Z_STEP;

using ParkEntranceIndex = uint8_t;
constexpr const ParkEntranceIndex PARK_ENTRANCE_INDEX_NULL = 255;

extern std::vector<CoordsXYZD> gParkEntrances;

extern CoordsXYZD gRideEntranceExitGhostPosition;
extern StationIndex gRideEntranceExitGhostStationIndex;

void park_entrance_remove_ghost();
money32 park_entrance_place_ghost(const CoordsXYZD& entranceLoc);

void reset_park_entrance();
void maze_entrance_hedge_replacement(const CoordsXYE& entrance);
void maze_entrance_hedge_removal(const CoordsXYE& entrance);

void fix_park_entrance_locations();
void UpdateParkEntranceLocations();

#endif
