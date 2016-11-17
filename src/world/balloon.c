#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../audio/audio.h"
#include "../network/network.h"
#include "../scenario.h"
#include "../util/util.h"
#include "sprite.h"

/**
 *
 *  rct2: 0x006736C7
 */
void create_balloon(int x, int y, int z, int colour, uint8 bl)
{
	rct_sprite* sprite = create_sprite(2);
	if (sprite != NULL) {
		sprite->balloon.var_14 = 13;
		sprite->balloon.var_09 = 22;
		sprite->balloon.var_15 = 11;
		sprite->balloon.sprite_identifier = SPRITE_IDENTIFIER_MISC;
		sprite_move(x, y, z, sprite);
		sprite->balloon.misc_identifier = SPRITE_MISC_BALLOON;
		sprite->balloon.frame = 0;
		sprite->balloon.colour = colour;
		sprite->balloon.popped = bl;
	}
}

static void balloon_pop(rct_balloon *balloon)
{
	balloon->popped = 1;
	balloon->frame = 0;
	audio_play_sound_at_location(SOUND_BALLOON_POP, balloon->x, balloon->y, balloon->z);
}

/**
 *
 *  rct2: 0x0067342C
 */
void balloon_update(rct_balloon *balloon)
{
	invalidate_sprite_2((rct_sprite*)balloon);
	if (balloon->popped == 1) {
		balloon->frame += 256;
		if (balloon->frame >= 1280)
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
static void balloon_press(rct_balloon *balloon)
{
	if (balloon->popped == 1)
		return;

	uint32 random = scenario_rand();
	if ((balloon->sprite_index & 7) || (random & 0xFFFF) < 0x2000) {
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

void game_command_balloon_press(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp)
{
	unsigned int balloon_num = *eax;
	int flags = *ebx;
	*ebx = 0;
	if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
		return;
	}
	if (balloon_num >= MAX_SPRITES) {
		log_error("Tried getting invalid sprite for balloon: %u", balloon_num);
		*ebx = MONEY32_UNDEFINED;
		return;
	}
	rct_sprite* sprite = get_sprite(balloon_num);
	if (!sprite) {
		return;
	}
	if (sprite->balloon.sprite_identifier == SPRITE_IDENTIFIER_MISC && sprite->balloon.misc_identifier == SPRITE_MISC_BALLOON) {
		balloon_press(&sprite->balloon);
	}
}
