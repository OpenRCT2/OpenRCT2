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
#include "../audio/audio.h"
#include "../interface/viewport.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../scenario.h"
#include "fountain.h"
#include "sprite.h"

enum {
	DUCK_STATE_FLY_TO_WATER,
	DUCK_STATE_SWIM,
	DUCK_STATE_DRINK,
	DUCK_STATE_DOUBLE_DRINK,
	DUCK_STATE_FLY_AWAY
};

rct_sprite* g_sprite_list = RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite);

static void duck_update_fly_to_water(rct_duck *duck);
static void duck_update_swim(rct_duck *duck);
static void duck_update_drink(rct_duck *duck);
static void duck_update_double_drink(rct_duck *duck);
static void duck_update_fly_away(rct_duck *duck);

/**
 * 
 *  rct2: 0x006736C7
 */
void create_balloon(int x, int y, int z, int colour, uint8 bl)
{
	rct_sprite* sprite = create_sprite(2);
	if (sprite != NULL)
	{
		sprite->balloon.var_14 = 13;
		sprite->balloon.var_09 = 22;
		sprite->balloon.var_15 = 11;
		sprite->balloon.sprite_identifier = SPRITE_IDENTIFIER_MISC;
		sprite_move(x, y, z, sprite);
		sprite->balloon.misc_identifier = SPRITE_MISC_BALLOON;
		sprite->balloon.var_26 = 0;
		sprite->balloon.colour = colour;
		sprite->balloon.popped = bl;
	}
}

void balloon_pop(rct_balloon *balloon)
{
	balloon->popped = 1;
	balloon->var_26 = 0;
	sound_play_panned(SOUND_BALLOON_POP, 0x8001, balloon->x, balloon->y, balloon->z);
}

/**
 *
 *  rct: 0x0067342C
 */
void balloon_update(rct_balloon *balloon)
{
	invalidate_sprite((rct_sprite*)balloon);
	if (balloon->popped == 1) {
		balloon->var_26 += 256;
		if (balloon->var_26 >= 1280)
			sprite_remove((rct_sprite*)balloon);

		return;
	}

	int original_var26a = balloon->var_26a;
	balloon->var_26a += 85;
	if (original_var26a < 255 - 85)
		return;

	balloon->var_26b++;
	sprite_move(balloon->x, balloon->y, balloon->z + 1, (rct_sprite*)balloon);

	int maxZ = 1967 - ((balloon->x ^ balloon->y) & 31);
	if (balloon->z < maxZ)
		return;

	balloon_pop(balloon);
}

/**
 *
 *  rct2: 0x006E88ED
 */
void balloon_press(rct_balloon *balloon)
{
	if (balloon->popped == 1)
		return;

	uint32 random = scenario_rand();
	if ((balloon->var_0A & 7) || (random & 0xFFFF) < 0x2000) {
		balloon_pop(balloon);
		return;
	}

	sprite_move(
		balloon->x + ((random & 0x80000000) ? -6 : 6),
		balloon->y,
		balloon->z,
		(rct_sprite*)balloon
	);
}

// rct2: 0x009A3B04
static const rct_xy16 duck_move_offset[] = {
	{ -1,  0 },
	{  0,  1 },
	{  1,  0 },
	{  0, -1 }
};

// rct2: 0x0097F073
static const uint8 duck_drink_animation[] = {
	1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 255
};

// rct2: 0x0097F08C
static const uint8 duck_double_drink_animation[] = {
	4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6,
	6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 0, 0, 0, 0, 255
};

/**
 * 
 *  rct2: 0x0067440F
 */
void create_duck(int targetX, int targetY)
{
	rct_sprite* sprite = create_sprite(2);
	if (sprite != NULL) {
		sprite->duck.sprite_identifier = SPRITE_IDENTIFIER_MISC;
		sprite->duck.misc_identifier = SPRITE_MISC_DUCK;
		sprite->duck.var_14 = 9;
		sprite->duck.var_09 = 0xC;
		sprite->duck.var_15 = 9;
		int offset_xy = scenario_rand() & 0x1E;
		targetX += offset_xy;
		targetY += offset_xy;
		sprite->duck.target_x = targetX;
		sprite->duck.target_y = targetY;
		uint8 direction = scenario_rand() & 3;
		switch (direction)
		{
		case 0:
			targetX = 8191 - (scenario_rand() & 0x3F);
			break;
		case 1:
			targetY = scenario_rand() & 0x3F;
			break;
		case 2:
			targetX = scenario_rand() & 0x3F;
			break;
		case 3:
			targetY = 8191 - (scenario_rand() & 0x3F);
			break;
		}
		sprite->duck.sprite_direction = direction << 3;
		sprite_move(targetX, targetY, 496, sprite);
		sprite->duck.state = DUCK_STATE_FLY_TO_WATER;
		sprite->duck.var_26 = 0;
	}
}

