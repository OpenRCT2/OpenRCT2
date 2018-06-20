/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cstring>

#include "../Game.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../interface/Viewport.h"
#include "../world/Wall.h"
#include "MapAnimation.h"
#include "Map.h"
#include "Scenery.h"
#include "SmallScenery.h"
#include "Sprite.h"
#include "Footpath.h"

using map_animation_invalidate_event_handler = bool (*)(int32_t x, int32_t y, int32_t baseZ);

static bool map_animation_invalidate(rct_map_animation *obj);

uint16_t gNumMapAnimations;
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
void map_animation_create(int32_t type, int32_t x, int32_t y, int32_t z)
{
    rct_map_animation *aobj = &gAnimatedObjects[0];
    int32_t numAnimatedObjects = gNumMapAnimations;
    if (numAnimatedObjects >= MAX_ANIMATED_OBJECTS) {
        log_error("Exceeded the maximum number of animations");
        return;
    }
    for (int32_t i = 0; i < numAnimatedObjects; i++, aobj++) {
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
    int32_t numAnimatedObjects = gNumMapAnimations;
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
 *
 *  rct2: 0x00666670
 */
static bool map_animation_invalidate_ride_entrance(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;
    Ride *ride;
    const rct_ride_entrance_definition *entranceDefinition;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
            continue;
        if (tileElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE)
            continue;

        ride = get_ride(tileElement->properties.entrance.ride_index);
        entranceDefinition = &RideEntranceDefinitions[ride->entrance_style];

        int32_t height = (tileElement->base_height * 8) + entranceDefinition->height + 8;
        map_invalidate_tile_zoom1(x, y, height, height + 16);
        return false;
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006A7BD4
 */
static bool map_animation_invalidate_queue_banner(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (!(tileElement->flags & 1))
            continue;
        if (!footpath_element_has_queue_banner(tileElement))
            continue;

        int32_t direction = (footpath_element_get_direction(tileElement) + get_current_rotation()) & 3;
        if (direction == TILE_ELEMENT_DIRECTION_NORTH || direction == TILE_ELEMENT_DIRECTION_EAST) {
            baseZ = tileElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, baseZ + 16, baseZ + 30);
        }
        return false;
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006E32C9
 */
static bool map_animation_invalidate_small_scenery(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;
    rct_scenery_entry *sceneryEntry;
    rct_sprite *sprite;
    rct_peep *peep;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
            continue;
        if (tileElement->flags & (1 << 4))
            continue;

        sceneryEntry = get_small_scenery_entry(tileElement->properties.scenery.type);
        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1 | SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4 | SMALL_SCENERY_FLAG_SWAMP_GOO | SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
        {
            map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
            return false;
        }

        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_IS_CLOCK))
        {
            // Peep, looking at scenery
            if (!(gCurrentTicks & 0x3FF) && game_is_not_paused()) {
                int32_t direction = tile_element_get_direction(tileElement);
                int32_t x2 = x - CoordsDirectionDelta[direction].x;
                int32_t y2 = y - CoordsDirectionDelta[direction].y;

                uint16_t spriteIdx = sprite_get_first_in_quadrant(x2, y2);
                for (; spriteIdx != SPRITE_INDEX_NULL; spriteIdx = sprite->unknown.next_in_quadrant) {
                    sprite = get_sprite(spriteIdx);
                    if (sprite->unknown.linked_list_type_offset != SPRITE_LIST_PEEP * 2)
                        continue;

                    peep = &sprite->peep;
                    if (peep->state != PEEP_STATE_WALKING)
                        continue;
                    if (peep->z != tileElement->base_height * 8)
                        continue;
                    if (peep->action < PEEP_ACTION_NONE_1)
                        continue;

                    peep->action = PEEP_ACTION_CHECK_TIME;
                    peep->action_frame = 0;
                    peep->action_sprite_image_offset = 0;
                    peep->UpdateCurrentActionSpriteType();
                    invalidate_sprite_1((rct_sprite*)peep);
                    break;
                }
            }
            map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
            return false;
        }

    } while (!(tileElement++)->IsLastForTile());
    return true;
}

/**
 *
 *  rct2: 0x00666C63
 */
