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
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../ride/track.h"
#include "map_animation.h"
#include "map.h"
#include "scenery.h"
#include "sprite.h"

typedef bool (*map_animation_invalidate_event_handler)(int x, int y, int baseZ);

static bool map_animation_invalidate(rct_map_animation *obj);

static const map_animation_invalidate_event_handler _animatedObjectEventHandlers[MAP_ANIMATION_TYPE_COUNT];

rct_map_animation *gAnimatedObjects = (rct_map_animation*)0x013886A0;

/**
 * 
 *  rct2: 0x0068AF67
 *
 * @param type (dh)
 * @param x (ax)
 * @param y (cx)
 * @param z (dl)
 */
void map_animation_create(int type, int x, int y, int z)
{
	rct_map_animation *aobj = &gAnimatedObjects[0];
	int numAnimatedObjects = RCT2_GLOBAL(0x0138B580, uint16);
	for (int i = 0; i < numAnimatedObjects; i++, aobj++) {
		if (aobj->x != x)
			continue;
		if (aobj->y != y)
			continue;
		if (aobj->baseZ != z)
			continue;
		if (aobj->type != type)
			continue;
		// Animation already exists
		return;
	}

	// Create new animation
	RCT2_GLOBAL(0x0138B580, uint16)++;
	aobj->type = type;
	aobj->x = x;
	aobj->y = y;
	aobj->baseZ = z;
}

/**
 *
 *  rct2: 0x0068AFAD
 */
void map_animation_invalidate_all()
{
	rct_map_animation *aobj = &gAnimatedObjects[0];
	int numAnimatedObjects = RCT2_GLOBAL(0x0138B580, uint16);
	while (numAnimatedObjects > 0) {
		if (map_animation_invalidate(aobj)) {
			// Remove animated object
			RCT2_GLOBAL(0x0138B580, uint16)--;
			numAnimatedObjects--;
			if (numAnimatedObjects > 0)
				memmove(aobj, aobj + 1, numAnimatedObjects * sizeof(rct_map_animation));
		} else {
			numAnimatedObjects--;
			aobj++;
		}
	}
}

/**
 * @returns true if the animation should be removed.
 */
static bool map_animation_invalidate(rct_map_animation *obj)
{
	assert(obj->type < MAP_ANIMATION_TYPE_COUNT);

	return _animatedObjectEventHandlers[obj->type](obj->x, obj->y, obj->baseZ);
}

/**
 *
 *  rct2: 0x00666670
 */
static bool map_animation_invalidate_ride_entrance(int x, int y, int baseZ)
{
	rct_map_element *mapElement;
	rct_ride *ride;
	const rct_ride_entrance_definition *entranceDefinition;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE)
			continue;
		if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE)
			continue;

		ride = GET_RIDE(mapElement->properties.entrance.ride_index);
		entranceDefinition = &RideEntranceDefinitions[ride->entrance_style];

		int height = (mapElement->base_height * 8) + entranceDefinition->height + 8;
		map_invalidate_tile(x, y, height, height + 16);
		return false;
	} while (!map_element_is_last_for_tile(mapElement++));

	return true;
}

/**
 *
 *  rct2: 0x006A7BD4
 */
static bool map_animation_invalidate_queue_banner(int x, int y, int baseZ)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;
		if (!(mapElement->flags & 1))
			continue;
		if (!(mapElement->properties.path.type & PATH_FLAG_QUEUE_BANNER))
			continue;

		int direction = ((mapElement->type >> 6) + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)) & 3;
		if (direction == MAP_ELEMENT_DIRECTION_NORTH || direction == MAP_ELEMENT_DIRECTION_EAST) {
			baseZ = mapElement->base_height * 8;
			map_invalidate_tile(x, y, baseZ + 16, baseZ + 30);
		}
		return false;
	} while (!map_element_is_last_for_tile(mapElement++));

	return true;
}

/**
 *
 *  rct2: 0x006E32C9
 */
