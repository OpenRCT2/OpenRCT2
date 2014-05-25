/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef _PARK_H_
#define _PARK_H_

#include "rct2.h"

#define DECRYPT_MONEY(money) rol32((money) ^ 0xF4EC9621, 13)
#define ENCRYPT_MONEY(money) (ror32((money), 13) ^ 0xF4EC9621)

typedef struct {
	uint16 time;
	uint16 type;
} rct_award;

enum {
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
};

enum {
	PARK_FLAGS_PARK_OPEN = (1 << 0),
	PARK_FLAGS_FORBID_LANDSCAPE_CHANGES = (1 << 2),
	PARK_FLAGS_FORBID_TREE_REMOVAL = (1 << 3),
	PARK_FLAGS_SHOW_REAL_GUEST_NAMES = (1 << 4),
	PARK_FLAGS_FORBID_HIGH_CONSTRUCTION = (1 << 5), // below tree height
	PARK_FLAGS_PREF_LESS_INTENSE_RIDES = (1 << 6),
	PARK_FLAGS_FORBID_MARKETING_CAMPAIGN = (1 << 7),
	PARK_FLAGS_PREF_MORE_INTENSE_RIDES = (1 << 8),
	PARK_FLAGS_11 = (1 << 11),
	PARK_FLAGS_DIFFICULT_GUEST_GENERATION = (1 << 12),
	PARK_FLAGS_PARK_FREE_ENTRY = (1 << 13),
	PARK_FLAGS_DIFFICULT_PARK_RATING = (1 << 14),
	PARK_FLAGS_NO_MONEY = (1 << 17),
	PARK_FLAGS_18 = (1 << 18)
};

int park_is_open();
void park_init();
void park_reset_awards_and_history();
int park_calculate_size();

int calculate_park_rating();
money32 calculate_park_value();
money32 calculate_company_value();
void reset_park_entrances();

void park_update();

#endif
