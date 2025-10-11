/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioCategory.h"

#include "../localisation/StringIds.h"

namespace OpenRCT2::Scenario
{
    // clang-format off
    const StringId kScenarioCategoryStringIds[EnumValue(Category::count)] = {
        STR_BEGINNER_PARKS,
        STR_CHALLENGING_PARKS,
        STR_EXPERT_PARKS,
        STR_REAL_PARKS,
        STR_OTHER_PARKS,
        STR_RCT_CLASSIC_GRAPHITE_GROUP,
        STR_RCT_CLASSIC_EMERALD_GROUP,
        STR_RCT_CLASSIC_RUBY_GROUP,
        STR_RCT_CLASSIC_SAPPHIRE_GROUP,
        STR_RCT_CLASSIC_AMETHYST_GROUP,
        STR_RCT_CLASSIC_CORAL_GROUP,
        STR_RCT_CLASSIC_IVORY_GROUP,
        STR_RCT_CLASSIC_BRONZE_GROUP,
        STR_RCT_CLASSIC_SILVER_GROUP,
        STR_RCT_CLASSIC_GOLD_GROUP,
        STR_DLC_PARKS,
        STR_BUILD_YOUR_OWN_PARKS,
        STR_COMPETITIONS,
        STR_UCES_TM,
        STR_UCES_KD,
        STR_BONUS_PARKS,
        STR_EMPTY
    };
    // clang-format on
}