static bool map_animation_invalidate_small_scenery(int x, int y, int baseZ)
{
	rct_map_element *mapElement;
	rct_scenery_entry *sceneryEntry;
	rct_sprite *sprite;
	rct_peep *peep;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_SCENERY)
			continue;
		if (mapElement->flags & (1 << 4))
			continue;

		sceneryEntry = g_smallSceneryEntries[mapElement->properties.scenery.type];
		if (sceneryEntry->small_scenery.flags & 0xD800) {
			map_invalidate_tile(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
			return false;
		}

		if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_IS_CLOCK) {
			// Peep, looking at scenery
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x3FF)) {
				int direction = mapElement->type & 3;
				int x2 = x - TileDirectionDelta[direction].x;
				int y2 = y - TileDirectionDelta[direction].y;

				uint16 spriteIdx = RCT2_ADDRESS(0x00F1EF60, uint16)[((x2 & 0x1FE0) << 3) | (y2 >> 5)];
				for (; spriteIdx != 0xFFFF; spriteIdx = sprite->unknown.next_in_quadrant) {
					sprite = &g_sprite_list[spriteIdx];
					if (sprite->unknown.linked_list_type_offset != SPRITE_LINKEDLIST_OFFSET_PEEP)
						continue;

					peep = &sprite->peep;
					if (peep->state != PEEP_STATE_WALKING)
						continue;
					if (peep->z != mapElement->base_height * 8)
						continue;
					if (peep->action < PEEP_ACTION_NONE_1)
						continue;

					peep->action = PEEP_ACTION_CHECK_TIME;
					peep->action_frame = 0;
					peep->action_sprite_image_offset = 0;
					sub_693B58(peep);
					sub_6EC53F((rct_sprite*)peep);
					break;
				}
			}
			map_invalidate_tile(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
			return false;
		}
	} while (!map_element_is_last_for_tile(mapElement++));
	return true;
}

/**
 *
 *  rct2: 0x00666C63
 */
static bool map_animation_invalidate_park_entrance(int x, int y, int baseZ)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE)
			continue;
		if (mapElement->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE)
			continue;
		if (mapElement->properties.entrance.index & 0x0F)
			continue;

		baseZ = mapElement->base_height * 8;
		map_invalidate_tile(x, y, baseZ + 32, baseZ + 64);
		return false;
	} while (!map_element_is_last_for_tile(mapElement++));

	return true;
}

/**
 *
 *  rct2: 0x006CE29E
 */
static bool map_animation_invalidate_track_waterfall(int x, int y, int baseZ)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if (mapElement->properties.track.type == TRACK_ELEM_WATERFALL) {
			int z = mapElement->base_height * 8;
			map_invalidate_tile(x, y, z + 14, z + 46);
			return false;
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	return true;
}

/**
 *
 *  rct2: 0x006CE2F3
 */
static bool map_animation_invalidate_track_rapids(int x, int y, int baseZ)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if (mapElement->properties.track.type == TRACK_ELEM_RAPIDS) {
			int z = mapElement->base_height * 8;
			map_invalidate_tile(x, y, z + 14, z + 18);
			return false;
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	return true;
}

/**
 *
 *  rct2: 0x006CE39D
 */
static bool map_animation_invalidate_track_onridephoto(int x, int y, int baseZ)
{
	rct_map_element *mapElement;

	bool wasInvalidated = false;
	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if (mapElement->properties.track.type == TRACK_ELEM_ON_RIDE_PHOTO) {
			int z = mapElement->base_height * 8;
			map_invalidate_tile(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
			if (mapElement->properties.track.sequence & 0xF0) {
				mapElement->properties.track.sequence -= 0x10;
				return false;
			} else {
				return true;
			}
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	return true;
}

/**
 *
 *  rct2: 0x006CE348
 */
static bool map_animation_invalidate_track_whirlpool(int x, int y, int baseZ)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if (mapElement->properties.track.type == TRACK_ELEM_WHIRLPOOL) {
			int z = mapElement->base_height * 8;
			map_invalidate_tile(x, y, z + 14, z + 18);
			return false;
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	return true;
}

/**
 *
 *  rct2: 0x006CE3FA
 */
static bool map_animation_invalidate_track_spinningtunnel(int x, int y, int baseZ)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if (mapElement->properties.track.type == TRACK_ELEM_SPINNING_TUNNEL) {
			int z = mapElement->base_height * 8;
			map_invalidate_tile(x, y, z + 14, z + 32);
			return false;
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	return true;
}

/**
 *
 *  rct2: 0x0068DF8F
 */
static bool map_animation_invalidate_remove(int x, int y, int baseZ)
{
	return true;
}

/**
 *
 *  rct2: 0x006BA2BB
 */
static bool map_animation_invalidate_banner(int x, int y, int baseZ)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_BANNER)
			continue;
		
		baseZ = mapElement->base_height * 8;
		map_invalidate_tile(x, y, baseZ, baseZ + 16);
		return false;
	} while (!map_element_is_last_for_tile(mapElement++));

	return true;
}

