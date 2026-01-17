/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"
#include "../drawing/PaletteIndex.h"

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

constexpr size_t kPaletteCount = 256;

constexpr auto kPaletteOffsetDynamic = OpenRCT2::Drawing::PaletteIndex::pi10;
constexpr uint8_t kPaletteLengthDynamic = 236;

constexpr auto kPaletteOffsetWaterWaves = OpenRCT2::Drawing::PaletteIndex::pi230;
constexpr auto kPaletteOffsetWaterSparkles = OpenRCT2::Drawing::PaletteIndex::pi235;
constexpr uint8_t kPaletteLengthWaterWaves = 5;
constexpr uint8_t kPaletteLengthWaterSparkles = 5;

constexpr auto kPaletteOffsetTrackRails = OpenRCT2::Drawing::PaletteIndex::pi240;
constexpr uint8_t kPaletteLengthTrackRails = 3;

constexpr auto kPaletteOffsetRemapPrimary = OpenRCT2::Drawing::PaletteIndex::pi243;
constexpr auto kPaletteOffsetRemapSecondary = OpenRCT2::Drawing::PaletteIndex::pi202;
constexpr auto kPaletteOffsetRemapTertiary = OpenRCT2::Drawing::PaletteIndex::pi46;
constexpr uint8_t kPaletteLengthRemap = 12;

constexpr auto kPaletteOffsetAnimated = OpenRCT2::Drawing::PaletteIndex::pi230;
constexpr uint8_t kPaletteLengthAnimated = 16;

constexpr uint8_t kColourNumOriginal = 32;
constexpr uint8_t kColourNumNormal = 54;

constexpr colour_t kTextColour254 = 254;
constexpr colour_t kTextColour255 = 255;

struct ColourShadeMap
{
    OpenRCT2::Drawing::PaletteIndex colour0;
    OpenRCT2::Drawing::PaletteIndex colour1;
    OpenRCT2::Drawing::PaletteIndex darkest;
    OpenRCT2::Drawing::PaletteIndex darker;
    OpenRCT2::Drawing::PaletteIndex dark;
    OpenRCT2::Drawing::PaletteIndex midDark;
    OpenRCT2::Drawing::PaletteIndex midLight;
    OpenRCT2::Drawing::PaletteIndex light;
    OpenRCT2::Drawing::PaletteIndex lighter;
    OpenRCT2::Drawing::PaletteIndex lightest;
    OpenRCT2::Drawing::PaletteIndex colour10;
    OpenRCT2::Drawing::PaletteIndex colour11;
};

extern ColourShadeMap ColourMapA[COLOUR_COUNT];

void ColoursInitMaps();

namespace OpenRCT2::Colour
{
    colour_t FromString(std::string_view s, colour_t defaultValue = COLOUR_BLACK);
    u8string ToString(colour_t colour);
} // namespace OpenRCT2::Colour

#ifndef DISABLE_TTF
OpenRCT2::Drawing::PaletteIndex BlendColours(
    OpenRCT2::Drawing::PaletteIndex paletteIndex1, OpenRCT2::Drawing::PaletteIndex paletteIndex2);
#endif

typedef OpenRCT2::Drawing::PaletteIndex BlendColourMapType[kPaletteCount][kPaletteCount];
BlendColourMapType* GetBlendColourMap();
