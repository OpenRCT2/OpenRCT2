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

#ifndef _COLOUR_H_
#define _COLOUR_H_

#include "../common.h"

/**
 * Colour IDs as used by the colour dropdown, NOT palette indices.
 */
enum {
	COLOUR_BLACK,
	COLOUR_GREY,
	COLOUR_WHITE,
	COLOUR_DARK_PURPLE,
	COLOUR_LIGHT_PURPLE,
	COLOUR_BRIGHT_PURPLE,
	COLOUR_DARK_BLUE,
	COLOUR_LIGHT_BLUE,
	COLOUR_ICY_BLUE,
	COLOUR_TEAL,
	COLOUR_AQUAMARINE,
	COLOUR_SATURATED_GREEN,
	COLOUR_DARK_GREEN,
	COLOUR_MOSS_GREEN,
	COLOUR_BRIGHT_GREEN,
	COLOUR_OLIVE_GREEN,
	COLOUR_DARK_OLIVE_GREEN,
	COLOUR_BRIGHT_YELLOW,
	COLOUR_YELLOW,
	COLOUR_DARK_YELLOW,
	COLOUR_LIGHT_ORANGE,
	COLOUR_DARK_ORANGE,
	COLOUR_LIGHT_BROWN,
	COLOUR_SATURATED_BROWN,
	COLOUR_DARK_BROWN,
	COLOUR_SALMON_PINK,
	COLOUR_BORDEAUX_RED,
	COLOUR_SATURATED_RED,
	COLOUR_BRIGHT_RED,
	COLOUR_DARK_PINK,
	COLOUR_BRIGHT_PINK,
	COLOUR_LIGHT_PINK,

	COLOUR_COUNT
};

/**
 * These colours change depending on the current water colours.
 */
enum {
	COLOUR_DARK_WATER = 9,
	COLOUR_LIGHT_WATER = 10
};

#define TEXT_COLOUR_254			(254)
#define TEXT_COLOUR_255			(255)

#define COLOUR_FLAG_OUTLINE		(1 << 5)
#define COLOUR_FLAG_INSET 		(1 << 6) // 64, 0x40
#define COLOUR_FLAG_TRANSLUCENT (1 << 7)
#define COLOUR_FLAG_8 			(1 << 8)
#define TRANSLUCENT(x) ((x) | COLOUR_FLAG_TRANSLUCENT)
#define NOT_TRANSLUCENT(x) ((x) & ~COLOUR_FLAG_TRANSLUCENT)
#define BASE_COLOUR(x) 			((x) & 0x1F)

#define NUM_COLOURS 32

typedef struct rct_colour_map {
	uint8 colour_0;
	uint8 colour_1;
	uint8 darkest;
	uint8 darker;
	uint8 dark;
	uint8 mid_dark;
	uint8 mid_light;
	uint8 light;
	uint8 lighter;
	uint8 lightest;
	uint8 colour_10;
	uint8 colour_11;
} rct_colour_map;

extern rct_colour_map ColourMapA[COLOUR_COUNT];

void colours_init_maps();

#endif
