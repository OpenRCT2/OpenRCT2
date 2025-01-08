/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Location.hpp"

enum EDGE_SLOPE
{
    EDGE_SLOPE_UPWARDS = (1 << 0),
    EDGE_SLOPE_DOWNWARDS = (1 << 1),
    EDGE_SLOPE_ELEVATED = (1 << 2),

    EDGE_SLOPE_UPWARDS_ELEVATED = EDGE_SLOPE_UPWARDS | EDGE_SLOPE_ELEVATED,
    EDGE_SLOPE_DOWNWARDS_ELEVATED = EDGE_SLOPE_DOWNWARDS | EDGE_SLOPE_ELEVATED,
};

void WallRemoveAt(const CoordsXYRangedZ& wallPos);
void WallRemoveAtZ(const CoordsXYZ& wallPos);
void WallRemoveIntersectingWalls(const CoordsXYRangedZ& wallPos, Direction direction);

uint8_t GetWallSlopeFromEdgeSlope(uint8_t Slope, uint8_t Edge);
