/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../core/Numerics.hpp"
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
#include "Map.h"
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

constexpr ZoomLevel kMaxZoom{ 2 };
constexpr ZoomLevel kMaxScrollingTextZoom{ 0 };

struct TemporaryMapAnimation
{
    CoordsXYZ location{};
    MapAnimations::TemporaryType type{};

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

static std::vector<bool> _mapAnimationsInvalidate = std::vector<bool>(
    kMaximumMapSizeTechnical * kMaximumMapSizeTechnical, false);
static std::set<TileCoordsXY, TileCoordsXYCmp> _mapAnimationsUpdate;

static std::set<TemporaryMapAnimation> _temporaryMapAnimations;

template<bool invalidateAllViewports>
static void Invalidate(
    const Viewport* const viewport, const int32_t x, const int32_t y, const int32_t z0, const int32_t z1,
    const ZoomLevel maxZoom)
{
    if constexpr (invalidateAllViewports)
    {
        ViewportsInvalidate(x, y, z0, z1, maxZoom);
    }
    else
    {
        viewport->Invalidate(x, y, z0, z1, maxZoom);
    }
}

template<bool invalidate, bool invalidateAllViewports>
static bool UpdateEntranceAnimation(
    const EntranceElement& entrance, const CoordsXYZ& loc, const int32_t baseZ, const Viewport* const viewport)
{
    if (entrance.GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        const auto* const ride = GetRide(entrance.GetRideIndex());
        if (ride != nullptr)
        {
            const auto* const stationObj = ride->getStationObject();
            if (stationObj != nullptr)
            {
                if constexpr (invalidate)
                {
                    Invalidate<invalidateAllViewports>(
                        viewport, loc.x, loc.y, baseZ + stationObj->Height + 8, baseZ + stationObj->Height + 24,
                        kMaxScrollingTextZoom);
                }
                return true;
            }
        }
    }
    else if (entrance.GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE && entrance.GetSequenceIndex() == 0)
    {
        if constexpr (invalidate)
        {
            const int32_t direction = (entrance.GetDirection() + GetCurrentRotation()) & 3;
            if (direction == TILE_ELEMENT_DIRECTION_SOUTH || direction == TILE_ELEMENT_DIRECTION_WEST)
            {
                Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ + 32, baseZ + 64, kMaxScrollingTextZoom);
            }
        }
        return true;
    }

    return false;
}

template<bool invalidate, bool invalidateAllViewports>
static bool UpdatePathAnimation(
    const PathElement& path, const CoordsXYZ& loc, const int32_t baseZ, const Viewport* const viewport)
{
    if (path.IsQueue() && path.HasQueueBanner())
    {
        if constexpr (invalidate)
        {
            const int32_t direction = (path.GetQueueBannerDirection() + GetCurrentRotation()) & 3;
            if (direction == TILE_ELEMENT_DIRECTION_NORTH || direction == TILE_ELEMENT_DIRECTION_EAST)
            {
                Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ + 16, baseZ + 30, kMaxScrollingTextZoom);
            }
        }
        return true;
    }
    return false;
}

template<bool invalidate, bool invalidateAllViewports>
static std::optional<UpdateType> UpdateSmallSceneryAnimation(
    const SmallSceneryElement& scenery, const CoordsXYZ& loc, const int32_t baseZ, const Viewport* const viewport)
{
    const auto* const entry = scenery.GetEntry();
    if (entry == nullptr)
    {
        return std::nullopt;
    }

    if (entry->HasFlag(
            SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1 | SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4 | SMALL_SCENERY_FLAG_SWAMP_GOO
            | SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS | SMALL_SCENERY_FLAG_IS_CLOCK))
    {
        auto animationType = UpdateType::invalidate;
        if (entry->HasFlag(SMALL_SCENERY_FLAG_IS_CLOCK))
        {
            animationType = UpdateType::update;

            // Don't apply anything to peeps when the scenery is a ghost.
            if (!scenery.IsGhost() && !(getGameState().currentTicks & 0x3FF))
            {
                const int32_t direction = scenery.GetDirection();
                auto quad = EntityTileList<Peep>(CoordsXY{ loc.x, loc.y } - CoordsDirectionDelta[direction]);
                for (auto peep : quad)
                {
                    if (peep->State != PeepState::walking || peep->z != baseZ || !peep->IsActionInterruptableSafely())
                        continue;
                    peep->Action = PeepActionType::checkTime;
                    peep->AnimationFrameNum = 0;
                    peep->AnimationImageIdOffset = 0;
                    peep->UpdateCurrentAnimationType();
                    if constexpr (invalidate)
                    {
                        peep->Invalidate();
                    }
                    break;
                }
                if constexpr (invalidate)
                {
                    Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ, scenery.GetClearanceZ(), kMaxZoom);
                }
            }
        }
        else if constexpr (invalidate)
        {
            Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ, scenery.GetClearanceZ(), kMaxZoom);
        }
        return std::optional(animationType);
    }

    return std::nullopt;
}