/**
 *
 *  rct: 0x006740E8
 */
void duck_update(rct_duck *duck)
{
	switch (duck->state) {
	case DUCK_STATE_FLY_TO_WATER:
		duck_update_fly_to_water(duck);
		break;
	case DUCK_STATE_SWIM:
		duck_update_swim(duck);
		break;
	case DUCK_STATE_DRINK:
		duck_update_drink(duck);
		break;
	case DUCK_STATE_DOUBLE_DRINK:
		duck_update_double_drink(duck);
		break;
	case DUCK_STATE_FLY_AWAY:
		duck_update_fly_away(duck);
		break;
	}
}

static void duck_invalidate(rct_duck *duck)
{
	sub_6EC60B((rct_sprite*)duck);
}

/**
 *
 *  rct: 0x00674108
 */
static void duck_update_fly_to_water(rct_duck *duck)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 3)
		return;

	duck->var_26++;
	if (duck->var_26 >= 6)
		duck->var_26 = 0;

	duck_invalidate(duck);
	int manhattanDistance = abs(duck->target_x - duck->x) + abs(duck->target_y - duck->y);
	int direction = duck->sprite_direction >> 3;
	int x = duck->x + duck_move_offset[direction].x;
	int y = duck->y + duck_move_offset[direction].y;
	int manhattanDistanceN = abs(duck->target_x - x) + abs(duck->target_y - y);

	rct_map_element *mapElement = map_get_surface_element_at(duck->target_x >> 5, duck->target_y >> 5);
	int waterHeight = mapElement->properties.surface.terrain & 0x1F;
	if (waterHeight == 0) {
		duck->state = DUCK_STATE_FLY_AWAY;
		duck_update_fly_away(duck);
		return;
	}
	waterHeight <<= 4;
	int z = abs(duck->z - waterHeight);

	if (manhattanDistanceN <= manhattanDistance) {
		if (z > manhattanDistanceN) {
			z = duck->z - 2;
			if (waterHeight >= duck->z)
				z += 4;

			duck->var_26 = 1;
		} else {
			z = duck->z;
		}
		sprite_move(x, y, z, (rct_sprite*)duck);
		duck_invalidate(duck);
	} else {
		if (z > 4) {
			duck->state = DUCK_STATE_FLY_AWAY;
			duck_update_fly_away(duck);
		} else {
			duck->state = DUCK_STATE_SWIM;
			duck->var_26 = 0;
			duck_update_swim(duck);
		}
	}
}

/**
 *
 *  rct: 0x00674282
 */
static void duck_update_swim(rct_duck *duck)
{
	if ((RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) + duck->var_0A) & 3)
		return;

	uint32 randomNumber = scenario_rand();
	if ((randomNumber & 0xFFFF) < 0x666) {
		if (randomNumber & 0x80000000) {
			duck->state = DUCK_STATE_DOUBLE_DRINK;
			duck->var_26 = -1;
			duck_update_double_drink(duck);
		} else {
			duck->state = DUCK_STATE_DRINK;
			duck->var_26 = -1;
			duck_update_drink(duck);
		}
		return;
	}

	int currentMonth = date_get_month(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16));
	if (currentMonth >= MONTH_SEPTEMBER && (randomNumber >> 16) < 218) {
		duck->state = DUCK_STATE_FLY_AWAY;
		duck_update_fly_away(duck);
		return;
	}

	duck_invalidate(duck);
	int landZ = map_element_height(duck->x, duck->y);
	int waterZ = (landZ >> 16) & 0xFFFF;
	landZ &= 0xFFFF;

	if (duck->z < landZ || waterZ == 0) {
		duck->state = DUCK_STATE_FLY_AWAY;
		duck_update_fly_away(duck);
		return;
	}

	duck->z = waterZ;
	randomNumber = scenario_rand();
	if ((randomNumber & 0xFFFF) <= 0xAAA) {
		randomNumber >>= 16;
		duck->sprite_direction = randomNumber & 0x18;
	}

	int direction = duck->sprite_direction >> 3;
	int x = duck->x + duck_move_offset[direction].x;
	int y = duck->y + duck_move_offset[direction].y;
	landZ = map_element_height(x, y);
	waterZ = (landZ >> 16) & 0xFFFF;
	landZ &= 0xFFFF;

	if (duck->z < landZ || duck->z != waterZ)
		return;

	sprite_move(x, y, waterZ, (rct_sprite*)duck);
	duck_invalidate(duck);
}

