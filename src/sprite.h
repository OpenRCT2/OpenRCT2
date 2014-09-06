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

enum SPRITE_IDENTIFIER{
	SPRITE_IDENTIFIER_VEHICLE = 0,
	SPRITE_IDENTIFIER_PEEP = 1,
	SPRITE_IDENTIFIER_FLOATING_TEXT = 2,
	SPRITE_IDENTIFIER_LITTER = 3,
};

enum SPRITE_LINKEDLIST_OFFSET {
	SPRITE_LINKEDLIST_OFFSET_VEHICLE = 2,
	SPRITE_LINKEDLIST_OFFSET_PEEP = 4,
	SPRITE_LINKEDLIST_OFFSET_FLOATING_TEXT = 6,
	SPRITE_LINKEDLIST_OFFSET_FLOATING_LITTER = 8,
	SPRITE_LINKEDLIST_OFFSET_FLOATING_UNKNOWN = 10
};

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 pad_01;
	uint16 var_02;
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	uint8 pad_09;
	uint16 sprite_index;			// 0x0A
	uint8 pad_0C[2];
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	uint8 var_14;					// 0x14
	uint8 var_15;					// 0x15
	sint16 var_16; //x related
	sint16 var_18; //y related
	sint16 var_1A; //x related
	sint16 var_1C; //y related
	uint8  sprite_direction; //direction of sprite? 0x1e
	uint8 pad_1F[3]; // 0x1f
	uint16 name_string_idx;			// 0x22
} rct_unk_sprite;

typedef struct {
	uint8 sprite_identifier;        // 0x00
	uint8 var_01;					// 0x01
	uint16 var_02;					// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
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
rct_sprite *create_sprite(uint8 bl);
void reset_sprite_list();
void reset_0x69EBE4();
void move_sprite_to_list(rct_sprite *sprite, uint8 cl);

#endif
