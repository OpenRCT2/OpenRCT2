/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Rule.h"

namespace OpenRCT2::World::MapGenerator::Rule
{
    enum class TextureRulePreset : uint8_t
    {
        SmallRockPatches,
        MediumDirtPatches,
        LargeGrassClumpPatches,
        MountainPeaks
    };

    enum class SceneryRulePreset : uint8_t
    {
        Mediterranean,
        Cold,
        Arid,
        Rainforest,
    };

    void createDefaultTextureRules(Settings& settings, uint32_t sharedOffset);
    void createNewTextureRule(Settings& settings);
    void createNewTextureRuleFromPreset(Settings& settings, TextureRulePreset preset);

    void createDefaultSceneryRules(Settings& settings, uint32_t sharedOffset);
    void createNewSceneryRule(Settings& settings);
    void createNewSceneryRuleFromPreset(Settings& settings, SceneryRulePreset preset);

    Condition createNewCondition(const Type& type);
} // namespace OpenRCT2::World::MapGenerator::Rule
