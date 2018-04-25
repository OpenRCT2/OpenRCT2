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

#ifndef _FORMAT_CODES_H_
#define _FORMAT_CODES_H_

#include "../common.h"

uint32 format_get_code(const char *token);
const char *format_get_token(uint32 code);

enum {
    // Font format codes

    // The next byte specifies the X coordinate
    FORMAT_MOVE_X = 1,
    // The next byte specifies the palette
    FORMAT_ADJUST_PALETTE,

    FORMAT_3,
    FORMAT_4,

    // Moves to the next line
    FORMAT_NEWLINE = 5,
    // Moves less than NEWLINE
    FORMAT_NEWLINE_SMALLER,

    FORMAT_TINYFONT,
    FORMAT_BIGFONT,
    FORMAT_MEDIUMFONT,
    FORMAT_SMALLFONT,

    FORMAT_OUTLINE,
    FORMAT_OUTLINE_OFF,

    // Changes the colour of the text to a predefined window colour.
    FORMAT_WINDOW_COLOUR_1,
    FORMAT_WINDOW_COLOUR_2,
    FORMAT_WINDOW_COLOUR_3,

    FORMAT_16,

    // The next 2 bytes specify the X and Y coordinates
    FORMAT_NEWLINE_X_Y = 17,

    // The next 4 bytes specify the sprite
    FORMAT_INLINE_SPRITE = 23,

    // Argument format codes
    FORMAT_ARGUMENT_CODE_START = 123, // 'z' == 122 or 0x7A
    FORMAT_COMMA32 = 123,
    FORMAT_INT32,
    FORMAT_COMMA2DP32,
    FORMAT_COMMA16,
    FORMAT_UINT16,
    FORMAT_CURRENCY2DP,
    FORMAT_CURRENCY,
    FORMAT_STRINGID,
    FORMAT_STRINGID2,
    FORMAT_STRING,
    FORMAT_MONTHYEAR,
    FORMAT_MONTH,
    FORMAT_VELOCITY,
    FORMAT_POP16,
    FORMAT_PUSH16,
    FORMAT_DURATION,
    FORMAT_REALTIME,
    FORMAT_LENGTH,
    FORMAT_SPRITE,
    FORMAT_ARGUMENT_CODE_END = FORMAT_SPRITE,

    // Colour format codes
    FORMAT_COLOUR_CODE_START = 142,
    FORMAT_BLACK = 142,
    FORMAT_GREY,
    FORMAT_WHITE,
    FORMAT_RED,
    FORMAT_GREEN,
    FORMAT_YELLOW,
    FORMAT_TOPAZ,
    FORMAT_CELADON,
    FORMAT_BABYBLUE,
    FORMAT_PALELAVENDER,
    FORMAT_PALEGOLD,
    FORMAT_LIGHTPINK,
    FORMAT_PEARLAQUA,
    FORMAT_PALESILVER,
    FORMAT_COLOUR_CODE_END = FORMAT_PALESILVER,

    // Extra non-ASCII characters
    FORMAT_INVERTEDEXCLAMATION = 161,
    FORMAT_POUND = 163,
    FORMAT_YEN = 165,
    FORMAT_COPYRIGHT = 169,
    FORMAT_LEFTGUILLEMET = 171,
    FORMAT_DEGREE = 176,
    FORMAT_SQUARED = 178,
    FORMAT_RIGHTGUILLEMET = 187,
    FORMAT_INVERTEDQUESTION = 191,

    FORMAT_OPENQUOTES = 8220,
    FORMAT_ENDQUOTES = 8221,

    FORMAT_BULLET = 8226,
    FORMAT_POWERNEGATIVEONE = 8315,
    FORMAT_EURO = 8364,

    FORMAT_APPROX = 8776,

    FORMAT_UP = 9650,
    FORMAT_RIGHT = 9654,
    FORMAT_DOWN = 9660,
    FORMAT_LEFT = 9664,

    FORMAT_SMALLUP = 9652,
    FORMAT_SMALLDOWN = 9662,

    FORMAT_TICK = 10003,
    FORMAT_CROSS = 10005,

    FORMAT_SYMBOL_RAILWAY = 128740,
    FORMAT_SYMBOL_ROAD = 128739,
    FORMAT_SYMBOL_FLAG = 128681,

    // Format codes that need suitable Unicode allocations
    FORMAT_COMMA1DP16 = 20004
};

enum RCT2Polish
{
    RCT2_A_OGONEK_UC = 159, // 0x9F
    RCT2_C_ACUTE_UC = 162, // 0xA2
    RCT2_E_OGONEK_UC = 166, // 0xA6
    RCT2_N_ACUTE_UC = 198, // 0xC6
    RCT2_L_STROKE_UC = 167, // 0xA7
    RCT2_S_ACUTE_UC = 208, // 0xD0
    RCT2_Z_DOT_UC = 216, // 0xD8
    RCT2_Z_ACUTE_UC = 215, // 0xD7

    RCT2_A_OGONEK = 221, // 0xDD
    RCT2_C_ACUTE = 222, // 0xDE
    RCT2_E_OGONEK = 230, // 0xE6
    RCT2_N_ACUTE = 240, // 0xF0
    RCT2_L_STROKE = 247, // 0xF7
    RCT2_S_ACUTE = 248, // 0xF8
    RCT2_Z_DOT = 253, // 0xFD
    RCT2_Z_ACUTE = 254, // 0xFE
};

enum UnicodePolish
{
    UNICODE_A_OGONEK_UC = 260,
    UNICODE_C_ACUTE_UC = 262,
    UNICODE_E_OGONEK_UC = 280,
    UNICODE_N_ACUTE_UC = 323,
    UNICODE_L_STROKE_UC = 321,
    UNICODE_S_ACUTE_UC = 346,
    UNICODE_Z_DOT_UC = 379,
    UNICODE_Z_ACUTE_UC = 377,

    UNICODE_A_OGONEK = 261,
    UNICODE_C_ACUTE = 263,
    UNICODE_E_OGONEK = 281,
    UNICODE_N_ACUTE = 324,
    UNICODE_L_STROKE = 322,
    UNICODE_S_ACUTE = 347,
    UNICODE_Z_DOT = 380,
    UNICODE_Z_ACUTE = 378,
};

#endif
