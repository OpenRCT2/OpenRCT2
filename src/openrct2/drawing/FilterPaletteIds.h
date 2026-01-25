/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/Colour.h"

namespace OpenRCT2::Drawing
{
    enum class FilterPaletteID : int32_t
    {
        paletteNull = 0,

        // Additional palettes
        paletteWater = COLOUR_COUNT,
        paletteLandMarker0, // North (corner/edge)
        paletteLandMarker1, // East (corner/edge)
        paletteLandMarker2, // South (corner/edge)
        paletteLandMarker3, // West (corner/edge)
        paletteSceneryGroundMarker,
        paletteWaterMarker,
        paletteQuarterMarker0, // North (not sure why it couldn't just use PaletteLandMarker0)
        paletteQuarterMarker1, // East
        paletteQuarterMarker2, // South
        paletteQuarterMarker3, // West
        paletteRideGroundMarker,
        paletteGhost, // Construction marker
        palette45,    // Decolourise + lighten
        palette46,
        paletteDarken3,
        paletteDarken1 = paletteDarken3 + 2,
        paletteDarken2,
        palette51, // Decolourise + darken

        // Translucent remap palettes
        paletteTranslucentGrey,
        paletteTranslucentGreyHighlight,
        paletteTranslucentGreyShadow,
        paletteTranslucentLightBlue,
        paletteTranslucentLightBlueHighlight,
        paletteTranslucentLightBlueShadow,
        paletteTranslucentBordeauxRed,
        paletteTranslucentBordeauxRedHighlight,
        paletteTranslucentBordeauxRedShadow,
        paletteTranslucentDarkGreen,
        paletteTranslucentDarkGreenHighlight,
        paletteTranslucentDarkGreenShadow,
        paletteTranslucentLightPurple,
        paletteTranslucentLightPurpleHighlight,
        paletteTranslucentLightPurpleShadow,
        paletteTranslucentDarkOliveGreen,
        paletteTranslucentDarkOliveGreenHighlight,
        paletteTranslucentDarkOliveGreenShadow,
        paletteTranslucentLightBrown,
        paletteTranslucentLightBrownHighlight,
        paletteTranslucentLightBrownShadow,
        paletteTranslucentYellow,
        paletteTranslucentYellowHighlight,
        paletteTranslucentYellowShadow,
        paletteTranslucentMossGreen,
        paletteTranslucentMossGreenHighlight,
        paletteTranslucentMossGreenShadow,
        paletteTranslucentOliveGreen,
        paletteTranslucentOliveGreenHighlight,
        paletteTranslucentOliveGreenShadow,
        paletteTranslucentBrightGreen,
        paletteTranslucentBrightGreenHighlight,
        paletteTranslucentBrightGreenShadow,
        paletteTranslucentSalmonPink,
        paletteTranslucentSalmonPinkHighlight,
        paletteTranslucentSalmonPinkShadow,
        paletteTranslucentBrightPurple,
        paletteTranslucentBrightPurpleHighlight,
        paletteTranslucentBrightPurpleShadow,
        paletteTranslucentBrightRed,
        paletteTranslucentBrightRedHighlight,
        paletteTranslucentBrightRedShadow,
        paletteTranslucentLightOrange,
        paletteTranslucentLightOrangeHighlight,
        paletteTranslucentLightOrangeShadow,
        paletteTranslucentTeal,
        paletteTranslucentTealHighlight,
        paletteTranslucentTealShadow,
        paletteTranslucentBrightPink,
        paletteTranslucentBrightPinkHighlight,
        paletteTranslucentBrightPinkShadow,
        paletteTranslucentDarkBrown,
        paletteTranslucentDarkBrownHighlight,
        paletteTranslucentDarkBrownShadow,
        paletteTranslucentLightPink,
        paletteTranslucentLightPinkHighlight,
        paletteTranslucentLightPinkShadow,
        paletteTranslucentWhite,
        paletteTranslucentWhiteHighlight,
        paletteTranslucentWhiteShadow,

