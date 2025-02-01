/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ConstructionClearance.h"

#include "../Game.h"
#include "../GameState.h"
#include "../localisation/Formatter.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/SmallSceneryEntry.h"
#include "../object/WallSceneryEntry.h"
#include "../openrct2/Cheats.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "Park.h"
#include "QuarterTile.h"
#include "Scenery.h"
#include "tile_element/EntranceElement.h"
#include "tile_element/LargeSceneryElement.h"
#include "tile_element/PathElement.h"
#include "tile_element/Slope.h"
#include "tile_element/SmallSceneryElement.h"
#include "tile_element/SurfaceElement.h"
#include "tile_element/TileElement.h"
#include "tile_element/TrackElement.h"
#include "tile_element/WallElement.h"

using namespace OpenRCT2;

static int32_t MapPlaceClearFunc(
    TileElement** tile_element, const CoordsXY& coords, uint8_t flags, money64* price, bool is_scenery)
{
    if ((*tile_element)->GetType() != TileElementType::SmallScenery)
        return 1;

    if (is_scenery && !(flags & GAME_COMMAND_FLAG_TRACK_DESIGN))
        return 1;

    auto* scenery = (*tile_element)->AsSmallScenery()->GetEntry();

    auto& gameState = GetGameState();
    if (gameState.Park.Flags & PARK_FLAGS_FORBID_TREE_REMOVAL)
    {
        if (scenery != nullptr && scenery->HasFlag(SMALL_SCENERY_FLAG_IS_TREE))
            return 1;
    }

    if (!(gameState.Park.Flags & PARK_FLAGS_NO_MONEY) && scenery != nullptr)
        *price += scenery->removal_price;

    if (flags & GAME_COMMAND_FLAG_GHOST)
        return 0;

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
        return 0;

    MapInvalidateTile({ coords, (*tile_element)->GetBaseZ(), (*tile_element)->GetClearanceZ() });

    TileElementRemove(*tile_element);

    (*tile_element)--;
    return 0;
}

/**
 *
 *  rct2: 0x006E0D6E, 0x006B8D88
 */
int32_t MapPlaceSceneryClearFunc(TileElement** tile_element, const CoordsXY& coords, uint8_t flags, money64* price)
{
    return MapPlaceClearFunc(tile_element, coords, flags, price, /*is_scenery=*/true);
}

/**
 *
 *  rct2: 0x006C5A4F, 0x006CDE57, 0x006A6733, 0x0066637E
 */
int32_t MapPlaceNonSceneryClearFunc(TileElement** tile_element, const CoordsXY& coords, uint8_t flags, money64* price)
{
    return MapPlaceClearFunc(tile_element, coords, flags, price, /*is_scenery=*/false);
}

static bool MapLoc68BABCShouldContinue(
    TileElement** tileElementPtr, const CoordsXYRangedZ& pos, CLEAR_FUNC clearFunc, uint8_t flags, money64& price,
    CreateCrossingMode crossingMode, bool canBuildCrossing)
{
    if (clearFunc != nullptr)
    {
        if (!clearFunc(tileElementPtr, pos, flags, &price))
        {
            return true;
        }
    }

    auto tileElement = *tileElementPtr;
    if (crossingMode == CreateCrossingMode::trackOverPath && canBuildCrossing && tileElement->GetType() == TileElementType::Path
        && tileElement->GetBaseZ() == pos.baseZ && !tileElement->AsPath()->IsQueue() && !tileElement->AsPath()->IsSloped())
    {
        return true;
    }
    else if (
        crossingMode == CreateCrossingMode::pathOverTrack && canBuildCrossing
        && tileElement->GetType() == TileElementType::Track && tileElement->GetBaseZ() == pos.baseZ
        && tileElement->AsTrack()->GetTrackType() == TrackElemType::Flat)
    {
        auto ride = GetRide(tileElement->AsTrack()->GetRideIndex());
        if (ride != nullptr && ride->GetRideTypeDescriptor().HasFlag(RtdFlag::supportsLevelCrossings))
        {
            return true;
        }
    }

    return false;
}

/**
 *
 *  rct2: 0x0068B932
 *  ax = x
 *  cx = y
 *  dl = zLow
 *  dh = zHigh
 *  ebp = clearFunc
 *  bl = bl
 */
