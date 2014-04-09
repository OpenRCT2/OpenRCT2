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

#ifndef _MAP_H_
#define _MAP_H_

#include "rct2.h"

/**
 * Map element structure
 * size: 0x08
 */
typedef struct {
	sint8 var_0;
	sint8 var_1;
	sint8 var_2;
	sint8 var_3;
	sint8 var_4;
	sint8 var_5;
	sint8 var_6;
	sint8 var_7;
} rct_map_element;

enum {
	MAP_ELEMENT_TYPE_UNKNOWN = (0 << 2),
	MAP_ELEMENT_TYPE_PATH = (1 << 2),
	MAP_ELEMENT_TYPE_TRACK = (2 << 2),
	MAP_ELEMENT_TYPE_SCENERY = (3 << 2),
	MAP_ELEMENT_TYPE_ENTRANCE = (4 << 2),
	MAP_ELEMENT_TYPE_FENCE = (5 << 2),
	MAP_ELEMENT_TYPE_SCENERY_M = (6 << 2),
	MAP_ELEMENT_TYPE_BANNER = (7 << 2)
};

#define MAP_ELEMENT_TYPE_MASK 0x3C
#define MAX_MAP_ELEMENTS 65536

void map_init();

#endif