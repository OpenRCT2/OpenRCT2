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
#include <map>
#include <set>

using namespace OpenRCT2;

constexpr ZoomLevel kMaxZoom{ 1 };

struct TemporaryMapAnimation
{
    CoordsXYZ location{};
    MapAnimation::TemporaryType type{};

    bool operator<(const TemporaryMapAnimation& rhs) const
    {
        return std::tie(location.y, location.x, type, location.z)
            < std::tie(rhs.location.y, rhs.location.x, rhs.type, rhs.location.z);
    }
};

enum class UpdateType : uint8_t
{
    invalidate,
    update,
};

struct TileCoordsXYCmp
{
    constexpr bool operator()(const TileCoordsXY& lhs, const TileCoordsXY& rhs) const noexcept
    {
        // NOTE: Ordering should match GetFirstElementAt which is x + (y * size)
        return std::tie(lhs.y, lhs.x) < std::tie(rhs.y, rhs.x);
    }
};

// A list of tile coordinates which contains animated tile elements.
static std::map<TileCoordsXY, UpdateType, TileCoordsXYCmp> _mapAnimations;

static std::set<TemporaryMapAnimation> _temporaryMapAnimations;

static bool InvalidateEntranceAnimation(const EntranceElement& entrance, const CoordsXYZ& loc, const int32_t baseZ)
{
    if (entrance.GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        const auto ride = GetRide(entrance.GetRideIndex());
        if (ride != nullptr)
        {
            const auto stationObj = ride->getStationObject();
            if (stationObj != nullptr)
            {
                ViewportsInvalidate(loc.x, loc.y, baseZ + stationObj->Height + 8, baseZ + stationObj->Height + 24, kMaxZoom);
                return true;
            }
        }
    }
    else if (entrance.GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE && entrance.GetSequenceIndex() == 0)
    {
        const int32_t direction = (entrance.GetDirection() + GetCurrentRotation()) & 3;
        if (direction == TILE_ELEMENT_DIRECTION_SOUTH || direction == TILE_ELEMENT_DIRECTION_WEST)
        {
            ViewportsInvalidate(loc.x, loc.y, baseZ + 32, baseZ + 64, kMaxZoom);
        }
        return true;
    }

    return false;
}

static bool InvalidatePathAnimation(const PathElement& path, const CoordsXYZ& loc, const int32_t baseZ)
{
    if (path.IsQueue() && path.HasQueueBanner())
    {
        const int32_t direction = (path.GetQueueBannerDirection() + GetCurrentRotation()) & 3;
        if (direction == TILE_ELEMENT_DIRECTION_NORTH || direction == TILE_ELEMENT_DIRECTION_EAST)
        {
            ViewportsInvalidate(loc.x, loc.y, baseZ + 16, baseZ + 30, kMaxZoom);
        }
        return true;
    }
    return false;
}

static std::optional<UpdateType> UpdateSmallSceneryAnimation(
    const SmallSceneryElement& scenery, const CoordsXYZ& loc, const int32_t baseZ)
{
    const auto entry = scenery.GetEntry();
    if (entry == nullptr)
    {
        return std::nullopt;
    }

    if (entry->HasFlag(
            SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1 | SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4 | SMALL_SCENERY_FLAG_SWAMP_GOO
            | SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS | SMALL_SCENERY_FLAG_IS_CLOCK))
    {
        auto animationType = UpdateType ::invalidate;
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
                animationType = UpdateType ::update;
            }
        }
        const auto clearZ = scenery.GetClearanceZ();
        ViewportsInvalidate(loc.x, loc.y, baseZ, clearZ, kMaxZoom);
        return std::optional(animationType);
    }

    return std::nullopt;
}

static bool InvalidateTrackAnimation(TrackElement& track, const CoordsXYZ& loc, const int32_t baseZ)
{
    switch (track.GetTrackType())
    {
        case TrackElemType::Waterfall:
            ViewportsInvalidate(loc.x, loc.y, baseZ + 14, baseZ + 46, kMaxZoom);
            return true;
        case TrackElemType::Rapids:
        case TrackElemType::Whirlpool:
            ViewportsInvalidate(loc.x, loc.y, baseZ + 14, baseZ + 18, kMaxZoom);
            return true;
        case TrackElemType::SpinningTunnel:
            ViewportsInvalidate(loc.x, loc.y, baseZ + 14, baseZ + 32, kMaxZoom);
            return true;
        default:
            break;
    }

    return false;
}

