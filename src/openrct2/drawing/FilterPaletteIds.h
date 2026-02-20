/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "Colour.h"

namespace OpenRCT2::Drawing
{
    enum class FilterPaletteID : int32_t
    {
        paletteNull = 0,

        // Additional palettes
        paletteWater = kColourNumTotal,
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
        paletteGlassBlack = paletteGlass + EnumValue(Colour::black),
        paletteGlassGrey = paletteGlass + EnumValue(Colour::grey),
        paletteGlassWhite = paletteGlass + EnumValue(Colour::white),
        paletteGlassDarkPurple = paletteGlass + EnumValue(Colour::darkPurple),
        paletteGlassLightPurple = paletteGlass + EnumValue(Colour::lightPurple),
        paletteGlassBrightPurple = paletteGlass + EnumValue(Colour::brightPurple),
        paletteGlassDarkBlue = paletteGlass + EnumValue(Colour::darkBlue),
        paletteGlassLightBlue = paletteGlass + EnumValue(Colour::lightBlue),
        paletteGlassIcyBlue = paletteGlass + EnumValue(Colour::icyBlue),
        paletteGlassTeal = paletteGlass + EnumValue(Colour::darkWater),
        paletteGlassAquamarine = paletteGlass + EnumValue(Colour::lightWater),
        paletteGlassSaturatedGreen = paletteGlass + EnumValue(Colour::saturatedGreen),
        paletteGlassDarkGreen = paletteGlass + EnumValue(Colour::darkGreen),
        paletteGlassMossGreen = paletteGlass + EnumValue(Colour::mossGreen),
        paletteGlassBrightGreen = paletteGlass + EnumValue(Colour::brightGreen),
        paletteGlassOliveGreen = paletteGlass + EnumValue(Colour::oliveGreen),
        paletteGlassDarkOliveGreen = paletteGlass + EnumValue(Colour::darkOliveGreen),
        paletteGlassBrightYellow = paletteGlass + EnumValue(Colour::brightYellow),
        paletteGlassYellow = paletteGlass + EnumValue(Colour::yellow),
        paletteGlassDarkYellow = paletteGlass + EnumValue(Colour::darkYellow),
        paletteGlassLightOrange = paletteGlass + EnumValue(Colour::lightOrange),
        paletteGlassDarkOrange = paletteGlass + EnumValue(Colour::darkOrange),
        paletteGlassLightBrown = paletteGlass + EnumValue(Colour::lightBrown),
        paletteGlassSaturatedBrown = paletteGlass + EnumValue(Colour::saturatedBrown),
        paletteGlassDarkBrown = paletteGlass + EnumValue(Colour::darkBrown),
        paletteGlassSalmonPink = paletteGlass + EnumValue(Colour::salmonPink),
        paletteGlassBordeauxRed = paletteGlass + EnumValue(Colour::bordeauxRed),
        paletteGlassSaturatedRed = paletteGlass + EnumValue(Colour::saturatedRed),
        paletteGlassBrightRed = paletteGlass + EnumValue(Colour::brightRed),
        paletteGlassDarkPink = paletteGlass + EnumValue(Colour::darkPink),
        paletteGlassBrightPink = paletteGlass + EnumValue(Colour::brightPink),
        paletteGlassLightPink = paletteGlass + EnumValue(Colour::lightPink),
        paletteGlassDarkOliveDark = paletteGlass + EnumValue(Colour::armyGreen),
        paletteGlassDarkOliveLight = paletteGlass + EnumValue(Colour::honeyDew),
        paletteGlassSaturatedBrownLight = paletteGlass + EnumValue(Colour::tan),
        paletteGlassBordeauxRedDark = paletteGlass + EnumValue(Colour::maroon),
        paletteGlassBordeauxRedLight = paletteGlass + EnumValue(Colour::coralPink),
        paletteGlassGrassGreenDark = paletteGlass + EnumValue(Colour::forestGreen),
        paletteGlassGrassGreenLight = paletteGlass + EnumValue(Colour::chartreuse),

        // Extended glass remap palettes
        paletteGlassOliveDark = paletteGlass + EnumValue(Colour::hunterGreen),
        paletteGlassOliveLight = paletteGlass + EnumValue(Colour::celadon),
        paletteGlassSaturatedGreenLight = paletteGlass + EnumValue(Colour::limeGreen),
        paletteGlassTanDark = paletteGlass + EnumValue(Colour::sepia),
        paletteGlassTanLight = paletteGlass + EnumValue(Colour::peach),
        paletteGlassDullPurpleLight = paletteGlass + EnumValue(Colour::periwinkle),
        paletteGlassDullGreenDark = paletteGlass + EnumValue(Colour::viridian),
        paletteGlassDullGreenLight = paletteGlass + EnumValue(Colour::seafoamGreen),
        paletteGlassSaturatedPurpleDark = paletteGlass + EnumValue(Colour::violet),
        paletteGlassSaturatedPurpleLight = paletteGlass + EnumValue(Colour::lavender),
        paletteGlassOrangeLight = paletteGlass + EnumValue(Colour::pastelOrange),
        paletteGlassAquaDark = paletteGlass + EnumValue(Colour::deepWater),
        paletteGlassMagentaLight = paletteGlass + EnumValue(Colour::pastelPink),
        paletteGlassDullBrownDark = paletteGlass + EnumValue(Colour::umber),
        paletteGlassDullBrownLight = paletteGlass + EnumValue(Colour::beige),
        paletteGlassInvisible = paletteGlass + EnumValue(Colour::invisible),
        paletteGlassVoid = paletteGlass + EnumValue(Colour::voidBackground),
    };
}
