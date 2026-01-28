/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../SpriteIds.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../object/ObjectEntryManager.h"
#include "../object/WaterEntry.h"
#include "../platform/Platform.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Location.hpp"
#include "LightFX.h"
#include "Rectangle.h"

#include <array>
#include <cassert>
#include <cstring>
#include <numeric>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

static constexpr auto kPaletteOffsetDynamic = PaletteIndex::pi10;
static constexpr uint8_t kPaletteLengthDynamic = 236;

static constexpr uint8_t kPaletteLengthWaterWaves = 5;
static constexpr uint8_t kPaletteLengthWaterSparkles = 5;

static constexpr auto kPaletteOffsetAnimated = PaletteIndex::waterWaves0;
static constexpr uint8_t kPaletteLengthAnimated = 16;

static auto _defaultPaletteMapping = []() {
    std::array<PaletteIndex, 256> res;
    for (size_t i = 0; i < std::size(res); i++)
    {
        res[i] = static_cast<PaletteIndex>(i);
    }
    return res;
}();

PaletteMap PaletteMap::GetDefault()
{
    return PaletteMap(_defaultPaletteMapping);
}

PaletteIndex& PaletteMap::operator[](size_t index)
{
    return _data[index];
}

PaletteIndex PaletteMap::operator[](size_t index) const
{
    return _data[index];
}

PaletteIndex PaletteMap::Blend(PaletteIndex src, PaletteIndex dst) const
{
#ifdef _DEBUG
    // src = 0 would be transparent so there is no blend palette for that, hence (src - 1)
    assert(src != PaletteIndex::transparent && (EnumValue(src) - 1) < _numMaps);
    assert(EnumValue(dst) < _mapLength);
#endif
    auto idx = ((EnumValue(src) - 1) * 256) + EnumValue(dst);
    return _data[idx];
}

void PaletteMap::Copy(PaletteIndex dstIndex, const PaletteMap& src, PaletteIndex srcIndex, size_t length)
{
    auto srcOffset = EnumValue(srcIndex);
    auto dstOffset = EnumValue(dstIndex);
    auto maxLength = std::min(_data.size() - srcOffset, _data.size() - dstOffset);
    assert(length <= maxLength);
    auto copyLength = std::min(length, maxLength);
    std::copy(src._data.begin() + srcOffset, src._data.begin() + srcOffset + copyLength, _data.begin() + dstOffset);
}

GamePalette gPalette;
GamePalette gGamePalette;
uint32_t gPaletteEffectFrame;

ImageId gPickupPeepImage;
int32_t gPickupPeepX;
int32_t gPickupPeepY;

// Originally 0x9ABE04
TextColours gTextPalette = {
    PaletteIndex::transparent,
    PaletteIndex::transparent,
    PaletteIndex::transparent,
};

bool gPaintForceRedraw{ false };

static constexpr FilterPaletteID kGlassPaletteIds[kColourNumTotal] = {
    FilterPaletteID::paletteGlassBlack,
    FilterPaletteID::paletteGlassGrey,
    FilterPaletteID::paletteGlassWhite,
    FilterPaletteID::paletteGlassDarkPurple,
    FilterPaletteID::paletteGlassLightPurple,
    FilterPaletteID::paletteGlassBrightPurple,
    FilterPaletteID::paletteGlassDarkBlue,
    FilterPaletteID::paletteGlassLightBlue,
    FilterPaletteID::paletteGlassIcyBlue,
    FilterPaletteID::paletteGlassTeal,
    FilterPaletteID::paletteGlassAquamarine,
    FilterPaletteID::paletteGlassSaturatedGreen,
    FilterPaletteID::paletteGlassDarkGreen,
    FilterPaletteID::paletteGlassMossGreen,
    FilterPaletteID::paletteGlassBrightGreen,
    FilterPaletteID::paletteGlassOliveGreen,
    FilterPaletteID::paletteGlassDarkOliveGreen,
    FilterPaletteID::paletteGlassBrightYellow,
    FilterPaletteID::paletteGlassYellow,
    FilterPaletteID::paletteGlassDarkYellow,
    FilterPaletteID::paletteGlassLightOrange,
    FilterPaletteID::paletteGlassDarkOrange,
    FilterPaletteID::paletteGlassLightBrown,
    FilterPaletteID::paletteGlassSaturatedBrown,
    FilterPaletteID::paletteGlassDarkBrown,
    FilterPaletteID::paletteGlassSalmonPink,
    FilterPaletteID::paletteGlassBordeauxRed,
    FilterPaletteID::paletteGlassSaturatedRed,
    FilterPaletteID::paletteGlassBrightRed,
    FilterPaletteID::paletteGlassDarkPink,
    FilterPaletteID::paletteGlassBrightPink,
    FilterPaletteID::paletteGlassLightPink,
    FilterPaletteID::paletteGlassDarkOliveDark,
    FilterPaletteID::paletteGlassDarkOliveLight,
    FilterPaletteID::paletteGlassSaturatedBrownLight,
    FilterPaletteID::paletteGlassBordeauxRedDark,
    FilterPaletteID::paletteGlassBordeauxRedLight,
    FilterPaletteID::paletteGlassGrassGreenDark,
    FilterPaletteID::paletteGlassGrassGreenLight,
    FilterPaletteID::paletteGlassOliveDark,
    FilterPaletteID::paletteGlassOliveLight,
    FilterPaletteID::paletteGlassSaturatedGreenLight,
    FilterPaletteID::paletteGlassTanDark,
    FilterPaletteID::paletteGlassTanLight,
    FilterPaletteID::paletteGlassDullPurpleLight,
    FilterPaletteID::paletteGlassDullGreenDark,
    FilterPaletteID::paletteGlassDullGreenLight,
    FilterPaletteID::paletteGlassSaturatedPurpleDark,
    FilterPaletteID::paletteGlassSaturatedPurpleLight,
    FilterPaletteID::paletteGlassOrangeLight,
    FilterPaletteID::paletteGlassAquaDark,
    FilterPaletteID::paletteGlassMagentaLight,
    FilterPaletteID::paletteGlassDullBrownDark,
    FilterPaletteID::paletteGlassDullBrownLight,
    FilterPaletteID::paletteGlassInvisible,
    FilterPaletteID::paletteGlassVoid,
};

