/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FormatCodes.h"

#include "../core/EnumMap.hpp"

#include <mutex>
#include <string>
#include <vector>

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
    { "MONTH",                FormatToken::Month,               },
    { "VELOCITY",             FormatToken::Velocity,            },
    { "POP16",                FormatToken::Pop16,               },
    { "PUSH16",               FormatToken::Push16,              },
    { "DURATION",             FormatToken::DurationShort,       },
    { "REALTIME",             FormatToken::DurationLong,        },
    { "LENGTH",               FormatToken::Length,              },
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

std::string_view GetFormatTokenStringWithBraces(FormatToken token)
{
    // Ensure cache is thread safe
    static std::mutex mutex;
    std::lock_guard<std::mutex> guard(mutex);

    static std::vector<std::string> cache;
    auto index = static_cast<size_t>(token);
    if (cache.size() <= index)
    {
        cache.resize(index + 1);
    }
    if (cache[index].empty())
    {
        cache[index] = "{" + std::string(FormatTokenToString(token)) + "}";
    }
    return cache[index];
}

FormatToken FormatTokenFromString(std::string_view token)
{
    auto result = FormatTokenMap.find(token);
    return result != std::end(FormatTokenMap) ? result->second : FormatToken::Unknown;
}

std::string_view FormatTokenToString(FormatToken token, bool withBraces)
{
    if (withBraces)
    {
        return GetFormatTokenStringWithBraces(token);
    }

    auto it = FormatTokenMap.find(token);
    if (it != FormatTokenMap.end())
        return it->first;

    return {};
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
        case FormatToken::Month:
        case FormatToken::Velocity:
        case FormatToken::DurationShort:
        case FormatToken::DurationLong:
        case FormatToken::Length:
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

size_t FormatTokenGetTextColourIndex(FormatToken token)
{
    switch (token)
    {
        case FormatToken::ColourBlack:
            return 0;
        case FormatToken::ColourGrey:
            return 1;
        case FormatToken::ColourWhite:
            return 2;
        case FormatToken::ColourRed:
            return 3;
        case FormatToken::ColourGreen:
            return 4;
        case FormatToken::ColourYellow:
            return 5;
        case FormatToken::ColourTopaz:
            return 6;
        case FormatToken::ColourCeladon:
            return 7;
        case FormatToken::ColourBabyBlue:
            return 8;
        case FormatToken::ColourPaleLavender:
            return 9;
        case FormatToken::ColourPaleGold:
            return 10;
        case FormatToken::ColourLightPink:
            return 11;
        case FormatToken::ColourPearlAqua:
            return 12;
        case FormatToken::ColourPaleSilver:
            return 13;
        default:
            return 0;
    }
}

FormatToken FormatTokenFromTextColour(size_t textColour)
{
    static constexpr const FormatToken tokens[] = {
        FormatToken::ColourBlack,        FormatToken::ColourGrey,       FormatToken::ColourWhite,
        FormatToken::ColourRed,          FormatToken::ColourGreen,      FormatToken::ColourYellow,
        FormatToken::ColourTopaz,        FormatToken::ColourCeladon,    FormatToken::ColourBabyBlue,
        FormatToken::ColourPaleLavender, FormatToken::ColourPaleGold,   FormatToken::ColourLightPink,
        FormatToken::ColourPearlAqua,    FormatToken::ColourPaleSilver,
    };
    if (textColour > std::size(tokens))
        return FormatToken::ColourBlack;
    return tokens[textColour];
}
