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

enum {
	PATTERN_CYCLIC_SQUARES,
	PATTERN_CONTINUOUS_CHASERS,
	PATTERN_BOUNCING_PAIRS,
	PATTERN_SPROUTING_BLOOMS,
	PATTERN_RACING_PAIRS,
	PATTERN_SPLITTING_CHASERS,
	PATTERN_DOPEY_JUMPERS,
	PATTERN_FAST_RANDOM_CHASERS
};

enum {
	FOUNTAIN_FLAG_FAST = 1 << 0,
	FOUNTAIN_FLAG_GOTO_EDGE = 1 << 1,
	FOUNTAIN_FLAG_SPLIT = 1 << 2,
	FOUNTAIN_FLAG_TERMINATE = 1 << 3,
	FOUNTAIN_FLAG_BOUNCE = 1 << 4,
	FOUNTAIN_FLAG_7 = 1 << 7
};

const rct_xy16 dword_97F000[] = {
	{ -32,   0 },
	{ -32, -32 },
	{   0,   0 },
	{ -32,   0 },
	{   0,   0 },
	{   0, -32 },
	{   0, -32 },
	{ -32, -32 },
};

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

// rct2: 0x0097F040
const uint8 _fountainDirections[] = { 0, 1, 2, 3, 0, 1, 2, 3 };

// rct2: 0x0097F048
const uint8 _fountainDirectionFlags[] = { 0, 0, FOUNTAIN_FLAG_7, FOUNTAIN_FLAG_7, FOUNTAIN_FLAG_7, FOUNTAIN_FLAG_7, 0, 0 };

// rct2: 0x0097F050
const uint8 _fountainPatternFlags[] = {
	FOUNTAIN_FLAG_TERMINATE,											// PATTERN_CYCLIC_SQUARES
	FOUNTAIN_FLAG_FAST | FOUNTAIN_FLAG_GOTO_EDGE,						// PATTERN_CONTINUOUS_CHASERS
	FOUNTAIN_FLAG_BOUNCE,												// PATTERN_BOUNCING_PAIRS
	FOUNTAIN_FLAG_FAST | FOUNTAIN_FLAG_SPLIT,							// PATTERN_SPROUTING_BLOOMS
	FOUNTAIN_FLAG_GOTO_EDGE,											// PATTERN_RACING_PAIRS
	FOUNTAIN_FLAG_FAST | FOUNTAIN_FLAG_GOTO_EDGE | FOUNTAIN_FLAG_SPLIT,	// PATTERN_SPLITTING_CHASERS
	0,																	// PATTERN_DOPEY_JUMPERS
	FOUNTAIN_FLAG_FAST													// PATTERN_FAST_RANDOM_CHASERS
};

static void jumping_fountain_continue(rct_jumping_fountain *jumpingFountain);
static bool is_jumping_fountain(int type, int x, int y, int z);

static void jumping_fountain_goto_edge(rct_jumping_fountain *jumpingFountain, int x, int y, int z, int availableDirections);
static void jumping_fountain_bounce(rct_jumping_fountain *jumpingFountain, int x, int y, int z, int availableDirections);
static void jumping_fountain_split(rct_jumping_fountain *jumpingFountain, int x, int y, int z, int availableDirections);
static void jumping_fountain_random(rct_jumping_fountain *jumpingFountain, int x, int y, int z, int availableDirections);
static void jumping_fountain_create_next(rct_jumping_fountain *jumpingFountain, int x, int y, int z, int direction);

/**
 *
 *  rct2: 0x00673DBA (water)
 *  rct2: 0x00673F51 (snow)
 */
