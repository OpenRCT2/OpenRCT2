/*****************************************************************************
 * Copyright (c) 2014 Dániel Tar
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

#ifndef _STAFF_H_
#define _STAFF_H_

#include "../common.h"
#include "peep.h"

#define STAFF_MAX_COUNT 0xC8
#define STAFF_TYPE_COUNT 0x04

enum STAFF_MODE {
	STAFF_MODE_NONE,
	STAFF_MODE_WALK,
	STAFF_MODE_PATROL = 3
};

enum STAFF_TYPE {
	STAFF_TYPE_HANDYMAN,
	STAFF_TYPE_MECHANIC,
	STAFF_TYPE_SECURITY,
	STAFF_TYPE_ENTERTAINER
};

enum STAFF_ORDERS{
	STAFF_ORDERS_SWEEPING = (1 << 0),
	STAFF_ORDERS_WATER_FLOWERS = (1 << 1),
	STAFF_ORDERS_EMPTY_BINS = (1 << 2),
	STAFF_ORDERS_MOWING = (1 << 3),
	STAFF_ORDERS_INSPECT_RIDES = (1 << 0),
	STAFF_ORDERS_FIX_RIDES = (1 << 1)
};

void game_command_update_staff_colour(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_hire_new_staff_member(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_set_staff_order(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);

void update_staff_colour(uint8 staffType, uint16 color);
uint16 hire_new_staff_member(uint8 staffType);
void sub_6C0C3F();
int mechanic_is_location_in_patrol(rct_peep *mechanic, int x, int y);
void staff_reset_stats();

#endif