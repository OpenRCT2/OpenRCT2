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

#ifndef _COLOUR_H_
#define _COLOUR_H_

#include "../common.h"

/**
 * Colour IDs as used by the colour dropdown, NOT palette indices.
 */
enum
{
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
enum
{
    COLOUR_DARK_WATER = 9,
    COLOUR_LIGHT_WATER = 10
};

enum
{
    PALETTE_INDEX_0 = 0,        //
    PALETTE_INDEX_10 = 10,      // Black (0-dark), Dark gray (0)
    PALETTE_INDEX_11 = 11,      // Black (middark)
    PALETTE_INDEX_12 = 12,      // Black (midlight), Dark gray (1-darkest)
    PALETTE_INDEX_14 = 14,      // Black (lighter), Dark gray (dark)
    PALETTE_INDEX_13 = 13,      //
    PALETTE_INDEX_16 = 16,      //
    PALETTE_INDEX_17 = 17,      // Black (11), Dark gray (light), White (dark)
    PALETTE_INDEX_20 = 20,      // Dark gray (10), White (light)
    PALETTE_INDEX_21 = 21,      // Dark gray (11), White (lighter-11)
    PALETTE_INDEX_40 = 40,      //
    PALETTE_INDEX_42 = 42,      // Light Brown (lighter), Saturated brown (11)
    PALETTE_INDEX_54 = 54,      //
    PALETTE_INDEX_55 = 55,      // Bright Yellow (light), Yellow (lightest)
    PALETTE_INDEX_56 = 56,      // Bright Yellow (lighter, lightest), Yellow (10)
    PALETTE_INDEX_61 = 61,      // Bordeaux Red (darker)
    PALETTE_INDEX_62 = 62,      //
    PALETTE_INDEX_68 = 68,      //
    PALETTE_INDEX_73 = 73,      //
    PALETTE_INDEX_99 = 99,      //
    PALETTE_INDEX_102 = 102,    // Bright green (lighter)
    PALETTE_INDEX_108 = 108,    //
    PALETTE_INDEX_111 = 111,    //
    PALETTE_INDEX_129 = 129,    // Light Purple (11)
    PALETTE_INDEX_135 = 135,    //
    PALETTE_INDEX_136 = 136,    // Dark Blue (10-11), Light Blue (midlight), Icy Blue (darker)
    PALETTE_INDEX_138 = 138,    // Light Blue (lighter), Icy Blue (middark)
    PALETTE_INDEX_141 = 141,    //
    PALETTE_INDEX_144 = 144,    // Dark Green (1-darkest,
    PALETTE_INDEX_161 = 161,    // Bright Purple (light)
    PALETTE_INDEX_162 = 162,    //
    PALETTE_INDEX_171 = 171,    // Saturated Red (lightest) Bright Red (middark)
    PALETTE_INDEX_172 = 172,    // Saturated Red (10-11), Bright Red (midlight)
    PALETTE_INDEX_183 = 183,    // Used to draw rides in the Map window
    PALETTE_INDEX_186 = 186,    //
    PALETTE_INDEX_194 = 194,    //
    PALETTE_INDEX_195 = 195,    //
    PALETTE_INDEX_209 = 209,    // Bright Pink (light)
    PALETTE_INDEX_222 = 222,    //
    PALETTE_INDEX_245 = 245,    //
};

#define TEXT_COLOUR_254         (254)
#define TEXT_COLOUR_255         (255)

#define COLOUR_FLAG_OUTLINE     (1 << 5)
#define COLOUR_FLAG_INSET       (1 << 6) // 64, 0x40
#define COLOUR_FLAG_TRANSLUCENT (1 << 7)
#define COLOUR_FLAG_8           (1 << 8)
#define TRANSLUCENT(x) ((x) | COLOUR_FLAG_TRANSLUCENT)
#define NOT_TRANSLUCENT(x) ((x) & ~COLOUR_FLAG_TRANSLUCENT)
#define BASE_COLOUR(x)          ((x) & 0x1F)

typedef struct rct_colour_map
{
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

#ifdef __cplusplus
extern "C" {
#endif

extern rct_colour_map ColourMapA[COLOUR_COUNT];

void colours_init_maps();

#ifdef __cplusplus
}
#endif

#endif