template<bool invalidate, bool invalidateAllViewports>
static bool UpdateTrackAnimation(TrackElement& track, const CoordsXYZ& loc, const int32_t baseZ, const Viewport* const viewport)
{
    switch (track.GetTrackType())
    {
        case TrackElemType::waterfall:
            if constexpr (invalidate)
            {
                Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ + 14, baseZ + 46, kMaxZoom);
            }
            return true;
        case TrackElemType::rapids:
        case TrackElemType::whirlpool:
            if constexpr (invalidate)
            {
                Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ + 14, baseZ + 18, kMaxZoom);
            }
            return true;
        case TrackElemType::spinningTunnel:
            if constexpr (invalidate)
            {
                Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ + 14, baseZ + 32, kMaxZoom);
            }
            return true;
        default:
            break;
    }

    return false;
}

template<bool invalidate, bool invalidateAllViewports>
static bool UpdateLargeSceneryAnimation(
    const LargeSceneryElement& scenery, const CoordsXYZ& loc, const int32_t baseZ, const Viewport* const viewport)
{
    const auto* const entry = scenery.GetEntry();
    if (entry != nullptr && (entry->flags & LARGE_SCENERY_FLAG_ANIMATED))
    {
        if constexpr (invalidate)
        {
            Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ, baseZ + 16, kMaxZoom);
        }
        return true;
    }

    return false;
}

template<bool invalidate, bool invalidateAllViewports>
static std::optional<UpdateType> UpdateWallAnimation(
    WallElement& wall, const CoordsXYZ& loc, const int32_t baseZ, const Viewport* const viewport)
{
    const auto* const entry = wall.GetEntry();
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
                if constexpr (invalidate)
                {
                    Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ, baseZ + 32, kMaxZoom);
                }
            }
        }

        return removeAnim ? std::nullopt : std::optional(UpdateType::update);
    }
    else if ((entry->flags2 & WALL_SCENERY_2_ANIMATED) || entry->scrolling_mode != kScrollingModeNone)
    {
        if constexpr (invalidate)
        {
            Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ, baseZ + 16, kMaxZoom);
        }
        return std::optional(UpdateType::invalidate);
    }

    return std::nullopt;
}

template<bool invalidate, bool invalidateAllViewports>
static bool UpdateBannerAnimation(
    [[maybe_unused]] const BannerElement& banner, const CoordsXYZ& loc, const int32_t baseZ, const Viewport* const viewport)
{
    if constexpr (invalidate)
    {
        Invalidate<invalidateAllViewports>(viewport, loc.x, loc.y, baseZ, baseZ + 16, kMaxScrollingTextZoom);
    }
    return true;
}

