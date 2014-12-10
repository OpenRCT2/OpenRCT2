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
#include "../interface/viewport.h"
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

/* rct2: 0x006EC473 */
void invalidate_sprite(rct_sprite* sprite){
	if (sprite->unknown.sprite_left == (sint16)0x8000) return;

	for (rct_viewport** viewport_p = RCT2_ADDRESS(RCT2_ADDRESS_ACTIVE_VIEWPORT_PTR_ARRAY, rct_viewport*); *viewport_p != NULL; viewport_p++){
		rct_viewport* viewport = *viewport_p;
		int left, right, top, bottom;
		left = sprite->unknown.sprite_left;
		right = sprite->unknown.sprite_right;
		top = sprite->unknown.sprite_top;
		bottom = sprite->unknown.sprite_bottom;

		if (viewport->zoom >= 3)continue;
		if (right <= viewport->view_x)continue;
		if (bottom <= viewport->view_y) continue;

		int viewport_right = viewport->view_x + viewport->view_width;
		int viewport_bottom = viewport->view_y + viewport->view_height;

		if (left >= viewport_right)continue;
		if (top >= viewport_bottom)continue;

		left = max(left, viewport->view_x);
		right = min(right, viewport_right);
		top = max(top, viewport->view_y);
		bottom = min(bottom, viewport_bottom);

		left -= viewport->view_x;
		top -= viewport->view_y;
		right -= viewport->view_x;
		bottom -= viewport->view_y;

		int zoom = 1 << viewport->zoom;
		left /= zoom;
		top /= zoom;
		right /= zoom;
		bottom /= zoom;

		gfx_set_dirty_blocks(left + viewport->x, top + viewport->y, right + viewport->x, bottom + viewport->y);
	}
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
		spr->unknown.linked_list_type_offset = 0;

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
* bl: if bl & 2 > 0, the sprite ends up in the FLOATING_TEXT linked list.
*/
rct_sprite *create_sprite(uint8 bl)
{
	SPRITE_LINKEDLIST_OFFSET linkedListTypeOffset = SPRITE_LINKEDLIST_OFFSET_UNKNOWN; // cl

	if ((bl & 2) != 0)
	{
		// 69EC96;
		uint16 cx = 0x12C - RCT2_GLOBAL(0x13573CE, uint16);
		if (cx >= RCT2_GLOBAL(0x13573C8, uint16))
		{
			return NULL;
		}

		linkedListTypeOffset = SPRITE_LINKEDLIST_OFFSET_FLOATING_TEXT;
	}
	else if (RCT2_GLOBAL(0x13573C8, uint16) <= 0)
	{
		return NULL;
	}

	rct_unk_sprite *sprite = &(g_sprite_list[RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_NEXT_INDEX, uint16)]).unknown;

	move_sprite_to_list((rct_sprite *)sprite, (uint8)linkedListTypeOffset);

	sprite->x = SPRITE_LOCATION_NULL;
	sprite->y = SPRITE_LOCATION_NULL;
	sprite->z = 0;
	sprite->name_string_idx = 0;
	sprite->var_14 = 0x10;
	sprite->var_09 = 0x14;
	sprite->var_15 = 0x8;
	sprite->pad_0C[0] = 0x0;
	sprite->sprite_left = SPRITE_LOCATION_NULL;

	sprite->var_02 = RCT2_GLOBAL(0xF3EF60, uint16);
	RCT2_GLOBAL(0xF3EF60, uint16) = sprite->sprite_index;

	return (rct_sprite*)sprite;
}

