#pragma once

#include <array>
#include <cstdint>

namespace OpenRCT2::Drawing
{

    struct PaletteBGRA
    {
        uint8_t Blue{};
        uint8_t Green{};
        uint8_t Red{};
        uint8_t Alpha{};
    };

    constexpr auto kGamePaletteSize = 256u;

    using GamePalette = std::array<PaletteBGRA, kGamePaletteSize>;

} // namespace OpenRCT2::Drawing