template<bool invalidate, bool invalidateAllViewports>
static std::optional<UpdateType> UpdateTile(const TileCoordsXY& coords, const Viewport* const viewport)
{
    auto* tileElement = MapGetFirstElementAt(coords);
    if (tileElement == nullptr)
    {
        return std::nullopt;
    }

    bool hasAnimations = false;
    auto updateType = UpdateType::invalidate;
    do
    {
        const auto baseZ = tileElement->GetBaseZ();
        const CoordsXYZ loc{ coords.ToCoordsXY(), baseZ };

        switch (tileElement->GetType())
        {
            case TileElementType::Entrance:
                hasAnimations |= UpdateEntranceAnimation<invalidate, invalidateAllViewports>(
                    *tileElement->AsEntrance(), loc, baseZ, viewport);
                break;
            case TileElementType::Path:
                hasAnimations |= UpdatePathAnimation<invalidate, invalidateAllViewports>(
                    *tileElement->AsPath(), loc, baseZ, viewport);
                break;
            case TileElementType::SmallScenery:
            {
                const auto result = UpdateSmallSceneryAnimation<invalidate, invalidateAllViewports>(
                    *tileElement->AsSmallScenery(), loc, baseZ, viewport);
                if (result)
                {
                    hasAnimations |= true;
                    if (result.value() == UpdateType::update)
                    {
                        updateType = UpdateType::update;
                    }
                }
                break;
            }
            case TileElementType::Track:
                hasAnimations |= UpdateTrackAnimation<invalidate, invalidateAllViewports>(
                    *tileElement->AsTrack(), loc, baseZ, viewport);
                break;
            case TileElementType::Banner:
                hasAnimations |= UpdateBannerAnimation<invalidate, invalidateAllViewports>(
                    *tileElement->AsBanner(), loc, baseZ, viewport);
                break;
            case TileElementType::LargeScenery:
                hasAnimations |= UpdateLargeSceneryAnimation<invalidate, invalidateAllViewports>(
                    *tileElement->AsLargeScenery(), loc, baseZ, viewport);
                break;
            case TileElementType::Wall:
            {
                const auto result = UpdateWallAnimation<invalidate, invalidateAllViewports>(
                    *tileElement->AsWall(), loc, baseZ, viewport);
                if (result)
                {
                    hasAnimations |= true;
                    if (result.value() == UpdateType::update)
                    {
                        updateType = UpdateType::update;
                    }
                }
                break;
            }
            default:
                break;
        }
    } while (!(tileElement++)->IsLastForTile());

    return hasAnimations ? std::optional(updateType) : std::nullopt;
}

template<bool invalidate>
static bool UpdateOnRidePhotoAnimation(TrackElement& track, const CoordsXYZ& coords)
{
    if (track.IsTakingPhoto())
    {
        track.DecrementPhotoTimeout();
        if constexpr (invalidate)
        {
            ViewportsInvalidate(coords.x, coords.y, coords.z, track.GetClearanceZ(), kMaxZoom);
        }
        return true;
    }
    return false;
}

template<bool invalidate>
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
        if constexpr (invalidate)
        {
            ViewportsInvalidate(coords.x, coords.y, coords.z, coords.z + 32, kMaxZoom);
        }
    }
    else if (doorAState != kLandEdgeDoorFrameClosed && doorAState != kLandEdgeDoorFrameOpen)
    {
        track.SetDoorAState(doorAState + 1);
        if constexpr (invalidate)
        {
            ViewportsInvalidate(coords.x, coords.y, coords.z, coords.z + 32, kMaxZoom);
        }
        isAnimating = true;
    }

    const auto doorBState = track.GetDoorBState();
    if (doorBState >= kLandEdgeDoorFrameEnd)
    {
        track.SetDoorBState(kLandEdgeDoorFrameClosed);
        if constexpr (invalidate)
        {
            ViewportsInvalidate(coords.x, coords.y, coords.z, coords.z + 32, kMaxZoom);
        }
    }
    else if (doorBState != kLandEdgeDoorFrameClosed && doorBState != kLandEdgeDoorFrameOpen)
    {
        track.SetDoorBState(doorBState + 1);
        if constexpr (invalidate)
        {
            ViewportsInvalidate(coords.x, coords.y, coords.z, coords.z + 32, kMaxZoom);
        }
        isAnimating = true;
    }

    return isAnimating;
}

