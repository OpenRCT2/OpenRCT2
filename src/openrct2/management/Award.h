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

struct Award
{
    uint16_t Time;
    uint16_t Type;
};

enum class ParkAward : uint8_t
{
    MostUntidy,
    MostTidy,
    BestRollercoasters, // BestRollercoasters or BestRollerCoasters?
    BestValue,
    MostBeautiful,
    WorstValue,
    Safest,
    BestStaff,
    BestFood,
    WorstFood,
    BestRestrooms,
    MostDisappointing,
    BestWaterRides,
    BestCustomDesignedRides,
    MostDazzlingRideColours,
    MostConfusingLayout,
    BestGentleRides,
    Count
};

#define MAX_AWARDS 4

extern Award gCurrentAwards[MAX_AWARDS];

bool award_is_positive(int32_t type);
void award_reset();
void award_update_all();
