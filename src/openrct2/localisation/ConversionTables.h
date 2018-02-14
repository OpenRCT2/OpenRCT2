#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

#pragma once

#include "../common.h"

struct encoding_convert_entry
{
    uint16 code;
    uint32 unicode;
};

extern const encoding_convert_entry GB2312ToUnicodeTable[7445];
extern const encoding_convert_entry Big5ToUnicodeTable[13710];
extern const encoding_convert_entry RCT2ToUnicodeTable[256];
extern const encoding_convert_entry CP932ToUnicodeTable[7916];
extern const encoding_convert_entry CP949ToUnicodeTable[17176];

enum RCT2Polish
{
    RCT2_A_OGONEK_UC = 159, // 0x9F
    RCT2_C_ACUTE_UC =  162, // 0xA2
    RCT2_E_OGONEK_UC = 166, // 0xA6
    RCT2_N_ACUTE_UC =  198, // 0xC6
    RCT2_L_STROKE_UC = 167, // 0xA7
    RCT2_S_ACUTE_UC =  208, // 0xD0
    RCT2_Z_DOT_UC =    216, // 0xD8
    RCT2_Z_ACUTE_UC =  215, // 0xD7

    RCT2_A_OGONEK =    221, // 0xDD
    RCT2_C_ACUTE =     222, // 0xDE
    RCT2_E_OGONEK =    230, // 0xE6
    RCT2_N_ACUTE =     240, // 0xF0
    RCT2_L_STROKE =    247, // 0xF7
    RCT2_S_ACUTE =     248, // 0xF8
    RCT2_Z_DOT =       253, // 0xFD
    RCT2_Z_ACUTE =     254, // 0xFE
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
