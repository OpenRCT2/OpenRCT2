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

#ifndef _AWARD_H_
#define _AWARD_H_

#include "../common.h"

typedef struct Award
{
    uint16 Time;
    uint16 Type;
} Award;

enum PARK_AWARD
{
    PARK_AWARD_MOST_UNTIDY,
    PARK_AWARD_MOST_TIDY,
    PARK_AWARD_BEST_ROLLERCOASTERS,
    PARK_AWARD_BEST_VALUE,
    PARK_AWARD_MOST_BEAUTIFUL,
    PARK_AWARD_WORST_VALUE,
    PARK_AWARD_SAFEST,
    PARK_AWARD_BEST_STAFF,
    PARK_AWARD_BEST_FOOD,
    PARK_AWARD_WORST_FOOD,
    PARK_AWARD_BEST_RESTROOMS,
    PARK_AWARD_MOST_DISAPPOINTING,
    PARK_AWARD_BEST_WATER_RIDES,
    PARK_AWARD_BEST_CUSTOM_DESIGNED_RIDES,
    PARK_AWARD_MOST_DAZZLING_RIDE_COLOURS,
    PARK_AWARD_MOST_CONFUSING_LAYOUT,
    PARK_AWARD_BEST_GENTLE_RIDES,
    PARK_AWARD_COUNT
};

#define MAX_AWARDS 4

#ifdef __cplusplus
extern "C"
{
#endif
    extern Award gCurrentAwards[MAX_AWARDS];

    bool award_is_positive(sint32 type);
    void award_reset();
    void award_update_all();
#ifdef __cplusplus
}
#endif

#endif