template<bool invalidate>
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
            case MapAnimations::TemporaryType::onRidePhoto:
            {
                if (tileElement->GetType() == TileElementType::Track && tileElement->BaseHeight == tileCoords.z
                    && tileElement->AsTrack()->GetTrackType() == TrackElemType::onRidePhoto)
                {
                    isAnimating |= UpdateOnRidePhotoAnimation<invalidate>(*tileElement->AsTrack(), animation.location);
                }
                break;
            }
            case MapAnimations::TemporaryType::landEdgeDoor:
            {
                if (tileElement->GetType() == TileElementType::Track && tileElement->BaseHeight == tileCoords.z)
                {
                    isAnimating |= UpdateLandEdgeDoorsAnimation<invalidate>(*tileElement->AsTrack(), animation.location);
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
            const auto* const wall = element.AsWall();
            const auto* const entry = wall->GetEntry();
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
            const auto* const scenery = element.AsSmallScenery();
            const auto* const entry = scenery->GetEntry();
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
            const auto* const scenery = element.AsLargeScenery();
            const auto* const entry = scenery->GetEntry();
            if (entry != nullptr && (entry->flags & LARGE_SCENERY_FLAG_ANIMATED))
            {
                return std::optional(UpdateType::invalidate);
            }
            break;
        }
        case TileElementType::Path:
        {
            const auto* const path = element.AsPath();
            if (path->HasQueueBanner())
            {
                return std::optional(UpdateType::invalidate);
            }
            break;
        }
        case TileElementType::Entrance:
        {
            const auto* const entrance = element.AsEntrance();
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
            const auto* const track = element.AsTrack();
            switch (track->GetTrackType())
            {
                case TrackElemType::waterfall:
                case TrackElemType::rapids:
                case TrackElemType::whirlpool:
                case TrackElemType::spinningTunnel:
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

void MapAnimations::MarkTileForInvalidation(const TileCoordsXY coords)
{
    if (!MapIsEdge(coords.ToCoordsXY()) && !_mapAnimationsUpdate.contains(coords))
    {
        _mapAnimationsInvalidate[coords.x + (coords.y * kMaximumMapSizeTechnical)] = true;
    }
}

void MapAnimations::MarkTileForUpdate(const TileCoordsXY coords)
{
    if (!MapIsEdge(coords.ToCoordsXY()))
    {
        _mapAnimationsInvalidate[coords.x + (coords.y * kMaximumMapSizeTechnical)] = false;
        _mapAnimationsUpdate.insert(coords);
    }
}

void MapAnimations::CreateTemporary(const CoordsXYZ& coords, const TemporaryType type)
{
    _temporaryMapAnimations.insert(TemporaryMapAnimation{ coords, type });
}

void MapAnimations::MarkAllTiles()
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
                    MarkTileForInvalidation(TileCoordsXY(it.x, it.y));
                    break;
                case UpdateType::update:
                    MarkTileForUpdate(TileCoordsXY(it.x, it.y));
                    break;
            }
        }
    }
}

static void InvalidateAll(const ViewportList& viewports)
{
    for (const auto* const viewport : viewports)
    {
        if (viewport->zoom > kMaxZoom)
        {
            continue;
        }

        // Code adapted from PaintSessionGenerateRotate.
        // Ideally this would iterate over tiles in memory order.
        constexpr int32_t maxTileHeightModifier = 88; // from magic value in PaintSessionGenerateRotate
        constexpr int32_t maxTileHeight = (kMaxTileElementHeight * kCoordsZStep) + maxTileHeightModifier;

        const auto direction = DirectionFlipXAxis(viewport->rotation);
        const int32_t numVerticalTiles = (viewport->ViewHeight() + maxTileHeight) / kScreenCoordsTileHeight;
        const TileCoordsXY nextVerticalTile = TileCoordsXY{ 1, 1 }.Rotate(direction);
        const int32_t screenCoordY = Numerics::floor2(
            (viewport->viewPos.y - kScreenCoordsTileHeightHalf), kScreenCoordsTileWidthHalf);

        for (int32_t x = viewport->viewPos.x; x < viewport->viewPos.x + viewport->ViewWidth() + kScreenCoordsTileWidth;
             x += kScreenCoordsTileWidthHalf)
        {
            const ScreenCoordsXY screenCoord = { Numerics::floor2(x, kScreenCoordsTileWidthHalf), screenCoordY };
            CoordsXY mapTile = { screenCoord.y - screenCoord.x / 2, screenCoord.y + screenCoord.x / 2 };
            mapTile = mapTile.Rotate(direction);
            if (direction & 1)
            {
                mapTile.y -= kCoordsXYHalfTile;
            }
            mapTile = mapTile.ToTileStart();
            TileCoordsXY tileCoords(mapTile);

            for (int32_t i = 0; i < numVerticalTiles; ++i)
            {
                if (!MapIsEdge(tileCoords.ToCoordsXY())
                    && _mapAnimationsInvalidate[tileCoords.x + (tileCoords.y * kMaximumMapSizeTechnical)])
                {
                    if (!UpdateTile<true, false>(tileCoords, viewport))
                    {
                        _mapAnimationsInvalidate[tileCoords.x + (tileCoords.y * kMaximumMapSizeTechnical)] = false;
                    }
                }
                tileCoords += nextVerticalTile;
            }
        }
    }
}

