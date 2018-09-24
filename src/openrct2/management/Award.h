/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

struct Award
{
    uint16_t Time;
    uint16_t Type;
};

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

extern Award gCurrentAwards[MAX_AWARDS];

bool award_is_positive(int32_t type);
void award_reset();
void award_update_all();
