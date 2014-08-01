/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill
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

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "rct2.h"

#define SPRITE_INDEX_NULL    0xFFFF
#define SPRITE_LOCATION_NULL 0x8000

#include "peep.h"
#include "vehicle.h"

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 pad_01;
	uint16 var_02;
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 var_08;
	uint8 pad_09;
	uint16 sprite_index;			// 0x0A
	uint8 pad_0C[2];
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	uint16 pad_14;
	sint16 var_16; //x related
	sint16 var_18; //y related
	sint16 var_1A; //x related
	sint16 var_1C; //y related
	uint8  sprite_direction; //direction of sprite?
} rct_unk_sprite;

typedef struct {
	uint32 pad_00;
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 var_08;
	uint8 pad_09;
	uint16 sprite_index;			// 0x0A
	uint8 pad_0B[0x19];
	uint32 var_24;
} rct_litter;

/**
 * Sprite structure.
 * size: 0x0100
 */
typedef union {
	uint8 pad_00[0x100];
	rct_unk_sprite unknown;
	rct_peep peep;
	rct_litter litter;
	rct_vehicle vehicle;
} rct_sprite;

// rct2: 0x010E63BC
extern rct_sprite* g_sprite_list;

void create_balloon(int x, int y, int z, int colour);
void reset_sprite_list();
void reset_0x69EBE4();

#endif
