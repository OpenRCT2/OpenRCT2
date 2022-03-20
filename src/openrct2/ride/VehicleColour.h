/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

struct rct_vehicle_colour
{
    uint8_t body_colour;
    uint8_t trim_colour;
};

struct VehicleColour
{
    uint8_t Body;
    uint8_t Trim;
    uint8_t Tertiary;
};