static bool map_animation_invalidate_park_entrance(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
            continue;
        if (tileElement->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE)
            continue;
        if (tileElement->properties.entrance.index & 0x0F)
            continue;

        baseZ = tileElement->base_height * 8;
        map_invalidate_tile_zoom1(x, y, baseZ + 32, baseZ + 64);
        return false;
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006CE29E
 */
static bool map_animation_invalidate_track_waterfall(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (track_element_get_type(tileElement) == TRACK_ELEM_WATERFALL) {
            int32_t z = tileElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z + 14, z + 46);
            return false;
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006CE2F3
 */
static bool map_animation_invalidate_track_rapids(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (track_element_get_type(tileElement) == TRACK_ELEM_RAPIDS) {
            int32_t z = tileElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z + 14, z + 18);
            return false;
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006CE39D
 */
static bool map_animation_invalidate_track_onridephoto(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement == nullptr)
            break;
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (track_element_get_type(tileElement) == TRACK_ELEM_ON_RIDE_PHOTO) {
            map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
            if (game_is_paused()) {
                return false;
            }
            if (tile_element_is_taking_photo(tileElement)) {
                tile_element_decrement_onride_photo_timout(tileElement);
                return false;
            } else {
                return true;
            }
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006CE348
 */
static bool map_animation_invalidate_track_whirlpool(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (track_element_get_type(tileElement) == TRACK_ELEM_WHIRLPOOL) {
            int32_t z = tileElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z + 14, z + 18);
            return false;
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006CE3FA
 */
static bool map_animation_invalidate_track_spinningtunnel(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (track_element_get_type(tileElement) == TRACK_ELEM_SPINNING_TUNNEL) {
            int32_t z = tileElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z + 14, z + 32);
            return false;
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x0068DF8F
 */
static bool map_animation_invalidate_remove([[maybe_unused]] int32_t x, [[maybe_unused]] int32_t y, [[maybe_unused]] int32_t baseZ)
{
    return true;
}

/**
 *
 *  rct2: 0x006BA2BB
 */
static bool map_animation_invalidate_banner(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_BANNER)
            continue;

        baseZ = tileElement->base_height * 8;
        map_invalidate_tile_zoom1(x, y, baseZ, baseZ + 16);
        return false;
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006B94EB
 */
static bool map_animation_invalidate_large_scenery(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;
    rct_scenery_entry *sceneryEntry;

    bool wasInvalidated = false;
    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_LARGE_SCENERY)
            continue;

        sceneryEntry = get_large_scenery_entry(tileElement->properties.scenery.type & 0x3FF);
        if (sceneryEntry->large_scenery.flags & LARGE_SCENERY_FLAG_ANIMATED) {
            int32_t z = tileElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z, z + 16);
            wasInvalidated = true;
        }
    } while (!(tileElement++)->IsLastForTile());

    return !wasInvalidated;
}

/**
 *
 *  rct2: 0x006E5B50
 */
static bool map_animation_invalidate_wall_door(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;
    rct_scenery_entry *sceneryEntry;

    if (gCurrentTicks & 1)
        return false;

    bool removeAnimation = true;
    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;

        sceneryEntry = get_wall_entry(tileElement->properties.scenery.type);
        if (!(sceneryEntry->wall.flags & WALL_SCENERY_IS_DOOR))
            continue;

        if (game_is_paused()) {
            return false;
        }

        bool invalidate = false;

        uint8_t currentFrame = wall_get_animation_frame(tileElement);
        if (currentFrame != 0) {
            if (currentFrame == 15) {
                currentFrame = 0;
            } else {
                removeAnimation = false;
                if (currentFrame != 5) {
                    currentFrame++;
                    if (currentFrame == 13 && !(sceneryEntry->wall.flags & WALL_SCENERY_LONG_DOOR_ANIMATION))
                        currentFrame = 15;

                    invalidate = true;
                }
            }
        }
        wall_set_animation_frame(tileElement, currentFrame);
        if (invalidate) {
            int32_t z = tileElement->base_height * 8;
            map_invalidate_tile_zoom1(x, y, z, z + 32);
        }
    } while (!(tileElement++)->IsLastForTile());

    return removeAnimation;
}

/**
 *
 *  rct2: 0x006E5EE4
 */
static bool map_animation_invalidate_wall(int32_t x, int32_t y, int32_t baseZ)
{
    rct_tile_element *tileElement;
    rct_scenery_entry *sceneryEntry;

    bool wasInvalidated = false;
    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;

        sceneryEntry = get_wall_entry(tileElement->properties.scenery.type);

        if (!(sceneryEntry->wall.flags2 & WALL_SCENERY_2_ANIMATED) && sceneryEntry->wall.scrolling_mode == 255)
            continue;

        int32_t z = tileElement->base_height * 8;
        map_invalidate_tile_zoom1(x, y, z, z + 16);
        wasInvalidated = true;
    } while (!(tileElement++)->IsLastForTile());

    return !wasInvalidated;
}

/**
 *
 *  rct2: 0x009819DC
 */
static constexpr const map_animation_invalidate_event_handler _animatedObjectEventHandlers[MAP_ANIMATION_TYPE_COUNT] = {
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

/**
 * @returns true if the animation should be removed.
 */
static bool map_animation_invalidate(rct_map_animation *obj)
{
    assert(obj->type < MAP_ANIMATION_TYPE_COUNT);
    
    return _animatedObjectEventHandlers[obj->type](obj->x, obj->y, obj->baseZ);
}