/**
 *
 *  rct2: 0x006B94EB
 */
static bool map_animation_invalidate_large_scenery(int x, int y, int baseZ)
{
	rct_map_element *mapElement;
	rct_scenery_entry *sceneryEntry;

	bool wasInvalidated = false;
	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_SCENERY_MULTIPLE)
			continue;

		sceneryEntry = g_largeSceneryEntries[mapElement->properties.scenery.type & 0x3FF];
		if (sceneryEntry->large_scenery.flags & (1 << 3)) {
			int z = mapElement->base_height * 8;
			map_invalidate_tile(x, y, z, z + 16);
			wasInvalidated = true;
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	return !wasInvalidated;
}

/**
 *
 *  rct2: 0x006E5B50
 */
static bool map_animation_invalidate_wall_unknown(int x, int y, int baseZ)
{
	rct_map_element *mapElement;
	rct_scenery_entry *sceneryEntry;

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 1)
		return false;

	bool wasInvalidated = false;
	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_FENCE)
			continue;

		sceneryEntry = g_wallSceneryEntries[mapElement->properties.scenery.type];
		if (!(sceneryEntry->wall.flags & (1 << 4)))
			continue;

		uint8 di = 0;
		uint8 bl = mapElement->properties.fence.item[2];
		uint8 bh = bl & 0x78;
		if (bh != 0) {
			if (bh == 0x78) {
				bl &= 0x87;
			} else {
				di |= 2;
				if (bh != 40) {
					bh += 8;
					if (bh == 104 && !(sceneryEntry->wall.flags & (1 << 5)))
						bh = 120;

					di |= 1;
					bl &= 135;
					bl |= bh;
				}
			}
		}

		mapElement->properties.fence.item[2] = bl;
		if (di & 1) {
			int z = mapElement->base_height * 8;
			map_invalidate_tile(x, y, z, z + 32);
		}
		if (di & 2)
			wasInvalidated = true;
	} while (!map_element_is_last_for_tile(mapElement++));

	return !wasInvalidated;
}

/**
 *
 *  rct2: 0x006E5EE4
 */
static bool map_animation_invalidate_wall(int x, int y, int baseZ)
{
	rct_map_element *mapElement;
	rct_scenery_entry *sceneryEntry;

	bool wasInvalidated = false;
	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (mapElement->base_height != baseZ)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_FENCE)
			continue;

		sceneryEntry = g_wallSceneryEntries[mapElement->properties.scenery.type];
		if (!(sceneryEntry->wall.flags2 & (1 << 4)) && sceneryEntry->wall.var_0D == 255)
			continue;

		int z = mapElement->base_height * 8;
		map_invalidate_tile(x, y, z, z + 16);
		wasInvalidated = true;
	} while (!map_element_is_last_for_tile(mapElement++));

	return !wasInvalidated;
}

/** rct2: 0x009819DC */
static const map_animation_invalidate_event_handler _animatedObjectEventHandlers[MAP_ANIMATION_TYPE_COUNT] = {
	map_animation_invalidate_ride_entrance,
	map_animation_invalidate_queue_banner,
	map_animation_invalidate_small_scenery,
	map_animation_invalidate_park_entrance,
	map_animation_invalidate_track_waterfall,
	map_animation_invalidate_track_rapids,
	map_animation_invalidate_track_onridephoto,
	map_animation_invalidate_track_whirlpool,
	map_animation_invalidate_track_spinningtunnel,
	map_animation_invalidate_remove,
	map_animation_invalidate_banner,
	map_animation_invalidate_large_scenery,
	map_animation_invalidate_wall_unknown,
	map_animation_invalidate_wall
};