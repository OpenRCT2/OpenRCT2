/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../SpriteIds.h"
#include "../localisation/StringIds.h"

#include <cstdint>
#include <vector>

enum class AwardType : uint16_t
{
    mostUntidy,
    mostTidy,
    bestRollerCoasters,
    bestValue,
    mostBeautiful,
    worstValue,
    safest,
    bestStaff,
    bestFood,
    worstFood,
    bestToilets,
    mostDisappointing,
    bestWaterRides,
    bestCustomDesignedRides,
    mostDazzlingRideColours,
    mostConfusingLayout,
    bestGentleRides,
    count,
};
static_assert(static_cast<uint32_t>(AwardType::count) < 32); // Awards are used in a 32bit bitset

struct Award
{
    uint16_t Time;
    AwardType Type;
};

bool AwardIsPositive(AwardType type);
ImageIndex AwardGetSprite(AwardType type);
StringId AwardGetText(AwardType type);
StringId AwardGetNews(AwardType type);
void AwardGrant(AwardType type);
void AwardReset();
void AwardUpdateAll();
