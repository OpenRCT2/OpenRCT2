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
#include <vector>

using namespace OpenRCT2;

// A list of tile coordinates which contains animated tile elements.
static std::vector<TileCoordsXY> _mapAnimations;

static void InvalidateTile(const CoordsXYZ& loc, int32_t zMin, int32_t zMax)
{
    MapInvalidateTileZoom1({ loc, zMin, zMax });
}

static bool UpdateEntranceAnimation(EntranceElement* entrance, CoordsXYZ& loc, int32_t baseZ)
{
    if (entrance->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        auto ride = GetRide(entrance->GetRideIndex());
        if (ride != nullptr)
        {
            auto stationObj = ride->getStationObject();
            if (stationObj != nullptr)
            {
                InvalidateTile(loc, baseZ + stationObj->Height + 8, baseZ + stationObj->Height + 24);
                return true;
            }
        }
    }
    else if (entrance->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE && entrance->GetSequenceIndex() == 0)
    {
        InvalidateTile(loc, baseZ + 32, baseZ + 64);
        return true;
    }

    return false;
}

static bool UpdatePathAnimation(PathElement* path, CoordsXYZ& loc, int32_t baseZ)
{
    if (path->IsQueue() && path->HasQueueBanner())
    {
        int32_t direction = (path->GetQueueBannerDirection() + GetCurrentRotation()) & 3;
        if (direction == TILE_ELEMENT_DIRECTION_NORTH || direction == TILE_ELEMENT_DIRECTION_EAST)
        {
            InvalidateTile(loc, baseZ + 16, baseZ + 30);
            return true;
        }
    }
    return false;
}

