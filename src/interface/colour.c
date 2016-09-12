#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

rct_colour_map_a ColourMapA[32] = { 0 };
rct_colour_map_b ColourMapB[32] = { 0 };

void colours_init_maps()
{
	// Get colour maps from g1
	for (int i = 0; i < 32; i++) {
		rct_g1_element *g1Element = &g1Elements[4915 + i];

		uint8 *dstA = (uint8*)&ColourMapA[i];
		uint8 *dstB = (uint8*)&ColourMapB[i];
		for (int j = 0; j < 4; j++) {
			dstA[j] = g1Element->offset[0xF5 + j];
		}
		for (int j = 0; j < 4; j++) {
			dstA[j + 4] = g1Element->offset[0xF9 + j];
		}
		for (int j = 0; j < 4; j++) {
			dstB[j] = g1Element->offset[0xFD + j];
		}
	}
}