static bool IsTileVisible(const ViewportList& viewports, const TileCoordsXY tileCoords)
{
    for (const auto* const viewport : viewports)
    {
        if (viewport->ContainsTile(tileCoords))
        {
            return true;
        }
    }
    return false;
}

static void UpdateAll(const ViewportList& viewports)
{
    // Currently nothing updates on odd ticks.
    if (getGameState().currentTicks & 1)
        return;

    auto it = _mapAnimationsUpdate.begin();
    while (it != _mapAnimationsUpdate.end())
    {
        const bool isVisible = IsTileVisible(viewports, *it);
        const auto result = isVisible ? UpdateTile<true, true>(*it, nullptr) : UpdateTile<false, true>(*it, nullptr);
        if (result)
        {
            if (result.value() == UpdateType::invalidate)
            {
                const auto& coords = *it;
                _mapAnimationsInvalidate[coords.x + (coords.y * kMaximumMapSizeTechnical)] = true;
                it = _mapAnimationsUpdate.erase(it);
            }
            else
            {
                ++it;
            }
        }
        else
        {
            it = _mapAnimationsUpdate.erase(it);
        }
    }
}

static void UpdateAllTemporary(const ViewportList& viewports)
{
    auto it = _temporaryMapAnimations.begin();
    while (it != _temporaryMapAnimations.end())
    {
        const auto& animation = *it;
        const bool isVisible = IsTileVisible(viewports, TileCoordsXY(animation.location));
        const auto result = isVisible ? UpdateTemporaryAnimation<true>(*it) : UpdateTemporaryAnimation<false>(*it);
        if (result)
        {
            ++it;
        }
        else
        {
            it = _temporaryMapAnimations.erase(it);
        }
    }
}

void MapAnimations::InvalidateAndUpdateAll()
{
    PROFILED_FUNCTION();

    const auto viewports = GetVisibleViewports();
    InvalidateAll(viewports);
    UpdateAll(viewports);
    UpdateAllTemporary(viewports);
}

void MapAnimations::ClearAll()
{
    _mapAnimationsInvalidate.assign(_mapAnimationsInvalidate.size(), false);
    _mapAnimationsUpdate.clear();
    _temporaryMapAnimations.clear();
}

void MapAnimations::ShiftAll(const TileCoordsXY amount)
{
    if (amount.x == 0 && amount.y == 0)
        return;

    std::vector<bool> newMapAnimationsInvalidate(_mapAnimationsInvalidate.size(), false);
    for (int32_t y = 0; y < kMaximumMapSizeTechnical; y++)
    {
        for (int32_t x = 0; x < kMaximumMapSizeTechnical; x++)
        {
            const bool animated = _mapAnimationsInvalidate[x + (y * kMaximumMapSizeTechnical)];
            const TileCoordsXY newCoords = TileCoordsXY(x, y) + amount;
            if (!MapIsEdge(newCoords.ToCoordsXY()))
            {
                newMapAnimationsInvalidate[newCoords.x + (newCoords.y * kMaximumMapSizeTechnical)] = animated;
            }
        }
    }
    _mapAnimationsInvalidate = std::move(newMapAnimationsInvalidate);

    std::set<TileCoordsXY, TileCoordsXYCmp> newMapAnimationsUpdate;
    for (const auto a : _mapAnimationsUpdate)
    {
        newMapAnimationsUpdate.insert(a + amount);
    }
    _mapAnimationsUpdate = std::move(newMapAnimationsUpdate);

    std::set<TemporaryMapAnimation> newTemporaryMapAnimations;
    for (const auto& a : _temporaryMapAnimations)
    {
        newTemporaryMapAnimations.insert(TemporaryMapAnimation{ a.location + CoordsXYZ(amount.ToCoordsXY(), 0), a.type });
    }
    _temporaryMapAnimations = std::move(newTemporaryMapAnimations);
}
