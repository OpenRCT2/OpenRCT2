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