// Previously 0x97FCBC use it to get the correct palette from g1_elements
// clang-format off
static constexpr uint16_t kPaletteToG1Offset[kPaletteTotalOffsets] = {
    // Main remap palettes
    SPR_PALETTE_BLACK,
    SPR_PALETTE_GREY,
    SPR_PALETTE_WHITE,
    SPR_PALETTE_DARK_PURPLE,
    SPR_PALETTE_LIGHT_PURPLE,
    SPR_PALETTE_BRIGHT_PURPLE,
    SPR_PALETTE_DARK_BLUE,
    SPR_PALETTE_LIGHT_BLUE,
    SPR_PALETTE_ICY_BLUE,
    SPR_PALETTE_TEAL,
    SPR_PALETTE_AQUAMARINE,
    SPR_PALETTE_SATURATED_GREEN,
    SPR_PALETTE_DARK_GREEN,
    SPR_PALETTE_MOSS_GREEN,
    SPR_PALETTE_BRIGHT_GREEN,
    SPR_PALETTE_OLIVE_GREEN,
    SPR_PALETTE_DARK_OLIVE_GREEN,
    SPR_PALETTE_BRIGHT_YELLOW,
    SPR_PALETTE_YELLOW,
    SPR_PALETTE_DARK_YELLOW,
    SPR_PALETTE_LIGHT_ORANGE,
    SPR_PALETTE_DARK_ORANGE,
    SPR_PALETTE_LIGHT_BROWN,
    SPR_PALETTE_SATURATED_BROWN,
    SPR_PALETTE_DARK_BROWN,
    SPR_PALETTE_SALMON_PINK,
    SPR_PALETTE_BORDEAUX_RED,
    SPR_PALETTE_SATURATED_RED,
    SPR_PALETTE_BRIGHT_RED,
    SPR_PALETTE_DARK_PINK,
    SPR_PALETTE_BRIGHT_PINK,
    SPR_PALETTE_LIGHT_PINK,

    // Extended remap palettes
    SPR_PALETTE_DARK_OLIVE_DARK,
    SPR_PALETTE_DARK_OLIVE_LIGHT,
    SPR_PALETTE_SATURATED_BROWN_LIGHT,
    SPR_PALETTE_BORDEAUX_RED_DARK,
    SPR_PALETTE_BORDEAUX_RED_LIGHT,
    SPR_PALETTE_GRASS_GREEN_DARK,
    SPR_PALETTE_GRASS_GREEN_LIGHT,
    SPR_PALETTE_OLIVE_DARK,
    SPR_PALETTE_OLIVE_LIGHT,
    SPR_PALETTE_SATURATED_GREEN_LIGHT,
    SPR_PALETTE_TAN_DARK,
    SPR_PALETTE_TAN_LIGHT,
    SPR_PALETTE_DULL_PURPLE_LIGHT,
    SPR_PALETTE_DULL_GREEN_DARK,
    SPR_PALETTE_DULL_GREEN_LIGHT,
    SPR_PALETTE_SATURATED_PURPLE_DARK,
    SPR_PALETTE_SATURATED_PURPLE_LIGHT,
    SPR_PALETTE_ORANGE_LIGHT,
    SPR_PALETTE_AQUA_DARK,
    SPR_PALETTE_MAGENTA_LIGHT,
    SPR_PALETTE_DULL_BROWN_DARK,
    SPR_PALETTE_DULL_BROWN_LIGHT,
    SPR_PALETTE_INVISIBLE,
    SPR_PALETTE_VOID,

    // Additional palettes
    SPR_PALETTE_WATER,
    SPR_PALETTE_LAND_MARKER_0,
    SPR_PALETTE_LAND_MARKER_1,
    SPR_PALETTE_LAND_MARKER_2,
    SPR_PALETTE_LAND_MARKER_3,
    SPR_PALETTE_SCENERY_GROUND_MARKER,
    SPR_PALETTE_WATER_MARKER,
    SPR_PALETTE_QUARTER_MARKER_0,
    SPR_PALETTE_QUARTER_MARKER_1,
    SPR_PALETTE_QUARTER_MARKER_2,
    SPR_PALETTE_QUARTER_MARKER_3,
    SPR_PALETTE_RIDE_GROUND_MARKER,
    SPR_PALETTE_GHOST,
    SPR_PALETTE_45,
    SPR_PALETTE_46,
    SPR_PALETTE_DARKEN_3,
    SPR_PALETTE_DECREASED_CONTRAST,
    SPR_PALETTE_DARKEN_1,
    SPR_PALETTE_DARKEN_2,
    SPR_PALETTE_51,

    // Translucent remap palettes
    SPR_PALETTE_TRANSLUCENT_GREY,
    SPR_PALETTE_TRANSLUCENT_GREY_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_GREY_SHADOW,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BLUE,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BLUE_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BLUE_SHADOW,
    SPR_PALETTE_TRANSLUCENT_BORDEAUX_RED,
    SPR_PALETTE_TRANSLUCENT_BORDEAUX_RED_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_BORDEAUX_RED_SHADOW,
    SPR_PALETTE_TRANSLUCENT_DARK_GREEN,
    SPR_PALETTE_TRANSLUCENT_DARK_GREEN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_DARK_GREEN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PURPLE,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PURPLE_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PURPLE_SHADOW,
    SPR_PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN,
    SPR_PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BROWN,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BROWN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BROWN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_YELLOW,
    SPR_PALETTE_TRANSLUCENT_YELLOW_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_YELLOW_SHADOW,
    SPR_PALETTE_TRANSLUCENT_MOSS_GREEN,
    SPR_PALETTE_TRANSLUCENT_MOSS_GREEN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_MOSS_GREEN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_OLIVE_GREEN,
    SPR_PALETTE_TRANSLUCENT_OLIVE_GREEN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_OLIVE_GREEN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_GREEN,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_GREEN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_GREEN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_SALMON_PINK,
    SPR_PALETTE_TRANSLUCENT_SALMON_PINK_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_SALMON_PINK_SHADOW,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PURPLE,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PURPLE_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PURPLE_SHADOW,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_RED,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_RED_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_RED_SHADOW,
    SPR_PALETTE_TRANSLUCENT_LIGHT_ORANGE,
    SPR_PALETTE_TRANSLUCENT_LIGHT_ORANGE_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_LIGHT_ORANGE_SHADOW,
    SPR_PALETTE_TRANSLUCENT_TEAL,
    SPR_PALETTE_TRANSLUCENT_TEAL_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_TEAL_SHADOW,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PINK,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PINK_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PINK_SHADOW,
    SPR_PALETTE_TRANSLUCENT_DARK_BROWN,
    SPR_PALETTE_TRANSLUCENT_DARK_BROWN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_DARK_BROWN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PINK,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PINK_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PINK_SHADOW,
    SPR_PALETTE_TRANSLUCENT_WHITE,
    SPR_PALETTE_TRANSLUCENT_WHITE_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_WHITE_SHADOW,

    // Main glass remap palettes
    SPR_PALETTE_GLASS_BLACK,
    SPR_PALETTE_GLASS_GREY,
    SPR_PALETTE_GLASS_WHITE,
    SPR_PALETTE_GLASS_DARK_PURPLE,
    SPR_PALETTE_GLASS_LIGHT_PURPLE,
    SPR_PALETTE_GLASS_BRIGHT_PURPLE,
    SPR_PALETTE_GLASS_DARK_BLUE,
    SPR_PALETTE_GLASS_LIGHT_BLUE,
    SPR_PALETTE_GLASS_ICY_BLUE,
    SPR_PALETTE_GLASS_TEAL,
    SPR_PALETTE_GLASS_AQUAMARINE,
    SPR_PALETTE_GLASS_SATURATED_GREEN,
    SPR_PALETTE_GLASS_DARK_GREEN,
    SPR_PALETTE_GLASS_MOSS_GREEN,
    SPR_PALETTE_GLASS_BRIGHT_GREEN,
    SPR_PALETTE_GLASS_OLIVE_GREEN,
    SPR_PALETTE_GLASS_DARK_OLIVE_GREEN,
    SPR_PALETTE_GLASS_BRIGHT_YELLOW,
    SPR_PALETTE_GLASS_YELLOW,
    SPR_PALETTE_GLASS_DARK_YELLOW,
    SPR_PALETTE_GLASS_LIGHT_ORANGE,
    SPR_PALETTE_GLASS_DARK_ORANGE,
    SPR_PALETTE_GLASS_LIGHT_BROWN,
    SPR_PALETTE_GLASS_SATURATED_BROWN,
    SPR_PALETTE_GLASS_DARK_BROWN,
    SPR_PALETTE_GLASS_SALMON_PINK,
    SPR_PALETTE_GLASS_BORDEAUX_RED,
    SPR_PALETTE_GLASS_SATURATED_RED,
    SPR_PALETTE_GLASS_BRIGHT_RED,
    SPR_PALETTE_GLASS_DARK_PINK,
    SPR_PALETTE_GLASS_BRIGHT_PINK,
    SPR_PALETTE_GLASS_LIGHT_PINK,

    // Extended glass remap palettes
    SPR_PALETTE_GLASS_DARK_OLIVE_DARK,
    SPR_PALETTE_GLASS_DARK_OLIVE_LIGHT,
    SPR_PALETTE_GLASS_SATURATED_BROWN_LIGHT,
    SPR_PALETTE_GLASS_BORDEAUX_RED_DARK,
    SPR_PALETTE_GLASS_BORDEAUX_RED_LIGHT,
    SPR_PALETTE_GLASS_GRASS_GREEN_DARK,
    SPR_PALETTE_GLASS_GRASS_GREEN_LIGHT,
    SPR_PALETTE_GLASS_OLIVE_DARK,
    SPR_PALETTE_GLASS_OLIVE_LIGHT,
    SPR_PALETTE_GLASS_SATURATED_GREEN_LIGHT,
    SPR_PALETTE_GLASS_TAN_DARK,
    SPR_PALETTE_GLASS_TAN_LIGHT,
    SPR_PALETTE_GLASS_DULL_PURPLE_LIGHT,
    SPR_PALETTE_GLASS_DULL_GREEN_DARK,
    SPR_PALETTE_GLASS_DULL_GREEN_LIGHT,
    SPR_PALETTE_GLASS_SATURATED_PURPLE_DARK,
    SPR_PALETTE_GLASS_SATURATED_PURPLE_LIGHT,
    SPR_PALETTE_GLASS_ORANGE_LIGHT,
    SPR_PALETTE_GLASS_AQUA_DARK,
    SPR_PALETTE_GLASS_MAGENTA_LIGHT,
    SPR_PALETTE_GLASS_DULL_BROWN_DARK,
    SPR_PALETTE_GLASS_DULL_BROWN_LIGHT,
    SPR_PALETTE_GLASS_INVISIBLE,
    SPR_PALETTE_GLASS_VOID,
};

