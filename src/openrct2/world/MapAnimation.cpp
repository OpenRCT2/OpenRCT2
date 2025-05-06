/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapAnimation.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
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
#include "Banner.h"
#include "Footpath.h"
#include "Scenery.h"
#include "tile_element/EntranceElement.h"
#include "tile_element/LargeSceneryElement.h"
#include "tile_element/PathElement.h"
#include "tile_element/SmallSceneryElement.h"
#include "tile_element/TileElement.h"
#include "tile_element/TrackElement.h"
#include "tile_element/WallElement.h"

#include <algorithm>
#include <set>

using namespace OpenRCT2;

struct TileCoordsXYCmp
{
    constexpr bool operator()(const TileCoordsXY& lhs, const TileCoordsXY& rhs) const noexcept
    {
        // NOTE: Ordering should match GetFirstElementAt which is x + (y * size)
        return std::tie(lhs.y, lhs.x) < std::tie(rhs.y, rhs.x);
    }
};

// A list of tile coordinates which contains animated tile elements.
static std::set<TileCoordsXY, TileCoordsXYCmp> _mapAnimations;

static bool UpdateEntranceAnimation(const EntranceElement& entrance, const CoordsXYZ& loc, const int32_t baseZ)
{
    if (entrance.GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        const auto ride = GetRide(entrance.GetRideIndex());
        if (ride != nullptr)
        {
            const auto stationObj = ride->getStationObject();
            if (stationObj != nullptr)
            {
                MapInvalidateTileZoom1({ loc, baseZ + stationObj->Height + 8, baseZ + stationObj->Height + 24 });
                return true;
            }
        }
    }
    else if (entrance.GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE && entrance.GetSequenceIndex() == 0)
    {
        const int32_t direction = (entrance.GetDirection() + GetCurrentRotation()) & 3;
        if (direction == TILE_ELEMENT_DIRECTION_SOUTH || direction == TILE_ELEMENT_DIRECTION_WEST)
        {
            MapInvalidateTileZoom1({ loc, baseZ + 32, baseZ + 64 });
        }
        return true;
    }

    return false;
}

static bool UpdatePathAnimation(const PathElement& path, const CoordsXYZ& loc, const int32_t baseZ)
{
    if (path.IsQueue() && path.HasQueueBanner())
    {
        const int32_t direction = (path.GetQueueBannerDirection() + GetCurrentRotation()) & 3;
        if (direction == TILE_ELEMENT_DIRECTION_NORTH || direction == TILE_ELEMENT_DIRECTION_EAST)
        {
            MapInvalidateTileZoom1({ loc, baseZ + 16, baseZ + 30 });
        }
        return true;
    }
    return false;
}

static bool UpdateSmallSceneryAnimation(const SmallSceneryElement& scenery, const CoordsXYZ& loc, const int32_t baseZ)
{
    const auto entry = scenery.GetEntry();
    if (entry == nullptr)
    {
        return false;
    }

    if (entry->HasFlag(
            SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1 | SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4 | SMALL_SCENERY_FLAG_SWAMP_GOO
            | SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS | SMALL_SCENERY_FLAG_IS_CLOCK))
    {
        // Don't apply anything to peeps when the scenery is a ghost.
        if (!scenery.IsGhost())
        {
            if (entry->HasFlag(SMALL_SCENERY_FLAG_IS_CLOCK) && !(getGameState().currentTicks & 0x3FF))
            {
                const int32_t direction = scenery.GetDirection();
                auto quad = EntityTileList<Peep>(CoordsXY{ loc.x, loc.y } - CoordsDirectionDelta[direction]);
                for (auto peep : quad)
                {
                    if (peep->State != PeepState::Walking || peep->z != baseZ || peep->Action < PeepActionType::Idle)
                        continue;
                    peep->Action = PeepActionType::CheckTime;
                    peep->AnimationFrameNum = 0;
                    peep->AnimationImageIdOffset = 0;
                    peep->UpdateCurrentAnimationType();
                    peep->Invalidate();
                    break;
                }
            }
        }
        const auto clearZ = scenery.GetClearanceZ();
        MapInvalidateTileZoom1({ loc, baseZ, clearZ });
        return true;
    }

    return false;
}

