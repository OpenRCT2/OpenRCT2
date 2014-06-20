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

#include "addresses.h"
#include <string.h>
#include "sprite.h"

/**
 * 
 *  rct2: 0x006736C7
 */
void create_balloon(int x, int y, int z, int colour)
{
	RCT2_CALLPROC_X(0x006736C7, x, colour << 8, y, z, 0, 0, 0);
}

/*
 *
 * rct2: 0x0069EB13
 */
void reset_sprite_list(){
	RCT2_GLOBAL(0x1388698, uint16) = 0;
	memset((uint32*)RCT2_ADDRESS_SPRITE_LIST, 0, 0x9C400);

	for (int i = 0; i < 6; ++i){
		RCT2_ADDRESS(RCT2_ADDRESS_SPRITES_NEXT_INDEX, uint16)[i] = -1;
		RCT2_ADDRESS(0x13573C8, uint16)[i] = 0;
	}

	rct_sprite* previous_spr = (rct_sprite*)SPRITE_INDEX_NULL;

	rct_sprite* spr = RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite);
	for (int i = 0; i < 0x2710; ++i){
		spr->unknown.sprite_identifier = 0xFF;
		spr->unknown.sprite_index = i;
		spr->unknown.next = SPRITE_INDEX_NULL;
		spr->unknown.var_08 = 0;

		if (previous_spr != (rct_sprite*)SPRITE_INDEX_NULL){
			spr->unknown.previous = previous_spr->unknown.sprite_index;
			previous_spr->unknown.next = i;
		}
		else{
			spr->unknown.previous = SPRITE_INDEX_NULL;
			RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_NEXT_INDEX, uint16) = i;
		}
		previous_spr = spr;
		spr++;
	}

	RCT2_GLOBAL(0x13573C8, uint16) = 0x2710;

	RCT2_CALLPROC_EBPSAFE(0x0069EBE4);
}