void jumping_fountain_begin(int type, int x, int y, rct_map_element *mapElement)
{
	int i, randomIndex;
	int z = mapElement->base_height * 8;

	// Change pattern approximately every 51 seconds
	int pattern = (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) >> 11) & 7;
	switch (pattern) {
	case PATTERN_CYCLIC_SQUARES:
		// 0, 1, 2, 3
		for (i = 0; i < 4; i++) {
			jumping_fountain_create(
				type,
				x + dword_97F020[i].x,
				y + dword_97F020[i].y,
				z,
				_fountainDirections[i],
				_fountainDirectionFlags[i] | _fountainPatternFlags[pattern],
				0
			);
		}
		break;
	case PATTERN_BOUNCING_PAIRS:
		// random [0, 2 or 1, 3]
		randomIndex = scenario_rand() & 1;
		for (i = randomIndex; i < 4; i += 2) {
			jumping_fountain_create(
				type,
				x + dword_97F020[i].x,
				y + dword_97F020[i].y,
				z,
				_fountainDirections[i],
				_fountainDirectionFlags[i] | _fountainPatternFlags[pattern],
				0
			);
		}
		break;
	case PATTERN_RACING_PAIRS:
		// random [0 - 3 and 4 - 7]
		z = mapElement->base_height * 8;
		randomIndex = scenario_rand() & 3;
		jumping_fountain_create(
			type,
			x + dword_97F020[randomIndex].x,
			y + dword_97F020[randomIndex].y,
			z,
			_fountainDirections[randomIndex],
			_fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern],
			0
		);
		randomIndex += 4;
		jumping_fountain_create(
			type,
			x + dword_97F020[randomIndex].x,
			y + dword_97F020[randomIndex].y,
			z,
			_fountainDirections[randomIndex],
			_fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern],
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
			_fountainDirections[randomIndex],
			_fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern],
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
void jumping_fountain_create(int type, int x, int y, int z, int direction, int flags, int iteration)
{
	rct_jumping_fountain *jumpingFountain;

	jumpingFountain = (rct_jumping_fountain*)create_sprite(SPRITE_IDENTIFIER_MISC);
	if (jumpingFountain == NULL)
		return;

	jumpingFountain->iteration = iteration;
	jumpingFountain->var_2E = direction;
	jumpingFountain->flags = flags;
	jumpingFountain->sprite_direction = direction << 3;
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

/**
 *
 *  rct: 0x006733CB (water)
 *  rct: 0x00673407 (snow)
 */
void jumping_fountain_update(rct_jumping_fountain *jumpingFountain)
{
	int original_var_26a = jumpingFountain->var_26a;
	jumpingFountain->var_26a += 160;
	if (original_var_26a <= 255 - 160)
		return;

	RCT2_CALLPROC_X(0x006EC60B, 0, 0, 0, 0, (int)jumpingFountain, 0, 0);
	jumpingFountain->var_26b++;

	switch (jumpingFountain->misc_identifier) {
	case SPRITE_MISC_JUMPING_FOUNTAIN_WATER:
		if (jumpingFountain->var_26b == 11 && (jumpingFountain->flags & FOUNTAIN_FLAG_FAST))
			jumping_fountain_continue(jumpingFountain);

		if (jumpingFountain->var_26b == 16 && !(jumpingFountain->flags & FOUNTAIN_FLAG_FAST))
			jumping_fountain_continue(jumpingFountain);
		break;
	case SPRITE_MISC_JUMPING_FOUNTAIN_SNOW:
		if (jumpingFountain->var_26b == 16)
			jumping_fountain_continue(jumpingFountain);
		break;
	}

	if (jumpingFountain->var_26b == 16)
		sprite_remove((rct_sprite*)jumpingFountain);
}

/**
 *
 *  rct2: 0x006739DE (water)
 *  rct2: 0x00673BCC (snow)
 */
static void jumping_fountain_continue(rct_jumping_fountain *jumpingFountain)
{
	int direction = (jumpingFountain->sprite_direction >> 3) & 7;
	int x = jumpingFountain->x + TileDirectionDelta[direction].x;
	int y = jumpingFountain->y + TileDirectionDelta[direction].y;
	int z = jumpingFountain->z;

	int type = jumpingFountain->misc_identifier == SPRITE_MISC_JUMPING_FOUNTAIN_SNOW ?
		JUMPING_FOUNTAIN_TYPE_SNOW :
		JUMPING_FOUNTAIN_TYPE_WATER;

	int availableDirections = 0;
	for (int i = 0; i < 8; i++) {
		if (is_jumping_fountain(type, x + dword_97F000[i].x, y + dword_97F000[i].y, z))
			availableDirections |= 1 << i;
	}

	if (availableDirections == 0)
		return;

	if (jumpingFountain->flags & FOUNTAIN_FLAG_TERMINATE)
		return;

	if (jumpingFountain->flags & FOUNTAIN_FLAG_GOTO_EDGE) {
		jumping_fountain_goto_edge(jumpingFountain, x, y, z, availableDirections);
		return;
	}

	if (jumpingFountain->flags & FOUNTAIN_FLAG_BOUNCE) {
		jumping_fountain_bounce(jumpingFountain, x, y, z, availableDirections);
		return;
	}

	if (jumpingFountain->flags & FOUNTAIN_FLAG_SPLIT) {
		jumping_fountain_split(jumpingFountain, x, y, z, availableDirections);
		return;
	}
	
	jumping_fountain_random(jumpingFountain, x, y, z, availableDirections);
}

static bool is_jumping_fountain(int type, int x, int y, int z)
{
	z = z >> 3;

	int pathBitFlagMask = type == JUMPING_FOUNTAIN_TYPE_SNOW ?
		PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW :
		PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER;

	rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;
		if (mapElement->base_height != z)
			continue;
		if (mapElement->properties.path.additions & 0x80)
			continue;

		int additions = mapElement->properties.path.additions & 0x0F;
		if (additions == 0)
			continue;

		rct_scenery_entry *sceneryEntry = g_pathBitSceneryEntries[additions - 1];
		if (!(sceneryEntry->path_bit.var_06 & pathBitFlagMask))
			continue;

		return true;
	} while (!map_element_is_last_for_tile(mapElement++));
	
	return false;
}

