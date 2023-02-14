/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapAnimation.h"

#include "../Context.h"
#include "../Game.h"
#include "../entity/EntityList.h"
#include "../entity/Peep.h"
#include "../interface/Viewport.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/SmallSceneryEntry.h"
#include "../object/StationObject.h"
#include "../object/WallSceneryEntry.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../world/Wall.h"
#include "Banner.h"
#include "Footpath.h"
#include "LargeScenery.h"
#include "Map.h"
#include "Scenery.h"

using map_animation_invalidate_event_handler = bool (*)(const CoordsXYZ& loc);

static std::vector<MapAnimation> _mapAnimations;

constexpr size_t MAX_ANIMATED_OBJECTS = 2000;

static bool InvalidateMapAnimation(const MapAnimation& obj);

static bool DoesAnimationExist(int32_t type, const CoordsXYZ& location)
{
    for (const auto& a : _mapAnimations)
    {
        if (a.type == type && a.location == location)
        {
            // Animation already exists
            return true;
        }
    }
    return false;
}

void MapAnimationCreate(int32_t type, const CoordsXYZ& loc)
{
    if (!DoesAnimationExist(type, loc))
    {
        if (_mapAnimations.size() < MAX_ANIMATED_OBJECTS)
        {
            // Create new animation
            _mapAnimations.push_back({ static_cast<uint8_t>(type), loc });
        }
        else
        {
            LOG_ERROR("Exceeded the maximum number of animations");
        }
    }
}

/**
 *
 *  rct2: 0x0068AFAD
 */
void MapAnimationInvalidateAll()
{
    PROFILED_FUNCTION();

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
static bool MapAnimationInvalidateRideEntrance(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    auto tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Entrance)
            continue;
        if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
            continue;

        auto ride = GetRide(tileElement->AsEntrance()->GetRideIndex());
        if (ride != nullptr)
        {
            auto stationObj = ride->GetStationObject();
            if (stationObj != nullptr)
            {
                int32_t height = loc.z + stationObj->Height + 8;
                MapInvalidateTileZoom1({ loc, height, height + 16 });
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
static bool MapAnimationInvalidateQueueBanner(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Path)
            continue;
        if (!(tileElement->AsPath()->IsQueue()))
            continue;
        if (!tileElement->AsPath()->HasQueueBanner())
            continue;

        int32_t direction = (tileElement->AsPath()->GetQueueBannerDirection() + GetCurrentRotation()) & 3;
        if (direction == TILE_ELEMENT_DIRECTION_NORTH || direction == TILE_ELEMENT_DIRECTION_EAST)
        {
            MapInvalidateTileZoom1({ loc, loc.z + 16, loc.z + 30 });
        }
        return false;
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006E32C9
 */
static bool MapAnimationInvalidateSmallScenery(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };

    auto tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::SmallScenery)
            continue;
        if (tileElement->IsGhost())
            continue;

        auto* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
        if (sceneryEntry == nullptr)
            continue;

        if (sceneryEntry->HasFlag(
                SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1 | SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4 | SMALL_SCENERY_FLAG_SWAMP_GOO
                | SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
        {
            MapInvalidateTileZoom1({ loc, loc.z, tileElement->GetClearanceZ() });
            return false;
        }

        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_IS_CLOCK))
        {
            // Peep, looking at scenery
            if (!(gCurrentTicks & 0x3FF) && GameIsNotPaused())
            {
                int32_t direction = tileElement->GetDirection();
                auto quad = EntityTileList<Peep>(CoordsXY{ loc } - CoordsDirectionDelta[direction]);
                for (auto peep : quad)
                {
                    if (peep->State != PeepState::Walking)
                        continue;
                    if (peep->z != loc.z)
                        continue;
                    if (peep->Action < PeepActionType::Idle)
                        continue;

                    peep->Action = PeepActionType::CheckTime;
                    peep->ActionFrame = 0;
                    peep->ActionSpriteImageOffset = 0;
                    peep->UpdateCurrentActionSpriteType();
                    peep->Invalidate();
                    break;
                }
            }
            MapInvalidateTileZoom1({ loc, loc.z, tileElement->GetClearanceZ() });
            return false;
        }

    } while (!(tileElement++)->IsLastForTile());
    return true;
}

/**
 *
 *  rct2: 0x00666C63
 */
static bool MapAnimationInvalidateParkEntrance(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Entrance)
            continue;
        if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
            continue;
        if (tileElement->AsEntrance()->GetSequenceIndex())
            continue;

        MapInvalidateTileZoom1({ loc, loc.z + 32, loc.z + 64 });
        return false;
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006CE29E
 */