/**
 *
 *  rct: 0x00674357
 */
static void duck_update_drink(rct_duck *duck)
{
	duck->var_26++;
	if (duck_drink_animation[duck->var_26] == 255) {
		duck->state = DUCK_STATE_SWIM;
		duck->var_26 = 0;
		duck_update_swim(duck);
	} else {
		duck_invalidate(duck);
	}
}

/**
 *
 *  rct: 0x00674372
 */
static void duck_update_double_drink(rct_duck *duck)
{
	duck->var_26++;
	if (duck_double_drink_animation[duck->var_26] == 255) {
		duck->state = DUCK_STATE_SWIM;
		duck->var_26 = 0;
		duck_update_swim(duck);
	} else {
		duck_invalidate(duck);
	}
}

/**
 *
 *  rct: 0x0067438D
 */
static void duck_update_fly_away(rct_duck *duck)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 3)
		return;

	duck->var_26++;
	if (duck->var_26 >= 6)
		duck->var_26 = 0;

	duck_invalidate(duck);
	int direction = duck->sprite_direction >> 3;
	int x = duck->x + (duck_move_offset[direction].x * 2);
	int y = duck->y + (duck_move_offset[direction].y * 2);
	if (x < 0 || y < 0 || x >= (32 * 256) || y >= (32 * 256)) {
		sprite_remove((rct_sprite*)duck);
		return;
	}

	int z = z = min(duck->z + 2, 496);
	sprite_move(x, y, z, (rct_sprite*)duck);
	duck_invalidate(duck);
}

/**
 *
 *  rct: 0x006E895D
 */
void duck_press(rct_duck *duck)
{
	sound_play_panned(SOUND_QUACK, 0x8001, duck->x, duck->y, duck->z);
}

/**
 *
 *  rct: 0x00674576
 */
void duck_remove_all()
{
	rct_unk_sprite* sprite;
	uint16 spriteIndex, nextSpriteIndex;

	for (spriteIndex = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_MISC, uint16); spriteIndex != SPRITE_INDEX_NULL; spriteIndex = nextSpriteIndex) {
		sprite = &(g_sprite_list[spriteIndex].unknown);
		nextSpriteIndex = sprite->next;
		if (sprite->misc_identifier == SPRITE_MISC_DUCK)
			sprite_remove((rct_sprite*)sprite);
	}
}

static const rct_xy16 _moneyEffectMoveOffset[] = {
	{  1, -1 },
	{  1,  1 },
	{ -1,  1 },
	{ -1, -1 }
};

/**
 *
 *  rct: 0x0067351F
 */
void money_effect_create_at(money32 value, int x, int y, int z)
{
	rct_money_effect *moneyEffect;
	rct_string_id stringId;
	char buffer[128];

	moneyEffect = (rct_money_effect*)create_sprite(2);
	if (moneyEffect == NULL)
		return;

	moneyEffect->value = value;
	moneyEffect->var_14 = 64;
	moneyEffect->var_09 = 20;
	moneyEffect->var_15 = 30;
	moneyEffect->sprite_identifier = SPRITE_IDENTIFIER_MISC;
	sprite_move(x, y, z, (rct_sprite*)moneyEffect);
	moneyEffect->misc_identifier = SPRITE_MISC_MONEY_EFFECT;
	moneyEffect->num_movements = 0;
	moneyEffect->move_delay = 0;

	stringId = 1388;
	if (value < 0) {
		value *= -1;
		stringId = 1399;
	}
	format_string(buffer, stringId, &value);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	moneyEffect->offset_x = -(gfx_get_string_width(buffer) / 2);
	moneyEffect->wiggle = 0;
}