static constexpr TranslucentWindowPalette kWindowPaletteGrey = { FilterPaletteID::paletteTranslucentGrey,                  FilterPaletteID::paletteTranslucentGreyHighlight,             FilterPaletteID::paletteTranslucentGreyShadow };
static constexpr TranslucentWindowPalette kWindowPaletteLightPurple = { FilterPaletteID::paletteTranslucentLightPurple,          FilterPaletteID::paletteTranslucentLightPurpleHighlight,     FilterPaletteID::paletteTranslucentLightPurpleShadow };
static constexpr TranslucentWindowPalette kWindowPaletteBrightPurple = { FilterPaletteID::paletteTranslucentBrightPurple,     FilterPaletteID::paletteTranslucentBrightPurpleHighlight,    FilterPaletteID::paletteTranslucentBrightPurpleShadow };
static constexpr TranslucentWindowPalette kWindowPaletteLightBlue = { FilterPaletteID::paletteTranslucentLightBlue,            FilterPaletteID::paletteTranslucentLightBlueHighlight,       FilterPaletteID::paletteTranslucentLightBlueShadow };
static constexpr TranslucentWindowPalette kWindowPaletteTeal = { FilterPaletteID::paletteTranslucentTeal,                  FilterPaletteID::paletteTranslucentTealHighlight,             FilterPaletteID::paletteTranslucentTealShadow };
static constexpr TranslucentWindowPalette kWindowPaletteBrightGreen = { FilterPaletteID::paletteTranslucentBrightGreen,          FilterPaletteID::paletteTranslucentBrightGreenHighlight,     FilterPaletteID::paletteTranslucentBrightGreenShadow };
static constexpr TranslucentWindowPalette kWindowPaletteDarkGreen = { FilterPaletteID::paletteTranslucentDarkGreen,        FilterPaletteID::paletteTranslucentDarkGreenHighlight,       FilterPaletteID::paletteTranslucentDarkGreenShadow };
static constexpr TranslucentWindowPalette kWindowPaletteMossGreen = { FilterPaletteID::paletteTranslucentMossGreen,        FilterPaletteID::paletteTranslucentMossGreenHighlight,       FilterPaletteID::paletteTranslucentMossGreenShadow };
static constexpr TranslucentWindowPalette kWindowPaletteOliveGreen = { FilterPaletteID::paletteTranslucentOliveGreen,       FilterPaletteID::paletteTranslucentOliveGreenHighlight,      FilterPaletteID::paletteTranslucentOliveGreenShadow };
static constexpr TranslucentWindowPalette kWindowPaletteDarkOliveGreen = { FilterPaletteID::paletteTranslucentDarkOliveGreen,  FilterPaletteID::paletteTranslucentDarkOliveGreenHighlight, FilterPaletteID::paletteTranslucentDarkOliveGreenShadow };
static constexpr TranslucentWindowPalette kWindowPaletteYellow = { FilterPaletteID::paletteTranslucentYellow,                FilterPaletteID::paletteTranslucentYellowHighlight,           FilterPaletteID::paletteTranslucentYellowShadow };
static constexpr TranslucentWindowPalette kWindowPaletteLightOrange = { FilterPaletteID::paletteTranslucentLightOrange,          FilterPaletteID::paletteTranslucentLightOrangeHighlight,     FilterPaletteID::paletteTranslucentLightOrangeShadow };
static constexpr TranslucentWindowPalette kWindowPaletteLightBrown = { FilterPaletteID::paletteTranslucentLightBrown,           FilterPaletteID::paletteTranslucentLightBrownHighlight,      FilterPaletteID::paletteTranslucentLightBrownShadow };
static constexpr TranslucentWindowPalette kWindowPaletteDarkBrown = { FilterPaletteID::paletteTranslucentDarkBrown,        FilterPaletteID::paletteTranslucentDarkBrownHighlight,       FilterPaletteID::paletteTranslucentDarkBrownShadow };
static constexpr TranslucentWindowPalette kWindowPaletteSalmonPink = { FilterPaletteID::paletteTranslucentSalmonPink,       FilterPaletteID::paletteTranslucentSalmonPinkHighlight,      FilterPaletteID::paletteTranslucentSalmonPinkShadow };
static constexpr TranslucentWindowPalette kWindowPaletteBordeauxRed = { FilterPaletteID::paletteTranslucentBordeauxRed,      FilterPaletteID::paletteTranslucentBordeauxRedHighlight,     FilterPaletteID::paletteTranslucentBordeauxRedShadow };
static constexpr TranslucentWindowPalette kWindowPaletteBrightRed = { FilterPaletteID::paletteTranslucentBrightRed,            FilterPaletteID::paletteTranslucentBrightRedHighlight,       FilterPaletteID::paletteTranslucentBrightRedShadow };
static constexpr TranslucentWindowPalette kWindowPaletteBrightPink = { FilterPaletteID::paletteTranslucentBrightPink,           FilterPaletteID::paletteTranslucentBrightPinkHighlight,      FilterPaletteID::paletteTranslucentBrightPinkShadow };