static bool MapAnimationInvalidateTrackWaterfall(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Track)
            continue;

        if (tileElement->AsTrack()->GetTrackType() == TrackElemType::Waterfall)
        {
            MapInvalidateTileZoom1({ loc, loc.z + 14, loc.z + 46 });
            return false;
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006CE2F3
 */
static bool MapAnimationInvalidateTrackRapids(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Track)
            continue;

        if (tileElement->AsTrack()->GetTrackType() == TrackElemType::Rapids)
        {
            MapInvalidateTileZoom1({ loc, loc.z + 14, loc.z + 18 });
            return false;
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006CE39D
 */
static bool MapAnimationInvalidateTrackOnRidePhoto(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Track)
            continue;

        if (tileElement->AsTrack()->GetTrackType() == TrackElemType::OnRidePhoto)
        {
            MapInvalidateTileZoom1({ loc, loc.z, tileElement->GetClearanceZ() });
            if (GameIsPaused())
            {
                return false;
            }
            if (tileElement->AsTrack()->IsTakingPhoto())
            {
                tileElement->AsTrack()->DecrementPhotoTimeout();
                return false;
            }

            return true;
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006CE348
 */
static bool MapAnimationInvalidateTrackWhirlpool(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Track)
            continue;

        if (tileElement->AsTrack()->GetTrackType() == TrackElemType::Whirlpool)
        {
            MapInvalidateTileZoom1({ loc, loc.z + 14, loc.z + 18 });
            return false;
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006CE3FA
 */
static bool MapAnimationInvalidateTrackSpinningTunnel(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Track)
            continue;

        if (tileElement->AsTrack()->GetTrackType() == TrackElemType::SpinningTunnel)
        {
            MapInvalidateTileZoom1({ loc, loc.z + 14, loc.z + 32 });
            return false;
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x0068DF8F
 */
static bool MapAnimationInvalidateRemove([[maybe_unused]] const CoordsXYZ& loc)
{
    return true;
}

/**
 *
 *  rct2: 0x006BA2BB
 */
static bool MapAnimationInvalidateBanner(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Banner)
            continue;
        MapInvalidateTileZoom1({ loc, loc.z, loc.z + 16 });
        return false;
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

/**
 *
 *  rct2: 0x006B94EB
 */
static bool MapAnimationInvalidateLargeScenery(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    bool wasInvalidated = false;
    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::LargeScenery)
            continue;

        auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
        if (sceneryEntry != nullptr && sceneryEntry->flags & LARGE_SCENERY_FLAG_ANIMATED)
        {
            MapInvalidateTileZoom1({ loc, loc.z, loc.z + 16 });
            wasInvalidated = true;
        }
    } while (!(tileElement++)->IsLastForTile());

    return !wasInvalidated;
}

/**
 *
 *  rct2: 0x006E5B50
 */
static bool MapAnimationInvalidateWallDoor(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    if (gCurrentTicks & 1)
        return false;

    bool removeAnimation = true;
    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return removeAnimation;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Wall)
            continue;

        auto* wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || !(wallEntry->flags & WALL_SCENERY_IS_DOOR))
            continue;

        if (GameIsPaused())
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
                    if (currentFrame == 13 && !(wallEntry->flags & WALL_SCENERY_LONG_DOOR_ANIMATION))
                        currentFrame = 15;

                    invalidate = true;
                }
            }
        }
        tileElement->AsWall()->SetAnimationFrame(currentFrame);
        if (invalidate)
        {
            MapInvalidateTileZoom1({ loc, loc.z, loc.z + 32 });
        }
    } while (!(tileElement++)->IsLastForTile());

    return removeAnimation;
}

/**
 *
 *  rct2: 0x006E5EE4
 */
static bool MapAnimationInvalidateWall(const CoordsXYZ& loc)
{
    TileCoordsXYZ tileLoc{ loc };
    TileElement* tileElement;

    bool wasInvalidated = false;
    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return true;
    do
    {
        if (tileElement->BaseHeight != tileLoc.z)
            continue;
        if (tileElement->GetType() != TileElementType::Wall)
            continue;

        auto* wallEntry = tileElement->AsWall()->GetEntry();

        if (wallEntry == nullptr
            || (!(wallEntry->flags2 & WALL_SCENERY_2_ANIMATED) && wallEntry->scrolling_mode == SCROLLING_MODE_NONE))
            continue;

        MapInvalidateTileZoom1({ loc, loc.z, loc.z + 16 });
        wasInvalidated = true;
    } while (!(tileElement++)->IsLastForTile());

    return !wasInvalidated;
}

