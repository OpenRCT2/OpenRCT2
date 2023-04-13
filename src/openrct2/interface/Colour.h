/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <string_view>

using PaletteIndex = uint8_t;

/**
 * Colour IDs as used by the colour dropdown, NOT palette indices.
 */
enum : colour_t
{
    // Original Colours
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

    // Extended Colour Set
    COLOUR_DARK_OLIVE_DARK,
    COLOUR_DARK_OLIVE_LIGHT,
    COLOUR_SATURATED_BROWN_LIGHT,
    COLOUR_BORDEAUX_RED_DARK,
    COLOUR_BORDEAUX_RED_LIGHT,
    COLOUR_GRASS_GREEN_DARK,
    COLOUR_GRASS_GREEN_LIGHT,
    COLOUR_OLIVE_DARK,
    COLOUR_OLIVE_LIGHT,
    COLOUR_SATURATED_GREEN_LIGHT,
    COLOUR_TAN_DARK,
    COLOUR_TAN_LIGHT,
    COLOUR_DULL_PURPLE_LIGHT,
    COLOUR_DULL_GREEN_DARK,
    COLOUR_DULL_GREEN_LIGHT,
    COLOUR_SATURATED_PURPLE_DARK,
    COLOUR_SATURATED_PURPLE_LIGHT,
    COLOUR_ORANGE_LIGHT,
    COLOUR_AQUA_DARK,
    COLOUR_MAGENTA_LIGHT,
    COLOUR_DULL_BROWN_DARK,
    COLOUR_DULL_BROWN_LIGHT,
    COLOUR_INVISIBLE,
    COLOUR_VOID,

    COLOUR_COUNT,

    COLOUR_NULL = 255,
};

/**
 * These colours change depending on the current water colours.
 */
enum
{
    COLOUR_DARK_WATER = 9,
    COLOUR_LIGHT_WATER = 10,
    COLOUR_DEEP_WATER = 50
};

enum : PaletteIndex
{
    PALETTE_INDEX_0 = 0,     // Transparent
    PALETTE_INDEX_10 = 10,   // Black (0-dark), Dark grey (0)
    PALETTE_INDEX_11 = 11,   // Black (middark)
    PALETTE_INDEX_12 = 12,   // Black (midlight), Dark grey (1-darkest)
    PALETTE_INDEX_14 = 14,   // Black (lighter), Dark grey (dark)
    PALETTE_INDEX_13 = 13,   //
    PALETTE_INDEX_16 = 16,   //
    PALETTE_INDEX_17 = 17,   // Black (11), Dark grey (light), White (dark)
    PALETTE_INDEX_20 = 20,   // Dark grey (10), White (light)
    PALETTE_INDEX_21 = 21,   // Dark grey (11), White (lighter-11)
    PALETTE_INDEX_40 = 40,   //
    PALETTE_INDEX_42 = 42,   // Light Brown (lighter), Saturated brown (11)
    PALETTE_INDEX_44 = 44,   // Construction marker
    PALETTE_INDEX_46 = 46,   // Tertiary remap 0  / Yellow (darkest)
    PALETTE_INDEX_47 = 47,   // Tertiary remap 1  / Yellow
    PALETTE_INDEX_48 = 48,   // Tertiary remap 2  / Yellow
    PALETTE_INDEX_49 = 49,   // Tertiary remap 3  / Yellow
    PALETTE_INDEX_50 = 50,   // Tertiary remap 4  / Yellow
    PALETTE_INDEX_51 = 51,   // Tertiary remap 5  / Yellow
    PALETTE_INDEX_52 = 52,   // Tertiary remap 6  / Yellow
    PALETTE_INDEX_53 = 53,   // Tertiary remap 7  / Yellow
    PALETTE_INDEX_54 = 54,   // Tertiary remap 8  / Yellow
    PALETTE_INDEX_55 = 55,   // Tertiary remap 9  / Yellow
    PALETTE_INDEX_56 = 56,   // Tertiary remap 10 / Yellow
    PALETTE_INDEX_57 = 57,   // Tertiary remap 11 / Yellow (lightest)
    PALETTE_INDEX_61 = 61,   // Bordeaux Red (darker)
    PALETTE_INDEX_62 = 62,   //
    PALETTE_INDEX_68 = 68,   //
    PALETTE_INDEX_73 = 73,   //
    PALETTE_INDEX_99 = 99,   //
    PALETTE_INDEX_102 = 102, // Bright green (lighter)
    PALETTE_INDEX_108 = 108, //
    PALETTE_INDEX_111 = 111, //
    PALETTE_INDEX_129 = 129, // Light Purple (11)
    PALETTE_INDEX_135 = 135, //
    PALETTE_INDEX_136 = 136, // Dark Blue (10-11), Light Blue (midlight), Icy Blue (darker)
    PALETTE_INDEX_138 = 138, // Light Blue (lighter), Icy Blue (middark)
    PALETTE_INDEX_141 = 141, //
    PALETTE_INDEX_144 = 144, // Dark Green (1-darkest,
    PALETTE_INDEX_161 = 161, // Bright Purple (light)
    PALETTE_INDEX_162 = 162, //
    PALETTE_INDEX_171 = 171, // Saturated Red (lightest) Bright Red (middark)
    PALETTE_INDEX_172 = 172, // Saturated Red (10-11), Bright Red (midlight)
    PALETTE_INDEX_173 = 173, // Used to draw intense lines in the Ride Graphs window
    PALETTE_INDEX_183 = 183, // Used to draw rides in the Map window
    PALETTE_INDEX_186 = 186, //
    PALETTE_INDEX_194 = 194, //
    PALETTE_INDEX_195 = 195, //
    PALETTE_INDEX_202 = 202, // Secondary remap 0  / Pink (darkest)
    PALETTE_INDEX_203 = 203, // Secondary remap 1  / Pink
    PALETTE_INDEX_204 = 204, // Secondary remap 2  / Pink
    PALETTE_INDEX_205 = 205, // Secondary remap 3  / Pink
    PALETTE_INDEX_206 = 206, // Secondary remap 4  / Pink
    PALETTE_INDEX_207 = 207, // Secondary remap 5  / Pink
    PALETTE_INDEX_208 = 208, // Secondary remap 6  / Pink
    PALETTE_INDEX_209 = 209, // Secondary remap 7  / Pink
    PALETTE_INDEX_210 = 210, // Secondary remap 8  / Pink
    PALETTE_INDEX_211 = 211, // Secondary remap 9  / Pink
    PALETTE_INDEX_212 = 212, // Secondary remap 10 / Pink
    PALETTE_INDEX_213 = 213, // Secondary remap 11 / Pink (lightest)
    PALETTE_INDEX_222 = 222, //
    PALETTE_INDEX_230 = 230, // Water (waves)
    PALETTE_INDEX_231 = 231, // Water (waves)
    PALETTE_INDEX_232 = 232, // Water (waves)
    PALETTE_INDEX_233 = 233, // Water (waves)
    PALETTE_INDEX_234 = 234, // Water (waves)
    PALETTE_INDEX_235 = 235, // Water (sparkles)
    PALETTE_INDEX_236 = 236, // Water (sparkles)
    PALETTE_INDEX_237 = 237, // Water (sparkles)
    PALETTE_INDEX_238 = 238, // Water (sparkles)
    PALETTE_INDEX_239 = 239, // Water (sparkles)
    PALETTE_INDEX_240 = 240, // Track rails
    PALETTE_INDEX_241 = 241, // Track rails
    PALETTE_INDEX_242 = 242, // Track rails
    PALETTE_INDEX_243 = 243, // Primary remap 0
    PALETTE_INDEX_244 = 244, // Primary remap 1
    PALETTE_INDEX_245 = 245, // Primary remap 2
    PALETTE_INDEX_246 = 246, // Primary remap 3
    PALETTE_INDEX_247 = 247, // Primary remap 4
    PALETTE_INDEX_248 = 248, // Primary remap 5
    PALETTE_INDEX_249 = 249, // Primary remap 6
    PALETTE_INDEX_250 = 250, // Primary remap 7
    PALETTE_INDEX_251 = 251, // Primary remap 8
    PALETTE_INDEX_252 = 252, // Primary remap 9
    PALETTE_INDEX_253 = 253, // Primary remap 10
    PALETTE_INDEX_254 = 254, // Primary remap 11
};

