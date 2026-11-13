/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "../localisation/StringIdType.h"

namespace OpenRCT2::Scenario
{
    enum class Category : uint8_t
    {
        // RCT2 categories (keep order)
        beginner,
        challenging,
        expert,
        real,
        other,

        // RCT Classic groups
        rctClassicGraphite,
        rctClassicEmerald,
        rctClassicRuby,
        rctClassicSapphire,
        rctClassicAmethyst,
        rctClassicCoral,
        rctClassicIvory,
        rctClassicBronze,
        rctClassicSilver,
        rctClassicGold,

        // OpenRCT2 categories
        dlc,
        buildYourOwn,
        competitions,
        timeMachine,
        katysDreamworld,

        // Special category for unlockable bonus scenarios (e.g. Mega Park, Tycoon Park)
        bonus,

        // No category
        none,

        count,
    };

    extern const StringId kScenarioCategoryStringIds[EnumValue(Category::count)];
} // namespace OpenRCT2::Scenario