const TranslucentWindowPalette kTranslucentWindowPalettes[kColourNumTotal] = {
    kWindowPaletteGrey,                    // Colour::black
    kWindowPaletteGrey,                    // Colour::grey
    { FilterPaletteID::paletteTranslucentWhite, FilterPaletteID::paletteTranslucentWhiteHighlight,FilterPaletteID::paletteTranslucentWhiteShadow },
    kWindowPaletteLightPurple,            // Colour::darkPurple
    kWindowPaletteLightPurple,            // Colour::lightPurple
    kWindowPaletteBrightPurple,           // Colour::brightPurple
    kWindowPaletteLightBlue,              // Colour::darkBlue
    kWindowPaletteLightBlue,              // Colour::lightBlue
    kWindowPaletteLightBlue,              // Colour::icyBlue
    kWindowPaletteTeal,                   // Colour::darkWater
    kWindowPaletteTeal,                   // Colour::lightWater
    kWindowPaletteBrightGreen,            // Colour::saturatedGreen
    kWindowPaletteDarkGreen,              // Colour::darkGreen
    kWindowPaletteMossGreen,              // Colour::mossGreen
    kWindowPaletteBrightGreen,            // Colour::brightGreen
    kWindowPaletteOliveGreen,             // Colour::oliveGreen
    kWindowPaletteDarkOliveGreen,        // Colour::darkOliveGreen
    kWindowPaletteYellow,                  // Colour::brightYellow
    kWindowPaletteYellow,                  // Colour::yellow
    kWindowPaletteYellow,                  // Colour::darkYellow
    kWindowPaletteLightOrange,            // Colour::lightOrange
    kWindowPaletteLightOrange,            // Colour::darkOrange
    kWindowPaletteLightBrown,             // Colour::lightBrown
    kWindowPaletteLightBrown,             // Colour::saturatedBrown
    kWindowPaletteDarkBrown,              // Colour::darkBrown
    kWindowPaletteSalmonPink,             // Colour::salmonPink
    kWindowPaletteBordeauxRed,            // Colour::bordeauxRed
    kWindowPaletteBrightRed,              // Colour::saturatedRed
    kWindowPaletteBrightRed,              // Colour::brightRed
    kWindowPaletteBrightPink,             // Colour::darkPink
    kWindowPaletteBrightPink,             // Colour::brightPink
    { FilterPaletteID::paletteTranslucentLightPink, FilterPaletteID::paletteTranslucentLightPinkHighlight, FilterPaletteID::paletteTranslucentLightPinkShadow },
    kWindowPaletteDarkOliveGreen,        // Colour::armyGreen
    kWindowPaletteDarkOliveGreen,        // Colour::honeyDew
    kWindowPaletteLightBrown,             // Colour::tan
    kWindowPaletteBordeauxRed,            // Colour::maroon
    kWindowPaletteBordeauxRed,            // Colour::coralPink
    kWindowPaletteMossGreen,              // Colour::forestGreen
    kWindowPaletteMossGreen,              // Colour::chartreuse
    kWindowPaletteOliveGreen,             // Colour::hunterGreen
    kWindowPaletteOliveGreen,             // Colour::celadon
    kWindowPaletteBrightGreen,            // Colour::limeGreen
    kWindowPaletteSalmonPink,             // Colour::sepia
    kWindowPaletteSalmonPink,             // Colour::peach
    kWindowPaletteLightPurple,            // Colour::periwinkle
    kWindowPaletteDarkGreen,              // Colour::viridian
    kWindowPaletteDarkGreen,              // Colour::seafoamGreen
    kWindowPaletteBrightPurple,           // Colour::violet
    kWindowPaletteBrightPurple,           // Colour::lavender
    kWindowPaletteLightOrange,            // Colour::pastelOrange
    kWindowPaletteTeal,                   // Colour::deepWater
    kWindowPaletteBrightPink,             // Colour::pastelPink
    kWindowPaletteDarkBrown,              // Colour::umber
    kWindowPaletteDarkBrown,              // Colour::beige
    { FilterPaletteID::paletteDarken1,           FilterPaletteID::paletteDarken1,      FilterPaletteID::paletteDarken1 },
    { FilterPaletteID::paletteDarken2,           FilterPaletteID::paletteDarken2,      FilterPaletteID::paletteDarken2 },
};
// clang-format on

