/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"

#include <cstdint>
#include <string_view>

using colour_t = uint8_t;

/**
 * Colour IDs as used by the colour dropdown, NOT palette indices.
 */
enum : colour_t
{
    // Original Colours
    COLOUR_BLACK,            // Black
    COLOUR_GREY,             // Grey
    COLOUR_WHITE,            // White
    COLOUR_DARK_PURPLE,      // Dark purple
    COLOUR_LIGHT_PURPLE,     // Light purple
    COLOUR_BRIGHT_PURPLE,    // Bright purple
    COLOUR_DARK_BLUE,        // Dark blue
    COLOUR_LIGHT_BLUE,       // Light blue
    COLOUR_ICY_BLUE,         // Icy blue
    COLOUR_TEAL,             // Dark water
    COLOUR_AQUAMARINE,       // Light water
    COLOUR_SATURATED_GREEN,  // Saturated green
    COLOUR_DARK_GREEN,       // Dark green
    COLOUR_MOSS_GREEN,       // Moss green
    COLOUR_BRIGHT_GREEN,     // Bright green
    COLOUR_OLIVE_GREEN,      // Olive green
    COLOUR_DARK_OLIVE_GREEN, // Dark olive green
    COLOUR_BRIGHT_YELLOW,    // Bright yellow
    COLOUR_YELLOW,           // Yellow
    COLOUR_DARK_YELLOW,      // Dark yellow
    COLOUR_LIGHT_ORANGE,     // Light orange
    COLOUR_DARK_ORANGE,      // Dark orange
    COLOUR_LIGHT_BROWN,      // Light brown
    COLOUR_SATURATED_BROWN,  // Saturated brown
    COLOUR_DARK_BROWN,       // Dark brown
    COLOUR_SALMON_PINK,      // Salmon pink
    COLOUR_BORDEAUX_RED,     // Bordeaux red
    COLOUR_SATURATED_RED,    // Saturated red
    COLOUR_BRIGHT_RED,       // Bright red
    COLOUR_DARK_PINK,        // Dark pink
    COLOUR_BRIGHT_PINK,      // Bright pink
    COLOUR_LIGHT_PINK,       // Light pink

