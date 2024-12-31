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
using PaletteIndex = uint8_t;

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

constexpr size_t kPaletteCount = 256;

constexpr uint8_t kPaletteOffsetDynamic = PALETTE_INDEX_10;
constexpr uint8_t kPaletteLengthDynamic = 236;

constexpr uint8_t kPaletteOffsetWaterWaves = PALETTE_INDEX_230;
constexpr uint8_t kPaletteOffsetWaterSparkles = PALETTE_INDEX_235;
constexpr uint8_t kPaletteLengthWaterWaves = 5;
constexpr uint8_t kPaletteLengthWaterSparkles = 5;

constexpr uint8_t kPaletteOffsetTrackRails = PALETTE_INDEX_240;
constexpr uint8_t kPaletteLengthTrackRails = 3;

constexpr uint8_t kPaletteOffsetRemapPrimary = PALETTE_INDEX_243;
constexpr uint8_t kPaletteOffsetRemapSecondary = PALETTE_INDEX_202;
constexpr uint8_t kPaletteOffsetRemapTertiary = PALETTE_INDEX_46;
constexpr uint8_t kPaletteLengthRemap = 12;

constexpr uint8_t kPaletteOffsetAnimated = PALETTE_INDEX_230;
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