static bool InvalidateLargeSceneryAnimation(const LargeSceneryElement& scenery, const CoordsXYZ& loc, const int32_t baseZ)
{
    const auto entry = scenery.GetEntry();
    if (entry != nullptr && (entry->flags & LARGE_SCENERY_FLAG_ANIMATED))
    {
        ViewportsInvalidate(loc.x, loc.y, baseZ, baseZ + 16, kMaxZoom);
        return true;
    }

    return false;
}

static std::optional<UpdateType> UpdateWallAnimation(WallElement& wall, const CoordsXYZ& loc, const int32_t baseZ)
{
    const auto entry = wall.GetEntry();
    if (entry == nullptr)
    {
        return std::nullopt;
    }

    if (entry->flags & WALL_SCENERY_IS_DOOR && wall.IsAnimating())
    {
        if (getGameState().currentTicks & 1)
        {
            return std::optional(UpdateType::update);
        }

        bool removeAnim = true;

        const auto currentFrame = wall.GetAnimationFrame();
        if (currentFrame != 0)
        {
            auto newFrame = currentFrame;
            if (currentFrame == 15)
            {
                newFrame = 0;
                wall.SetIsAnimating(false);
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
                ViewportsInvalidate(loc.x, loc.y, baseZ, baseZ + 32, kMaxZoom);
            }
        }

        return removeAnim ? std::nullopt : std::optional(UpdateType::update);
    }
    else if ((entry->flags2 & WALL_SCENERY_2_ANIMATED) || entry->scrolling_mode != kScrollingModeNone)
    {
        ViewportsInvalidate(loc.x, loc.y, baseZ, baseZ + 16, kMaxZoom);
        return std::optional(UpdateType::invalidate);
    }

    return std::nullopt;
}

static bool InvalidateBannerAnimation([[maybe_unused]] const BannerElement& banner, const CoordsXYZ& loc, const int32_t baseZ)
{
    ViewportsInvalidate(loc.x, loc.y, baseZ, baseZ + 16, kMaxZoom);
    return true;
}

template<bool noInvalidations>
static std::optional<UpdateType> UpdateTileAnimations(const TileCoordsXY& coords)
{
    auto tileElement = MapGetFirstElementAt(coords);
    if (tileElement == nullptr)
    {
        return std::nullopt;
    }

    bool hasAnimations = false;
    auto animationType = UpdateType ::invalidate;
    do
    {
        const auto baseZ = tileElement->GetBaseZ();
        const CoordsXYZ loc{ coords.ToCoordsXY(), baseZ };

        switch (tileElement->GetType())
        {
            case TileElementType::Entrance:
                if constexpr (!noInvalidations)
                {
                    hasAnimations |= InvalidateEntranceAnimation(*tileElement->AsEntrance(), loc, baseZ);
                }
                break;
            case TileElementType::Path:
                if constexpr (!noInvalidations)
                {
                    hasAnimations |= InvalidatePathAnimation(*tileElement->AsPath(), loc, baseZ);
                }
                break;
            case TileElementType::SmallScenery:
            {
                const auto result = UpdateSmallSceneryAnimation(*tileElement->AsSmallScenery(), loc, baseZ);
                if (result)
                {
                    hasAnimations |= true;
                    if (result.value() == UpdateType::update)
                    {
                        animationType = UpdateType::update;
                    }
                }
                break;
            }
            case TileElementType::Track:
                if constexpr (!noInvalidations)
                {
                    hasAnimations |= InvalidateTrackAnimation(*tileElement->AsTrack(), loc, baseZ);
                }
                break;
            case TileElementType::Banner:
                if constexpr (!noInvalidations)
                {
                    hasAnimations |= InvalidateBannerAnimation(*tileElement->AsBanner(), loc, baseZ);
                }
                break;
            case TileElementType::LargeScenery:
                if constexpr (!noInvalidations)
                {
                    hasAnimations |= InvalidateLargeSceneryAnimation(*tileElement->AsLargeScenery(), loc, baseZ);
                }
                break;
            case TileElementType::Wall:
            {
                const auto result = UpdateWallAnimation(*tileElement->AsWall(), loc, baseZ);
                if (result)
                {
                    hasAnimations |= true;
                    if (result.value() == UpdateType::update)
                    {
                        animationType = UpdateType::update;
                    }
                }
                break;
            }
            default:
                break;
        }
    } while (!(tileElement++)->IsLastForTile());

    return hasAnimations ? std::optional(animationType) : std::nullopt;
}

