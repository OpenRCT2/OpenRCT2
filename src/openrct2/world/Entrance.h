/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../common.h"
#include "Location.hpp"
#include "Map.h"

#include <vector>

#pragma pack(push, 1)
struct rct_entrance_type
{
    StringId string_idx;    // 0x00
    uint32_t image_id;      // 0x02
    uint8_t scrolling_mode; // 0x06
    uint8_t text_height;    // 0x07
};
assert_struct_size(rct_entrance_type, 8);
#pragma pack(pop)

struct TileElement;

enum
{
    ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY = (1 << 0),
};

namespace EntranceSequence
{
    constexpr const uint8_t Centre = 0;
    constexpr const uint8_t Left = 1;
    constexpr const uint8_t Right = 2;
}; // namespace EntranceSequence

constexpr const uint8_t ParkEntranceHeight = 12 * COORDS_Z_STEP;
constexpr const uint8_t RideEntranceHeight = 7 * COORDS_Z_STEP;
constexpr const uint8_t RideExitHeight = 5 * COORDS_Z_STEP;

extern bool gParkEntranceGhostExists;
extern CoordsXYZD gParkEntranceGhostPosition;

constexpr int32_t MaxRideEntranceOrExitHeight = 244 * COORDS_Z_STEP;

extern std::vector<CoordsXYZD> gParkEntrances;

extern CoordsXYZD gRideEntranceExitGhostPosition;
extern StationIndex gRideEntranceExitGhostStationIndex;

void park_entrance_remove_ghost();

void reset_park_entrance();
void maze_entrance_hedge_replacement(const CoordsXYE& entrance);
void maze_entrance_hedge_removal(const CoordsXYE& entrance);

void fix_park_entrance_locations();
void UpdateParkEntranceLocations();
