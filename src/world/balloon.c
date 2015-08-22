#include "../audio/audio.h"
#include "../scenario.h"
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
	invalidate_sprite_2((rct_sprite*)balloon);
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

	uint32 random = rand();
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