static bool UpdateOnRidePhotoAnimation(TrackElement& track, const CoordsXYZ& coords)
{
    if (track.IsTakingPhoto())
    {
        track.DecrementPhotoTimeout();
        ViewportsInvalidate(coords.x, coords.y, coords.z, track.GetClearanceZ(), kMaxZoom);
        return true;
    }
    return false;
}

static bool UpdateLandEdgeDoorsAnimation(TrackElement& track, const CoordsXYZ& coords)
{
    if (getGameState().currentTicks & 3)
    {
        return true;
    }

    bool isAnimating = false;

    const auto doorAState = track.GetDoorAState();
    if (doorAState >= kLandEdgeDoorFrameEnd)
    {
        track.SetDoorAState(kLandEdgeDoorFrameClosed);
        ViewportsInvalidate(coords.x, coords.y, coords.z, coords.z + 32, kMaxZoom);
    }
    else if (doorAState != kLandEdgeDoorFrameClosed && doorAState != kLandEdgeDoorFrameOpen)
    {
        track.SetDoorAState(doorAState + 1);
        ViewportsInvalidate(coords.x, coords.y, coords.z, coords.z + 32, kMaxZoom);
        isAnimating = true;
    }

    const auto doorBState = track.GetDoorBState();
    if (doorBState >= kLandEdgeDoorFrameEnd)
    {
        track.SetDoorBState(kLandEdgeDoorFrameClosed);
        ViewportsInvalidate(coords.x, coords.y, coords.z, coords.z + 32, kMaxZoom);
    }
    else if (doorBState != kLandEdgeDoorFrameClosed && doorBState != kLandEdgeDoorFrameOpen)
    {
        track.SetDoorBState(doorBState + 1);
        ViewportsInvalidate(coords.x, coords.y, coords.z, coords.z + 32, kMaxZoom);
        isAnimating = true;
    }

    return isAnimating;
}

static bool UpdateTemporaryAnimation(const TemporaryMapAnimation& animation)
{
    const TileCoordsXYZ tileCoords{ animation.location };
    TileElement* tileElement = MapGetFirstElementAt(tileCoords);
    if (tileElement == nullptr)
    {
        return true;
    }

    bool isAnimating = false;
    do
    {
        switch (animation.type)
        {
            case MapAnimation::TemporaryType::onRidePhoto:
            {
                if (tileElement->GetType() == TileElementType::Track && tileElement->BaseHeight == tileCoords.z
                    && tileElement->AsTrack()->GetTrackType() == TrackElemType::OnRidePhoto)
                {
                    isAnimating |= UpdateOnRidePhotoAnimation(*tileElement->AsTrack(), animation.location);
                }
                break;
            }
            case MapAnimation::TemporaryType::landEdgeDoor:
            {
                if (tileElement->GetType() == TileElementType::Track && tileElement->BaseHeight == tileCoords.z)
                {
                    isAnimating |= UpdateLandEdgeDoorsAnimation(*tileElement->AsTrack(), animation.location);
                }
            }
        }
    } while (!(tileElement++)->IsLastForTile());

    return isAnimating;
}

