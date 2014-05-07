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

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 pad_01;
	uint16 pad_02;
	uint16 next;					// 0x04
	uint8 pad_06[0x08];
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
} rct_unk_sprite;

typedef struct {
	uint32 pad_00;
	uint16 next;					// 0x04
	uint8 pad_06[0x1E];
	uint32 var_24;
} rct_litter;

typedef struct {
	uint8 sprite_idetifier;			// 0x00
	uint8 pad_01[0x0D];
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	uint8 pad_14[0x2a];
	uint16 next_car;				// 0x3E
} rct_car;

/**
 * Sprite structure.
 * size: 0x0100
 */
typedef union {
	uint8 pad_00[0x100];
	rct_unk_sprite unknown;
	rct_peep peep;
	rct_litter litter;
	rct_car car;
} rct_sprite;

#endif