/**
 *
 *  rct: 0x0069C5D0
 */
void money_effect_create(money32 value)
{
	rct_window *mainWindow;
	rct_viewport *mainViewport;
	rct_xyz16 mapPosition;

	mapPosition.x = RCT2_GLOBAL(0x009DEA5E, uint16);
	mapPosition.y = RCT2_GLOBAL(0x009DEA60, uint16);
	mapPosition.z = RCT2_GLOBAL(0x009DEA62, uint16);
	if (mapPosition.x == (sint16)0x8000) {
		mainWindow = window_get_main();
		if (mainWindow == NULL)
			return;

		mainViewport = mainWindow->viewport;
		mapPosition.x = mainViewport->x + (mainViewport->width / 2);
		mapPosition.y = mainViewport->y + (mainViewport->height / 2);

		int eax = mapPosition.x, ebx = mapPosition.y, ecx, edx, esi, edi, ebp;
		RCT2_CALLFUNC_X(0x00688972, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
		mapPosition.x = eax;
		mapPosition.y = ebx;

		if (mapPosition.x == (sint16)0x8000)
			return;

		mapPosition.z = map_element_height(mapPosition.x, mapPosition.y) & 0xFFFF;
	}
	mapPosition.z += 10;
	money_effect_create_at(-value, mapPosition.x, mapPosition.y, mapPosition.z);
}

/**
 *
 *  rct: 0x00673232
 */
void money_effect_update(rct_money_effect *moneyEffect)
{
	invalidate_sprite((rct_sprite*)moneyEffect);
	moneyEffect->wiggle++;
	if (moneyEffect->wiggle >= 22)
		moneyEffect->wiggle = 0;

	moneyEffect->move_delay++;
	if (moneyEffect->move_delay < 2)
		return;

	moneyEffect->move_delay = 0;
	int x = moneyEffect->x + _moneyEffectMoveOffset[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)].x;
	int y = moneyEffect->y + _moneyEffectMoveOffset[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)].y;
	int z = moneyEffect->z;
	sprite_move(x, y, z, (rct_sprite*)moneyEffect);

	moneyEffect->num_movements++;
	if (moneyEffect->num_movements < 55)
		return;

	sprite_remove((rct_sprite*)moneyEffect);
}

/*
 *
 * rct2: 0x006EC473
 */
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

/**
 * Similar to invalidate sprite...
 * rct2: 0x006EC60B
 */
void sub_6EC60B(rct_sprite* sprite)
{
	RCT2_CALLPROC_X(0x006EC60B, 0, 0, 0, 0, (int)sprite, 0, 0);
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

	reset_0x69EBE4();
}

/**
 * rct2: 0x0069EBE4
 * This function looks as though it sets some sort of order for sprites.
 * Sprites can share thier position if this is the case.
 */
void reset_0x69EBE4(){
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
			spr->unknown.next_in_quadrant = ax;
		}
	}
}

/**
 * Clears all the unused sprite memory to zero. Probably so that it can be compressed better when saving.
 * rct2: 0x0069EBA4
 */
void sprite_clear_all_unused()
{
	rct_unk_sprite *sprite;
	uint16 spriteIndex, nextSpriteIndex, previousSpriteIndex;
	
	spriteIndex = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_NEXT_INDEX, uint16);
	while (spriteIndex != SPRITE_INDEX_NULL) {
		sprite = &g_sprite_list[spriteIndex].unknown;
		nextSpriteIndex = sprite->next;
		previousSpriteIndex = sprite->previous;
		memset(sprite, 0, sizeof(rct_sprite));
		sprite->sprite_identifier = SPRITE_IDENTIFIER_NULL;
		sprite->next = nextSpriteIndex;
		sprite->previous = previousSpriteIndex;
		sprite->linked_list_type_offset = SPRITE_LINKEDLIST_OFFSET_NULL;
		sprite->sprite_index = spriteIndex;
		spriteIndex = nextSpriteIndex;
	}
}

