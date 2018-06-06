#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <algorithm>
#include <cmath>
#include "../drawing/Drawing.h"
#include "../sprites.h"
#include "Colour.h"

rct_colour_map ColourMapA[COLOUR_COUNT] = {};

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

void colours_init_maps()
{
    // Get colour maps from g1
    for (sint32 i = 0; i < COLOUR_COUNT; i++)
    {
        const rct_g1_element * g1 = gfx_get_g1_element(SPR_PALETTE_2_START + i);
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

#ifndef NO_TTF
static uint8 BlendColourMap[PALETTE_COUNT][PALETTE_COUNT] = {0};

static uint8 findClosestPaletteIndex(uint8 red, uint8 green, uint8 blue)
{
    sint16 closest = -1;
    sint32 closestDistance = INT32_MAX;

    for (int i = PALETTE_INDEX_0; i < PALETTE_INDEX_230; i++)
    {
        const sint32 distance =
            std::pow(gPalette[i].red - red, 2) + std::pow(gPalette[i].green - green, 2) + std::pow(gPalette[i].blue - blue, 2);

        if (distance < closestDistance)
        {
            closest = i;
            closestDistance = distance;
        }
    }

    return closest;
}

uint8 blendColours(const uint8 paletteIndex1, const uint8 paletteIndex2)
{
    const uint8 cMin = std::min(paletteIndex1, paletteIndex2);
    const uint8 cMax = std::max(paletteIndex1, paletteIndex2);

    if (BlendColourMap[cMin][cMax] != 0)
    {
        return BlendColourMap[cMin][cMax];
    }

    uint8 red = (gPalette[cMin].red + gPalette[cMax].red) / 2;
    uint8 green = (gPalette[cMin].green + gPalette[cMax].green) / 2;
    uint8 blue = (gPalette[cMin].blue + gPalette[cMax].blue) / 2;

    BlendColourMap[cMin][cMax] = findClosestPaletteIndex(red, green, blue);
    return BlendColourMap[cMin][cMax];
}
#endif


