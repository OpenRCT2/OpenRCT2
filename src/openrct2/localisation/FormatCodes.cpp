/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FormatCodes.h"

#include "../core/EnumMap.hpp"
#include "../core/EnumUtils.hpp"
#include "../drawing/TextColour.h"

using namespace OpenRCT2;

// clang-format off
static const EnumMap<FormatToken> FormatTokenMap = {
    { "MOVE_X",               FormatToken::Move,                },
    { "NEWLINE",              FormatToken::Newline,             },
    { "NEWLINE_SMALLER",      FormatToken::NewlineSmall,        },
    { "TINYFONT",             FormatToken::FontTiny,            },
    { "MEDIUMFONT",           FormatToken::FontMedium,          },
    { "SMALLFONT",            FormatToken::FontSmall,           },
    { "OUTLINE",              FormatToken::OutlineEnable,       },
    { "OUTLINE_OFF",          FormatToken::OutlineDisable,      },
    { "WINDOW_COLOUR_1",      FormatToken::ColourWindow1,       },
    { "WINDOW_COLOUR_2",      FormatToken::ColourWindow2,       },
    { "WINDOW_COLOUR_3",      FormatToken::ColourWindow3,       },
    { "INLINE_SPRITE",        FormatToken::InlineSprite,        },
    { "COMMA32",              FormatToken::Comma32,             },
    { "INT32",                FormatToken::Int32,               },
    { "COMMA1DP16",           FormatToken::Comma1dp16,          },
    { "COMMA2DP32",           FormatToken::Comma2dp32,          },
    { "COMMA16",              FormatToken::Comma16,             },
    { "UINT16",               FormatToken::UInt16,              },
    { "CURRENCY2DP",          FormatToken::Currency2dp,         },
    { "CURRENCY",             FormatToken::Currency,            },
    { "STRINGID",             FormatToken::StringById,          },
    { "STRING",               FormatToken::String,              },
    { "MONTHYEAR",            FormatToken::MonthYear,           },
    { "MONTHYEAR_SENTENCE",   FormatToken::MonthYearSentence,   },
    { "MONTH",                FormatToken::Month,               },
    { "VELOCITY",             FormatToken::Velocity,            },
    { "POP16",                FormatToken::Pop16,               },
    { "PUSH16",               FormatToken::Push16,              },
    { "DURATION",             FormatToken::DurationShort,       },
    { "REALTIME",             FormatToken::DurationLong,        },
    { "LENGTH",               FormatToken::Length,              },
    { "HEIGHT",               FormatToken::Height,              },
    { "HEIGHT2DP",            FormatToken::Height2dp,           },
    { "SPRITE",               FormatToken::Sprite,              },
    { "BLACK",                FormatToken::ColourBlack,         },
    { "GREY",                 FormatToken::ColourGrey,          },
    { "WHITE",                FormatToken::ColourWhite,         },
    { "RED",                  FormatToken::ColourRed,           },
    { "GREEN",                FormatToken::ColourGreen,         },
    { "YELLOW",               FormatToken::ColourYellow,        },
    { "TOPAZ",                FormatToken::ColourTopaz,         },
    { "CELADON",              FormatToken::ColourCeladon,       },
    { "BABYBLUE",             FormatToken::ColourBabyBlue,      },
    { "PALELAVENDER",         FormatToken::ColourPaleLavender,  },
    { "PALEGOLD",             FormatToken::ColourPaleGold,      },
    { "LIGHTPINK",            FormatToken::ColourLightPink,     },
    { "PEARLAQUA",            FormatToken::ColourPearlAqua,     },
    { "PALESILVER",           FormatToken::ColourPaleSilver,    },
};
// clang-format on

FormatToken FormatTokenFromString(std::string_view token)
{
    auto result = FormatTokenMap.find(token);
    return result != std::end(FormatTokenMap) ? result->second : FormatToken::Unknown;
}

std::string FormatTokenToString(FormatToken token)
{
    auto it = FormatTokenMap.find(token);
    if (it != FormatTokenMap.end())
        return std::string(it->first);

    return {};
}

std::string FormatTokenToStringWithBraces(FormatToken token)
{
    return "{" + FormatTokenToString(token) + "}";
}

bool FormatTokenTakesArgument(FormatToken token)
{
    switch (token)
    {
        case FormatToken::Comma32:
        case FormatToken::Int32:
        case FormatToken::Comma1dp16:
        case FormatToken::Comma2dp32:
        case FormatToken::Comma16:
        case FormatToken::UInt16:
        case FormatToken::Currency2dp:
        case FormatToken::Currency:
        case FormatToken::StringById:
        case FormatToken::String:
        case FormatToken::MonthYear:
        case FormatToken::MonthYearSentence:
        case FormatToken::Month:
        case FormatToken::Velocity:
        case FormatToken::DurationShort:
        case FormatToken::DurationLong:
        case FormatToken::Length:
        case FormatToken::Height:
        case FormatToken::Height2dp:
        case FormatToken::Sprite:
            return true;
        default:
            return false;
    }
}

bool FormatTokenIsColour(FormatToken token)
{
    switch (token)
    {
        case FormatToken::ColourBlack:
        case FormatToken::ColourGrey:
        case FormatToken::ColourWhite:
        case FormatToken::ColourRed:
        case FormatToken::ColourGreen:
        case FormatToken::ColourYellow:
        case FormatToken::ColourTopaz:
        case FormatToken::ColourCeladon:
        case FormatToken::ColourBabyBlue:
        case FormatToken::ColourPaleLavender:
        case FormatToken::ColourPaleGold:
        case FormatToken::ColourLightPink:
        case FormatToken::ColourPearlAqua:
        case FormatToken::ColourPaleSilver:
            return true;
        default:
            return false;
    }
}

TextColour FormatTokenToTextColour(FormatToken token)
{
    uint8_t value = EnumValue(token) - EnumValue(FormatToken::ColourBlack);
    if (value >= kNumTextColours)
        return TextColour::black;

    return static_cast<TextColour>(value);
}

FormatToken FormatTokenFromTextColour(TextColour textColour)
{
    static constexpr FormatToken tokens[] = {
        FormatToken::ColourBlack,        // TextColour::black
        FormatToken::ColourGrey,         // TextColour::grey
        FormatToken::ColourWhite,        // TextColour::white
        FormatToken::ColourRed,          // TextColour::red
        FormatToken::ColourGreen,        // TextColour::green
        FormatToken::ColourYellow,       // TextColour::yellow
        FormatToken::ColourTopaz,        // TextColour::topaz
        FormatToken::ColourCeladon,      // TextColour::celadon
        FormatToken::ColourBabyBlue,     // TextColour::babyBlue
        FormatToken::ColourPaleLavender, // TextColour::paleLavender
        FormatToken::ColourPaleGold,     // TextColour::paleGold
        FormatToken::ColourLightPink,    // TextColour::lightPink
        FormatToken::ColourPearlAqua,    // TextColour::pearlAqua
        FormatToken::ColourPaleSilver,   // TextColour::paleSilver
    };

    return tokens[EnumValue(textColour)];
}
