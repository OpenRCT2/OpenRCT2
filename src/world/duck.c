#include "../addresses.h"
#include "../audio/audio.h"
#include "../localisation/date.h"
#include "../scenario.h"
#include "sprite.h"

enum {
	DUCK_STATE_FLY_TO_WATER,
	DUCK_STATE_SWIM,
	DUCK_STATE_DRINK,
	DUCK_STATE_DOUBLE_DRINK,
	DUCK_STATE_FLY_AWAY
};

static void duck_update_fly_to_water(rct_duck *duck);
static void duck_update_swim(rct_duck *duck);
static void duck_update_drink(rct_duck *duck);
static void duck_update_double_drink(rct_duck *duck);
static void duck_update_fly_away(rct_duck *duck);

// rct2: 0x009A3B04
static const rct_xy16 duck_move_offset[] = {
	{ -1,  0 },
	{ 0,  1 },
	{ 1,  0 },
	{ 0, -1 }
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
		switch (direction) {
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
	invalidate_sprite_0((rct_sprite*)duck);
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
