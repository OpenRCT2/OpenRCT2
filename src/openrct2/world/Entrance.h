/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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

struct TileElement;

enum
{
    ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY = (1 << 0),
};

namespace EntranceSequence
{
    constexpr uint8_t Centre = 0;
    constexpr uint8_t Left = 1;
    constexpr uint8_t Right = 2;
}; // namespace EntranceSequence

constexpr const uint8_t ParkEntranceHeight = 12 * COORDS_Z_STEP;
constexpr const uint8_t RideEntranceHeight = 7 * COORDS_Z_STEP;
constexpr const uint8_t RideExitHeight = 5 * COORDS_Z_STEP;

extern bool gParkEntranceGhostExists;
extern CoordsXYZD gParkEntranceGhostPosition;

constexpr int32_t MaxRideEntranceOrExitHeight = 244 * COORDS_Z_STEP;

extern CoordsXYZD gRideEntranceExitGhostPosition;
extern StationIndex gRideEntranceExitGhostStationIndex;

void ParkEntranceRemoveGhost();
int32_t ParkEntranceGetIndex(const CoordsXYZ& entrancePos);

void ParkEntranceReset();
void MazeEntranceHedgeReplacement(const CoordsXYE& entrance);
void MazeEntranceHedgeRemoval(const CoordsXYE& entrance);

void ParkEntranceFixLocations();
void ParkEntranceUpdateLocations();
