/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "../drawing/ImageIndexType.h"
#include "../localisation/StringIdType.h"
#include "ObjectTypes.h"

namespace OpenRCT2
{
    enum class WaterObjectFlag : uint8_t
    {
        allowDucks,
    };
    using WaterObjectFlags = FlagHolder<uint16_t, WaterObjectFlag>;

    struct WaterObjectEntry
    {
        static constexpr auto kObjectType = ObjectType::water;

        StringId stringId;
        ImageIndex mainPalette;
        ImageIndex waterWavesPalette;
        ImageIndex waterSparklesPalette;
        WaterObjectFlags flags;
    };

    const WaterObjectEntry& getActiveWaterEntry();
} // namespace OpenRCT2