static bool UpdateSmallSceneryAnimation(SmallSceneryElement* scenery, CoordsXYZ& loc, int32_t baseZ)
{
    auto entry = scenery->GetEntry();
    if (entry == nullptr)
    {
        return false;
    }

    if (entry->HasFlag(
            SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1 | SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4 | SMALL_SCENERY_FLAG_SWAMP_GOO
            | SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS | SMALL_SCENERY_FLAG_IS_CLOCK))
    {
        // Don't apply anything to peeps when the scenery is a ghost.
        if (!scenery->IsGhost())
        {
            if (entry->HasFlag(SMALL_SCENERY_FLAG_IS_CLOCK) && !(getGameState().currentTicks & 0x3FF) && !GameIsPaused())
            {
                int32_t direction = scenery->GetDirection();
                auto quad = EntityTileList<Peep>(CoordsXY{ loc } - CoordsDirectionDelta[direction]);
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
        auto clearZ = scenery->GetClearanceZ();
        InvalidateTile(loc, baseZ, clearZ);
        return true;
    }

    return false;
}

static bool UpdateTrackAnimation(TrackElement* track, CoordsXYZ& loc, int32_t baseZ)
{
    auto clearZ = track->GetClearanceZ();
    switch (track->GetTrackType())
    {
        case TrackElemType::Waterfall:
            InvalidateTile(loc, baseZ + 14, baseZ + 46);
            return true;
        case TrackElemType::Rapids:
        case TrackElemType::Whirlpool:
            InvalidateTile(loc, baseZ + 14, baseZ + 18);
            return true;
        case TrackElemType::SpinningTunnel:
            InvalidateTile(loc, baseZ + 14, baseZ + 32);
            return true;
        case TrackElemType::OnRidePhoto:
            if (!GameIsPaused() && track->IsTakingPhoto())
            {
                InvalidateTile(loc, baseZ, clearZ);
                track->DecrementPhotoTimeout();
            }
            return true;
        default:
            break;
    }

    return false;
}

static bool UpdateLargeSceneryAnimation(LargeSceneryElement* scenery, CoordsXYZ& loc, int32_t baseZ)
{
    auto entry = scenery->GetEntry();
    if (entry != nullptr && (entry->flags & LARGE_SCENERY_FLAG_ANIMATED))
    {
        InvalidateTile(loc, baseZ, baseZ + 16);
        return true;
    }

    return false;
}

static bool UpdateWallAnimation(WallElement* wall, CoordsXYZ& loc, int32_t baseZ)
{
    auto entry = wall->GetEntry();
    if (entry == nullptr)
    {
        return false;
    }

    if (entry->flags & WALL_SCENERY_IS_DOOR)
    {
        if (!(getGameState().currentTicks & 1) && !GameIsPaused())
        {
            const auto currentFrame = wall->GetAnimationFrame();
            if (currentFrame != 0)
            {
                auto newFrame = currentFrame;
                if (currentFrame == 15)
                {
                    newFrame = 0;
                }
                else if (currentFrame != 5)
                {
                    newFrame++;
                    if (newFrame == 13 && !(entry->flags & WALL_SCENERY_LONG_DOOR_ANIMATION))
                        newFrame = 15;
                }

                if (currentFrame != newFrame)
                {
                    wall->SetAnimationFrame(newFrame);
                    InvalidateTile(loc, baseZ, baseZ + 32);
                }
            }
        }

        return true;
    }
    else if ((entry->flags2 & WALL_SCENERY_2_ANIMATED) || entry->scrolling_mode != SCROLLING_MODE_NONE)
    {
        InvalidateTile(loc, baseZ, baseZ + 16);
        return true;
    }

    return false;
}

static bool UpdateBannerAnimation([[maybe_unused]] BannerElement* banner, CoordsXYZ& loc, int32_t baseZ)
{
    InvalidateTile(loc, baseZ, baseZ + 16);
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
        auto baseZ = tileElement->GetBaseZ();
        CoordsXYZ loc{ coords.ToCoordsXY(), baseZ };

        switch (tileElement->GetType())
        {
            case TileElementType::Entrance:
                hasAnimations |= UpdateEntranceAnimation(tileElement->AsEntrance(), loc, baseZ);
                break;
            case TileElementType::Path:
                hasAnimations |= UpdatePathAnimation(tileElement->AsPath(), loc, baseZ);
                break;
            case TileElementType::SmallScenery:
                hasAnimations |= UpdateSmallSceneryAnimation(tileElement->AsSmallScenery(), loc, baseZ);
                break;
            case TileElementType::Track:
                hasAnimations |= UpdateTrackAnimation(tileElement->AsTrack(), loc, baseZ);
                break;
            case TileElementType::Banner:
                hasAnimations |= UpdateBannerAnimation(tileElement->AsBanner(), loc, baseZ);
                break;
            case TileElementType::LargeScenery:
                hasAnimations |= UpdateLargeSceneryAnimation(tileElement->AsLargeScenery(), loc, baseZ);
                break;
            case TileElementType::Wall:
                hasAnimations |= UpdateWallAnimation(tileElement->AsWall(), loc, baseZ);
                break;
            default:
                break;
        }
    } while (!(tileElement++)->IsLastForTile());

    return hasAnimations;
}

static void MarkTileAnimated(const CoordsXY& loc)
{
    auto coords = TileCoordsXY{ loc };

    auto it = std::lower_bound(_mapAnimations.begin(), _mapAnimations.end(), coords, [](auto& a, auto& b) {
        // NOTE: Ordering should match GetFirstElementAt which is x + (y * size)
        return std::tie(a.y, a.x) < std::tie(b.y, a.x);
    });

    if (it != _mapAnimations.end() && *it == coords)
    {
        return; // Already exists
    }

    _mapAnimations.insert(it, coords);
}

void MapAnimationUpdateAll()
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

void ClearMapAnimations()
{
    _mapAnimations.clear();
}

static bool IsElementAnimated(const TileElementBase* el)
{
    if (el == nullptr)
    {
        return false;
    }

    switch (el->GetType())
    {
        case TileElementType::Banner:
            return true;
        case TileElementType::Wall:
        {
            auto wall = el->AsWall();
            auto entry = wall->GetEntry();
            if (entry != nullptr && ((entry->flags2 & WALL_SCENERY_2_ANIMATED) || entry->scrolling_mode != SCROLLING_MODE_NONE))
            {
                return true;
            }
            break;
        }
        case TileElementType::SmallScenery:
        {
            auto scenery = el->AsSmallScenery();
            auto entry = scenery->GetEntry();
            if (entry != nullptr && entry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED))
            {
                return true;
            }
            break;
        }
        case TileElementType::LargeScenery:
        {
            auto scenery = el->AsLargeScenery();
            auto entry = scenery->GetEntry();
            if (entry != nullptr && (entry->flags & LARGE_SCENERY_FLAG_ANIMATED))
            {
                return true;
            }
            break;
        }
        case TileElementType::Path:
        {
            auto path = el->AsPath();
            if (path->HasQueueBanner())
            {
                return true;
            }
            break;
        }
        case TileElementType::Entrance:
        {
            auto entrance = el->AsEntrance();
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
            auto track = el->AsTrack();
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

void MapAnimationCreate(const CoordsXY& coords, const TileElementBase* tileElement)
{
    if (!IsElementAnimated(tileElement))
    {
        return;
    }

    MarkTileAnimated(coords);
}

void MapAnimationCreate(const CoordsXY& loc)
{
    auto* tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return;

    do
    {
        MapAnimationCreate(loc, tileElement);
    } while (!(tileElement++)->IsLastForTile());
}

void MapAnimationAutoCreate()
{
    ClearMapAnimations();

    TileElementIterator it;
    TileElementIteratorBegin(&it);
    while (TileElementIteratorNext(&it))
    {
        MapAnimationCreate(TileCoordsXY(it.x, it.y).ToCoordsXY(), it.element);
    }
}

void ShiftAllMapAnimations(CoordsXY amount)
{
    if (amount.x == 0 && amount.y == 0)
        return;

    for (auto& a : _mapAnimations)
    {
        a += TileCoordsXY{ amount };
    }
}
