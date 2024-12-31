/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "Location.hpp"

struct CoordsXYE;

constexpr uint8_t ParkEntranceHeight = 12 * kCoordsZStep;
constexpr uint8_t RideEntranceHeight = 7 * kCoordsZStep;
constexpr uint8_t RideExitHeight = 5 * kCoordsZStep;

extern bool gParkEntranceGhostExists;
extern CoordsXYZD gParkEntranceGhostPosition;

constexpr int32_t MaxRideEntranceOrExitHeight = 244 * kCoordsZStep;

extern CoordsXYZD gRideEntranceExitGhostPosition;
extern StationIndex gRideEntranceExitGhostStationIndex;

void ParkEntranceRemoveGhost();
int32_t ParkEntranceGetIndex(const CoordsXYZ& entrancePos);

void ParkEntranceReset();
void MazeEntranceHedgeReplacement(const CoordsXYE& entrance);
void MazeEntranceHedgeRemoval(const CoordsXYE& entrance);

void ParkEntranceFixLocations();
void ParkEntranceUpdateLocations();