static bool UpdateTrackAnimation(TrackElement& track, const CoordsXYZ& loc, const int32_t baseZ)
{
    const auto clearZ = track.GetClearanceZ();
    switch (track.GetTrackType())
    {
        case TrackElemType::Waterfall:
            MapInvalidateTileZoom1({ loc, baseZ + 14, baseZ + 46 });
            return true;
        case TrackElemType::Rapids:
        case TrackElemType::Whirlpool:
            MapInvalidateTileZoom1({ loc, baseZ + 14, baseZ + 18 });
            return true;
        case TrackElemType::SpinningTunnel:
            MapInvalidateTileZoom1({ loc, baseZ + 14, baseZ + 32 });
            return true;
        case TrackElemType::OnRidePhoto:
            if (track.IsTakingPhoto())
            {
                MapInvalidateTileZoom1({ loc, baseZ, clearZ });
                track.DecrementPhotoTimeout();
                return true;
            }
        default:
            break;
    }

    return false;
}

static bool UpdateLargeSceneryAnimation(const LargeSceneryElement& scenery, const CoordsXYZ& loc, const int32_t baseZ)
{
    const auto entry = scenery.GetEntry();
    if (entry != nullptr && (entry->flags & LARGE_SCENERY_FLAG_ANIMATED))
    {
        MapInvalidateTileZoom1({ loc, baseZ, baseZ + 16 });
        return true;
    }

    return false;
}

static bool UpdateWallAnimation(WallElement& wall, const CoordsXYZ& loc, const int32_t baseZ)
{
    const auto entry = wall.GetEntry();
    if (entry == nullptr)
    {
        return false;
    }

    if (entry->flags & WALL_SCENERY_IS_DOOR)
    {
        if (getGameState().currentTicks & 1)
        {
            return true;
        }

        bool removeAnim = true;

        const auto currentFrame = wall.GetAnimationFrame();
        if (currentFrame != 0)
        {
            auto newFrame = currentFrame;
            if (currentFrame == 15)
            {
                newFrame = 0;
            }
            else
            {
                removeAnim = false;
                if (currentFrame != 5)
                {
                    newFrame++;

                    if (newFrame == 13 && !(entry->flags & WALL_SCENERY_LONG_DOOR_ANIMATION))
                        newFrame = 15;
                }
            }

            if (currentFrame != newFrame)
            {
                wall.SetAnimationFrame(newFrame);
                MapInvalidateTileZoom1({ loc, baseZ, baseZ + 32 });
            }
        }

        return !removeAnim;
    }
    else if ((entry->flags2 & WALL_SCENERY_2_ANIMATED) || entry->scrolling_mode != kScrollingModeNone)
    {
        MapInvalidateTileZoom1({ loc, baseZ, baseZ + 16 });
        return true;
    }

    return false;
}

static bool UpdateBannerAnimation([[maybe_unused]] const BannerElement& banner, const CoordsXYZ& loc, const int32_t baseZ)
{
    MapInvalidateTileZoom1({ loc, baseZ, baseZ + 16 });
    return true;
}

static bool UpdateTileAnimations(const TileCoordsXY& coords)
{
    auto tileElement = MapGetFirstElementAt(coords);
    if (tileElement == nullptr)
    {
        return false;
    }

    bool hasAnimations = false;
    do
    {
        const auto baseZ = tileElement->GetBaseZ();
        const CoordsXYZ loc{ coords.ToCoordsXY(), baseZ };

        switch (tileElement->GetType())
        {
            case TileElementType::Entrance:
                hasAnimations |= UpdateEntranceAnimation(*tileElement->AsEntrance(), loc, baseZ);
                break;
            case TileElementType::Path:
                hasAnimations |= UpdatePathAnimation(*tileElement->AsPath(), loc, baseZ);
                break;
            case TileElementType::SmallScenery:
                hasAnimations |= UpdateSmallSceneryAnimation(*tileElement->AsSmallScenery(), loc, baseZ);
                break;
            case TileElementType::Track:
                hasAnimations |= UpdateTrackAnimation(*tileElement->AsTrack(), loc, baseZ);
                break;
            case TileElementType::Banner:
                hasAnimations |= UpdateBannerAnimation(*tileElement->AsBanner(), loc, baseZ);
                break;
            case TileElementType::LargeScenery:
                hasAnimations |= UpdateLargeSceneryAnimation(*tileElement->AsLargeScenery(), loc, baseZ);
                break;
            case TileElementType::Wall:
                hasAnimations |= UpdateWallAnimation(*tileElement->AsWall(), loc, baseZ);
                break;
            default:
                break;
        }
    } while (!(tileElement++)->IsLastForTile());

    return hasAnimations;
}