/*
* rct2: 0x0069EC6B
* bl: if bl & 2 > 0, the sprite ends up in the MISC linked list.
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

		linkedListTypeOffset = SPRITE_LINKEDLIST_OFFSET_MISC;
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
	sprite->sprite_width = 0x10;
	sprite->sprite_height_negative = 0x14;
	sprite->sprite_height_positive = 0x8;
	sprite->var_0C = 0;
	sprite->sprite_left = SPRITE_LOCATION_NULL;

	sprite->next_in_quadrant = RCT2_ADDRESS(0xF1EF60, uint16)[0x10000];
	RCT2_ADDRESS(0xF1EF60, uint16)[0x10000] = sprite->sprite_index;

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
 *  rct: 0x00673200
 */
static void sub_673200(rct_sprite *sprite)
{
	RCT2_CALLPROC_X(0x00673200, 0, 0, 0, 0, (int)sprite, 0, 0);
}

/**
 *
 *  rct: 0x00673298
 */
static void sub_673298(rct_sprite *sprite)
{
	RCT2_CALLPROC_X(0x00673298, 0, 0, 0, 0, (int)sprite, 0, 0);
}

/**
 *
 *  rct: 0x00673385
 */
static void sub_673385(rct_sprite *sprite)
{
	RCT2_CALLPROC_X(0x00673385, 0, 0, 0, 0, (int)sprite, 0, 0);
}

/**
 *
 *  rct: 0x0067339D
 */
static void sub_67339D(rct_sprite *sprite)
{
	RCT2_CALLPROC_X(0x0067339D, 0, 0, 0, 0, (int)sprite, 0, 0);
}

/**
 *
 *  rct: 0x006733B4
 */
static void sub_6733B4(rct_sprite *sprite)
{
	RCT2_CALLPROC_X(0x006733B4, 0, 0, 0, 0, (int)sprite, 0, 0);
}

/**
 *
 *  rct: 0x006731CD
 */
void sprite_misc_update(rct_sprite *sprite)
{
	switch (sprite->unknown.misc_identifier) {
	case SPRITE_MISC_0:
		sub_673200(sprite);
		break;
	case SPRITE_MISC_MONEY_EFFECT:
		money_effect_update(&sprite->money_effect);
		break;
	case SPRITE_MISC_2:
		sub_673298(sprite);
		break;
	case SPRITE_MISC_3:
		sub_673385(sprite);
		break;
	case SPRITE_MISC_4:
		sub_67339D(sprite);
		break;
	case SPRITE_MISC_5:
		sub_6733B4(sprite);
		break;
	case SPRITE_MISC_JUMPING_FOUNTAIN_WATER:
	case SPRITE_MISC_JUMPING_FOUNTAIN_SNOW:
		jumping_fountain_update(&sprite->jumping_fountain);
		break;
	case SPRITE_MISC_BALLOON:
		balloon_update(&sprite->balloon);
		break;
	case SPRITE_MISC_DUCK:
		duck_update(&sprite->duck);
		break;
	}
}

/**
 *
 *  rct: 0x00672AA4
 */
void sprite_misc_update_all()
{
	rct_sprite *sprite;
	uint16 spriteIndex;

	spriteIndex = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_MISC, uint16);
	while (spriteIndex != SPRITE_INDEX_NULL) {
		sprite = &g_sprite_list[spriteIndex];
		spriteIndex = sprite->unknown.next;
		sprite_misc_update(sprite);
	}
}

/**
 *  rct2: 0x0069E9D3
 *  Moves a sprite to a new location.
 * ax: x
 * cx: y
 * dx: z
 */
