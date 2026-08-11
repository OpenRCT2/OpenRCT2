/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
    // clang-format off
    static const EnumMap<FormatToken> FormatTokenMap = {
        { "MOVE_X",               FormatToken::move,                },
        { "NEWLINE",              FormatToken::newline,             },
        { "NEWLINE_SMALLER",      FormatToken::newlineSmall,        },
        { "TINYFONT",             FormatToken::fontTiny,            },
        { "MEDIUMFONT",           FormatToken::fontMedium,          },
        { "SMALLFONT",            FormatToken::fontSmall,           },
        { "OUTLINE",              FormatToken::outlineEnable,       },
        { "OUTLINE_OFF",          FormatToken::outlineDisable,      },
        { "WINDOW_COLOUR_1",      FormatToken::colourWindow1,       },
        { "WINDOW_COLOUR_2",      FormatToken::colourWindow2,       },
        { "WINDOW_COLOUR_3",      FormatToken::colourWindow3,       },
        { "INLINE_SPRITE",        FormatToken::inlineSprite,        },
        { "COMMA32",              FormatToken::comma32,             },
        { "INT32",                FormatToken::int32,               },
        { "COMMA1DP16",           FormatToken::comma1dp16,          },
        { "COMMA2DP32",           FormatToken::comma2dp32,          },
        { "COMMA16",              FormatToken::comma16,             },
        { "UINT16",               FormatToken::uint16,              },
        { "CURRENCY2DP",          FormatToken::currency2dp,         },
        { "CURRENCY",             FormatToken::currency,            },
        { "STRINGID",             FormatToken::stringById,          },
        { "STRING",               FormatToken::string,              },
        { "MONTHYEAR",            FormatToken::monthYear,           },
        { "MONTHYEAR_SENTENCE",   FormatToken::monthYearSentence,   },
        { "MONTH",                FormatToken::month,               },
        { "VELOCITY",             FormatToken::velocity,            },
        { "POP16",                FormatToken::pop16,               },
        { "PUSH16",               FormatToken::push16,              },
        { "DURATION",             FormatToken::durationShort,       },
        { "REALTIME",             FormatToken::durationLong,        },
        { "LENGTH",               FormatToken::length,              },
        { "HEIGHT",               FormatToken::height,              },
        { "SPRITE",               FormatToken::sprite,              },
        { "BLACK",                FormatToken::colourBlack,         },
        { "GREY",                 FormatToken::colourGrey,          },
        { "WHITE",                FormatToken::colourWhite,         },
        { "RED",                  FormatToken::colourRed,           },
        { "GREEN",                FormatToken::colourGreen,         },
        { "YELLOW",               FormatToken::colourYellow,        },
        { "TOPAZ",                FormatToken::colourTopaz,         },
        { "CELADON",              FormatToken::colourCeladon,       },
        { "BABYBLUE",             FormatToken::colourBabyBlue,      },
        { "PALELAVENDER",         FormatToken::colourPaleLavender,  },
        { "PALEGOLD",             FormatToken::colourPaleGold,      },
        { "LIGHTPINK",            FormatToken::colourLightPink,     },
        { "PEARLAQUA",            FormatToken::colourPearlAqua,     },
        { "PALESILVER",           FormatToken::colourPaleSilver,    },
    };
    // clang-format on

    FormatToken FormatTokenFromString(std::string_view token)
    {
        auto result = FormatTokenMap.find(token);
        return result != std::end(FormatTokenMap) ? result->second : FormatToken::unknown;
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
            case FormatToken::comma32:
            case FormatToken::int32:
            case FormatToken::comma1dp16:
            case FormatToken::comma2dp32:
            case FormatToken::comma16:
            case FormatToken::uint16:
            case FormatToken::currency2dp:
            case FormatToken::currency:
            case FormatToken::stringById:
            case FormatToken::string:
            case FormatToken::monthYear:
            case FormatToken::monthYearSentence:
            case FormatToken::month:
            case FormatToken::velocity:
            case FormatToken::durationShort:
            case FormatToken::durationLong:
            case FormatToken::length:
            case FormatToken::height:
            case FormatToken::sprite:
                return true;
            default:
                return false;
        }
    }

    bool FormatTokenIsColour(FormatToken token)
    {
        switch (token)
        {
            case FormatToken::colourBlack:
            case FormatToken::colourGrey:
            case FormatToken::colourWhite:
            case FormatToken::colourRed:
            case FormatToken::colourGreen:
            case FormatToken::colourYellow:
            case FormatToken::colourTopaz:
            case FormatToken::colourCeladon:
            case FormatToken::colourBabyBlue:
            case FormatToken::colourPaleLavender:
            case FormatToken::colourPaleGold:
            case FormatToken::colourLightPink:
            case FormatToken::colourPearlAqua:
            case FormatToken::colourPaleSilver:
                return true;
            default:
                return false;
        }
    }

    Drawing::TextColour FormatTokenToTextColour(FormatToken token)
    {
        uint8_t value = EnumValue(token) - EnumValue(FormatToken::colourBlack);
        if (value >= Drawing::kNumTextColours)
            return Drawing::TextColour::black;

        return static_cast<Drawing::TextColour>(value);
    }

    FormatToken FormatTokenFromTextColour(Drawing::TextColour textColour)
    {
        static constexpr FormatToken tokens[] = {
            FormatToken::colourBlack,        // TextColour::black
            FormatToken::colourGrey,         // TextColour::grey
            FormatToken::colourWhite,        // TextColour::white
            FormatToken::colourRed,          // TextColour::red
            FormatToken::colourGreen,        // TextColour::green
            FormatToken::colourYellow,       // TextColour::yellow
            FormatToken::colourTopaz,        // TextColour::topaz
            FormatToken::colourCeladon,      // TextColour::celadon
            FormatToken::colourBabyBlue,     // TextColour::babyBlue
            FormatToken::colourPaleLavender, // TextColour::paleLavender
            FormatToken::colourPaleGold,     // TextColour::paleGold
            FormatToken::colourLightPink,    // TextColour::lightPink
            FormatToken::colourPearlAqua,    // TextColour::pearlAqua
            FormatToken::colourPaleSilver,   // TextColour::paleSilver
        };

        return tokens[EnumValue(textColour)];
    }
} // namespace OpenRCT2
