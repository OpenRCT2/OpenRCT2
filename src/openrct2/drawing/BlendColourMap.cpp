/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BlendColourMap.h"

#ifndef DISABLE_TTF

    #include "../core/EnumUtils.hpp"
    #include "ColourPalette.h"
    #include "Drawing.h"

    #include <cmath>

namespace OpenRCT2::Drawing
{
    static BlendColourMapType _blendColourMap = {};

    static bool _blendColourMapInitialised = false;

    static PaletteIndex FindClosestPaletteIndex(uint8_t red, uint8_t green, uint8_t blue)
    {
        PaletteIndex closest = PaletteIndex::pi255;
        int32_t closestDistance = INT32_MAX;

        for (auto i = PaletteIndex::transparent; i < PaletteIndex::waterWaves0; i = static_cast<PaletteIndex>(EnumValue(i) + 1))
        {
            const auto& paletteEntry = gPalette[EnumValue(i)];
            const int32_t distance = std::pow(paletteEntry.red - red, 2) + std::pow(paletteEntry.green - green, 2)
                + std::pow(paletteEntry.blue - blue, 2);

            if (distance < closestDistance)
            {
                closest = i;
                closestDistance = distance;
            }
        }

        return closest;
    }

    static void InitBlendColourMap()
    {
        for (size_t i = 0; i < kGamePaletteSize; i++)
        {
            for (size_t j = i; j < kGamePaletteSize; j++)
            {
                uint8_t red = (gPalette[i].red + gPalette[j].red) / 2;
                uint8_t green = (gPalette[i].green + gPalette[j].green) / 2;
                uint8_t blue = (gPalette[i].blue + gPalette[j].blue) / 2;

                auto colour = FindClosestPaletteIndex(red, green, blue);
                _blendColourMap[i][j] = colour;
                _blendColourMap[j][i] = colour;
            }
        }
        _blendColourMapInitialised = true;
    }

    BlendColourMapType* GetBlendColourMap()
    {
        if (!_blendColourMapInitialised)
        {
            InitBlendColourMap();
        }
        return &_blendColourMap;
    }

    PaletteIndex BlendColours(const PaletteIndex paletteIndex1, const PaletteIndex paletteIndex2)
    {
        if (!_blendColourMapInitialised)
        {
            InitBlendColourMap();
        }
        return _blendColourMap[EnumValue(paletteIndex1)][EnumValue(paletteIndex2)];
    }
} // namespace OpenRCT2::Drawing
#else
namespace OpenRCT2::Drawing
{
    BlendColourMapType* GetBlendColourMap()
    {
        return nullptr;
    }
} // namespace OpenRCT2::Drawing
#endif