void sprite_move(sint16 x, sint16 y, sint16 z, rct_sprite* sprite){
	if (x < 0 || y < 0 || x > 0x1FFF || y > 0x1FFF)
		x = 0x8000;

	int new_position = x;
	if (x == (sint16)0x8000)new_position = 0x10000;
	else{
		new_position &= 0x1FE0;
		new_position = (y >> 5) | (new_position << 3);
	}

	int current_position = sprite->unknown.x;
	if (sprite->unknown.x == (sint16)0x8000)current_position = 0x10000;
	else{
		current_position &= 0x1FE0;
		current_position = (sprite->unknown.y >> 5) | (current_position << 3);
	}

	if (new_position != current_position){
		uint16* sprite_idx = &RCT2_ADDRESS(0xF1EF60, uint16)[current_position];
		rct_sprite* sprite2 = &g_sprite_list[*sprite_idx];
		while (sprite != sprite2){
			sprite_idx = &sprite2->unknown.next_in_quadrant;
			sprite2 = &g_sprite_list[*sprite_idx];
		}
		*sprite_idx = sprite->unknown.next_in_quadrant;

		int temp_sprite_idx = RCT2_ADDRESS(0xF1EF60, uint16)[new_position];
		RCT2_ADDRESS(0xF1EF60, uint16)[new_position] = sprite->unknown.sprite_index;
		sprite->unknown.next_in_quadrant = temp_sprite_idx;
	}

	if (x == (sint16)0x8000){
		sprite->unknown.sprite_left = 0x8000;
		sprite->unknown.x = x;
		sprite->unknown.y = y;
		sprite->unknown.z = z;
		return;
	}
	sint16 new_x = x, new_y = y, start_x = x;
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

	sprite->unknown.sprite_left = new_x - sprite->unknown.sprite_width;
	sprite->unknown.sprite_right = new_x + sprite->unknown.sprite_width;
	sprite->unknown.sprite_top = new_y - sprite->unknown.sprite_height_negative;
	sprite->unknown.sprite_bottom = new_y + sprite->unknown.sprite_height_positive;
	sprite->unknown.x = x;
	sprite->unknown.y = y;
	sprite->unknown.z = z;
}

/**
 *
 *  rct2: 0x0069EDB6
 */
void sprite_remove(rct_sprite *sprite)
{
	RCT2_CALLPROC_X(0x0069EDB6, 0, 0, 0, 0, (int)sprite, 0, 0);
}

static bool litter_can_be_at(int x, int y, int z)
{
	rct_map_element *mapElement;

	if (!map_is_location_owned(x & 0xFFE0, y & 0xFFE0, z))
		return false;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;

		int pathZ = mapElement->base_height * 8;
		if (pathZ < z || pathZ >= z + 32)
			continue;

		if (map_element_is_underground(mapElement))
			return false;

		return true;
	} while (!map_element_is_last_for_tile(mapElement++));
	return false;
}

/**
 *
 *  rct2: 0x0067375D
 */
void litter_create(int x, int y, int z, int direction, int type)
{
	rct_litter *litter, *newestLitter;
	uint16 spriteIndex, nextSpriteIndex;
	uint32 newestLitterCreationTick;

	x += TileDirectionDelta[direction].x / 8;
	y += TileDirectionDelta[direction].y / 8;

	if (!litter_can_be_at(x, y, z))
		return;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_COUNT_LITTER, uint16) >= 500) {
		newestLitter = NULL;
		newestLitterCreationTick = 0;
		for (spriteIndex = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_LITTER, uint16); spriteIndex != SPRITE_INDEX_NULL; spriteIndex = nextSpriteIndex) {
			litter = &(g_sprite_list[spriteIndex].litter);
			nextSpriteIndex = litter->next;
			if (newestLitterCreationTick <= litter->creationTick) {
				newestLitterCreationTick = litter->creationTick;
				newestLitter = litter;
			}
		}

		if (newestLitter != NULL) {
			sub_6EC60B((rct_sprite*)newestLitter);
			sprite_remove((rct_sprite*)newestLitter);
		}
	}

	litter = (rct_litter*)create_sprite(1);
	if (litter == NULL)
		return;

	move_sprite_to_list((rct_sprite*)litter, SPRITE_LINKEDLIST_OFFSET_LITTER);
	litter->sprite_direction = direction;
	litter->sprite_width = 6;
	litter->sprite_height_negative = 6;
	litter->sprite_height_positive = 3;
	litter->sprite_identifier = SPRITE_IDENTIFIER_LITTER;
	litter->type = type;
	sprite_move(x, y, z, (rct_sprite*)litter);
	sub_6EC60B((rct_sprite*)litter);
	litter->creationTick = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32);
}

/**
 *
 *  rct2: 0x006EC53F
 */
void sub_6EC53F(rct_sprite *sprite)
{
	RCT2_CALLPROC_X(0x006EC53F, 0, 0, 0, 0, (int)sprite, 0, 0);
}
