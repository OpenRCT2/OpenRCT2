#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../common.h"
#include "format_codes.h"
#include "localisation.h"

#pragma region Format codes

typedef struct format_code_token {
    uint32 code;
    const char *token;
} format_code_token;

static const format_code_token format_code_tokens[] = {
    { FORMAT_MOVE_X,                    "MOVE_X"                },
    { FORMAT_ADJUST_PALETTE,            "ADJUST_PALETTE"        },
    { FORMAT_NEWLINE,                   "NEWLINE"               },
    { FORMAT_NEWLINE_SMALLER,           "NEWLINE_SMALLER"       },
    { FORMAT_TINYFONT,                  "TINYFONT"              },
    { FORMAT_BIGFONT,                   "BIGFONT"               },
    { FORMAT_MEDIUMFONT,                "MEDIUMFONT"            },
    { FORMAT_SMALLFONT,                 "SMALLFONT"             },
    { FORMAT_OUTLINE,                   "OUTLINE"               },
    { FORMAT_OUTLINE_OFF,               "OUTLINE_OFF"           },
    { FORMAT_WINDOW_COLOUR_1,           "WINDOW_COLOUR_1"       },
    { FORMAT_WINDOW_COLOUR_2,           "WINDOW_COLOUR_2"       },
    { FORMAT_WINDOW_COLOUR_3,           "WINDOW_COLOUR_3"       },
    { FORMAT_NEWLINE_X_Y,               "NEWLINE_X_Y"           },
    { FORMAT_INLINE_SPRITE,             "INLINE_SPRITE"         },
    { FORMAT_ENDQUOTES,                 "ENDQUOTES"             },
    { FORMAT_COMMA32,                   "COMMA32"               },
    { FORMAT_INT32,                     "INT32"                 },
    { FORMAT_COMMA2DP32,                "COMMA2DP32"            },
    { FORMAT_COMMA16,                   "COMMA16"               },
    { FORMAT_UINT16,                    "UINT16"                },
    { FORMAT_CURRENCY2DP,               "CURRENCY2DP"           },
    { FORMAT_CURRENCY,                  "CURRENCY"              },
    { FORMAT_STRINGID,                  "STRINGID"              },
    { FORMAT_STRINGID2,                 "STRINGID2"             },
    { FORMAT_STRING,                    "STRING"                },
    { FORMAT_MONTHYEAR,                 "MONTHYEAR"             },
    { FORMAT_MONTH,                     "MONTH"                 },
    { FORMAT_VELOCITY,                  "VELOCITY"              },
    { FORMAT_POP16,                     "POP16"                 },
    { FORMAT_PUSH16,                    "PUSH16"                },
    { FORMAT_DURATION,                  "DURATION"              },
    { FORMAT_REALTIME,                  "REALTIME"              },
    { FORMAT_LENGTH,                    "LENGTH"                },
    { FORMAT_SPRITE,                    "SPRITE"                },
    { FORMAT_BLACK,                     "BLACK"                 },
    { FORMAT_GREY,                      "GREY"                  },
    { FORMAT_WHITE,                     "WHITE"                 },
    { FORMAT_RED,                       "RED"                   },
    { FORMAT_GREEN,                     "GREEN"                 },
    { FORMAT_YELLOW,                    "YELLOW"                },
    { FORMAT_TOPAZ,                     "TOPAZ"                 },
    { FORMAT_CELADON,                   "CELADON"               },
    { FORMAT_BABYBLUE,                  "BABYBLUE"              },
    { FORMAT_PALELAVENDER,              "PALELAVENDER"          },
    { FORMAT_PALEGOLD,                  "PALEGOLD"              },
    { FORMAT_LIGHTPINK,                 "LIGHTPINK"             },
    { FORMAT_PEARLAQUA,                 "PEARLAQUA"             },
    { FORMAT_PALESILVER,                "PALESILVER"            },
    { FORMAT_AMINUSCULE,                "AMINUSCULE"            },
    { FORMAT_UP,                        "UP"                    },
    { FORMAT_POUND,                     "POUND"                 },
    { FORMAT_YEN,                       "YEN"                   },
    { FORMAT_COPYRIGHT,                 "COPYRIGHT"             },
    { FORMAT_DOWN,                      "DOWN"                  },
    { FORMAT_LEFTGUILLEMET,             "LEFTGUILLEMET"         },
    { FORMAT_TICK,                      "TICK"                  },
    { FORMAT_CROSS,                     "CROSS"                 },
    { FORMAT_RIGHT,                     "RIGHT"                 },
    { FORMAT_DEGREE,                    "DEGREE"                },
    { FORMAT_SQUARED,                   "SQUARED"               },
    { FORMAT_OPENQUOTES,                "OPENQUOTES"            },
    { FORMAT_EURO,                      "EURO"                  },
    { FORMAT_APPROX,                    "APPROX"                },
    { FORMAT_POWERNEGATIVEONE,          "POWERNEGATIVEONE"      },
    { FORMAT_BULLET,                    "BULLET"                },
    { FORMAT_RIGHTGUILLEMET,            "RIGHTGUILLEMET"        },
    { FORMAT_SMALLUP,                   "SMALLUP"               },
    { FORMAT_SMALLDOWN,                 "SMALLDOWN"             },
    { FORMAT_LEFT,                      "LEFT"                  },
    { FORMAT_INVERTEDQUESTION,          "INVERTEDQUESTION"      },
    { FORMAT_COMMA1DP16,                "COMMA1DP16"            }
};

uint32 format_get_code(const char *token)
{
    sint32 i;
    for (i = 0; i < countof(format_code_tokens); i++)
        if (_strcmpi(token, format_code_tokens[i].token) == 0)
            return format_code_tokens[i].code;
    return 0;
}

const char *format_get_token(uint32 code)
{
    sint32 i;
    for (i = 0; i < countof(format_code_tokens); i++)
        if (code == format_code_tokens[i].code)
            return format_code_tokens[i].token;
    return 0;
}

bool utf8_should_use_sprite_for_codepoint(sint32 codepoint)
{
    switch (codepoint) {
    case FORMAT_UP:
    case FORMAT_DOWN:
    case FORMAT_LEFTGUILLEMET:
    case FORMAT_TICK:
    case FORMAT_CROSS:
    case FORMAT_RIGHT:
    case FORMAT_RIGHTGUILLEMET:
    case FORMAT_SMALLUP:
    case FORMAT_SMALLDOWN:
    case FORMAT_LEFT:
    case FORMAT_OPENQUOTES:
    case FORMAT_ENDQUOTES:
        return true;
    default:
        return false;
    }
}

#pragma endregion
