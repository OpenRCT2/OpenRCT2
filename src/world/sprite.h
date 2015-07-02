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

#include "../common.h"
#include "../peep/peep.h"
#include "../ride/vehicle.h"

#define SPRITE_INDEX_NULL    0xFFFF
#define SPRITE_LOCATION_NULL 0x8000
#define MAX_SPRITES          10000

enum SPRITE_IDENTIFIER{
	SPRITE_IDENTIFIER_VEHICLE = 0,
	SPRITE_IDENTIFIER_PEEP = 1,
	SPRITE_IDENTIFIER_MISC = 2,
	SPRITE_IDENTIFIER_LITTER = 3,
	SPRITE_IDENTIFIER_NULL = 255
};

typedef enum {
	SPRITE_LINKEDLIST_OFFSET_NULL = 0,
	SPRITE_LINKEDLIST_OFFSET_VEHICLE = 2,
	SPRITE_LINKEDLIST_OFFSET_PEEP = 4,
	SPRITE_LINKEDLIST_OFFSET_MISC = 6,
	SPRITE_LINKEDLIST_OFFSET_LITTER = 8,
	SPRITE_LINKEDLIST_OFFSET_UNKNOWN = 10
} SPRITE_LINKEDLIST_OFFSET;

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 misc_identifier;			// 0x01
	uint16 next_in_quadrant;		// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	// Height from center of sprite to bottom
	uint8 sprite_height_negative;	// 0x09
	uint16 sprite_index;			// 0x0A
	uint16 var_0C;
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	// Width from center of sprite to edge
	uint8 sprite_width;				// 0x14
	// Height from center of sprite to top
	uint8 sprite_height_positive;	// 0x15
	sint16 sprite_left;				// 0x16
	sint16 sprite_top;				// 0x18
	sint16 sprite_right;			// 0x1A
	sint16 sprite_bottom;			// 0x1C
	uint8  sprite_direction; //direction of sprite? 0x1e
	uint8 pad_1F[3]; // 0x1f
	uint16 name_string_idx;			// 0x22
	uint8 pad_24[7];
	uint8 var_2B;
	uint8 pad_2C[0x45];
	uint8 var_71;
} rct_unk_sprite;

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 type;						// 0x01
	uint16 next_in_quadrant;		// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	uint8 sprite_height_negative;	// 0x09
	uint16 sprite_index;			// 0x0A
	uint16 pad_0C;
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	uint8 sprite_width;				// 0x14
	uint8 sprite_height_positive;	// 0x15
	uint8 pad_16[8];
	uint8 sprite_direction;			// 0x1E
	uint8 pad_1F[5];
	uint32 creationTick;			// 0x24
} rct_litter;

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 misc_identifier;			// 0x01
	uint16 next_in_quadrant;		// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	uint8 var_09;					// 0x09
	uint16 var_0A;
	uint8 pad_0C[0x2];
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	uint8 var_14;					// 0x14
	uint8 var_15;					// 0x15
	uint8 pad_16[0xE];
	uint16 popped;					// 0x24
	union {
		uint16 var_26;
		struct {
			uint8 var_26a;
			uint8 var_26b;
		};
	};
	uint8 pad_28[4];
	uint8 colour;					// 0x2C
} rct_balloon;

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 misc_identifier;			// 0x01
	uint16 next_in_quadrant;		// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	uint8 var_09;					// 0x09
	uint16 var_0A;
	uint8 pad_0C[0x2];
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	uint8 var_14;					// 0x14
	uint8 var_15;					// 0x15
	uint8 pad_16[0x8];
	uint8 sprite_direction;			// 0x1E
	uint8 pad_1F[0x7];
	uint16 var_26;
	uint8 pad_28[0x8];
	sint16 target_x;				// 0x30
	sint16 target_y;				// 0x32
	uint8 pad_34[0x14];
	uint8 state;					// 0x48
} rct_duck;

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 misc_identifier;			// 0x01
	uint16 next_in_quadrant;		// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	uint8 var_09;
	uint8 pad_0A[0x4];
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	uint8 var_14;
	uint8 var_15;
	uint8 pad_16[0x8];
	uint8 sprite_direction;			// 0x1E
	uint8 pad_1F[0x7];
	union {
		uint16 var_26;
		struct {
			uint8 var_26a;
			uint8 var_26b;
		};
	};
	uint8 pad_28[0x6];
	uint8 var_2E;
	uint8 flags;
	sint16 target_x;				// 0x30
	sint16 target_y;				// 0x32
	uint8 pad_34[0x12];
	uint16 iteration;				// 0x46
} rct_jumping_fountain;

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 misc_identifier;			// 0x01
	uint16 var_02;					// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	uint8 var_09;
	uint8 pad_0A[0x4];
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	uint8 var_14;
	uint8 var_15;
	uint8 pad_16[0xE];
	uint16 move_delay;				// 0x24
	uint16 num_movements;			// 0x26
	money32 value;					// 0x28
	uint8 pad_2C[0x18];
	sint16 offset_x;				// 0x44
	uint16 wiggle;					// 0x46
} rct_money_effect;

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
	rct_balloon balloon;
	rct_duck duck;
	rct_jumping_fountain jumping_fountain;
	rct_money_effect money_effect;
} rct_sprite;

enum {
	SPRITE_MISC_0,
	SPRITE_MISC_MONEY_EFFECT,
	SPRITE_MISC_2,							// (related to vehicle crash, probably crash particles)
	SPRITE_MISC_3,							// (related to vehicle crash, probably crash particles)
	SPRITE_MISC_4,							// (related to vehicle crash, probably crash particles)
	SPRITE_MISC_5,							// (related to vehicle crash, probably crash particles)
	SPRITE_MISC_JUMPING_FOUNTAIN_WATER,
	SPRITE_MISC_BALLOON,
	SPRITE_MISC_DUCK,
	SPRITE_MISC_JUMPING_FOUNTAIN_SNOW
};

// rct2: 0x010E63BC
extern rct_sprite* g_sprite_list;

void create_balloon(int x, int y, int z, int colour, uint8 bl);
void balloon_press(rct_balloon *balloon);
void create_duck(int targetX, int targetY);
void duck_press(rct_duck *duck);
void duck_remove_all();
void money_effect_create(money32 value);
rct_sprite *create_sprite(uint8 bl);
void reset_sprite_list();
void reset_0x69EBE4();
void sprite_clear_all_unused();
void move_sprite_to_list(rct_sprite *sprite, uint8 cl);
void sprite_misc_update_all();
void sprite_move(sint16 x, sint16 y, sint16 z, rct_sprite* sprite);
void invalidate_sprite(rct_sprite *sprite);
void sub_6EC60B(rct_sprite* sprite);
void sprite_remove(rct_sprite *sprite);
void litter_create(int x, int y, int z, int direction, int type);
void sub_6EC53F(rct_sprite *sprite);
void sub_6738E1(int x, int y, int z);

#endif
