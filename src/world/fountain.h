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

#ifndef _WORLD_FOUNTAIN_H_
#define _WORLD_FOUNTAIN_H_

#include "../common.h"
#include "map.h"
#include "sprite.h"

enum {
	JUMPING_FOUNTAIN_TYPE_WATER,
	JUMPING_FOUNTAIN_TYPE_SNOW
};

void jumping_fountain_begin(int type, int x, int y, rct_map_element *mapElement);
void jumping_fountain_create(int type, int x, int y, int z, int direction, int flags, int iteration);
void jumping_fountain_update(rct_jumping_fountain *jumpingFountain);

#endif
