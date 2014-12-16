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
#include "../game.h"
#include "footpath.h"
#include "map.h"

money32 footpath_place(int type, int x, int y, int z, int slope, int flags)
{
	return game_do_command(x, (slope << 8) | flags, y, (type << 8) | z, GAME_COMMAND_PLACE_PATH, 0, 0);
}

void footpath_remove(int x, int y, int z, int flags)
{
	game_do_command(x, flags, y, z, GAME_COMMAND_REMOVE_PATH, 0, 0);
}

/**
 * 
 *  rct2: 0x006A76FF
 */
money32 footpath_provisional_set(int type, int x, int y, int z, int slope)
{
	money32 cost;

	footpath_provisional_remove();

	cost = footpath_place(type, x, y, z, slope, (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 0));
	if (cost != MONEY32_UNDEFINED) {
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Y, uint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Z, uint8) = z & 0xFF;
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) |= (1 << 1);

		viewport_set_visibility(RCT2_GLOBAL(0x00F3EFA4, uint8) & 2 ? 1 : 3);
	}

	return cost;
}

/**
 * 
 *  rct2: 0x006A77FF
 */
void footpath_provisional_remove()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) & (1 << 1)) {
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) &= ~(1 << 1);

		footpath_remove(
			RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Y, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Z, uint16),
			(1 << 0) | (1 << 3) | (1 << 5)
		);
	}
}

/**
 * 
 *  rct2: 0x006A7831
 */
void sub_6A7831()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) & (1 << 0)) {
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) &= ~(1 << 0);

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint8) &= ~(1 << 2);
		map_invalidate_tile_full(
			RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16)
		);
	}
	footpath_provisional_remove();
}