constexpr size_t PALETTE_COUNT = 256;

constexpr uint8_t PALETTE_OFFSET_DYNAMIC = PALETTE_INDEX_10;
constexpr uint8_t PALETTE_LENGTH_DYNAMIC = 236;

constexpr uint8_t PALETTE_OFFSET_WATER_WAVES = PALETTE_INDEX_230;
constexpr uint8_t PALETTE_OFFSET_WATER_SPARKLES = PALETTE_INDEX_235;
constexpr uint8_t PALETTE_LENGTH_WATER_WAVES = 5;
constexpr uint8_t PALETTE_LENGTH_WATER_SPARKLES = 5;

constexpr uint8_t PALETTE_OFFSET_TRACK_RAILS = PALETTE_INDEX_240;
constexpr uint8_t PALETTE_LENGTH_TRACK_RAILS = 3;

constexpr uint8_t PALETTE_OFFSET_REMAP_PRIMARY = PALETTE_INDEX_243;
constexpr uint8_t PALETTE_OFFSET_REMAP_SECONDARY = PALETTE_INDEX_202;
constexpr uint8_t PALETTE_OFFSET_REMAP_TERTIARY = PALETTE_INDEX_46;
constexpr uint8_t PALETTE_LENGTH_REMAP = 12;

constexpr uint8_t PALETTE_OFFSET_ANIMATED = PALETTE_INDEX_230;
constexpr uint8_t PALETTE_LENGTH_ANIMATED = 16;

constexpr uint8_t COLOUR_NUM_ORIGINAL = 32;
constexpr uint8_t COLOUR_NUM_NORMAL = 54;

#define TEXT_COLOUR_254 (254)
#define TEXT_COLOUR_255 (255)

enum
{
    COLOUR_FLAG_OUTLINE = (1 << 5),
    COLOUR_FLAG_INSET = (1 << 6), // 64, 0x40
    COLOUR_FLAG_TRANSLUCENT = (1 << 7),
    COLOUR_FLAG_8 = (1 << 8)
};

#define TRANSLUCENT(x) ((x) | static_cast<uint8_t>(COLOUR_FLAG_TRANSLUCENT))
#define NOT_TRANSLUCENT(x) ((x) & ~static_cast<uint8_t>(COLOUR_FLAG_TRANSLUCENT))
#define BASE_COLOUR(x) ((x)&0x1F)

struct ColourShadeMap
{
    uint8_t colour_0;
    uint8_t colour_1;
    uint8_t darkest;
    uint8_t darker;
    uint8_t dark;
    uint8_t mid_dark;
    uint8_t mid_light;
    uint8_t light;
    uint8_t lighter;
    uint8_t lightest;
    uint8_t colour_10;
    uint8_t colour_11;
};

extern ColourShadeMap ColourMapA[COLOUR_COUNT];

void ColoursInitMaps();

namespace Colour
{
    colour_t FromString(std::string_view s, colour_t defaultValue = COLOUR_BLACK);
} // namespace Colour

#ifndef NO_TTF
uint8_t BlendColours(const uint8_t paletteIndex1, const uint8_t paletteIndex2);
#endif
