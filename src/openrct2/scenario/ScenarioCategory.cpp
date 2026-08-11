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
    const StringId kScenarioCategoryStringIds[EnumValue(Category::count)] = {
        STR_BEGINNER_PARKS, STR_CHALLENGING_PARKS,    STR_EXPERT_PARKS, STR_REAL_PARKS, STR_OTHER_PARKS,
        STR_DLC_PARKS,      STR_BUILD_YOUR_OWN_PARKS, STR_COMPETITIONS, STR_UCES_TM,    STR_UCES_KD,
    };
}
