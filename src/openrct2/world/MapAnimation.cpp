/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapAnimation.h"

#include "../Context.h"
#include "../Game.h"
#include "../interface/Viewport.h"
#include "../object/StationObject.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../world/Wall.h"
#include "Banner.h"
#include "Footpath.h"
#include "LargeScenery.h"
#include "Map.h"
#include "Scenery.h"
#include "SmallScenery.h"
#include "Sprite.h"

using map_animation_invalidate_event_handler = bool (*)(int32_t x, int32_t y, int32_t baseZ);

static std::vector<MapAnimation> _mapAnimations;

constexpr size_t MAX_ANIMATED_OBJECTS = 2000;

static bool InvalidateMapAnimation(const MapAnimation& obj);

static bool DoesAnimationExist(int32_t type, const CoordsXYZ& location)
{
    for (const auto& a : _mapAnimations)
    {
        if (a.type == type && a.location.x == location.x && a.location.y == location.y && a.location.z == location.z)
        {
            // Animation already exists
            return true;
        }
    }
    return false;
}

void map_animation_create(int32_t type, int32_t x, int32_t y, int32_t z)
{
    auto location = CoordsXYZ{ x, y, z };
    if (!DoesAnimationExist(type, location))
    {
        if (_mapAnimations.size() < MAX_ANIMATED_OBJECTS)
        {
            // Create new animation
            _mapAnimations.push_back({ (uint8_t)type, location });
        }
        else
        {
            log_error("Exceeded the maximum number of animations");
        }
    }
}

/**
 *
 *  rct2: 0x0068AFAD
 */
void map_animation_invalidate_all()
{
    auto it = _mapAnimations.begin();
    while (it != _mapAnimations.end())
    {
        if (InvalidateMapAnimation(*it))
        {
            // Map animation has finished, remove it
            it = _mapAnimations.erase(it);
        }
        else
        {
            it++;
        }
    }
}

/**
 *
 *  rct2: 0x00666670
 */