/**
 *
 *  rct: 0x00673B6E
 */
static void jumping_fountain_goto_edge(rct_jumping_fountain *jumpingFountain, int x, int y, int z, int availableDirections)
{
	int direction = (jumpingFountain->sprite_direction >> 3) << 1;
	if (availableDirections & (1 << direction)) {
		jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
		return;
	}

	direction++;
	if (availableDirections & (1 << direction)) {
		jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
		return;
	}

	uint32 randomIndex = scenario_rand();
	if ((randomIndex & 0xFFFF) < 0x3333)
		return;

	if (jumpingFountain->flags & FOUNTAIN_FLAG_SPLIT) {
		jumping_fountain_split(jumpingFountain, x, y, z, availableDirections);
		return;
	}
		
	direction = randomIndex & 7;
	while (!(availableDirections & (1 << direction)))
		direction = (direction + 1) & 7;

	jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
}

/**
 *
 *  rct: 0x00673B45
 */
static void jumping_fountain_bounce(rct_jumping_fountain *jumpingFountain, int x, int y, int z, int availableDirections)
{
	jumpingFountain->iteration++;
	if (jumpingFountain->iteration >= 8)
		return;

	int direction = ((jumpingFountain->sprite_direction >> 3) ^ 2) << 1;
	if (availableDirections & (1 << direction)) {
		jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
		return;
	}

	direction++;
	if (availableDirections & (1 << direction))
		jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
}

/**
 *
 *  rct: 0x00673ACE
 */
static void jumping_fountain_split(rct_jumping_fountain *jumpingFountain, int x, int y, int z, int availableDirections)
{
	if (jumpingFountain->iteration >= 3)
		return;

	int type = jumpingFountain->misc_identifier == SPRITE_MISC_JUMPING_FOUNTAIN_SNOW ?
		JUMPING_FOUNTAIN_TYPE_SNOW :
		JUMPING_FOUNTAIN_TYPE_WATER;

	int direction = ((jumpingFountain->sprite_direction >> 3) ^ 2) << 1;
	availableDirections &= ~(1 << direction);
	direction++;
	availableDirections &= ~(1 << direction);

	for (direction = 0; direction < 8; direction++) {
		if (availableDirections & (1 << direction)) {
			jumping_fountain_create(
				type,
				x, y, z,
				direction >> 1,
				jumpingFountain->flags & ~FOUNTAIN_FLAG_7,
				jumpingFountain->iteration + 1
			);
		}
		direction++;
		if (availableDirections & (1 << direction)) {
			jumping_fountain_create(
				type,
				x, y, z,
				direction >> 1,
				jumpingFountain->flags | FOUNTAIN_FLAG_7,
				jumpingFountain->iteration + 1
			);
		}
	}
}

/**
 *
 *  rct: 0x00673AAC
 */
static void jumping_fountain_random(rct_jumping_fountain *jumpingFountain, int x, int y, int z, int availableDirections)
{
	uint32 randomIndex = scenario_rand();
	if ((randomIndex & 0xFFFF) < 0x2000)
		return;

	int direction = randomIndex & 7;
	while (!(availableDirections & (1 << direction)))
		direction = (direction + 1) & 7;

	jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
}

/**
 *
 *  rct: 0x00673B45
 */
static void jumping_fountain_create_next(rct_jumping_fountain *jumpingFountain, int x, int y, int z, int direction)
{
	int flags = jumpingFountain->flags & ~FOUNTAIN_FLAG_7;
	if (direction & 1)
		flags |= FOUNTAIN_FLAG_7;

	int type = jumpingFountain->misc_identifier == SPRITE_MISC_JUMPING_FOUNTAIN_SNOW ?
		JUMPING_FOUNTAIN_TYPE_SNOW :
		JUMPING_FOUNTAIN_TYPE_WATER;

	jumping_fountain_create(type, x, y, z, direction >> 1, flags, jumpingFountain->iteration);
}