static bool IsElementAnimated(const TileElementBase& element, const bool skipDoors)
{
    switch (element.GetType())
    {
        case TileElementType::Banner:
            return true;
        case TileElementType::Wall:
        {
            const auto wall = element.AsWall();
            const auto entry = wall->GetEntry();
            if (entry != nullptr)
            {
                if ((entry->flags2 & WALL_SCENERY_2_ANIMATED) || entry->scrolling_mode != kScrollingModeNone)
                {
                    return true;
                }
                if (!skipDoors && (entry->flags & WALL_SCENERY_IS_DOOR))
                {
                    return true;
                }
            }
            break;
        }
        case TileElementType::SmallScenery:
        {
            const auto scenery = element.AsSmallScenery();
            const auto entry = scenery->GetEntry();
            if (entry != nullptr && entry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED))
            {
                return true;
            }
            break;
        }
        case TileElementType::LargeScenery:
        {
            const auto scenery = element.AsLargeScenery();
            const auto entry = scenery->GetEntry();
            if (entry != nullptr && (entry->flags & LARGE_SCENERY_FLAG_ANIMATED))
            {
                return true;
            }
            break;
        }
        case TileElementType::Path:
        {
            const auto path = element.AsPath();
            if (path->HasQueueBanner())
            {
                return true;
            }
            break;
        }
        case TileElementType::Entrance:
        {
            const auto entrance = element.AsEntrance();
            if (entrance->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE && entrance->GetSequenceIndex() == 0)
            {
                return true;
            }
            else if (entrance->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
            {
                return true;
            }
            break;
        }
        case TileElementType::Track:
        {
            const auto track = element.AsTrack();
            switch (track->GetTrackType())
            {
                case TrackElemType::Waterfall:
                case TrackElemType::Rapids:
                case TrackElemType::Whirlpool:
                case TrackElemType::SpinningTunnel:
                case TrackElemType::OnRidePhoto:
                    return true;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    return false;
}

void MapAnimation::Create(const CoordsXY coords)
{
    _mapAnimations.insert(TileCoordsXY(coords));
}

void MapAnimation::CreateAll()
{
    ClearAll();

    TileElementIterator it;
    TileElementIteratorBegin(&it);
    while (TileElementIteratorNext(&it))
    {
        if (IsElementAnimated(*it.element, true))
        {
            _mapAnimations.insert(TileCoordsXY(it.x, it.y));
        }
    }
}

void MapAnimation::UpdateAll()
{
    PROFILED_FUNCTION();

    auto it = _mapAnimations.begin();
    while (it != _mapAnimations.end())
    {
        if (UpdateTileAnimations(*it))
        {
            ++it; // Tile was updated, keep it
        }
        else
        {
            it = _mapAnimations.erase(it); // No updates, remove tile
        }
    }
}

void MapAnimation::ClearAll()
{
    _mapAnimations.clear();
}

void MapAnimation::ShiftAll(const CoordsXY amount)
{
    if (amount.x == 0 && amount.y == 0)
        return;

    std::set<TileCoordsXY, TileCoordsXYCmp> newMapAnimations;
    for (const auto& a : _mapAnimations)
    {
        newMapAnimations.insert(a + TileCoordsXY(amount));
    }
    _mapAnimations = std::move(newMapAnimations);
}