    // Extended Colour Set
    COLOUR_DARK_OLIVE_DARK,        // Army green
    COLOUR_DARK_OLIVE_LIGHT,       // Honeydew
    COLOUR_SATURATED_BROWN_LIGHT,  // Tan
    COLOUR_BORDEAUX_RED_DARK,      // Maroon
    COLOUR_BORDEAUX_RED_LIGHT,     // Coral pink
    COLOUR_GRASS_GREEN_DARK,       // Forest green
    COLOUR_GRASS_GREEN_LIGHT,      // Chartreuse
    COLOUR_OLIVE_DARK,             // Hunter green
    COLOUR_OLIVE_LIGHT,            // Celadon
    COLOUR_SATURATED_GREEN_LIGHT,  // Lime green
    COLOUR_TAN_DARK,               // Sepia
    COLOUR_TAN_LIGHT,              // Peach
    COLOUR_DULL_PURPLE_LIGHT,      // Periwinkle
    COLOUR_DULL_GREEN_DARK,        // Viridian
    COLOUR_DULL_GREEN_LIGHT,       // Seafoam green
    COLOUR_SATURATED_PURPLE_DARK,  // Violet
    COLOUR_SATURATED_PURPLE_LIGHT, // Lavender
    COLOUR_ORANGE_LIGHT,           // Pastel orange
    COLOUR_AQUA_DARK,              // Deep water
    COLOUR_MAGENTA_LIGHT,          // Pastel pink
    COLOUR_DULL_BROWN_DARK,        // Umber
    COLOUR_DULL_BROWN_LIGHT,       // Beige
    COLOUR_INVISIBLE,              // Invisible
    COLOUR_VOID,                   // Void

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

enum PaletteIndex : uint8_t
{
    _0 = 0,     // Transparent
    _10 = 10,   // Black (0-dark), Dark grey (0)
    _11 = 11,   // Black (middark)
    _12 = 12,   // Black (midlight), Dark grey (1-darkest)
    _14 = 14,   // Black (lighter), Dark grey (dark)
    _13 = 13,   //
    _16 = 16,   //
    _17 = 17,   // Black (11), Dark grey (light), White (dark)
    _20 = 20,   // Dark grey (10), White (light)
    _21 = 21,   // Dark grey (11), White (lighter-11)
    _40 = 40,   //
    _42 = 42,   // Light Brown (lighter), Saturated brown (11)
    _44 = 44,   // Construction marker
    _46 = 46,   // Tertiary remap 0  / Yellow (darkest)
    _47 = 47,   // Tertiary remap 1  / Yellow
    _48 = 48,   // Tertiary remap 2  / Yellow
    _49 = 49,   // Tertiary remap 3  / Yellow
    _50 = 50,   // Tertiary remap 4  / Yellow
    _51 = 51,   // Tertiary remap 5  / Yellow
    _52 = 52,   // Tertiary remap 6  / Yellow
    _53 = 53,   // Tertiary remap 7  / Yellow
    _54 = 54,   // Tertiary remap 8  / Yellow
    _55 = 55,   // Tertiary remap 9  / Yellow
    _56 = 56,   // Tertiary remap 10 / Yellow
    _57 = 57,   // Tertiary remap 11 / Yellow (lightest)
    _61 = 61,   // Bordeaux Red (darker)
    _62 = 62,   //
    _68 = 68,   //
    _73 = 73,   //
    _99 = 99,   //
    _102 = 102, // Bright green (lighter)
    _108 = 108, //
    _111 = 111, //
    _129 = 129, // Light Purple (11)
    _135 = 135, //
    _136 = 136, // Dark Blue (10-11), Light Blue (midlight), Icy Blue (darker)
    _138 = 138, // Light Blue (lighter), Icy Blue (middark)
    _141 = 141, //
    _144 = 144, // Dark Green (1-darkest,
    _161 = 161, // Bright Purple (light)
    _162 = 162, //
    _171 = 171, // Saturated Red (lightest) Bright Red (middark)
    _172 = 172, // Saturated Red (10-11), Bright Red (midlight)
    _173 = 173, // Used to draw intense lines in the Ride Graphs window
    _183 = 183, // Used to draw rides in the Map window
    _186 = 186, //
    _194 = 194, //
    _195 = 195, //
    _202 = 202, // Secondary remap 0  / Pink (darkest)
    _203 = 203, // Secondary remap 1  / Pink
    _204 = 204, // Secondary remap 2  / Pink
    _205 = 205, // Secondary remap 3  / Pink
    _206 = 206, // Secondary remap 4  / Pink
    _207 = 207, // Secondary remap 5  / Pink
    _208 = 208, // Secondary remap 6  / Pink
    _209 = 209, // Secondary remap 7  / Pink
    _210 = 210, // Secondary remap 8  / Pink
    _211 = 211, // Secondary remap 9  / Pink
    _212 = 212, // Secondary remap 10 / Pink
    _213 = 213, // Secondary remap 11 / Pink (lightest)
    _222 = 222, //
    _230 = 230, // Water (waves)
    _231 = 231, // Water (waves)
    _232 = 232, // Water (waves)
    _233 = 233, // Water (waves)
    _234 = 234, // Water (waves)
    _235 = 235, // Water (sparkles)
    _236 = 236, // Water (sparkles)
    _237 = 237, // Water (sparkles)
    _238 = 238, // Water (sparkles)
    _239 = 239, // Water (sparkles)
    _240 = 240, // Track rails
    _241 = 241, // Track rails
    _242 = 242, // Track rails
    _243 = 243, // Primary remap 0
    _244 = 244, // Primary remap 1
    _245 = 245, // Primary remap 2
    _246 = 246, // Primary remap 3
    _247 = 247, // Primary remap 4
    _248 = 248, // Primary remap 5
    _249 = 249, // Primary remap 6
    _250 = 250, // Primary remap 7
    _251 = 251, // Primary remap 8
    _252 = 252, // Primary remap 9
    _253 = 253, // Primary remap 10
    _254 = 254, // Primary remap 11
};

constexpr size_t kPaletteCount = 256;

constexpr auto kPaletteOffsetDynamic = PaletteIndex::_10;
constexpr uint8_t kPaletteLengthDynamic = 236;

constexpr auto kPaletteOffsetWaterWaves = PaletteIndex::_230;
constexpr auto kPaletteOffsetWaterSparkles = PaletteIndex::_235;
constexpr uint8_t kPaletteLengthWaterWaves = 5;
constexpr uint8_t kPaletteLengthWaterSparkles = 5;

constexpr auto kPaletteOffsetTrackRails = PaletteIndex::_240;
constexpr uint8_t kPaletteLengthTrackRails = 3;

constexpr auto kPaletteOffsetRemapPrimary = PaletteIndex::_243;
constexpr auto kPaletteOffsetRemapSecondary = PaletteIndex::_202;
constexpr auto kPaletteOffsetRemapTertiary = PaletteIndex::_46;
constexpr uint8_t kPaletteLengthRemap = 12;

constexpr auto kPaletteOffsetAnimated = PaletteIndex::_230;
constexpr uint8_t kPaletteLengthAnimated = 16;

constexpr uint8_t kColourNumOriginal = 32;
constexpr uint8_t kColourNumNormal = 54;

static constexpr uint8_t kLegacyColourFlagTranslucent = (1 << 7);

constexpr colour_t kTextColour254 = 254;
constexpr colour_t kTextColour255 = 255;

enum class ColourFlag : uint8_t
{
    translucent,
    inset,
    withOutline,
};

struct ColourWithFlags
{
    colour_t colour{};
    uint8_t flags{};

    bool hasFlag(ColourFlag flag) const;

    void setFlag(ColourFlag flag, bool on);

    ColourWithFlags withFlag(ColourFlag flag, bool on) const;

    static ColourWithFlags fromLegacy(uint8_t legacy);

    ColourWithFlags& operator=(colour_t rhs);
};

struct ColourShadeMap
{
    PaletteIndex colour_0;
    PaletteIndex colour_1;
    PaletteIndex darkest;
    PaletteIndex darker;
    PaletteIndex dark;
    PaletteIndex mid_dark;
    PaletteIndex mid_light;
    PaletteIndex light;
    PaletteIndex lighter;
    PaletteIndex lightest;
    PaletteIndex colour_10;
    PaletteIndex colour_11;
};

extern ColourShadeMap ColourMapA[COLOUR_COUNT];

void ColoursInitMaps();

namespace OpenRCT2::Colour
{
    colour_t FromString(std::string_view s, colour_t defaultValue = COLOUR_BLACK);
    u8string ToString(colour_t colour);
} // namespace OpenRCT2::Colour

#ifndef NO_TTF
uint8_t BlendColours(const uint8_t paletteIndex1, const uint8_t paletteIndex2);
#endif

typedef uint8_t BlendColourMapType[kPaletteCount][kPaletteCount];
BlendColourMapType* GetBlendColourMap();