ImageCatalogue ImageId::GetCatalogue() const
{
    auto index = GetIndex();
    if (index == SPR_TEMP)
    {
        return ImageCatalogue::TEMPORARY;
    }
    if (index < SPR_RCTC_G1_END)
    {
        return ImageCatalogue::G1;
    }
    if (index < SPR_G2_END)
    {
        return ImageCatalogue::G2;
    }
    if (index < SPR_CSG_END)
    {
        return ImageCatalogue::CSG;
    }
    if (index < SPR_IMAGE_LIST_END)
    {
        return ImageCatalogue::OBJECT;
    }
    return ImageCatalogue::UNKNOWN;
}

static auto GetMaskFunction()
{
    if (Platform::AVX2Available())
    {
        LOG_VERBOSE("registering AVX2 mask function");
        return MaskAvx2;
    }
    else if (Platform::SSE41Available())
    {
        LOG_VERBOSE("registering SSE4.1 mask function");
        return MaskSse4_1;
    }
    else
    {
        LOG_VERBOSE("registering scalar mask function");
        return MaskScalar;
    }
}

static const auto MaskFunc = GetMaskFunction();

void MaskFn(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    MaskFunc(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
}

void GfxFilterPixel(RenderTarget& rt, const ScreenCoordsXY& coords, FilterPaletteID palette)
{
    Rectangle::filter(rt, { coords, coords }, palette);
}

/**
 *
 *  rct2: 0x00683854
 * a1 (ebx)
 * product (cl)
 */
void GfxTransposePalette(ImageIndex pal, uint8_t product)
{
    const auto* g1 = GfxGetG1Palette(pal);
    if (g1 == nullptr)
        return;

    auto index = g1->startIndex;
    auto* src = g1->palette;

    for (auto numColours = g1->numColours; numColours > 0; numColours--)
    {
        auto& dst = gGamePalette[index];
        // Make sure the image never gets darker than the void colour (not-quite-black), to avoid the background colour
        // jumping between void and 100% black.
        dst.blue = std::max<uint8_t>(35, ((src->blue * product) >> 8));
        dst.green = std::max<uint8_t>(35, ((src->green * product) >> 8));
        dst.red = std::max<uint8_t>(23, ((src->red * product) >> 8));
        src++;

        index++;
    }
    UpdatePalette(gGamePalette, PaletteIndex::pi10, 236);
}

/**
 *
 *  rct2: 0x006837E3
 */
void LoadPalette()
{
    if (gOpenRCT2NoGraphics)
    {
        return;
    }

    uint32_t palette = SPR_GAME_DEFAULT_PALETTE;

    auto water_type = OpenRCT2::ObjectManager::GetObjectEntry<WaterObjectEntry>(0);
    if (water_type != nullptr)
    {
        Guard::Assert(water_type->mainPalette != kImageIndexUndefined, "Failed to load water palette");
        palette = water_type->mainPalette;
    }

    const auto* g1 = GfxGetG1Palette(palette);
    if (g1 != nullptr)
    {
        auto index = g1->startIndex;
        auto* src = g1->palette;
        for (auto numColours = g1->numColours; numColours > 0; numColours--)
        {
            auto& dst = gGamePalette[index];
            dst.blue = src->blue;
            dst.green = src->green;
            dst.red = src->red;
            src++;
            index++;
        }
    }
    UpdatePalette(gGamePalette, PaletteIndex::pi10, 236);
    GfxInvalidateScreen();
}

/**
 *
 *  rct2: 0x006ED7E5
 */
void GfxInvalidateScreen()
{
    GfxSetDirtyBlocks({ { 0, 0 }, { ContextGetWidth(), ContextGetHeight() } });
}

/*
 *
 * rct2: 0x006EE53B
 * left (ax)
 * width (bx)
 * top (cx)
 * height (dx)
 * drawpixelinfo (edi)
 */
bool ClipRenderTarget(RenderTarget& dst, RenderTarget& src, const ScreenCoordsXY& coords, int32_t width, int32_t height)
{
    assert(src.zoom_level == ZoomLevel{ 0 });
    int32_t right = coords.x + width;
    int32_t bottom = coords.y + height;

    dst = src;
    dst.zoom_level = ZoomLevel{ 0 };

    if (coords.x > dst.x)
    {
        uint16_t clippedFromLeft = coords.x - dst.x;
        dst.width -= clippedFromLeft;
        dst.x = coords.x;
        dst.pitch += clippedFromLeft;
        dst.bits += clippedFromLeft;
    }

    int32_t stickOutWidth = dst.x + dst.width - right;
    if (stickOutWidth > 0)
    {
        dst.width -= stickOutWidth;
        dst.pitch += stickOutWidth;
    }

    if (coords.y > dst.y)
    {
        uint16_t clippedFromTop = coords.y - dst.y;
        dst.height -= clippedFromTop;
        dst.y = coords.y;
        uint32_t bitsPlus = dst.LineStride() * clippedFromTop;
        dst.bits += bitsPlus;
    }

    int32_t bp = dst.y + dst.height - bottom;
    if (bp > 0)
    {
        dst.height -= bp;
    }

    if (dst.width > 0 && dst.height > 0)
    {
        dst.x -= coords.x;
        dst.y -= coords.y;
        return true;
    }

    return false;
}

void GfxInvalidatePickedUpPeep()
{
    auto imageId = gPickupPeepImage;
    if (imageId.HasValue())
    {
        auto* g1 = GfxGetG1Element(imageId);
        if (g1 != nullptr)
        {
            int32_t left = gPickupPeepX + g1->xOffset;
            int32_t top = gPickupPeepY + g1->yOffset;
            int32_t right = left + g1->width;
            int32_t bottom = top + g1->height;
            GfxSetDirtyBlocks({ { left, top }, { right, bottom } });
        }
    }
}

void GfxDrawPickedUpPeep(RenderTarget& rt)
{
    if (gPickupPeepImage.HasValue())
    {
        GfxDrawSprite(rt, gPickupPeepImage, { gPickupPeepX, gPickupPeepY });
    }
}

std::optional<uint32_t> GetPaletteG1Index(FilterPaletteID paletteId)
{
    if (EnumValue(paletteId) < kPaletteTotalOffsets)
    {
        return kPaletteToG1Offset[EnumValue(paletteId)];
    }
    return std::nullopt;
}

std::optional<PaletteMap> GetPaletteMapForColour(FilterPaletteID paletteId)
{
    auto g1Index = GetPaletteG1Index(paletteId);
    if (g1Index.has_value())
    {
        auto g1 = GfxGetG1Element(g1Index.value());
        if (g1 != nullptr)
        {
            return PaletteMap(reinterpret_cast<PaletteIndex*>(g1->offset), g1->height, g1->width);
        }
    }
    return std::nullopt;
}

FilterPaletteID GetGlassPaletteId(Colour c)
{
    return kGlassPaletteIds[EnumValue(c)];
}

void UpdatePalette(std::span<const BGRAColour> palette, PaletteIndex startIndex, int32_t numColours)
{
    for (int32_t i = EnumValue(startIndex); i < numColours + EnumValue(startIndex); i++)
    {
        const auto& colour = palette[i];
        uint8_t b = colour.blue;
        uint8_t g = colour.green;
        uint8_t r = colour.red;

        if (LightFx::IsAvailable())
        {
            LightFx::ApplyPaletteFilter(i, &r, &g, &b);
        }
        else
        {
            float night = gDayNightCycle;
            if (night >= 0 && gClimateLightningFlash == 0)
            {
                r = Lerp(r, SoftLight(r, 8), night);
                g = Lerp(g, SoftLight(g, 8), night);
                b = Lerp(b, SoftLight(b, 128), night);
            }
        }

        gPalette[i].blue = b;
        gPalette[i].green = g;
        gPalette[i].red = r;
        gPalette[i].alpha = 0;
    }

    // Fix #1749 and #6535: rainbow path, donut shop and pause button contain black spots that should be white.
    gPalette[255].blue = 255;
    gPalette[255].green = 255;
    gPalette[255].red = 255;
    gPalette[255].alpha = 0;

    if (!gOpenRCT2Headless)
    {
        DrawingEngineSetPalette(gPalette);
    }
}

/**
 *
 *  rct2: 0x006838BD
 */
void UpdatePaletteEffects()
{
    auto water_type = OpenRCT2::ObjectManager::GetObjectEntry<WaterObjectEntry>(0);

    if (gClimateLightningFlash == 1)
    {
        // Change palette to lighter colour during lightning
        int32_t palette = SPR_GAME_DEFAULT_PALETTE;

        if (water_type != nullptr)
        {
            palette = water_type->mainPalette;
        }
        const auto* g1 = GfxGetG1Palette(palette);
        if (g1 != nullptr)
        {
            auto startIndex = g1->startIndex;

            for (int32_t i = 0; i < g1->numColours; i++)
            {
                auto& paletteOffset = gGamePalette[startIndex + i];
                const auto& g1PaletteEntry = g1->palette[i];
                paletteOffset.blue = -((0xFF - g1PaletteEntry.blue) / 2) - 1;
                paletteOffset.green = -((0xFF - g1PaletteEntry.green) / 2) - 1;
                paletteOffset.red = -((0xFF - g1PaletteEntry.red) / 2) - 1;
            }

            UpdatePalette(gGamePalette, kPaletteOffsetDynamic, kPaletteLengthDynamic);
        }
        gClimateLightningFlash++;
    }
    else
    {
        if (gClimateLightningFlash == 2)
        {
            // Change palette back to normal after lightning
            int32_t palette = SPR_GAME_DEFAULT_PALETTE;

            if (water_type != nullptr)
            {
                palette = water_type->mainPalette;
            }

            const auto* g1 = GfxGetG1Palette(palette);
            if (g1 != nullptr)
            {
                auto startIndex = g1->startIndex;

                for (int32_t i = 0; i < g1->numColours; i++)
                {
                    auto& paletteOffset = gGamePalette[startIndex + i];
                    const auto& g1PaletteEntry = g1->palette[i];
                    paletteOffset.blue = g1PaletteEntry.blue;
                    paletteOffset.green = g1PaletteEntry.green;
                    paletteOffset.red = g1PaletteEntry.red;
                }
            }
        }

        // Animate the water/lava/chain movement palette
        uint32_t shade = 0;
        if (Config::Get().general.renderWeatherGloom)
        {
            auto paletteId = ClimateGetWeatherGloomPaletteId(getGameState().weatherCurrent);
            if (paletteId != FilterPaletteID::paletteNull)
            {
                shade = 1;
                if (paletteId != FilterPaletteID::paletteDarken1)
                {
                    shade = 2;
                }
            }
        }
        uint32_t j = gPaletteEffectFrame;
        j = ((static_cast<uint16_t>((~j / 2) * 128) * 15) >> 16);
        uint32_t waterId = SPR_GAME_PALETTE_WATER;
        if (water_type != nullptr)
        {
            waterId = water_type->waterWavesPalette;
        }
        const auto* g1 = GfxGetG1Palette(shade + waterId);
        if (g1 != nullptr)
        {
            const auto* g1PaletteEntry = &g1->palette[j];
            int32_t n = kPaletteLengthWaterWaves;
            for (int32_t i = 0; i < n; i++)
            {
                auto& vd = gGamePalette[EnumValue(PaletteIndex::waterWaves0) + i];
                vd.blue = g1PaletteEntry->blue;
                vd.green = g1PaletteEntry->green;
                vd.red = g1PaletteEntry->red;
                g1PaletteEntry += 3;
                if (g1PaletteEntry >= &g1->palette[3 * n])
                {
                    g1PaletteEntry -= 3 * n;
                }
            }
        }

        waterId = SPR_GAME_PALETTE_3;
        if (water_type != nullptr)
        {
            waterId = water_type->waterSparklesPalette;
        }

        g1 = GfxGetG1Palette(shade + waterId);
        if (g1 != nullptr)
        {
            auto* src = &g1->palette[j];
            int32_t n = kPaletteLengthWaterSparkles;
            for (int32_t i = 0; i < n; i++)
            {
                auto& vd = gGamePalette[EnumValue(PaletteIndex::waterSparkles0) + i];
                vd.blue = src->blue;
                vd.green = src->green;
                vd.red = src->red;
                src += 3;
                if (src >= &g1->palette[3 * n])
                {
                    src -= 3 * n;
                }
            }
        }

        j = (static_cast<uint16_t>(gPaletteEffectFrame * -960) * 3) >> 16;
        waterId = SPR_GAME_PALETTE_4;
        g1 = GfxGetG1Palette(shade + waterId);
        if (g1 != nullptr)
        {
            auto* src = &g1->palette[j];
            const int32_t n = 3;
            for (int32_t i = 0; i < n; i++)
            {
                auto& vd = gGamePalette[EnumValue(PaletteIndex::primaryRemap0) + i];
                vd.blue = src->blue;
                vd.green = src->green;
                vd.red = src->red;
                src++;
                if (src >= &g1->palette[3])
                {
                    src -= n;
                }
            }
        }

        UpdatePalette(gGamePalette, kPaletteOffsetAnimated, kPaletteLengthAnimated);
        if (gClimateLightningFlash == 2)
        {
            UpdatePalette(gGamePalette, kPaletteOffsetDynamic, kPaletteLengthDynamic);
            gClimateLightningFlash = 0;
        }
    }
}

void RefreshVideo()
{
    ContextRecreateWindow();
    DrawingEngineSetPalette(gPalette);
    GfxInvalidateScreen();
}

void ToggleWindowedMode()
{
    int32_t rt = Config::Get().general.fullscreenMode == 0 ? 2 : 0;
    ContextSetFullscreenMode(rt);
    Config::Get().general.fullscreenMode = rt;
    Config::Save();
}

void DebugRT(RenderTarget& rt)
{
    ScreenCoordsXY topLeft = { rt.x, rt.y };
    ScreenCoordsXY bottomRight = { rt.x + rt.width - 1, rt.y + rt.height - 1 };
    ScreenCoordsXY topRight = { rt.x + rt.width - 1, rt.y };
    ScreenCoordsXY bottomLeft = { rt.x, rt.y + rt.height - 1 };

    GfxDrawLine(rt, { topLeft, bottomRight }, PaletteIndex::pi136);
    GfxDrawLine(rt, { bottomLeft, topRight }, PaletteIndex::pi136);
    GfxDrawLine(rt, { topLeft, topRight }, PaletteIndex::pi129);
    GfxDrawLine(rt, { topRight, bottomRight }, PaletteIndex::pi129);
    GfxDrawLine(rt, { bottomLeft, bottomRight }, PaletteIndex::pi129);
    GfxDrawLine(rt, { topLeft, bottomLeft }, PaletteIndex::pi129);

    GfxDrawLine(rt, { topLeft, topLeft + ScreenCoordsXY{ 4, 0 } }, PaletteIndex::pi136);

    const auto str = std::to_string(rt.x);
    DrawText(rt, ScreenCoordsXY{ rt.x, rt.y }, { Colour::white, FontStyle::tiny }, str.c_str());

    const auto str2 = std::to_string(rt.y);
    DrawText(rt, ScreenCoordsXY{ rt.x, rt.y + 6 }, { Colour::white, FontStyle::tiny }, str2.c_str());
}
