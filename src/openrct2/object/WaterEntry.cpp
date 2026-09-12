/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WaterEntry.h"

#include "../SpriteIds.h"
#include "ObjectEntryManager.h"

namespace OpenRCT2
{
    static constexpr WaterObjectEntry kFallbackWaterEntry = {
        .stringId = kStringIdNone,
        .mainPalette = SPR_GAME_DEFAULT_PALETTE,
        .waterWavesPalette = SPR_GAME_PALETTE_WATER,
        .waterSparklesPalette = SPR_GAME_PALETTE_3,
        .flags = 0,
    };

    const WaterObjectEntry& getActiveWaterEntry()
    {
        const auto* waterEntry = ObjectEntryManager::GetObjectEntry<WaterObjectEntry>(0);
        if (waterEntry != nullptr)
            return *waterEntry;

        return kFallbackWaterEntry;
    }
} // namespace OpenRCT2