/*
* rct2: 0x0069ED0B
* This function moves a sprite to the specified sprite linked list.
* There are 5/6 of those, and cl specifies a pointer offset 
* of the desired linked list in a uint16 array. Known valid values are
* 2, 4, 6, 8 or 10 (SPRITE_LINKEDLIST_OFFSET_...)
*/
void move_sprite_to_list(rct_sprite *sprite, uint8 cl)
{
	rct_unk_sprite *unkSprite = &sprite->unknown;

	// No need to move if the sprite is already in the desired list
	if (unkSprite->linked_list_type_offset == cl)
		return;

	// If the sprite is currently the head of the list, the 
	// sprite following this one becomes the new head of the list.
	if (unkSprite->previous == SPRITE_INDEX_NULL)
	{
		RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_NEXT_INDEX + unkSprite->linked_list_type_offset, uint16) = unkSprite->next;
	}
	else
	{
		// Hook up sprite->previous->next to sprite->next, removing the sprite from its old list
		g_sprite_list[unkSprite->previous].unknown.next = unkSprite->next;
	}

	// Similarly, hook up sprite->next->previous to sprite->previous
	if (unkSprite->next != SPRITE_INDEX_NULL)
	{
		g_sprite_list[unkSprite->next].unknown.previous = unkSprite->previous;
	}

	int oldListTypeOffset = unkSprite->linked_list_type_offset;
	unkSprite->previous = SPRITE_INDEX_NULL; // We become the new head of the target list, so there's no previous sprite
	unkSprite->linked_list_type_offset = cl;

	unkSprite->next = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_NEXT_INDEX + cl, uint16); // This sprite's next sprite is the old head, since we're the new head
	RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_NEXT_INDEX + cl, uint16) = unkSprite->sprite_index; // Store this sprite's index as head of its new list

	if (unkSprite->next != SPRITE_INDEX_NULL)
	{
		// Fix the chain by settings sprite->next->previous to sprite_index
		g_sprite_list[unkSprite->next].unknown.previous = unkSprite->sprite_index;
	}

	// These globals are probably counters for each sprite list?
	// Decrement old list counter, increment new list counter.
	--(RCT2_GLOBAL(0x13573C8 + oldListTypeOffset, uint16));
	++(RCT2_GLOBAL(0x13573C8 + cl, uint16));
}

/**
 *
 *  rct: 0x00672AA4
 */
void texteffect_update_all()
{
	RCT2_CALLPROC_EBPSAFE(0x00672AA4);
}

/**
 *  rct2: 0x0069E9D3
 *  Moves a sprite to a new location.
 * ax: x
 * cx: y
 * dx: z
 */
void sprite_move(int x, int y, int z, rct_sprite* sprite){
	int new_position = x;
	if ((uint16)x == 0x8000)new_position = 0x10000;
	else{
		new_position &= 0x1FE0;
		new_position = (y >> 5) | (new_position << 3);
	}

	int current_position = sprite->unknown.x;
	if ((uint16)sprite->unknown.x == 0x8000)current_position = 0x10000;
	else{
		current_position &= 0x1FE0;
		current_position = (sprite->unknown.y >> 5) | (current_position << 3);
	}

	if (new_position != current_position){
		uint16* sprite_idx = &RCT2_ADDRESS(0xF1EF60, uint16)[current_position];
		rct_sprite* sprite2 = &g_sprite_list[*sprite_idx];
		while (sprite != sprite2){
			sprite_idx = &sprite2->unknown.var_02;
			sprite2 = &g_sprite_list[*sprite_idx];
		}
		*sprite_idx = sprite->unknown.var_02;

		int temp_sprite_idx = RCT2_ADDRESS(0xF1EF60, uint16)[new_position];
		RCT2_ADDRESS(0xF1EF60, uint16)[new_position] = sprite->unknown.sprite_index;
		sprite->unknown.var_02 = temp_sprite_idx;
	}

	if (x == 0x8000){
		sprite->unknown.sprite_left = 0x8000;
		sprite->unknown.x = x;
		sprite->unknown.y = y;
		sprite->unknown.z = z;
		return;
	}
	int new_x = x, new_y = y, start_x = x;
	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)){
	case 0:
		new_x = new_y - new_x;
		new_y = (new_y + start_x) / 2 - z;
		break;
	case 1:
		new_x = -new_y - new_x;
		new_y = (new_y - start_x) / 2 - z;
		break;
	case 2:
		new_x = -new_y + new_x;
		new_y = (-new_y - start_x) / 2 - z;
		break;
	case 3:
		new_x = new_y + new_x;
		new_y = (-new_y + start_x) / 2 - z;
		break;
	}

	sprite->unknown.sprite_left = new_x - sprite->unknown.var_14;
	sprite->unknown.sprite_right = new_x + sprite->unknown.var_14;
	sprite->unknown.sprite_top = new_y - sprite->unknown.var_09;
	sprite->unknown.sprite_bottom = new_y + sprite->unknown.var_15;
	sprite->unknown.x = x;
	sprite->unknown.y = y;
	sprite->unknown.z = z;
}

/**
 *
 *  rct2: 0x006E88D7
 */
void balloon_pop(rct_sprite *sprite)
{
	RCT2_CALLPROC_X(0x006E88D7, 0, 0, 0, (int)sprite, 0, 0, 0);
}

/**
 *
 *  rct2: 0x0069EDB6
 */
void sprite_remove(rct_sprite *sprite)
{
	RCT2_CALLPROC_X(0x0069EDB6, 0, 0, 0, 0, (int)sprite, 0, 0);
}