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

#include "../drawing/drawing.h"
#include "colour.h"
#include "../sprites.h"

rct_colour_map ColourMapA[32] = { 0 };

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
	for (sint32 i = 0; i < 32; i++) {
		rct_g1_element *g1Element = &g1Elements[SPR_PALETTE_2_START + i];

		ColourMapA[i].colour_0 = g1Element->offset[INDEX_COLOUR_0];
		ColourMapA[i].colour_1 = g1Element->offset[INDEX_COLOUR_1];
		ColourMapA[i].darkest = g1Element->offset[INDEX_DARKEST];
		ColourMapA[i].darker = g1Element->offset[INDEX_DARKER];
		ColourMapA[i].dark = g1Element->offset[INDEX_DARK];
		ColourMapA[i].mid_dark = g1Element->offset[INDEX_MID_DARK];
		ColourMapA[i].mid_light = g1Element->offset[INDEX_MID_LIGHT];
		ColourMapA[i].light = g1Element->offset[INDEX_LIGHT];
		ColourMapA[i].lighter = g1Element->offset[INDEX_LIGHTER];
		ColourMapA[i].lightest = g1Element->offset[INDEX_LIGHTEST];
		ColourMapA[i].colour_10 = g1Element->offset[INDEX_COLOUR_10];
		ColourMapA[i].colour_11 = g1Element->offset[INDEX_COLOUR_11];
	}
}
