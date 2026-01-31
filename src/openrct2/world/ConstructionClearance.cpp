/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../ride/TrackData.h"
#include "Map.h"
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
using OpenRCT2::GameActions::CommandFlag;
using OpenRCT2::GameActions::CommandFlags;

static bool MapPlaceClearFunc(
    TileElement** tile_element, const CoordsXY& coords, CommandFlags flags, money64* price, bool is_scenery)
{
    if ((*tile_element)->GetType() != TileElementType::SmallScenery)
        return false;

    if (is_scenery && !flags.has(CommandFlag::trackDesign))
        return false;

    auto* scenery = (*tile_element)->AsSmallScenery()->GetEntry();

    auto& park = getGameState().park;
    if (park.flags & PARK_FLAGS_FORBID_TREE_REMOVAL)
    {
        if (scenery != nullptr && scenery->HasFlag(SMALL_SCENERY_FLAG_IS_TREE))
            return false;
    }

    if (!(park.flags & PARK_FLAGS_NO_MONEY) && scenery != nullptr)
        *price += scenery->removal_price;

    if (flags.has(CommandFlag::ghost))
        return true;

    if (!flags.has(CommandFlag::apply))
        return true;

    MapInvalidateTile({ coords, (*tile_element)->GetBaseZ(), (*tile_element)->GetClearanceZ() });

    TileElementRemove(*tile_element);

    (*tile_element)--;
    return true;
}

/**
 *
 *  rct2: 0x006E0D6E, 0x006B8D88
 */
bool MapPlaceSceneryClearFunc(TileElement** tile_element, const CoordsXY& coords, CommandFlags flags, money64* price)
{
    return MapPlaceClearFunc(tile_element, coords, flags, price, /*is_scenery=*/true);
}

/**
 *
 *  rct2: 0x006C5A4F, 0x006CDE57, 0x006A6733, 0x0066637E
 */
bool MapPlaceNonSceneryClearFunc(TileElement** tile_element, const CoordsXY& coords, CommandFlags flags, money64* price)
{
    return MapPlaceClearFunc(tile_element, coords, flags, price, /*is_scenery=*/false);
}

static bool landSlopeFitsUnderTrack(int32_t baseZ, uint8_t slope, const TrackElement& trackElement)
{
    const auto [slopeNorthZ, slopeEastZ, slopeSouthZ, slopeWestZ] = GetSlopeCornerHeights(baseZ, slope);

    const TrackElemType trackElemType = trackElement.GetTrackType();
    const auto& ted = TrackMetaData::GetTrackElementDescriptor(trackElemType);
    const uint8_t sequenceIndex = trackElemType == TrackElemType::maze ? 0 : trackElement.GetSequenceIndex();
    const auto& trackClearances = ted.sequences[sequenceIndex].clearance;
    const auto trackQuarters = trackClearances.quarterTile.Rotate(trackElement.GetDirection());
    const auto trackQuarterHeights = trackQuarters.GetQuarterHeights(trackElement.GetBaseZ());
    const uint8_t trackOccupiedQuarters = trackQuarters.GetBaseQuarterOccupied();

    if ((!(trackOccupiedQuarters & 0b0001) || slopeNorthZ <= trackQuarterHeights.north)
        && (!(trackOccupiedQuarters & 0b0010) || slopeEastZ <= trackQuarterHeights.east)
        && (!(trackOccupiedQuarters & 0b0100) || slopeSouthZ <= trackQuarterHeights.south)
        && (!(trackOccupiedQuarters & 0b1000) || slopeWestZ <= trackQuarterHeights.west))
    {
        return true;
    }

    return false;
}

static bool landSlopeFitsUnderPath(int32_t baseZ, uint8_t slope, const PathElement& pathElement)
{
    const auto slopeCornerHeights = GetSlopeCornerHeights(baseZ, slope);

    const uint8_t pathSlope = Numerics::rol4(kTileSlopeSWSideUp, pathElement.GetSlopeDirection());
    const auto pathCornerHeights = GetSlopeCornerHeights(pathElement.GetBaseZ(), pathSlope);

    return (slopeCornerHeights <= pathCornerHeights);
}