GameActions::Result MapCanConstructWithClearAt(
    const CoordsXYRangedZ& pos, CLEAR_FUNC clearFunc, QuarterTile quarterTile, uint8_t flags, CreateCrossingMode crossingMode,
    bool isTree)
{
    auto res = GameActions::Result();

    uint8_t groundFlags = ELEMENT_IS_ABOVE_GROUND;
    res.SetData(ConstructClearResult{ groundFlags });

    bool canBuildCrossing = false;
    if (MapIsEdge(pos))
    {
        res.Error = GameActions::Status::InvalidParameters;
        res.ErrorMessage = STR_OFF_EDGE_OF_MAP;
        return res;
    }

    if (GetGameState().Cheats.disableClearanceChecks)
    {
        res.SetData(ConstructClearResult{ groundFlags });
        return res;
    }

    TileElement* tileElement = MapGetFirstElementAt(pos);
    if (tileElement == nullptr)
    {
        res.Error = GameActions::Status::Unknown;
        res.ErrorMessage = kStringIdNone;
        return res;
    }

    do
    {
        if (tileElement->GetType() != TileElementType::Surface)
        {
            if (pos.baseZ < tileElement->GetClearanceZ() && pos.clearanceZ > tileElement->GetBaseZ()
                && !(tileElement->IsGhost()))
            {
                if (tileElement->GetOccupiedQuadrants() & (quarterTile.GetBaseQuarterOccupied()))
                {
                    if (MapLoc68BABCShouldContinue(
                            &tileElement, pos, clearFunc, flags, res.Cost, crossingMode, canBuildCrossing))
                    {
                        continue;
                    }

                    MapGetObstructionErrorText(tileElement, res);
                    res.Error = GameActions::Status::NoClearance;
                    return res;
                }
            }
            continue;
        }

        const auto waterHeight = tileElement->AsSurface()->GetWaterHeight();
        if (waterHeight && waterHeight > pos.baseZ && tileElement->GetBaseZ() < pos.clearanceZ)
        {
            groundFlags |= ELEMENT_IS_UNDERWATER;
            if (waterHeight < pos.clearanceZ)
            {
                if (clearFunc != nullptr && clearFunc(&tileElement, pos, flags, &res.Cost))
                {
                    res.Error = GameActions::Status::NoClearance;
                    res.ErrorMessage = STR_CANNOT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_WATER;
                    return res;
                }
            }
        }

        if (GetGameState().Park.Flags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION && !isTree)
        {
            const auto heightFromGround = pos.clearanceZ - tileElement->GetBaseZ();

            if (heightFromGround > (18 * kCoordsZStep))
            {
                res.Error = GameActions::Status::Disallowed;
                res.ErrorMessage = STR_LOCAL_AUTHORITY_WONT_ALLOW_CONSTRUCTION_ABOVE_TREE_HEIGHT;
                return res;
            }
        }

        // Only allow building crossings directly on a flat surface tile.
        if (tileElement->GetType() == TileElementType::Surface && (tileElement->AsSurface()->GetSlope()) == kTileSlopeFlat
            && tileElement->GetBaseZ() == pos.baseZ)
        {
            canBuildCrossing = true;
        }

        if (quarterTile.GetZQuarterOccupied() != 0b1111)
        {
            if (tileElement->GetBaseZ() >= pos.clearanceZ)
            {
                // Loc68BA81
                groundFlags |= ELEMENT_IS_UNDERGROUND;
                groundFlags &= ~ELEMENT_IS_ABOVE_GROUND;
            }
            else
            {
                auto northZ = tileElement->GetBaseZ();
                auto eastZ = northZ;
                auto southZ = northZ;
                auto westZ = northZ;
                const auto slope = tileElement->AsSurface()->GetSlope();
                if (slope & kTileSlopeNCornerUp)
                {
                    northZ += kLandHeightStep;
                    if (slope == (kTileSlopeSCornerDown | kTileSlopeDiagonalFlag))
                        northZ += kLandHeightStep;
                }
                if (slope & kTileSlopeECornerUp)
                {
                    eastZ += kLandHeightStep;
                    if (slope == (kTileSlopeWCornerDown | kTileSlopeDiagonalFlag))
                        eastZ += kLandHeightStep;
                }
                if (slope & kTileSlopeSCornerUp)
                {
                    southZ += kLandHeightStep;
                    if (slope == (kTileSlopeNCornerDown | kTileSlopeDiagonalFlag))
                        southZ += kLandHeightStep;
                }
                if (slope & kTileSlopeWCornerUp)
                {
                    westZ += kLandHeightStep;
                    if (slope == (kTileSlopeECornerDown | kTileSlopeDiagonalFlag))
                        westZ += kLandHeightStep;
                }
                const auto baseHeight = pos.baseZ + (4 * kCoordsZStep);
                const auto baseQuarter = quarterTile.GetBaseQuarterOccupied();
                const auto zQuarter = quarterTile.GetZQuarterOccupied();
                if ((!(baseQuarter & 0b0001) || ((zQuarter & 0b0001 || pos.baseZ >= northZ) && baseHeight >= northZ))
                    && (!(baseQuarter & 0b0010) || ((zQuarter & 0b0010 || pos.baseZ >= eastZ) && baseHeight >= eastZ))
                    && (!(baseQuarter & 0b0100) || ((zQuarter & 0b0100 || pos.baseZ >= southZ) && baseHeight >= southZ))
                    && (!(baseQuarter & 0b1000) || ((zQuarter & 0b1000 || pos.baseZ >= westZ) && baseHeight >= westZ)))
                {
                    continue;
                }

                if (MapLoc68BABCShouldContinue(&tileElement, pos, clearFunc, flags, res.Cost, crossingMode, canBuildCrossing))
                {
                    continue;
                }

                MapGetObstructionErrorText(tileElement, res);
                res.Error = GameActions::Status::NoClearance;
                return res;
            }
        }
    } while (!(tileElement++)->IsLastForTile());

    res.SetData(ConstructClearResult{ groundFlags });

    return res;
}

