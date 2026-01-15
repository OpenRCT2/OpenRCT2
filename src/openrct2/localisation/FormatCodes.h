/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../drawing/TextColour.h"

#include <cstdint>
#include <string>
#include <string_view>

namespace OpenRCT2
{
    enum class FormatToken : uint8_t
    {
        unknown,
        literal,
        escaped,

        newline,
        newlineSmall,

        // With parameters
        move,
        inlineSprite,

        // With arguments
        comma32,
        int32,
        comma1dp16,
        comma2dp32,
        comma16,
        uint16,
        currency2dp,
        currency,
        stringById,
        string,
        monthYear,
        monthYearSentence,
        month,
        velocity,
        durationShort,
        durationLong,
        length,
        height,
        sprite,
        pop16,
        push16,

        // Colours
        colourWindow1,
        colourWindow2,
        colourWindow3,
        colourBlack,
        colourGrey,
        colourWhite,
        colourRed,
        colourGreen,
        colourYellow,
        colourTopaz,
        colourCeladon,
        colourBabyBlue,
        colourPaleLavender,
        colourPaleGold,
        colourLightPink,
        colourPearlAqua,
        colourPaleSilver,

        // Fonts
        fontTiny,
        fontSmall,
        fontMedium,

        outlineEnable,
        outlineDisable,
    };

    FormatToken FormatTokenFromString(std::string_view token);
    std::string FormatTokenToString(FormatToken token);
    std::string FormatTokenToStringWithBraces(FormatToken token);
    bool FormatTokenTakesArgument(FormatToken token);
    bool FormatTokenIsColour(FormatToken token);
    Drawing::TextColour FormatTokenToTextColour(FormatToken token);
    FormatToken FormatTokenFromTextColour(Drawing::TextColour textColour);
} // namespace OpenRCT2
