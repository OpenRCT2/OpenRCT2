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
        gold,
        amber,
        amethyst,
        asparagus,
        brown,
        burntPink,
        cactus,
        caramel,
        copper,
        crimson,
        darkerWater,
        emerald,
        indigo,
        pesto,
        pineGreen,
        ruby,
        sapphire,
        silver,
    };

    constexpr uint8_t kColourNumNormal = 72;
    constexpr uint8_t kColourNumTotal = 74;

    constexpr auto kColourNull = static_cast<Colour>(255);

    constexpr bool colourIsValid(Colour colour)
    {
        return EnumValue(colour) < kColourNumTotal;
    }

    Colour colourFromString(u8string s, Colour defaultValue = Colour::black);
    u8string colourToString(Colour colour);

    /**
     * Get a random colour. NOT network safe!
     */
    Colour getRandomColour();
    /**
     * Get a random colour in a way that works the same across all clients.
     */
    Colour getRandomColourNetworkSafe();
    /**
     * Get a colour based on the current time (in ticks), which will cycle through all available colours.
     */
    Colour getCycleColour(uint32_t ticks);
} // namespace OpenRCT2::Drawing
