#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../game.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../ride/track.h"
#include "../interface/viewport.h"
#include "map_animation.h"
#include "map.h"
#include "scenery.h"
#include "sprite.h"

typedef bool (*map_animation_invalidate_event_handler)(sint32 x, sint32 y, sint32 baseZ);

static bool map_animation_invalidate(rct_map_animation *obj);

static const map_animation_invalidate_event_handler _animatedObjectEventHandlers[MAP_ANIMATION_TYPE_COUNT];

uint16 gNumMapAnimations;
rct_map_animation gAnimatedObjects[MAX_ANIMATED_OBJECTS];

/**
 *
 *  rct2: 0x0068AF67
 *
 * @param type (dh)
 * @param x (ax)
 * @param y (cx)
 * @param z (dl)
 */
void map_animation_create(sint32 type, sint32 x, sint32 y, sint32 z)
{
    rct_map_animation *aobj = &gAnimatedObjects[0];
    sint32 numAnimatedObjects = gNumMapAnimations;
    if (numAnimatedObjects >= MAX_ANIMATED_OBJECTS) {
        log_error("Exceeded the maximum number of animations");
        return;
    }
    for (sint32 i = 0; i < numAnimatedObjects; i++, aobj++) {
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
    gNumMapAnimations++;
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
    sint32 numAnimatedObjects = gNumMapAnimations;
    while (numAnimatedObjects > 0) {
        if (map_animation_invalidate(aobj)) {
            // Remove animated object
            gNumMapAnimations--;
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
static bool map_animation_invalidate_ride_entrance(sint32 x, sint32 y, sint32 baseZ)
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

        ride = get_ride(mapElement->properties.entrance.ride_index);
        entranceDefinition = &RideEntranceDefinitions[ride->entrance_style];

        sint32 height = (mapElement->base_height * 8) + entranceDefinition->height + 8;
        map_invalidate_tile_zoom1(x, y, height, height + 16);
        return false;
    } while (!map_element_is_last_for_tile(mapElement++));

    return true;
}

/**
 *
 *  rct2: 0x006A7BD4
 */
static bool map_animation_invalidate_queue_banner(sint32 x, sint32 y, sint32 baseZ)
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

        sint32 direction = ((mapElement->type >> 6) + get_current_rotation()) & 3;
        if (direction == MAP_ELEMENT_DIRECTION_NORTH || direction == MAP_ELEMENT_DIRECTION_EAST) {
            baseZ = mapElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, baseZ + 16, baseZ + 30);
        }
        return false;
    } while (!map_element_is_last_for_tile(mapElement++));

    return true;
}

/**
 *
 *  rct2: 0x006E32C9
 */
static bool map_animation_invalidate_small_scenery(sint32 x, sint32 y, sint32 baseZ)
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

        sceneryEntry = get_small_scenery_entry(mapElement->properties.scenery.type);
        if (sceneryEntry->small_scenery.flags & (SMALL_SCENERY_FLAG12 | SMALL_SCENERY_FLAG13 | SMALL_SCENERY_FLAG15 | SMALL_SCENERY_FLAG16)) {
            map_invalidate_tile_zoom1(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
            return false;
        }

        if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_IS_CLOCK) {
            // Peep, looking at scenery
            if (!(gCurrentTicks & 0x3FF) && game_is_not_paused()) {
                sint32 direction = mapElement->type & 3;
                sint32 x2 = x - TileDirectionDelta[direction].x;
                sint32 y2 = y - TileDirectionDelta[direction].y;

                uint16 spriteIdx = sprite_get_first_in_quadrant(x2, y2);
                for (; spriteIdx != SPRITE_INDEX_NULL; spriteIdx = sprite->unknown.next_in_quadrant) {
                    sprite = get_sprite(spriteIdx);
                    if (sprite->unknown.linked_list_type_offset != SPRITE_LIST_PEEP * 2)
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
                    peep_update_current_action_sprite_type(peep);
                    invalidate_sprite_1((rct_sprite*)peep);
                    break;
                }
            }
            map_invalidate_tile_zoom1(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
            return false;
        }
    } while (!map_element_is_last_for_tile(mapElement++));
    return true;
}

/**
 *
 *  rct2: 0x00666C63
 */
static bool map_animation_invalidate_park_entrance(sint32 x, sint32 y, sint32 baseZ)
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
        map_invalidate_tile_zoom1(x, y, baseZ + 32, baseZ + 64);
        return false;
    } while (!map_element_is_last_for_tile(mapElement++));

    return true;
}

/**
 *
 *  rct2: 0x006CE29E
 */
static bool map_animation_invalidate_track_waterfall(sint32 x, sint32 y, sint32 baseZ)
{
    rct_map_element *mapElement;

    mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (mapElement->base_height != baseZ)
            continue;
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
            continue;

        if (mapElement->properties.track.type == TRACK_ELEM_WATERFALL) {
            sint32 z = mapElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z + 14, z + 46);
            return false;
        }
    } while (!map_element_is_last_for_tile(mapElement++));

    return true;
}

/**
 *
 *  rct2: 0x006CE2F3
 */
static bool map_animation_invalidate_track_rapids(sint32 x, sint32 y, sint32 baseZ)
{
    rct_map_element *mapElement;

    mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (mapElement->base_height != baseZ)
            continue;
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
            continue;

        if (mapElement->properties.track.type == TRACK_ELEM_RAPIDS) {
            sint32 z = mapElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z + 14, z + 18);
            return false;
        }
    } while (!map_element_is_last_for_tile(mapElement++));

    return true;
}

