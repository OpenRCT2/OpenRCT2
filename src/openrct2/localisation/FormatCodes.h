/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

enum class FormatToken : uint8_t
{
    Unknown,
    Literal,
    Escaped,

    Newline,
    NewlineSmall,

    // With parameters
    Move,
    InlineSprite,

    // With arguments
    Comma32,
    Int32,
    Comma1dp16,
    Comma2dp32,
    Comma16,
    UInt16,
    Currency2dp,
    Currency,
    StringById,
    String,
    MonthYear,
    MonthYearSentence,
    Month,
    Velocity,
    DurationShort,
    DurationLong,
    Length,
    Height,
    Height2dp,
    Sprite,
    Pop16,
    Push16,

    // Colours
    ColourWindow1,
    ColourWindow2,
    ColourWindow3,
    ColourBlack,
    ColourGrey,
    ColourWhite,
    ColourRed,
    ColourGreen,
    ColourYellow,
    ColourTopaz,
    ColourCeladon,
    ColourBabyBlue,
    ColourPaleLavender,
    ColourPaleGold,
    ColourLightPink,
    ColourPearlAqua,
    ColourPaleSilver,

    // Fonts
    FontTiny,
    FontSmall,
    FontMedium,

    OutlineEnable,
    OutlineDisable,
};

FormatToken FormatTokenFromString(std::string_view token);
std::string FormatTokenToString(FormatToken token);
std::string FormatTokenToStringWithBraces(FormatToken token);
bool FormatTokenTakesArgument(FormatToken token);
bool FormatTokenIsColour(FormatToken token);
OpenRCT2::TextColour FormatTokenToTextColour(FormatToken token);
FormatToken FormatTokenFromTextColour(OpenRCT2::TextColour textColour);
