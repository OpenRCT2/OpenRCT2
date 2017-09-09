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

#ifndef _RCT2_H_
#define _RCT2_H_

#include "common.h"

#define RCT2_MAX_RIDES_IN_PARK              255
#define RCT2_MAX_STAFF                      200
#define RCT2_MAX_BANNERS_IN_PARK            250
#define RCT2_MAX_VEHICLES_PER_RIDE          32
#define RCT2_MAX_CARS_PER_TRAIN             32
#define RCT2_MAX_CATEGORIES_PER_RIDE        2
#define RCT2_MAX_RIDE_TYPES_PER_RIDE_ENTRY  3
#define RCT2_MAX_VEHICLES_PER_RIDE_ENTRY    4
#define RCT2_DOWNTIME_HISTORY_SIZE          8
#define RCT2_CUSTOMER_HISTORY_SIZE          10

typedef struct rct2_install_info {
    uint32 installLevel;
    char title[260];
    char path[260];
    uint32 var_20C;
    uint8 pad_210[256];
    char expansionPackNames[16][128];
    uint32 activeExpansionPacks;        //0xB10
} rct2_install_info;

#pragma pack(push, 1)

#ifdef __cplusplus

/**
 * scores.dat file header.
 * size: 0x10
 */
struct rct_scores_header
{
    uint32 var_0;
    uint32 var_4;
    uint32 var_8;
    uint32 ScenarioCount;
};
assert_struct_size(rct_scores_header, 0x10);

/**
 * An entry of scores.dat
 * size: 0x02B0
 */
struct rct_scores_entry
{
    char    Path[256];
    uint8   Category;
    uint8   pad_0101[0x1F];
    sint8   ObjectiveType;
    sint8   ObjectiveArg1;
    sint32  objectiveArg2;
    sint16  objectiveArg3;
    char    Name[64];
    char    Details[256];
    sint32  Flags;
    money32 CompanyValue;
    char    CompletedBy[64];
};
assert_struct_size(rct_scores_entry, 0x02B0);

#endif // __cplusplus

#pragma pack(pop)

#endif