/**
 *
 *  rct2: 0x006CE39D
 */
static bool map_animation_invalidate_track_onridephoto(sint32 x, sint32 y, sint32 baseZ)
{
    rct_map_element *mapElement;

    mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (mapElement->base_height != baseZ)
            continue;
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
            continue;

        if (mapElement->properties.track.type == TRACK_ELEM_ON_RIDE_PHOTO) {
            map_invalidate_tile_zoom1(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
            if (game_is_paused()) {
                return false;
            }
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
static bool map_animation_invalidate_track_whirlpool(sint32 x, sint32 y, sint32 baseZ)
{
    rct_map_element *mapElement;

    mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (mapElement->base_height != baseZ)
            continue;
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
            continue;

        if (mapElement->properties.track.type == TRACK_ELEM_WHIRLPOOL) {
            sint32 z = mapElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z + 14, z + 18);
            return false;
        }
    } while (!map_element_is_last_for_tile(mapElement++));

    return true;
}

/**
 *
 *  rct2: 0x006CE3FA
 */
static bool map_animation_invalidate_track_spinningtunnel(sint32 x, sint32 y, sint32 baseZ)
{
    rct_map_element *mapElement;

    mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (mapElement->base_height != baseZ)
            continue;
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
            continue;

        if (mapElement->properties.track.type == TRACK_ELEM_SPINNING_TUNNEL) {
            sint32 z = mapElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z + 14, z + 32);
            return false;
        }
    } while (!map_element_is_last_for_tile(mapElement++));

    return true;
}

/**
 *
 *  rct2: 0x0068DF8F
 */
static bool map_animation_invalidate_remove(sint32 x, sint32 y, sint32 baseZ)
{
    return true;
}

/**
 *
 *  rct2: 0x006BA2BB
 */
static bool map_animation_invalidate_banner(sint32 x, sint32 y, sint32 baseZ)
{
    rct_map_element *mapElement;

    mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (mapElement->base_height != baseZ)
            continue;
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_BANNER)
            continue;

        baseZ = mapElement->base_height * 8;
        map_invalidate_tile_zoom1(x, y, baseZ, baseZ + 16);
        return false;
    } while (!map_element_is_last_for_tile(mapElement++));

    return true;
}

/**
 *
 *  rct2: 0x006B94EB
 */
static bool map_animation_invalidate_large_scenery(sint32 x, sint32 y, sint32 baseZ)
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

        sceneryEntry = get_large_scenery_entry(mapElement->properties.scenery.type & 0x3FF);
        if (sceneryEntry->large_scenery.flags & LARGE_SCENERY_FLAG_ANIMATED) {
            sint32 z = mapElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z, z + 16);
            wasInvalidated = true;
        }
    } while (!map_element_is_last_for_tile(mapElement++));

    return !wasInvalidated;
}

/**
 *
 *  rct2: 0x006E5B50
 */
static bool map_animation_invalidate_wall_door(sint32 x, sint32 y, sint32 baseZ)
{
    rct_map_element *mapElement;
    rct_scenery_entry *sceneryEntry;

    if (gCurrentTicks & 1)
        return false;

    bool removeAnimation = true;
    mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (mapElement->base_height != baseZ)
            continue;
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_WALL)
            continue;

        sceneryEntry = get_wall_entry(mapElement->properties.scenery.type);
        if (!(sceneryEntry->wall.flags & WALL_SCENERY_IS_DOOR))
            continue;

        if (game_is_paused()) {
            return false;
        }

        bool invalidate = false;

        uint8 currentFrame = wall_element_get_animation_frame(mapElement);
        if (currentFrame != 0) {
            if (currentFrame == 15) {
                currentFrame = 0;
            } else {
                removeAnimation = false;
                if (currentFrame != 5) {
                    currentFrame++;
                    if (currentFrame == 13 && !(sceneryEntry->wall.flags & WALL_SCENERY_FLAG6))
                        currentFrame = 15;

                    invalidate = true;
                }
            }
        }
        wall_element_set_animation_frame(mapElement, currentFrame);
        if (invalidate) {
            sint32 z = mapElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z, z + 32);
        }
    } while (!map_element_is_last_for_tile(mapElement++));

    return removeAnimation;
}

/**
 *
 *  rct2: 0x006E5EE4
 */
static bool map_animation_invalidate_wall(sint32 x, sint32 y, sint32 baseZ)
{
    rct_map_element *mapElement;
    rct_scenery_entry *sceneryEntry;

    bool wasInvalidated = false;
    mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (mapElement->base_height != baseZ)
            continue;
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_WALL)
            continue;

        sceneryEntry = get_wall_entry(mapElement->properties.scenery.type);
        if (!(sceneryEntry->wall.flags2 & WALL_SCENERY_2_FLAG5) && sceneryEntry->wall.scrolling_mode == 255)
            continue;

        sint32 z = mapElement->base_height * 8;
        map_invalidate_tile_zoom1(x, y, z, z + 16);
        wasInvalidated = true;
    } while (!map_element_is_last_for_tile(mapElement++));

    return !wasInvalidated;
}

/**
 *
 *  rct2: 0x009819DC
 */
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
    map_animation_invalidate_wall_door,
    map_animation_invalidate_wall
};
