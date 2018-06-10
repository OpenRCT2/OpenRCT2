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

#include "../common.h"

#define RCT12_MAX_RIDES_IN_PARK             255
#define RCT12_MAX_AWARDS                    4
#define RCT12_MAX_NEWS_ITEMS                61
#define RCT12_MAX_STATIONS_PER_RIDE         4
#define RCT12_MAX_PEEP_SPAWNS               2
#define RCT12_MAX_PARK_ENTRANCES            4
// The number of elements in the patrol_areas array per staff member. Every bit in the array represents a 4x4 square.
// In RCT1, that's an 8-bit array. 8 * 128 = 1024 bits, which is also the number of 4x4 squares on a 128x128 map.
// For RCT2, it's a 32-bit array. 32 * 128 = 4096 bits, which is also the number of 4x4 squares on a 256x256 map.
#define RCT12_PATROL_AREA_SIZE              128
#define RCT12_STAFF_TYPE_COUNT              4
#define RCT12_NUM_COLOUR_SCHEMES            4
#define RCT12_MAX_VEHICLES_PER_RIDE         32
#define RCT12_MAX_VEHICLE_COLOURS           32
#define RCT12_SOUND_ID_NULL                 0xFF

#define RCT12_EXPENDITURE_TABLE_MONTH_COUNT 16
#define RCT12_EXPENDITURE_TYPE_COUNT        14
#define RCT12_FINANCE_GRAPH_SIZE            128

#define RCT12_MAX_USER_STRINGS              1024
#define RCT12_USER_STRING_MAX_LENGTH        32


#pragma pack(push, 1)

struct rct12_award
{
    uint16 time;
    uint16 type;
};
assert_struct_size(rct12_award, 4);

/**
 * A single news item / message.
 * size: 0x10C
 */
struct rct12_news_item
{
    uint8   Type;
    uint8   Flags;
    uint32  Assoc;
    uint16  Ticks;
    uint16  MonthYear;
    uint8   Day;
    uint8   pad_0B;
    char    Text[256];
};
assert_struct_size(rct12_news_item, 0x10C);

struct rct12_xyzd8
{
    uint8 x, y, z, direction;
};
assert_struct_size(rct12_xyzd8, 4);

struct rct12_peep_spawn
{
    uint16 x;
    uint16 y;
    uint8 z;
    uint8 direction;
};
assert_struct_size(rct12_peep_spawn, 6);

#pragma pack(pop)
