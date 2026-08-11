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

#include <cstdint>

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

namespace OpenRCT2
{
    constexpr uint8_t kLegacyColourFlagTranslucent = (1 << 7);

    enum class ColourFlag : uint8_t
    {
        translucent,
        inset,
        withOutline,
    };
    using ColourFlags = FlagHolder<uint8_t, ColourFlag>;

    struct ColourWithFlags
    {
        Drawing::Colour colour{};
        ColourFlags flags{};

        ColourWithFlags withFlag(ColourFlag flag, bool on) const;

        static ColourWithFlags fromLegacy(uint8_t legacy);

        ColourWithFlags& operator=(Drawing::Colour rhs);
    };
} // namespace OpenRCT2
