/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Colour.h"

#include "../core/EnumMap.hpp"
#include "../drawing/Drawing.h"
#include "../sprites.h"

#include <algorithm>
#include <cmath>

ColourShadeMap ColourMapA[COLOUR_COUNT] = {};

enum
{
    INDEX_COLOUR_0 = 243,
    INDEX_COLOUR_1 = 245,
    INDEX_DARKEST = 245,
    INDEX_DARKER = 246,
    INDEX_DARK = 247,
    INDEX_MID_DARK = 248,
    INDEX_MID_LIGHT = 249,
    INDEX_LIGHT = 250,
    INDEX_LIGHTER = 251,
    INDEX_LIGHTEST = 252,
    INDEX_COLOUR_10 = 253,
    INDEX_COLOUR_11 = 254,
};

void ColoursInitMaps()
{
    // Get colour maps from g1
    for (int32_t i = 0; i < COLOUR_COUNT; i++)
    {
        // Get palette index in g1 / g2
        const auto paletteIndex = (i < COLOUR_NUM_ORIGINAL) ? SPR_PALETTE_2_START : SPR_G2_PALETTE_BEGIN - COLOUR_NUM_ORIGINAL;
        const G1Element* g1 = GfxGetG1Element(paletteIndex + i);
        if (g1 != nullptr)
        {
            ColourMapA[i].colour_0 = g1->offset[INDEX_COLOUR_0];
            ColourMapA[i].colour_1 = g1->offset[INDEX_COLOUR_1];
            ColourMapA[i].darkest = g1->offset[INDEX_DARKEST];
            ColourMapA[i].darker = g1->offset[INDEX_DARKER];
            ColourMapA[i].dark = g1->offset[INDEX_DARK];
            ColourMapA[i].mid_dark = g1->offset[INDEX_MID_DARK];
            ColourMapA[i].mid_light = g1->offset[INDEX_MID_LIGHT];
            ColourMapA[i].light = g1->offset[INDEX_LIGHT];
            ColourMapA[i].lighter = g1->offset[INDEX_LIGHTER];
            ColourMapA[i].lightest = g1->offset[INDEX_LIGHTEST];
            ColourMapA[i].colour_10 = g1->offset[INDEX_COLOUR_10];
            ColourMapA[i].colour_11 = g1->offset[INDEX_COLOUR_11];
        }
    }
}

namespace Colour
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

} // namespace Colour

#ifndef NO_TTF
static uint8_t BlendColourMap[PALETTE_COUNT][PALETTE_COUNT] = { 0 };

static uint8_t FindClosestPaletteIndex(uint8_t red, uint8_t green, uint8_t blue)
{
    int16_t closest = -1;
    int32_t closestDistance = INT32_MAX;

    for (int i = PALETTE_INDEX_0; i < PALETTE_INDEX_230; i++)
    {
        const int32_t distance = std::pow(gPalette[i].Red - red, 2) + std::pow(gPalette[i].Green - green, 2)
            + std::pow(gPalette[i].Blue - blue, 2);

        if (distance < closestDistance)
        {
            closest = i;
            closestDistance = distance;
        }
    }

    return closest;
}

uint8_t GetPaletteMapIndexForColour(colour_t colour)
{
    return COLOUR_USABLE_ORDER[colour];
}

uint8_t BlendColours(const uint8_t paletteIndex1, const uint8_t paletteIndex2)
{
    const uint8_t cMin = std::min(paletteIndex1, paletteIndex2);
    const uint8_t cMax = std::max(paletteIndex1, paletteIndex2);

    if (BlendColourMap[cMin][cMax] != 0)
    {
        return BlendColourMap[cMin][cMax];
    }

    uint8_t red = (gPalette[cMin].Red + gPalette[cMax].Red) / 2;
    uint8_t green = (gPalette[cMin].Green + gPalette[cMax].Green) / 2;
    uint8_t blue = (gPalette[cMin].Blue + gPalette[cMax].Blue) / 2;

    BlendColourMap[cMin][cMax] = FindClosestPaletteIndex(red, green, blue);
    return BlendColourMap[cMin][cMax];
}
#endif
