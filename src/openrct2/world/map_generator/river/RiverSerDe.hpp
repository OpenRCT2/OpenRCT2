/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../SettingsTypesSerDe.hpp"
#include "RiverTypes.hpp"

#include <nlohmann/json.hpp>

namespace OpenRCT2::World::MapGenerator::River
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        RiverSettings, generate, catchmentThreshold, pruneThreshold, breachMaxLength, breachMaxLength,
        offMapCatchmentMultiplier, riverGrowthExponent, riverWidthMax);

} // namespace OpenRCT2::World::MapGenerator::River
