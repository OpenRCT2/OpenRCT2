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

#pragma once

// Structures shared between both RCT1 and RCT2.

#include "common.h"

#define RCT12_MAX_AWARDS            4
#define RCT12_MAX_NEWS_ITEMS        61
#define RCT12_MAX_STATIONS_PER_RIDE 4
#define RCT12_MAX_PEEP_SPAWNS       2
#define RCT12_MAX_PARK_ENTRANCES    4

#pragma pack(push, 1)

typedef struct rct12_award
{
    uint16 time;
    uint16 type;
} rct12_award;
assert_struct_size(rct12_award, 4);

/**
 * A single news item / message.
 * size: 0x10C
 */
typedef struct rct12_news_item
{
    uint8   Type;
    uint8   Flags;
    uint32  Assoc;
    uint16  Ticks;
    uint16  MonthYear;
    uint8   Day;
    uint8   pad_0B;
    char    Text[256];
} rct12_news_item;
assert_struct_size(rct12_news_item, 0x10C);

#pragma pack(pop)
