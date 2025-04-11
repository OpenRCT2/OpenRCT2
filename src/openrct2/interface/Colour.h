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
    pi0 = 0,     // Transparent
    pi10 = 10,   // Black (0-dark), Dark grey (0)
    pi11 = 11,   // Black (middark)
    pi12 = 12,   // Black (midlight), Dark grey (1-darkest)
    pi14 = 14,   // Black (lighter), Dark grey (dark)
    pi13 = 13,   //
    pi16 = 16,   //
    pi17 = 17,   // Black (11), Dark grey (light), White (dark)
    pi20 = 20,   // Dark grey (10), White (light)
    pi21 = 21,   // Dark grey (11), White (lighter-11)
    pi40 = 40,   //
    pi42 = 42,   // Light Brown (lighter), Saturated brown (11)
    pi44 = 44,   // Construction marker
    pi46 = 46,   // Tertiary remap 0  / Yellow (darkest)
    pi47 = 47,   // Tertiary remap 1  / Yellow
    pi48 = 48,   // Tertiary remap 2  / Yellow
    pi49 = 49,   // Tertiary remap 3  / Yellow
    pi50 = 50,   // Tertiary remap 4  / Yellow
    pi51 = 51,   // Tertiary remap 5  / Yellow
    pi52 = 52,   // Tertiary remap 6  / Yellow
    pi53 = 53,   // Tertiary remap 7  / Yellow
    pi54 = 54,   // Tertiary remap 8  / Yellow
    pi55 = 55,   // Tertiary remap 9  / Yellow
    pi56 = 56,   // Tertiary remap 10 / Yellow
    pi57 = 57,   // Tertiary remap 11 / Yellow (lightest)
    pi61 = 61,   // Bordeaux Red (darker)
    pi62 = 62,   //
    pi68 = 68,   //
    pi73 = 73,   //
    pi99 = 99,   //
    pi102 = 102, // Bright green (lighter)
    pi108 = 108, //
    pi111 = 111, //
    pi129 = 129, // Light Purple (11)
    pi135 = 135, //
    pi136 = 136, // Dark Blue (10-11), Light Blue (midlight), Icy Blue (darker)
    pi138 = 138, // Light Blue (lighter), Icy Blue (middark)
    pi141 = 141, //
    pi144 = 144, // Dark Green (1-darkest,
    pi161 = 161, // Bright Purple (light)
    pi162 = 162, //
    pi171 = 171, // Saturated Red (lightest) Bright Red (middark)
    pi172 = 172, // Saturated Red (10-11), Bright Red (midlight)
    pi173 = 173, // Used to draw intense lines in the Ride Graphs window
    pi183 = 183, // Used to draw rides in the Map window
    pi186 = 186, //
    pi194 = 194, //
    pi195 = 195, //
    pi202 = 202, // Secondary remap 0  / Pink (darkest)
    pi203 = 203, // Secondary remap 1  / Pink
    pi204 = 204, // Secondary remap 2  / Pink
    pi205 = 205, // Secondary remap 3  / Pink
    pi206 = 206, // Secondary remap 4  / Pink
    pi207 = 207, // Secondary remap 5  / Pink
    pi208 = 208, // Secondary remap 6  / Pink
    pi209 = 209, // Secondary remap 7  / Pink
    pi210 = 210, // Secondary remap 8  / Pink
    pi211 = 211, // Secondary remap 9  / Pink
    pi212 = 212, // Secondary remap 10 / Pink
    pi213 = 213, // Secondary remap 11 / Pink (lightest)
    pi222 = 222, //
    pi230 = 230, // Water (waves)
    pi231 = 231, // Water (waves)
    pi232 = 232, // Water (waves)
    pi233 = 233, // Water (waves)
    pi234 = 234, // Water (waves)
    pi235 = 235, // Water (sparkles)
    pi236 = 236, // Water (sparkles)
    pi237 = 237, // Water (sparkles)
    pi238 = 238, // Water (sparkles)
    pi239 = 239, // Water (sparkles)
    pi240 = 240, // Track rails
    pi241 = 241, // Track rails
    pi242 = 242, // Track rails
    pi243 = 243, // Primary remap 0
    pi244 = 244, // Primary remap 1
    pi245 = 245, // Primary remap 2
    pi246 = 246, // Primary remap 3
    pi247 = 247, // Primary remap 4
    pi248 = 248, // Primary remap 5
    pi249 = 249, // Primary remap 6
    pi250 = 250, // Primary remap 7
    pi251 = 251, // Primary remap 8
    pi252 = 252, // Primary remap 9
    pi253 = 253, // Primary remap 10
    pi254 = 254, // Primary remap 11
};

constexpr size_t kPaletteCount = 256;

constexpr auto kPaletteOffsetDynamic = PaletteIndex::pi10;
constexpr uint8_t kPaletteLengthDynamic = 236;

constexpr auto kPaletteOffsetWaterWaves = PaletteIndex::pi230;
constexpr auto kPaletteOffsetWaterSparkles = PaletteIndex::pi235;
constexpr uint8_t kPaletteLengthWaterWaves = 5;
constexpr uint8_t kPaletteLengthWaterSparkles = 5;

constexpr auto kPaletteOffsetTrackRails = PaletteIndex::pi240;
constexpr uint8_t kPaletteLengthTrackRails = 3;

constexpr auto kPaletteOffsetRemapPrimary = PaletteIndex::pi243;
constexpr auto kPaletteOffsetRemapSecondary = PaletteIndex::pi202;
constexpr auto kPaletteOffsetRemapTertiary = PaletteIndex::pi46;
constexpr uint8_t kPaletteLengthRemap = 12;

constexpr auto kPaletteOffsetAnimated = PaletteIndex::pi230;
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

#ifndef DISABLE_TTF
uint8_t BlendColours(const uint8_t paletteIndex1, const uint8_t paletteIndex2);
#endif

typedef uint8_t BlendColourMapType[kPaletteCount][kPaletteCount];
BlendColourMapType* GetBlendColourMap();