GameActions::Result MapCanConstructAt(const CoordsXYRangedZ& pos, QuarterTile bl)
{
    return MapCanConstructWithClearAt(pos, nullptr, bl, 0);
}

/**
 *
 *  rct2: 0x0068BB18
 */
void MapGetObstructionErrorText(TileElement* tileElement, GameActions::Result& res)
{
    Ride* ride;

    res.ErrorMessage = STR_OBJECT_IN_THE_WAY;
    switch (tileElement->GetType())
    {
        case TileElementType::Surface:
            res.ErrorMessage = STR_RAISE_OR_LOWER_LAND_FIRST;
            break;
        case TileElementType::Path:
            res.ErrorMessage = STR_FOOTPATH_IN_THE_WAY;
            break;
        case TileElementType::Track:
            ride = GetRide(tileElement->AsTrack()->GetRideIndex());
            if (ride != nullptr)
            {
                res.ErrorMessage = STR_X_IN_THE_WAY;

                Formatter ft(res.ErrorMessageArgs.data());
                ride->FormatNameTo(ft);
            }
            break;
        case TileElementType::SmallScenery:
        {
            auto* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
            res.ErrorMessage = STR_X_IN_THE_WAY;
            auto ft = Formatter(res.ErrorMessageArgs.data());
            StringId stringId = sceneryEntry != nullptr ? sceneryEntry->name : static_cast<StringId>(kStringIdEmpty);
            ft.Add<StringId>(stringId);
            break;
        }
        case TileElementType::Entrance:
            switch (tileElement->AsEntrance()->GetEntranceType())
            {
                case ENTRANCE_TYPE_RIDE_ENTRANCE:
                    res.ErrorMessage = STR_RIDE_ENTRANCE_IN_THE_WAY;
                    break;
                case ENTRANCE_TYPE_RIDE_EXIT:
                    res.ErrorMessage = STR_RIDE_EXIT_IN_THE_WAY;
                    break;
                case ENTRANCE_TYPE_PARK_ENTRANCE:
                    res.ErrorMessage = STR_PARK_ENTRANCE_IN_THE_WAY;
                    break;
            }
            break;
        case TileElementType::Wall:
        {
            auto* wallEntry = tileElement->AsWall()->GetEntry();
            res.ErrorMessage = STR_X_IN_THE_WAY;
            auto ft = Formatter(res.ErrorMessageArgs.data());
            StringId stringId = wallEntry != nullptr ? wallEntry->name : static_cast<StringId>(kStringIdEmpty);
            ft.Add<StringId>(stringId);
            break;
        }
        case TileElementType::LargeScenery:
        {
            auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            res.ErrorMessage = STR_X_IN_THE_WAY;
            auto ft = Formatter(res.ErrorMessageArgs.data());
            StringId stringId = sceneryEntry != nullptr ? sceneryEntry->name : static_cast<StringId>(kStringIdEmpty);
            ft.Add<StringId>(stringId);
            break;
        }
        case TileElementType::Banner:
            break;
    }
}
