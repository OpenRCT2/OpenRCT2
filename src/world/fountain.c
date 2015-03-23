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
#include "../scenario.h"
#include "fountain.h"
#include "map.h"
#include "scenery.h"
#include "sprite.h"

const rct_xy16 dword_97F020[] = {
	{ 32,  0 },
	{  0,  0 },
	{  0, 32 },
	{ 32, 32 },
	{ 32, 32 },
	{ 32,  0 },
	{  0,  0 },
	{  0, 32 }
};

const uint8 byte_97F040[] = { 0, 1, 2, 3, 0, 1, 2, 3 };
const uint8 byte_97F048[] = { 0, 0, 128, 128, 128, 128, 0, 0 };
const uint8 byte_97F050[] = { 8, 3, 16, 5, 2, 7, 0, 1 };

/**
 *
 *  rct2: 0x00673DBA (water)
 *  rct2: 0x00673F51 (snow)
 */
void jumping_fountain_begin(int type, int x, int y, rct_map_element *mapElement)
{
	int i, randomIndex;
	int z = mapElement->base_height * 8;
	uint32 ticks = (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) >> 11) & 7;
	switch (ticks) {
	case 0:
		// 0, 1, 2, 3
		for (i = 0; i < 4; i++) {
			jumping_fountain_create(
				type,
				x + dword_97F020[i].x,
				y + dword_97F020[i].y,
				z,
				byte_97F040[i],
				byte_97F048[i] | byte_97F050[ticks],
				0
			);
		}
		break;
	case 2:
		// random [0, 2 or 1, 3]
		randomIndex = scenario_rand() & 1;
		for (i = randomIndex; i < 4; i += 2) {
			jumping_fountain_create(
				type,
				x + dword_97F020[i].x,
				y + dword_97F020[i].y,
				z,
				byte_97F040[i],
				byte_97F048[i] | byte_97F050[ticks],
				0
			);
		}
		break;
	case 4:
		// random [0 - 3 and 4 - 7]
		z = mapElement->base_height * 8;
		randomIndex = scenario_rand() & 3;
		jumping_fountain_create(
			type,
			x + dword_97F020[randomIndex].x,
			y + dword_97F020[randomIndex].y,
			z,
			byte_97F040[randomIndex],
			byte_97F048[randomIndex] | byte_97F050[ticks],
			0
		);
		randomIndex += 4;
		jumping_fountain_create(
			type,
			x + dword_97F020[randomIndex].x,
			y + dword_97F020[randomIndex].y,
			z,
			byte_97F040[randomIndex],
			byte_97F048[randomIndex] | byte_97F050[ticks],
			0
		);
		break;
	default:
		// random [0 - 7]
		randomIndex = scenario_rand() & 7;
		jumping_fountain_create(
			type,
			x + dword_97F020[randomIndex].x,
			y + dword_97F020[randomIndex].y,
			z,
			byte_97F040[randomIndex],
			byte_97F048[randomIndex] | byte_97F050[ticks],
			0
		);
		break;
	}
}

/**
 *
 *  rct2: 0x0067396A (water)
 *  rct2: 0x006739A4 (snow)
 */
void jumping_fountain_create(int type, int x, int y, int z, uint16 bl, uint16 bh, uint16 bp)
{
	rct_jumping_fountain *jumpingFountain;

	jumpingFountain = (rct_jumping_fountain*)create_sprite(SPRITE_IDENTIFIER_MISC);
	if (jumpingFountain == NULL)
		return;

	jumpingFountain->var_46 = bp;
	jumpingFountain->var_2E = (bh << 8) | bl;
	jumpingFountain->sprite_direction = bl << 3;
	jumpingFountain->var_14 = 33;
	jumpingFountain->var_09 = 36;
	jumpingFountain->var_15 = 12;
	jumpingFountain->sprite_identifier = SPRITE_IDENTIFIER_MISC;
	sprite_move(x, y, z, (rct_sprite*)jumpingFountain);
	jumpingFountain->misc_identifier = type == JUMPING_FOUNTAIN_TYPE_SNOW ?
		SPRITE_MISC_JUMPING_FOUNTAIN_SNOW :
		SPRITE_MISC_JUMPING_FOUNTAIN_WATER;
	jumpingFountain->var_26 = 0;
}