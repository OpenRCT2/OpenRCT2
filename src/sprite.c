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

rct_sprite* g_sprite_list = RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite);

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
	memset(g_sprite_list, 0, sizeof(rct_sprite)* 0x2710);

	for (int i = 0; i < 6; ++i){
		RCT2_ADDRESS(RCT2_ADDRESS_SPRITES_NEXT_INDEX, uint16)[i] = -1;
		RCT2_ADDRESS(0x13573C8, uint16)[i] = 0;
	}

	rct_sprite* previous_spr = (rct_sprite*)SPRITE_INDEX_NULL;

	rct_sprite* spr = g_sprite_list;
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

	//RCT2_CALLPROC_EBPSAFE(0x0069EBE4);
	reset_0x69EBE4();
}

/*
 *
 * rct: 0x0069EBE4
 * This function looks as though it sets some sort of order for sprites.
 * Sprites can share thier position if this is the case.
 */
void reset_0x69EBE4(){
	//RCT2_CALLPROC_EBPSAFE(0x0069EBE4);
	//return;
	memset((uint16*)0xF1EF60, -1, 0x10001*2);

	rct_sprite* spr = g_sprite_list;
	for (; spr < (rct_sprite*)RCT2_ADDRESS_SPRITES_NEXT_INDEX; spr++){

		if (spr->unknown.sprite_identifier != 0xFF){
			uint32 edi = spr->unknown.x;
			if ((uint16)(spr->unknown.x) == SPRITE_LOCATION_NULL){
				edi = 0x10000;
			}
			else{
				int ecx = spr->unknown.y;
				ecx >>= 5;
				edi &= 0x1FE0;
				edi <<= 3;
				edi |= ecx;
			}
			uint16 ax = RCT2_ADDRESS(0xF1EF60,uint16)[edi];
			RCT2_ADDRESS(0xF1EF60,uint16)[edi] = spr->unknown.sprite_index;
			spr->unknown.var_02 = ax;
		}
	}
}

/*
* rct2: 0x0069EC6B
* bl: unclear what this does
*/
rct_sprite *create_sprite(uint8 bl)
{
	int ecx = 0xA;

	if ((bl & 2) != 0)
	{
		// 69EC96;
		uint16 cx = 0x12C - RCT2_GLOBAL(0x13573CE, uint16);
		if (cx >= RCT2_GLOBAL(0x13573C8, uint16))
		{
			return NULL;
		}

		ecx = 6;
	}
	else if (RCT2_GLOBAL(0x13573C8, uint16) <= 0)
	{
		return NULL;
	}

	rct_unk_sprite *sprite = &(g_sprite_list[RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_NEXT_INDEX, uint16)]).unknown;

	RCT2_CALLPROC_X(0x0069ED0B, 0, 0, ecx, 0, (int)sprite, 0, 0);

	sprite->x = SPRITE_LOCATION_NULL;
	sprite->y = SPRITE_LOCATION_NULL;
	sprite->z = 0;
	sprite->name_string_idx = 0;
	sprite->var_14 = 0x10;
	sprite->pad_09 = 0x14;
	sprite->var_15 = 0x8;
	sprite->pad_0C[0] = 0x0;
	sprite->var_16 = SPRITE_LOCATION_NULL;

	sprite->var_02 = RCT2_GLOBAL(0xF3EF60, uint16);
	RCT2_GLOBAL(0xF3EF60, uint16) = sprite->sprite_index;

	return (rct_sprite*)sprite;
}