/**
 *
 *  rct2: 0x009819DC
 */
static constexpr const map_animation_invalidate_event_handler _animatedObjectEventHandlers[MAP_ANIMATION_TYPE_COUNT] = {
    MapAnimationInvalidateRideEntrance,     MapAnimationInvalidateQueueBanner,    MapAnimationInvalidateSmallScenery,
    MapAnimationInvalidateParkEntrance,     MapAnimationInvalidateTrackWaterfall, MapAnimationInvalidateTrackRapids,
    MapAnimationInvalidateTrackOnRidePhoto, MapAnimationInvalidateTrackWhirlpool, MapAnimationInvalidateTrackSpinningTunnel,
    MapAnimationInvalidateRemove,           MapAnimationInvalidateBanner,         MapAnimationInvalidateLargeScenery,
    MapAnimationInvalidateWallDoor,         MapAnimationInvalidateWall,
};

/**
 * @returns true if the animation should be removed.
 */
static bool InvalidateMapAnimation(const MapAnimation& a)
{
    if (a.type < std::size(_animatedObjectEventHandlers))
    {
        return _animatedObjectEventHandlers[a.type](a.location);
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

void MapAnimationAutoCreate()
{
    ClearMapAnimations();

    TileElementIterator it;
    TileElementIteratorBegin(&it);
    while (TileElementIteratorNext(&it))
    {
        auto el = it.element;
        auto loc = CoordsXYZ{ TileCoordsXY(it.x, it.y).ToCoordsXY(), el->GetBaseZ() };
        switch (el->GetType())
        {
            case TileElementType::Banner:
                MapAnimationCreate(MAP_ANIMATION_TYPE_BANNER, loc);
                break;
            case TileElementType::Wall:
            {
                auto wallEl = el->AsWall();
                auto* entry = wallEl->GetEntry();
                if (entry != nullptr
                    && ((entry->flags2 & WALL_SCENERY_2_ANIMATED) || entry->scrolling_mode != SCROLLING_MODE_NONE))
                {
                    MapAnimationCreate(MAP_ANIMATION_TYPE_WALL, loc);
                }
                break;
            }
            case TileElementType::SmallScenery:
            {
                auto sceneryEl = el->AsSmallScenery();
                auto* sceneryEntry = sceneryEl->GetEntry();
                if (sceneryEntry != nullptr && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED))
                {
                    MapAnimationCreate(MAP_ANIMATION_TYPE_SMALL_SCENERY, loc);
                }
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto sceneryEl = el->AsLargeScenery();
                auto entry = sceneryEl->GetEntry();
                if (entry != nullptr && (entry->flags & LARGE_SCENERY_FLAG_ANIMATED))
                {
                    MapAnimationCreate(MAP_ANIMATION_TYPE_LARGE_SCENERY, loc);
                }
                break;
            }
            case TileElementType::Path:
            {
                auto path = el->AsPath();
                if (path->HasQueueBanner())
                {
                    MapAnimationCreate(MAP_ANIMATION_TYPE_QUEUE_BANNER, loc);
                }
                break;
            }
            case TileElementType::Entrance:
            {
                auto entrance = el->AsEntrance();
                switch (entrance->GetEntranceType())
                {
                    case ENTRANCE_TYPE_PARK_ENTRANCE:
                        if (entrance->GetSequenceIndex() == 0)
                        {
                            MapAnimationCreate(MAP_ANIMATION_TYPE_PARK_ENTRANCE, loc);
                        }
                        break;
                    case ENTRANCE_TYPE_RIDE_ENTRANCE:
                        MapAnimationCreate(MAP_ANIMATION_TYPE_RIDE_ENTRANCE, loc);
                        break;
                }
                break;
            }
            case TileElementType::Track:
            {
                auto track = el->AsTrack();
                switch (track->GetTrackType())
                {
                    case TrackElemType::Waterfall:
                        MapAnimationCreate(MAP_ANIMATION_TYPE_TRACK_WATERFALL, loc);
                        break;
                    case TrackElemType::Rapids:
                        MapAnimationCreate(MAP_ANIMATION_TYPE_TRACK_RAPIDS, loc);
                        break;
                    case TrackElemType::Whirlpool:
                        MapAnimationCreate(MAP_ANIMATION_TYPE_TRACK_WHIRLPOOL, loc);
                        break;
                    case TrackElemType::SpinningTunnel:
                        MapAnimationCreate(MAP_ANIMATION_TYPE_TRACK_SPINNINGTUNNEL, loc);
                        break;
                }
                break;
            }
            case TileElementType::Surface:
                break;
        }
    }
}
