/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Location.hpp"

enum
{
    SLOPE_S_THRESHOLD_FLAGS = (1 << 0),
    SLOPE_W_THRESHOLD_FLAGS = (1 << 1),
    SLOPE_N_THRESHOLD_FLAGS = (1 << 2),
    SLOPE_E_THRESHOLD_FLAGS = (1 << 3)
};

int32_t map_smooth(int32_t l, int32_t t, int32_t r, int32_t b);
int32_t tile_smooth(const TileCoordsXY& tileCoords);
