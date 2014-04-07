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

#ifndef _RIDE_H_
#define _RIDE_H_

#include "rct2.h"

/**
 * Ride structure.
 * size: 0x0260
 */
typedef struct {
	uint8 type;						// 0x000
	uint8 pad_001[0xEF];
	sint16 excitement;				// 0x0F0
	sint16 intensity;				// 0x0F2
	sint16 nausea;					// 0x0F4
	uint8 pad_0F6[0x2E];
	sint16 var_124;
	sint16 var_126;
	sint16 var_128;
	sint16 var_12A;
	sint16 var_12C;
	sint16 var_12E;
	uint16 age;						// 0x130
	sint16 running_cost;			// 0x132
	sint16 var_134;
	sint16 var_136;
	uint8 pad_138[0x08];
	sint16 var_140;
	sint16 var_142;
	uint16 pad_144;
	uint16 reliability;				// 0x146
	uint8 pad_148[0x51];
	uint8 var_199;
	uint8 pad_19A[0xC6];
} rct_ride;

enum {
	RIDE_TYPE_NULL = (uint8)-1
};

#endif