static bool map_animation_invalidate_ride_entrance(int32_t x, int32_t y, int32_t baseZ)
{
    auto tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
            continue;
        if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
            continue;

        auto ride = get_ride(tileElement->AsEntrance()->GetRideIndex());
        if (ride != nullptr)
        {
            auto stationObj = ride_get_station_object(ride);
            if (stationObj != nullptr)
            {
                int32_t height = (tileElement->base_height * 8) + stationObj->Height + 8;
                map_invalidate_tile_zoom1(x, y, height, height + 16);
            }
        }
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
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (!(tileElement->AsPath()->IsQueue()))
            continue;
        if (!tileElement->AsPath()->HasQueueBanner())
            continue;

        int32_t direction = (tileElement->AsPath()->GetQueueBannerDirection() + get_current_rotation()) & 3;
        if (direction == TILE_ELEMENT_DIRECTION_NORTH || direction == TILE_ELEMENT_DIRECTION_EAST)
        {
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
    TileElement* tileElement;
    rct_scenery_entry* sceneryEntry;
    rct_sprite* sprite;
    Peep* peep;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
            continue;
        if (tileElement->IsGhost())
            continue;

        sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
        if (sceneryEntry == nullptr)
            continue;

        if (scenery_small_entry_has_flag(
                sceneryEntry,
                SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1 | SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4 | SMALL_SCENERY_FLAG_SWAMP_GOO
                    | SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
        {
            map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
            return false;
        }

        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_IS_CLOCK))
        {
            // Peep, looking at scenery
            if (!(gCurrentTicks & 0x3FF) && game_is_not_paused())
            {
                int32_t direction = tileElement->GetDirection();
                int32_t x2 = x - CoordsDirectionDelta[direction].x;
                int32_t y2 = y - CoordsDirectionDelta[direction].y;

                uint16_t spriteIdx = sprite_get_first_in_quadrant(x2, y2);
                for (; spriteIdx != SPRITE_INDEX_NULL; spriteIdx = sprite->generic.next_in_quadrant)
                {
                    sprite = get_sprite(spriteIdx);
                    if (sprite->generic.linked_list_index != SPRITE_LIST_PEEP)
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
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
            continue;
        if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
            continue;
        if (tileElement->AsEntrance()->GetSequenceIndex())
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
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (tileElement->AsTrack()->GetTrackType() == TRACK_ELEM_WATERFALL)
        {
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
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (tileElement->AsTrack()->GetTrackType() == TRACK_ELEM_RAPIDS)
        {
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
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (tileElement->AsTrack()->GetTrackType() == TRACK_ELEM_ON_RIDE_PHOTO)
        {
            map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
            if (game_is_paused())
            {
                return false;
            }
            if (tileElement->AsTrack()->IsTakingPhoto())
            {
                tileElement->AsTrack()->DecrementPhotoTimeout();
                return false;
            }
            else
            {
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
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (tileElement->AsTrack()->GetTrackType() == TRACK_ELEM_WHIRLPOOL)
        {
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
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (tileElement->AsTrack()->GetTrackType() == TRACK_ELEM_SPINNING_TUNNEL)
        {
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
static bool map_animation_invalidate_remove(
    [[maybe_unused]] int32_t x, [[maybe_unused]] int32_t y, [[maybe_unused]] int32_t baseZ)
{
    return true;
}

/**
 *
 *  rct2: 0x006BA2BB
 */
static bool map_animation_invalidate_banner(int32_t x, int32_t y, int32_t baseZ)
{
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
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
    TileElement* tileElement;
    rct_scenery_entry* sceneryEntry;

    bool wasInvalidated = false;
    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_LARGE_SCENERY)
            continue;

        sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
        if (sceneryEntry->large_scenery.flags & LARGE_SCENERY_FLAG_ANIMATED)
        {
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
    TileElement* tileElement;
    rct_scenery_entry* sceneryEntry;

    if (gCurrentTicks & 1)
        return false;

    bool removeAnimation = true;
    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return removeAnimation;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;

        sceneryEntry = tileElement->AsWall()->GetEntry();
        if (!(sceneryEntry->wall.flags & WALL_SCENERY_IS_DOOR))
            continue;

        if (game_is_paused())
        {
            return false;
        }

        bool invalidate = false;

        uint8_t currentFrame = tileElement->AsWall()->GetAnimationFrame();
        if (currentFrame != 0)
        {
            if (currentFrame == 15)
            {
                currentFrame = 0;
            }
            else
            {
                removeAnimation = false;
                if (currentFrame != 5)
                {
                    currentFrame++;
                    if (currentFrame == 13 && !(sceneryEntry->wall.flags & WALL_SCENERY_LONG_DOOR_ANIMATION))
                        currentFrame = 15;

                    invalidate = true;
                }
            }
        }
        tileElement->AsWall()->SetAnimationFrame(currentFrame);
        if (invalidate)
        {
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
    TileElement* tileElement;
    rct_scenery_entry* sceneryEntry;

    bool wasInvalidated = false;
    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->base_height != baseZ)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;

        sceneryEntry = tileElement->AsWall()->GetEntry();

        if (!sceneryEntry
            || (!(sceneryEntry->wall.flags2 & WALL_SCENERY_2_ANIMATED)
                && sceneryEntry->wall.scrolling_mode == SCROLLING_MODE_NONE))
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
static bool InvalidateMapAnimation(const MapAnimation& a)
{
    if (a.type < std::size(_animatedObjectEventHandlers))
    {
        return _animatedObjectEventHandlers[a.type](a.location.x, a.location.y, a.location.z);
    }
    return true;
}

const std::vector<MapAnimation>& GetMapAnimations()
{
    return _mapAnimations;
}

static void ClearMapAnimations()
{
    _mapAnimations.clear();
}

void AutoCreateMapAnimations()
{
    ClearMapAnimations();

    tile_element_iterator it;
    tile_element_iterator_begin(&it);
    while (tile_element_iterator_next(&it))
    {
        auto el = it.element;
        auto loc = CoordsXYZ{ it.x * 32, it.y * 32, el->base_height };
        switch (el->GetType())
        {
            case TILE_ELEMENT_TYPE_BANNER:
                map_animation_create(MAP_ANIMATION_TYPE_BANNER, loc.x, loc.y, loc.z);
                break;
            case TILE_ELEMENT_TYPE_WALL:
            {
                auto wallEl = el->AsWall();
                auto entry = wallEl->GetEntry();
                if (entry != nullptr
                    && ((entry->wall.flags2 & WALL_SCENERY_2_ANIMATED) || entry->wall.scrolling_mode != SCROLLING_MODE_NONE))
                {
                    map_animation_create(MAP_ANIMATION_TYPE_WALL, loc.x, loc.y, loc.z);
                }
                break;
            }
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            {
                auto sceneryEl = el->AsSmallScenery();
                auto entry = sceneryEl->GetEntry();
                if (entry != nullptr && scenery_small_entry_has_flag(entry, SMALL_SCENERY_FLAG_ANIMATED))
                {
                    map_animation_create(MAP_ANIMATION_TYPE_SMALL_SCENERY, loc.x, loc.y, loc.z);
                }
                break;
            }
            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            {
                auto sceneryEl = el->AsLargeScenery();
                auto entry = sceneryEl->GetEntry();
                if (entry != nullptr && (entry->large_scenery.flags & LARGE_SCENERY_FLAG_ANIMATED))
                {
                    map_animation_create(MAP_ANIMATION_TYPE_LARGE_SCENERY, loc.x, loc.y, loc.z);
                }
                break;
            }
            case TILE_ELEMENT_TYPE_PATH:
            {
                auto path = el->AsPath();
                if (path->HasQueueBanner())
                {
                    map_animation_create(MAP_ANIMATION_TYPE_QUEUE_BANNER, loc.x, loc.y, loc.z);
                }
                break;
            }
            case TILE_ELEMENT_TYPE_ENTRANCE:
            {
                auto entrance = el->AsEntrance();
                switch (entrance->GetEntranceType())
                {
                    case ENTRANCE_TYPE_PARK_ENTRANCE:
                        if (entrance->GetSequenceIndex() == 0)
                        {
                            map_animation_create(MAP_ANIMATION_TYPE_PARK_ENTRANCE, loc.x, loc.y, loc.z);
                        }
                        break;
                    case ENTRANCE_TYPE_RIDE_ENTRANCE:
                        map_animation_create(MAP_ANIMATION_TYPE_RIDE_ENTRANCE, loc.x, loc.y, loc.z);
                        break;
                }
                break;
            }
            case TILE_ELEMENT_TYPE_TRACK:
            {
                auto track = el->AsTrack();
                switch (track->GetTrackType())
                {
                    case TRACK_ELEM_WATERFALL:
                        map_animation_create(MAP_ANIMATION_TYPE_TRACK_WATERFALL, loc.x, loc.y, loc.z);
                        break;
                    case TRACK_ELEM_RAPIDS:
                        map_animation_create(MAP_ANIMATION_TYPE_TRACK_RAPIDS, loc.x, loc.y, loc.z);
                        break;
                    case TRACK_ELEM_WHIRLPOOL:
                        map_animation_create(MAP_ANIMATION_TYPE_TRACK_WHIRLPOOL, loc.x, loc.y, loc.z);
                        break;
                    case TRACK_ELEM_SPINNING_TUNNEL:
                        map_animation_create(MAP_ANIMATION_TYPE_TRACK_SPINNINGTUNNEL, loc.x, loc.y, loc.z);
                        break;
                }
                break;
            }
        }
    }
}
