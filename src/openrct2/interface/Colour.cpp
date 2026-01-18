/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Colour.h"

#include "../SpriteIds.h"
#include "../core/EnumMap.hpp"
#include "../core/EnumUtils.hpp"
#include "../drawing/Drawing.h"

#include <cmath>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Colour
{
    static const EnumMap<colour_t> LookupTable{
        { "black", COLOUR_BLACK },
        { "grey", COLOUR_GREY },
        { "white", COLOUR_WHITE },
        { "dark_purple", COLOUR_DARK_PURPLE },
        { "light_purple", COLOUR_LIGHT_PURPLE },
        { "bright_purple", COLOUR_BRIGHT_PURPLE },
        { "dark_blue", COLOUR_DARK_BLUE },
        { "light_blue", COLOUR_LIGHT_BLUE },
        { "icy_blue", COLOUR_ICY_BLUE },
        { "teal", COLOUR_TEAL },
        { "aquamarine", COLOUR_AQUAMARINE },
        { "saturated_green", COLOUR_SATURATED_GREEN },
        { "dark_green", COLOUR_DARK_GREEN },
        { "moss_green", COLOUR_MOSS_GREEN },
        { "bright_green", COLOUR_BRIGHT_GREEN },
        { "olive_green", COLOUR_OLIVE_GREEN },
        { "dark_olive_green", COLOUR_DARK_OLIVE_GREEN },
        { "bright_yellow", COLOUR_BRIGHT_YELLOW },
        { "yellow", COLOUR_YELLOW },
        { "dark_yellow", COLOUR_DARK_YELLOW },
        { "light_orange", COLOUR_LIGHT_ORANGE },
        { "dark_orange", COLOUR_DARK_ORANGE },
        { "light_brown", COLOUR_LIGHT_BROWN },
        { "saturated_brown", COLOUR_SATURATED_BROWN },
        { "dark_brown", COLOUR_DARK_BROWN },
        { "salmon_pink", COLOUR_SALMON_PINK },
        { "bordeaux_red", COLOUR_BORDEAUX_RED },
        { "saturated_red", COLOUR_SATURATED_RED },
        { "bright_red", COLOUR_BRIGHT_RED },
        { "dark_pink", COLOUR_DARK_PINK },
        { "bright_pink", COLOUR_BRIGHT_PINK },
        { "light_pink", COLOUR_LIGHT_PINK },
        { "dark_olive_dark", COLOUR_DARK_OLIVE_DARK },
        { "dark_olive_light", COLOUR_DARK_OLIVE_LIGHT },
        { "saturated_brown_light", COLOUR_SATURATED_BROWN_LIGHT },
        { "bordeaux_red_dark", COLOUR_BORDEAUX_RED_DARK },
        { "bordeaux_red_light", COLOUR_BORDEAUX_RED_LIGHT },
        { "grass_green_dark", COLOUR_GRASS_GREEN_DARK },
        { "grass_green_light", COLOUR_GRASS_GREEN_LIGHT },
        { "olive_dark", COLOUR_OLIVE_DARK },
        { "olive_light", COLOUR_OLIVE_LIGHT },
        { "saturated_green_light", COLOUR_SATURATED_GREEN_LIGHT },
        { "tan_dark", COLOUR_TAN_DARK },
        { "tan_light", COLOUR_TAN_LIGHT },
        { "dull_purple_light", COLOUR_DULL_PURPLE_LIGHT },
        { "dull_green_dark", COLOUR_DULL_GREEN_DARK },
        { "dull_green_light", COLOUR_DULL_GREEN_LIGHT },
        { "saturated_purple_dark", COLOUR_SATURATED_PURPLE_DARK },
        { "saturated_purple_light", COLOUR_SATURATED_PURPLE_LIGHT },
        { "orange_light", COLOUR_ORANGE_LIGHT },
        { "aqua_dark", COLOUR_AQUA_DARK },
        { "magenta_light", COLOUR_MAGENTA_LIGHT },
        { "dull_brown_dark", COLOUR_DULL_BROWN_DARK },
        { "dull_brown_light", COLOUR_DULL_BROWN_LIGHT },
        { "invisible", COLOUR_INVISIBLE },
        { "void", COLOUR_VOID },
    };

    colour_t FromString(std::string_view s, colour_t defaultValue)
    {
        auto result = LookupTable.find(s);
        return (result != LookupTable.end()) ? result->second : defaultValue;
    }

    u8string ToString(colour_t colour)
    {
        auto result = LookupTable.find(colour);
        if (result != LookupTable.end())
            return u8string(result->first);

        return "black";
    }

} // namespace OpenRCT2::Colour

#ifndef DISABLE_TTF
static BlendColourMapType BlendColourMap = {};

static bool BlendColourMapInitialised = false;

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
            BlendColourMap[i][j] = colour;
            BlendColourMap[j][i] = colour;
        }
    }
    BlendColourMapInitialised = true;
}

BlendColourMapType* GetBlendColourMap()
{
    if (!BlendColourMapInitialised)
    {
        InitBlendColourMap();
    }
    return &BlendColourMap;
}

PaletteIndex BlendColours(const PaletteIndex paletteIndex1, const PaletteIndex paletteIndex2)
{
    if (!BlendColourMapInitialised)
    {
        InitBlendColourMap();
    }
    return BlendColourMap[EnumValue(paletteIndex1)][EnumValue(paletteIndex2)];
}
#else
BlendColourMapType* GetBlendColourMap()
{
    return nullptr;
}
#endif
