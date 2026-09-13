/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Colour.h"

#include "../core/EnumMap.hpp"

namespace OpenRCT2::Drawing
{
    static const EnumMap<Colour> kLookupTable{
        { "black", Colour::black },
        { "grey", Colour::grey },
        { "white", Colour::white },
        { "dark_purple", Colour::darkPurple },
        { "light_purple", Colour::lightPurple },
        { "bright_purple", Colour::brightPurple },
        { "dark_blue", Colour::darkBlue },
        { "light_blue", Colour::lightBlue },
        { "icy_blue", Colour::icyBlue },
        { "teal", Colour::darkWater },
        { "aquamarine", Colour::lightWater },
        { "saturated_green", Colour::saturatedGreen },
        { "dark_green", Colour::darkGreen },
        { "moss_green", Colour::mossGreen },
        { "bright_green", Colour::brightGreen },
        { "olive_green", Colour::oliveGreen },
        { "dark_olive_green", Colour::darkOliveGreen },
        { "bright_yellow", Colour::brightYellow },
        { "yellow", Colour::yellow },
        { "dark_yellow", Colour::darkYellow },
        { "light_orange", Colour::lightOrange },
        { "dark_orange", Colour::darkOrange },
        { "light_brown", Colour::lightBrown },
        { "saturated_brown", Colour::saturatedBrown },
        { "dark_brown", Colour::darkBrown },
        { "salmon_pink", Colour::salmonPink },
        { "bordeaux_red", Colour::bordeauxRed },
        { "saturated_red", Colour::saturatedRed },
        { "bright_red", Colour::brightRed },
        { "dark_pink", Colour::darkPink },
        { "bright_pink", Colour::brightPink },
        { "light_pink", Colour::lightPink },
        { "dark_olive_dark", Colour::armyGreen },
        { "dark_olive_light", Colour::honeyDew },
        { "saturated_brown_light", Colour::tan },
        { "bordeaux_red_dark", Colour::maroon },
        { "bordeaux_red_light", Colour::coralPink },
        { "grass_green_dark", Colour::forestGreen },
        { "grass_green_light", Colour::chartreuse },
        { "olive_dark", Colour::hunterGreen },
        { "olive_light", Colour::celadon },
        { "saturated_green_light", Colour::limeGreen },
        { "tan_dark", Colour::sepia },
        { "tan_light", Colour::peach },
        { "dull_purple_light", Colour::periwinkle },
        { "dull_green_dark", Colour::viridian },
        { "dull_green_light", Colour::seafoamGreen },
        { "saturated_purple_dark", Colour::violet },
        { "saturated_purple_light", Colour::lavender },
        { "orange_light", Colour::pastelOrange },
        { "aqua_dark", Colour::deepWater },
        { "magenta_light", Colour::pastelPink },
        { "dull_brown_dark", Colour::umber },
        { "dull_brown_light", Colour::beige },
        { "invisible", Colour::invisible },
        { "void", Colour::voidBackground },
    };

    Colour colourFromString(u8string s, Colour defaultValue)
    {
        auto result = kLookupTable.find(s);
        return (result != kLookupTable.end()) ? result->second : defaultValue;
    }

    u8string colourToString(Colour colour)
    {
        auto result = kLookupTable.find(colour);
        if (result != kLookupTable.end())
            return u8string(result->first);

        return "black";
    }
} // namespace OpenRCT2::Drawing
