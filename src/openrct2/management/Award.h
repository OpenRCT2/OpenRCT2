/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <vector>

enum class AwardType : uint16_t
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
    BestToilets,
    MostDisappointing,
    BestWaterRides,
    BestCustomDesignedRides,
    MostDazzlingRideColours,
    MostConfusingLayout,
    BestGentleRides,
    Count
};
static_assert(static_cast<uint32_t>(AwardType::Count) < 32); // Awards are used in a 32bit bitset

struct Award
{
    uint16_t Time;
    AwardType Type;
};

bool AwardIsPositive(AwardType type);
void AwardReset();
void AwardUpdateAll();
