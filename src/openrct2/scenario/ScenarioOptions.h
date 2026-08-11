/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "ScenarioCategory.h"
#include "ScenarioObjective.h"

#include <string>

namespace OpenRCT2::Scenario
{
    struct Options
    {
        Objective objective;
        Category category;
        std::string name;
        std::string details;

        money64 initialCash;
        money64 landPrice;
        money64 constructionRightsPrice;

        money64 guestInitialCash;
        uint8_t guestInitialHappiness;
        uint8_t guestInitialHunger;
        uint8_t guestInitialThirst;
    };
} // namespace OpenRCT2::Scenario