static bool MapLoc68BABCShouldContinue(
    TileElement** tileElementPtr, const CoordsXYRangedZ& pos, ClearingFunction clearFunc, const CommandFlags flags,
    money64& price, const CreateCrossingMode crossingMode, const bool canBuildCrossing, const uint8_t slope)
{
    if (clearFunc(tileElementPtr, pos, flags, &price))
    {
        return true;
    }

    const TileElement* const tileElement = *tileElementPtr;

    if (slope != kTileSlopeFlat && tileElement->GetType() == TileElementType::Track)
    {
        if (landSlopeFitsUnderTrack(pos.baseZ, slope, *tileElement->AsTrack()))
        {
            return true;
        }
    }

    if (slope != kTileSlopeFlat && tileElement->GetType() == TileElementType::Path && tileElement->AsPath()->IsSloped())
    {
        if (landSlopeFitsUnderPath(pos.baseZ, slope, *tileElement->AsPath()))
        {
            return true;
        }
    }

    if (crossingMode == CreateCrossingMode::trackOverPath && canBuildCrossing && tileElement->GetType() == TileElementType::Path
        && tileElement->GetBaseZ() == pos.baseZ && !tileElement->AsPath()->IsQueue() && !tileElement->AsPath()->IsSloped())
    {
        return true;
    }
    else if (
        crossingMode == CreateCrossingMode::pathOverTrack && canBuildCrossing
        && tileElement->GetType() == TileElementType::Track && tileElement->GetBaseZ() == pos.baseZ
        && tileElement->AsTrack()->GetTrackType() == TrackElemType::flat)
    {
        auto ride = GetRide(tileElement->AsTrack()->GetRideIndex());
        if (ride != nullptr && ride->getRideTypeDescriptor().HasFlag(RtdFlag::supportsLevelCrossings))
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
    const CoordsXYRangedZ& pos, ClearingFunction clearFunc, const QuarterTile quarterTile, const CommandFlags flags,
    const uint8_t slope, const CreateCrossingMode crossingMode, const bool isTree, const RideId ignoreRideId)
{
    auto res = GameActions::Result();

    uint8_t groundFlags = ELEMENT_IS_ABOVE_GROUND;
    res.setData(ConstructClearResult{ groundFlags });

    bool canBuildCrossing = false;
    if (MapIsEdge(pos))
    {
        res.error = GameActions::Status::invalidParameters;
        res.errorMessage = STR_OFF_EDGE_OF_MAP;
        return res;
    }

    if (getGameState().cheats.disableClearanceChecks)
    {
        res.setData(ConstructClearResult{ groundFlags });
        return res;
    }

    TileElement* tileElement = MapGetFirstElementAt(pos);
    if (tileElement == nullptr)
    {
        res.error = GameActions::Status::unknown;
        res.errorMessage = kStringIdNone;
        return res;
    }

    do
    {
        if (tileElement->GetType() != TileElementType::Surface)
        {
            // Skip track elements belonging to the ride that's being ignored for rides that intersect themselves.
            if (!ignoreRideId.IsNull() && tileElement->GetType() == TileElementType::Track
                && tileElement->AsTrack()->GetRideIndex() == ignoreRideId)
            {
                continue;
            }

            if (pos.baseZ < tileElement->GetClearanceZ() && pos.clearanceZ > tileElement->GetBaseZ()
                && !(tileElement->IsGhost()))
            {
                if (tileElement->GetOccupiedQuadrants() & (quarterTile.GetBaseQuarterOccupied()))
                {
                    if (MapLoc68BABCShouldContinue(
                            &tileElement, pos, clearFunc, flags, res.cost, crossingMode, canBuildCrossing, slope))
                    {
                        continue;
                    }

                    MapGetObstructionErrorText(tileElement, res);
                    res.error = GameActions::Status::noClearance;
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
                if (!clearFunc(&tileElement, pos, flags, &res.cost))
                {
                    res.error = GameActions::Status::noClearance;
                    res.errorMessage = STR_CANNOT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_WATER;
                    return res;
                }
            }
        }

        if (getGameState().park.flags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION && !isTree)
        {
            const auto heightFromGround = pos.clearanceZ - tileElement->GetBaseZ();

            if (heightFromGround > (18 * kCoordsZStep))
            {
                res.error = GameActions::Status::disallowed;
                res.errorMessage = STR_LOCAL_AUTHORITY_WONT_ALLOW_CONSTRUCTION_ABOVE_TREE_HEIGHT;
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
                const auto [northZ, eastZ, southZ, westZ] = GetSlopeCornerHeights(
                    tileElement->GetBaseZ(), tileElement->AsSurface()->GetSlope());
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

                if (MapLoc68BABCShouldContinue(
                        &tileElement, pos, clearFunc, flags, res.cost, crossingMode, canBuildCrossing, slope))
                {
                    continue;
                }

                MapGetObstructionErrorText(tileElement, res);
                res.error = GameActions::Status::noClearance;
                return res;
            }
        }
    } while (!(tileElement++)->IsLastForTile());

    res.setData(ConstructClearResult{ groundFlags });

    return res;
}

static bool dummyClearFunc(
    [[maybe_unused]] TileElement** tile_element, [[maybe_unused]] const CoordsXY& coords, [[maybe_unused]] CommandFlags flags,
    [[maybe_unused]] money64* price)
{
    return false;
}

GameActions::Result MapCanConstructAt(const CoordsXYRangedZ& pos, QuarterTile bl)
{
    return MapCanConstructWithClearAt(pos, dummyClearFunc, bl, {}, kTileSlopeFlat);
}

/**
 *
 *  rct2: 0x0068BB18
 */
void MapGetObstructionErrorText(TileElement* tileElement, GameActions::Result& res)
{
    Ride* ride;

    res.errorMessage = STR_OBJECT_IN_THE_WAY;
    switch (tileElement->GetType())
    {
        case TileElementType::Surface:
            res.errorMessage = STR_RAISE_OR_LOWER_LAND_FIRST;
            break;
        case TileElementType::Path:
            res.errorMessage = STR_FOOTPATH_IN_THE_WAY;
            break;
        case TileElementType::Track:
            ride = GetRide(tileElement->AsTrack()->GetRideIndex());
            if (ride != nullptr)
            {
                res.errorMessage = STR_X_IN_THE_WAY;

                Formatter ft(res.errorMessageArgs.data());
                ride->formatNameTo(ft);
            }
            break;
        case TileElementType::SmallScenery:
        {
            auto* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
            res.errorMessage = STR_X_IN_THE_WAY;
            auto ft = Formatter(res.errorMessageArgs.data());
            StringId stringId = sceneryEntry != nullptr ? sceneryEntry->name : static_cast<StringId>(kStringIdEmpty);
            ft.Add<StringId>(stringId);
            break;
        }
        case TileElementType::Entrance:
            switch (tileElement->AsEntrance()->GetEntranceType())
            {
                case ENTRANCE_TYPE_RIDE_ENTRANCE:
                    res.errorMessage = STR_RIDE_ENTRANCE_IN_THE_WAY;
                    break;
                case ENTRANCE_TYPE_RIDE_EXIT:
                    res.errorMessage = STR_RIDE_EXIT_IN_THE_WAY;
                    break;
                case ENTRANCE_TYPE_PARK_ENTRANCE:
                    res.errorMessage = STR_PARK_ENTRANCE_IN_THE_WAY;
                    break;
            }
            break;
        case TileElementType::Wall:
        {
            auto* wallEntry = tileElement->AsWall()->GetEntry();
            res.errorMessage = STR_X_IN_THE_WAY;
            auto ft = Formatter(res.errorMessageArgs.data());
            StringId stringId = wallEntry != nullptr ? wallEntry->name : static_cast<StringId>(kStringIdEmpty);
            ft.Add<StringId>(stringId);
            break;
        }
        case TileElementType::LargeScenery:
        {
            auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            res.errorMessage = STR_X_IN_THE_WAY;
            auto ft = Formatter(res.errorMessageArgs.data());
            StringId stringId = sceneryEntry != nullptr ? sceneryEntry->name : static_cast<StringId>(kStringIdEmpty);
            ft.Add<StringId>(stringId);
            break;
        }
        case TileElementType::Banner:
            break;
    }
}