        // Main glass remap palettes
        paletteGlass,
        paletteGlassBlack = paletteGlass + COLOUR_BLACK,
        paletteGlassGrey = paletteGlass + COLOUR_GREY,
        paletteGlassWhite = paletteGlass + COLOUR_WHITE,
        paletteGlassDarkPurple = paletteGlass + COLOUR_DARK_PURPLE,
        paletteGlassLightPurple = paletteGlass + COLOUR_LIGHT_PURPLE,
        paletteGlassBrightPurple = paletteGlass + COLOUR_BRIGHT_PURPLE,
        paletteGlassDarkBlue = paletteGlass + COLOUR_DARK_BLUE,
        paletteGlassLightBlue = paletteGlass + COLOUR_LIGHT_BLUE,
        paletteGlassIcyBlue = paletteGlass + COLOUR_ICY_BLUE,
        paletteGlassTeal = paletteGlass + COLOUR_DARK_WATER,
        paletteGlassAquamarine = paletteGlass + COLOUR_LIGHT_WATER,
        paletteGlassSaturatedGreen = paletteGlass + COLOUR_SATURATED_GREEN,
        paletteGlassDarkGreen = paletteGlass + COLOUR_DARK_GREEN,
        paletteGlassMossGreen = paletteGlass + COLOUR_MOSS_GREEN,
        paletteGlassBrightGreen = paletteGlass + COLOUR_BRIGHT_GREEN,
        paletteGlassOliveGreen = paletteGlass + COLOUR_OLIVE_GREEN,
        paletteGlassDarkOliveGreen = paletteGlass + COLOUR_DARK_OLIVE_GREEN,
        paletteGlassBrightYellow = paletteGlass + COLOUR_BRIGHT_YELLOW,
        paletteGlassYellow = paletteGlass + COLOUR_YELLOW,
        paletteGlassDarkYellow = paletteGlass + COLOUR_DARK_YELLOW,
        paletteGlassLightOrange = paletteGlass + COLOUR_LIGHT_ORANGE,
        paletteGlassDarkOrange = paletteGlass + COLOUR_DARK_ORANGE,
        paletteGlassLightBrown = paletteGlass + COLOUR_LIGHT_BROWN,
        paletteGlassSaturatedBrown = paletteGlass + COLOUR_SATURATED_BROWN,
        paletteGlassDarkBrown = paletteGlass + COLOUR_DARK_BROWN,
        paletteGlassSalmonPink = paletteGlass + COLOUR_SALMON_PINK,
        paletteGlassBordeauxRed = paletteGlass + COLOUR_BORDEAUX_RED,
        paletteGlassSaturatedRed = paletteGlass + COLOUR_SATURATED_RED,
        paletteGlassBrightRed = paletteGlass + COLOUR_BRIGHT_RED,
        paletteGlassDarkPink = paletteGlass + COLOUR_DARK_PINK,
        paletteGlassBrightPink = paletteGlass + COLOUR_BRIGHT_PINK,
        paletteGlassLightPink = paletteGlass + COLOUR_LIGHT_PINK,
        paletteGlassDarkOliveDark = paletteGlass + COLOUR_DARK_OLIVE_DARK,
        paletteGlassDarkOliveLight = paletteGlass + COLOUR_DARK_OLIVE_LIGHT,
        paletteGlassSaturatedBrownLight = paletteGlass + COLOUR_SATURATED_BROWN_LIGHT,
        paletteGlassBordeauxRedDark = paletteGlass + COLOUR_BORDEAUX_RED_DARK,
        paletteGlassBordeauxRedLight = paletteGlass + COLOUR_BORDEAUX_RED_LIGHT,
        paletteGlassGrassGreenDark = paletteGlass + COLOUR_GRASS_GREEN_DARK,
        paletteGlassGrassGreenLight = paletteGlass + COLOUR_GRASS_GREEN_LIGHT,

        // Extended glass remap palettes
        paletteGlassOliveDark = paletteGlass + COLOUR_OLIVE_DARK,
        paletteGlassOliveLight = paletteGlass + COLOUR_OLIVE_LIGHT,
        paletteGlassSaturatedGreenLight = paletteGlass + COLOUR_SATURATED_GREEN_LIGHT,
        paletteGlassTanDark = paletteGlass + COLOUR_TAN_DARK,
        paletteGlassTanLight = paletteGlass + COLOUR_TAN_LIGHT,
        paletteGlassDullPurpleLight = paletteGlass + COLOUR_DULL_PURPLE_LIGHT,
        paletteGlassDullGreenDark = paletteGlass + COLOUR_DULL_GREEN_DARK,
        paletteGlassDullGreenLight = paletteGlass + COLOUR_DULL_GREEN_LIGHT,
        paletteGlassSaturatedPurpleDark = paletteGlass + COLOUR_SATURATED_PURPLE_DARK,
        paletteGlassSaturatedPurpleLight = paletteGlass + COLOUR_SATURATED_PURPLE_LIGHT,
        paletteGlassOrangeLight = paletteGlass + COLOUR_ORANGE_LIGHT,
        paletteGlassAquaDark = paletteGlass + COLOUR_DEEP_WATER,
        paletteGlassMagentaLight = paletteGlass + COLOUR_MAGENTA_LIGHT,
        paletteGlassDullBrownDark = paletteGlass + COLOUR_DULL_BROWN_DARK,
        paletteGlassDullBrownLight = paletteGlass + COLOUR_DULL_BROWN_LIGHT,
        paletteGlassInvisible = paletteGlass + COLOUR_INVISIBLE,
        paletteGlassVoid = paletteGlass + COLOUR_VOID,
    };
}
