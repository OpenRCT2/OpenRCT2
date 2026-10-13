/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <array>
#include <cstdint>

namespace OpenRCT2::Drawing
{
    struct BGRColour
    {
        uint8_t blue{};
        uint8_t green{};
        uint8_t red{};
    };

    struct BGRAColour
    {
        uint8_t blue{};
        uint8_t green{};
        uint8_t red{};
        uint8_t alpha{};
    };

    constexpr auto kGamePaletteSize = 256u;

    using GamePalette = std::array<BGRAColour, kGamePaletteSize>;

} // namespace OpenRCT2::Drawing
