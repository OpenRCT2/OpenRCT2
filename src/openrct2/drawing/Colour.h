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
#include "../core/StringTypes.h"

#include <cstdint>

/**
 * Colour IDs as used by the colour dropdown, NOT palette indices.
 */
namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t
    {
        // Original Colours
        black,
        grey,
        white,
        darkPurple,
        lightPurple,
        brightPurple,
        darkBlue,
        lightBlue,
        icyBlue,
        darkWater,
        lightWater,
        saturatedGreen,
        darkGreen,
        mossGreen,
        brightGreen,
        oliveGreen,
        darkOliveGreen,
        brightYellow,
        yellow,
        darkYellow,
        lightOrange,
        darkOrange,
        lightBrown,
        saturatedBrown,
        darkBrown,
        salmonPink,
        bordeauxRed,
        saturatedRed,
        brightRed,
        darkPink,
        brightPink,
        lightPink,

        // Extended Colour Set
        armyGreen,
        honeyDew,
        tan,
        maroon,
        coralPink,
        forestGreen,
        chartreuse,
        hunterGreen,
        celadon,
        limeGreen,
        sepia,
        peach,
        periwinkle,
        viridian,
        seafoamGreen,
        violet,
        lavender,
        pastelOrange,
        deepWater,
        pastelPink,
        umber,
        beige,
        invisible,
        voidBackground,
    };

    constexpr uint8_t kColourNumNormal = 54;
    constexpr uint8_t kColourNumTotal = 56;

    constexpr auto kColourNull = static_cast<Colour>(255);

    constexpr bool colourIsValid(Colour colour)
    {
        return EnumValue(colour) < kColourNumTotal;
    }

    Colour colourFromString(u8string s, Colour defaultValue = Colour::black);
    u8string colourToString(Colour colour);
} // namespace OpenRCT2::Drawing