static std::optional<UpdateType> IsElementAnimated(const TileElementBase& element)
{
    switch (element.GetType())
    {
        case TileElementType::Banner:
            return std::optional(UpdateType::invalidate);
        case TileElementType::Wall:
        {
            const auto wall = element.AsWall();
            const auto entry = wall->GetEntry();
            if (entry != nullptr)
            {
                if ((entry->flags2 & WALL_SCENERY_2_ANIMATED) || entry->scrolling_mode != kScrollingModeNone)
                {
                    return std::optional(UpdateType::invalidate);
                }
                if (entry->flags & WALL_SCENERY_IS_DOOR && wall->IsAnimating())
                {
                    return std::optional(UpdateType::update);
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
                if (entry->HasFlag(SMALL_SCENERY_FLAG_IS_CLOCK))
                {
                    return std::optional(UpdateType::update);
                }
                else
                {
                    return std::optional(UpdateType::invalidate);
                }
            }
            break;
        }
        case TileElementType::LargeScenery:
        {
            const auto scenery = element.AsLargeScenery();
            const auto entry = scenery->GetEntry();
            if (entry != nullptr && (entry->flags & LARGE_SCENERY_FLAG_ANIMATED))
            {
                return std::optional(UpdateType::invalidate);
            }
            break;
        }
        case TileElementType::Path:
        {
            const auto path = element.AsPath();
            if (path->HasQueueBanner())
            {
                return std::optional(UpdateType::invalidate);
            }
            break;
        }
        case TileElementType::Entrance:
        {
            const auto entrance = element.AsEntrance();
            if (entrance->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE && entrance->GetSequenceIndex() == 0)
            {
                return std::optional(UpdateType::invalidate);
            }
            else if (entrance->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
            {
                return std::optional(UpdateType::invalidate);
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
                    return std::optional(UpdateType::invalidate);
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    return std::nullopt;
}

void MapAnimation::MarkTileForInvalidation(const CoordsXY coords)
{
    _mapAnimations[TileCoordsXY(coords)];
}

void MapAnimation::MarkTileForUpdate(const CoordsXY coords)
{
    _mapAnimations[TileCoordsXY(coords)] = UpdateType::update;
}

void MapAnimation::CreateTemporary(const CoordsXYZ& coords, const TemporaryType type)
{
    _temporaryMapAnimations.insert(TemporaryMapAnimation{ coords, type });
}

void MapAnimation::MarkAllTiles()
{
    TileElementIterator it;
    TileElementIteratorBegin(&it);
    while (TileElementIteratorNext(&it))
    {
        const auto isAnimated = IsElementAnimated(*it.element);
        if (isAnimated)
        {
            switch (*isAnimated)
            {
                case UpdateType::invalidate:
                    _mapAnimations[TileCoordsXY(it.x, it.y)];
                    break;
                case UpdateType::update:
                    _mapAnimations[TileCoordsXY(it.x, it.y)] = UpdateType::update;
                    break;
            }
        }
    }
}

static bool IsTileInView(const TileCoordsXY coords, const ViewportList& viewports) noexcept
{
    for (const auto viewport : viewports)
    {
        if (viewport->ContainsTile(coords))
        {
            return true;
        }
    }
    return false;
}

void MapAnimation::UpdateAll()
{
    PROFILED_FUNCTION();

    {
        const auto viewports = GetVisibleViewports();

        auto it = _mapAnimations.begin();
        while (it != _mapAnimations.end())
        {
            const auto& tile = *it;
            const bool tileInView = IsTileInView(tile.first, viewports);
            if (!tileInView && tile.second == UpdateType::invalidate)
            {
                ++it;
                continue;
            }

            const auto result = tileInView ? UpdateTileAnimations<false>(tile.first) : UpdateTileAnimations<true>(tile.first);
            if (result)
            {
                (*it).second = result.value();
                ++it;
            }
            else
            {
                it = _mapAnimations.erase(it);
            }
        }
    }

    {
        auto it = _temporaryMapAnimations.begin();
        while (it != _temporaryMapAnimations.end())
        {
            if (UpdateTemporaryAnimation(*it))
            {
                ++it;
            }
            else
            {
                it = _temporaryMapAnimations.erase(it);
            }
        }
    }
}

void MapAnimation::ClearAll()
{
    _mapAnimations.clear();
    _temporaryMapAnimations.clear();
}

void MapAnimation::ShiftAll(const CoordsXY amount)
{
    if (amount.x == 0 && amount.y == 0)
        return;

    std::map<TileCoordsXY, UpdateType, TileCoordsXYCmp> newMapAnimations;
    for (const auto& a : _mapAnimations)
    {
        newMapAnimations[a.first + TileCoordsXY(amount)] = a.second;
    }
    _mapAnimations = std::move(newMapAnimations);

    std::set<TemporaryMapAnimation> newTemporaryMapAnimations;
    for (const auto& a : _temporaryMapAnimations)
    {
        newTemporaryMapAnimations.insert(TemporaryMapAnimation{ a.location + CoordsXYZ(amount, 0), a.type });
    }
    _temporaryMapAnimations = std::move(newTemporaryMapAnimations);
}
