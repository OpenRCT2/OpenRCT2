/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathLayoutPlaceAction.h"

#include "../Cheats.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideConstruction.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/QuarterTile.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"

namespace OpenRCT2::GameActions
{
    FootpathLayoutPlaceAction::FootpathLayoutPlaceAction(
        const CoordsXYZ& loc, uint8_t slope, ObjectEntryIndex type, ObjectEntryIndex railingsType, uint8_t edges,
        PathConstructFlags constructFlags)
        : _loc(loc)
        , _slope(slope)
        , _type(type)
        , _railingsType(railingsType)
        , _edges(edges)
        , _constructFlags(constructFlags)
    {
    }

    void FootpathLayoutPlaceAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_slope) << DS_TAG(_type) << DS_TAG(_railingsType) << DS_TAG(_edges)
               << DS_TAG(_constructFlags);
    }

    void FootpathLayoutPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
        visitor.Visit("slope", _slope);
        visitor.Visit("object", _type);
        visitor.Visit("railingsObject", _railingsType);
        visitor.Visit("edges", _edges);
        visitor.Visit("constructFlags", _constructFlags);
    }

    uint16_t FootpathLayoutPlaceAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags();
    }

    Result FootpathLayoutPlaceAction::Query() const
    {
        auto res = Result();
        res.Cost = 0;
        res.Expenditure = ExpenditureType::landscaping;
        res.Position = _loc.ToTileCentre();

        gFootpathGroundFlags = 0;

        if (!LocationValid(_loc) || MapIsEdge(_loc))
        {
            return Result(Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_OFF_EDGE_OF_MAP);
        }

        if (!(gLegacyScene == LegacyScene::scenarioEditor || getGameState().cheats.sandboxMode) && !MapIsLocationOwned(_loc))
        {
            return Result(Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }

        if (_loc.z < kFootpathMinHeight)
        {
            return Result(Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_LOW);
        }

        if (_loc.z > kFootpathMaxHeight)
        {
            return Result(Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_HIGH);
        }

        return ElementInsertQuery(std::move(res));
    }

    Result FootpathLayoutPlaceAction::Execute() const
    {
        auto res = Result();
        res.Cost = 0;
        res.Expenditure = ExpenditureType::landscaping;
        res.Position = _loc.ToTileCentre();

        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            FootpathInterruptPeeps(_loc);
        }

        gFootpathGroundFlags = 0;

        // Force ride construction to recheck area
        _currentTrackSelectionFlags.set(TrackSelectionFlag::recheck);

        return ElementInsertExecute(std::move(res));
    }

    Result FootpathLayoutPlaceAction::ElementInsertQuery(Result res) const
    {
        bool entrancePath = false, entranceIsSamePath = false;

        if (!MapCheckCapacityAndReorganise(_loc))
        {
            return Result(
                Status::NoFreeElements, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TILE_ELEMENT_LIMIT_REACHED);
        }

        res.Cost = 12.00_GBP;

        QuarterTile quarterTile{ 0b1111, 0 };
        auto zLow = _loc.z;
        auto zHigh = zLow + kPathClearance;
        if (_slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED)
        {
            quarterTile = QuarterTile{ 0b1111, 0b1100 }.Rotate(_slope & kTileElementDirectionMask);
            zHigh += kPathHeightStep;
        }

        auto entranceElement = MapGetParkEntranceElementAt(_loc, false);
        // Make sure the entrance part is the middle
        if (entranceElement != nullptr && (entranceElement->GetSequenceIndex()) == 0)
        {
            entrancePath = true;
            // Make the price the same as replacing a path
            if (IsSameAsEntranceElement(*entranceElement))
                entranceIsSamePath = true;
            else
                res.Cost -= 6.00_GBP;
        }

        // Do not attempt to build a crossing with a queue or a sloped path.
        auto isQueue = _constructFlags & PathConstructFlag::IsQueue;
        auto crossingMode = isQueue || (_slope != kTileSlopeFlat) ? CreateCrossingMode::none
                                                                  : CreateCrossingMode::pathOverTrack;
        auto canBuild = MapCanConstructWithClearAt(
            { _loc, zLow, zHigh }, &MapPlaceNonSceneryClearFunc, quarterTile, GetFlags(), crossingMode);
        if (!entrancePath && canBuild.Error != Status::Ok)
        {
            canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return canBuild;
        }
        res.Cost += canBuild.Cost;

        const auto clearanceData = canBuild.GetData<ConstructClearResult>();
        gFootpathGroundFlags = clearanceData.GroundFlags;

        if (!getGameState().cheats.disableClearanceChecks && (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER))
        {
            return Result(Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_CANT_BUILD_THIS_UNDERWATER);
        }

        auto surfaceElement = MapGetSurfaceElementAt(_loc);
        if (surfaceElement == nullptr)
        {
            return Result(
                Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
        }
        int32_t supportHeight = zLow - surfaceElement->GetBaseZ();
        res.Cost += supportHeight < 0 ? 20.00_GBP : (supportHeight / kPathHeightStep) * 5.00_GBP;

        // Prevent the place sound from being spammed
        if (entranceIsSamePath)
            res.Cost = 0;

        return res;
    }

    Result FootpathLayoutPlaceAction::ElementInsertExecute(Result res) const
    {
        bool entrancePath = false, entranceIsSamePath = false;

        if (!(GetFlags() & (GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST)))
        {
            FootpathRemoveLitter(_loc);
        }

        res.Cost = 12.00_GBP;

        QuarterTile quarterTile{ 0b1111, 0 };
        auto zLow = _loc.z;
        auto zHigh = zLow + kPathClearance;
        if (_slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED)
        {
            quarterTile = QuarterTile{ 0b1111, 0b1100 }.Rotate(_slope & kTileElementDirectionMask);
            zHigh += kPathHeightStep;
        }

        auto entranceElement = MapGetParkEntranceElementAt(_loc, false);
        // Make sure the entrance part is the middle
        if (entranceElement != nullptr && (entranceElement->GetSequenceIndex()) == 0)
        {
            entrancePath = true;
            // Make the price the same as replacing a path
            if (IsSameAsEntranceElement(*entranceElement))
                entranceIsSamePath = true;
            else
                res.Cost -= 6.00_GBP;
        }

        // Do not attempt to build a crossing with a queue or a sloped path.
        auto isQueue = _constructFlags & PathConstructFlag::IsQueue;
        auto crossingMode = isQueue || (_slope != kTileSlopeFlat) ? CreateCrossingMode::none
                                                                  : CreateCrossingMode::pathOverTrack;
        auto canBuild = MapCanConstructWithClearAt(
            { _loc, zLow, zHigh }, &MapPlaceNonSceneryClearFunc, quarterTile, GAME_COMMAND_FLAG_APPLY | GetFlags(),
            crossingMode);
        if (!entrancePath && canBuild.Error != Status::Ok)
        {
            canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return canBuild;
        }
        res.Cost += canBuild.Cost;

        const auto clearanceData = canBuild.GetData<ConstructClearResult>();
        gFootpathGroundFlags = clearanceData.GroundFlags;

        auto surfaceElement = MapGetSurfaceElementAt(_loc);
        if (surfaceElement == nullptr)
        {
            return Result(
                Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
        }
        int32_t supportHeight = zLow - surfaceElement->GetBaseZ();
        res.Cost += supportHeight < 0 ? 20.00_GBP : (supportHeight / kPathHeightStep) * 5.00_GBP;

        if (entrancePath)
        {
            if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !entranceIsSamePath)
            {
                if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
                {
                    entranceElement->SetLegacyPathEntryIndex(_type);
                }
                else
                {
                    entranceElement->SetSurfaceEntryIndex(_type);
                }
                MapInvalidateTileFull(_loc);
            }
        }
        else
        {
            auto* pathElement = TileElementInsert<PathElement>(_loc, 0b1111);
            Guard::Assert(pathElement != nullptr);

            pathElement->SetClearanceZ(zHigh);
            if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
            {
                pathElement->SetLegacyPathEntryIndex(_type);
            }
            else
            {
                pathElement->SetSurfaceEntryIndex(_type);
                pathElement->SetRailingsEntryIndex(_railingsType);
            }
            pathElement->SetSlopeDirection(_slope & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK);
            pathElement->SetSloped(_slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED);
            pathElement->SetIsQueue(isQueue);
            pathElement->SetAddition(0);
            pathElement->SetRideIndex(RideId::GetNull());
            pathElement->SetAdditionStatus(255);
            pathElement->SetIsBroken(false);
            pathElement->SetEdges(_edges);
            pathElement->SetCorners(0);
            pathElement->SetGhost(GetFlags() & GAME_COMMAND_FLAG_GHOST);

            MapInvalidateTileFull(_loc);
        }

        // Prevent the place sound from being spammed
        if (entranceIsSamePath)
            res.Cost = 0;

        return res;
    }

    bool FootpathLayoutPlaceAction::IsSameAsEntranceElement(const EntranceElement& entranceElement) const
    {
        if (entranceElement.HasLegacyPathEntry())
        {
            if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
            {
                return entranceElement.GetLegacyPathEntryIndex() == _type;
            }

            return false;
        }

        if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
        {
            return false;
        }

        return entranceElement.GetSurfaceEntryIndex() == _type;
    }
} // namespace OpenRCT2::GameActions
