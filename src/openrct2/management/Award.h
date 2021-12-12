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

#include <vector>

enum class ParkAward : uint16_t
{
    MostUntidy,
    MostTidy,
    BestRollerCoasters,
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
    Count // Count must be less than 32
};

struct Award
{
    uint16_t Time;
    ParkAward Type;
};

#define MAX_AWARDS 4

std::vector<Award>& GetAwards();

bool award_is_positive(ParkAward type);
void award_reset();
void award_update_all();
