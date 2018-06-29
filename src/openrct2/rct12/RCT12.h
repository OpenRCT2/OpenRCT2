/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
    uint16_t time;
    uint16_t type;
};
assert_struct_size(rct12_award, 4);

/**
 * A single news item / message.
 * size: 0x10C
 */
struct rct12_news_item
{
    uint8_t   Type;
    uint8_t   Flags;
    uint32_t  Assoc;
    uint16_t  Ticks;
    uint16_t  MonthYear;
    uint8_t   Day;
    uint8_t   pad_0B;
    char    Text[256];
};
assert_struct_size(rct12_news_item, 0x10C);

struct rct12_xyzd8
{
    uint8_t x, y, z, direction;
};
assert_struct_size(rct12_xyzd8, 4);

struct rct12_peep_spawn
{
    uint16_t x;
    uint16_t y;
    uint8_t z;
    uint8_t direction;
};
assert_struct_size(rct12_peep_spawn, 6);

#pragma pack(pop)
