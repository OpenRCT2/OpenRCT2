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

#include "../addresses.h"
#include "map.h"
#include "scenery.h"

/**
 *
 *  rct2: 0x00673DBA
 */
void jumping_fountain_create_water(int x, int y, rct_map_element *mapElement)
{
	RCT2_CALLPROC_X(0x00673DBA, x, 0, y, 0, (int)mapElement, 0, 0);
}

/**
 *
 *  rct2: 0x00673F51
 */
void jumping_fountain_create_snowball(int x, int y, rct_map_element *mapElement)
{
	RCT2_CALLPROC_X(0x00673F51, x, 0, y, 0, (int)mapElement, 0, 0);
}