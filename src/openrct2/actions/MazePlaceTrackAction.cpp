/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "MazePlaceTrackAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/Guard.hpp"
#include "../management/Finance.h"
#include "../ride/MazeCost.h"
#include "../ride/RideData.h"
#include "../ride/TrackData.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/Wall.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"

namespace OpenRCT2::GameActions
{
    using namespace OpenRCT2::TrackMetaData;

    MazePlaceTrackAction::MazePlaceTrackAction(const CoordsXYZ& location, RideId rideIndex, uint16_t mazeEntry)
        : _loc(location)
        , _rideIndex(rideIndex)
        , _mazeEntry(mazeEntry)
    {
    }

    void MazePlaceTrackAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("mazeEntry", _mazeEntry);
    }

    void MazePlaceTrackAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_loc) << DS_TAG(_rideIndex) << DS_TAG(_mazeEntry);
    }

    Result MazePlaceTrackAction::Query(GameState_t& gameState) const
    {
        auto res = Result();

        res.position = _loc + CoordsXYZ{ 8, 8, 0 };
        res.expenditure = ExpenditureType::rideConstruction;
        res.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
        if ((_loc.z & 0xF) != 0)
        {
            res.error = Status::unknown;
            res.errorMessage = STR_INVALID_HEIGHT;
            return res;
        }

        if (!LocationValid(_loc))
        {
            res.error = Status::invalidParameters;
            res.errorMessage = STR_OFF_EDGE_OF_MAP;
            return res;
        }

        if (!MapIsLocationOwned(_loc) && !gameState.cheats.sandboxMode)
        {
            res.error = Status::notOwned;
            res.errorMessage = STR_LAND_NOT_OWNED_BY_PARK;
            return res;
        }

        if (!MapCheckCapacityAndReorganise(_loc))
        {
            res.error = Status::noFreeElements;
            res.errorMessage = STR_TILE_ELEMENT_LIMIT_REACHED;
            return res;
        }
        auto surfaceElement = MapGetSurfaceElementAt(_loc);
        if (surfaceElement == nullptr)
        {
            res.error = Status::unknown;
            res.errorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
            return res;
        }

        auto baseHeight = _loc.z;
        auto clearanceHeight = _loc.z + kMazeClearanceHeight;

        auto heightDifference = clearanceHeight - surfaceElement->GetBaseZ();
        if (heightDifference >= 0 && !gameState.cheats.disableSupportLimits)
        {
            heightDifference /= kCoordsZPerTinyZ;

            auto* ride = GetRide(_rideIndex);
            const auto& rtd = ride->getRideTypeDescriptor();
            if (heightDifference > rtd.Heights.MaxHeight)
            {
                res.error = Status::tooHigh;
                res.errorMessage = STR_TOO_HIGH_FOR_SUPPORTS;
                return res;
            }
        }

        auto canBuild = MapCanConstructWithClearAt(
            { _loc.ToTileStart(), baseHeight, clearanceHeight }, MapPlaceNonSceneryClearFunc, { 0b1111, 0 }, GetFlags(),
            kTileSlopeFlat);
        if (canBuild.error != Status::ok)
        {
            canBuild.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return canBuild;
        }

        const auto clearanceData = canBuild.getData<ConstructClearResult>();
        if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER)
        {
            res.error = Status::noClearance;
            res.errorMessage = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
            return res;
        }

        if (clearanceData.GroundFlags & ELEMENT_IS_UNDERGROUND)
        {
            res.error = Status::noClearance;
            res.errorMessage = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
            return res;
        }

        auto ride = GetRide(_rideIndex);
        if (ride == nullptr || ride->type == kRideTypeNull)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex);
            res.error = Status::invalidParameters;
            res.errorMessage = STR_ERR_RIDE_NOT_FOUND;
            return res;
        }

        res.cost = MazeCalculateCost(canBuild.cost, *ride, _loc);

        return res;
    }

    Result MazePlaceTrackAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();

        res.position = _loc + CoordsXYZ{ 8, 8, 0 };
        res.expenditure = ExpenditureType::rideConstruction;
        res.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex);
            res.error = Status::invalidParameters;
            res.errorMessage = STR_ERR_RIDE_NOT_FOUND;
            return res;
        }

        auto flags = GetFlags();
        if (!flags.has(CommandFlag::ghost))
        {
            FootpathRemoveLitter(_loc);
            WallRemoveAt({ _loc.ToTileStart(), _loc.z, _loc.z + 32 });
        }

        auto baseHeight = _loc.z;
        auto clearanceHeight = _loc.z + kMazeClearanceHeight;

        auto canBuild = MapCanConstructWithClearAt(
            { _loc.ToTileStart(), baseHeight, clearanceHeight }, MapPlaceNonSceneryClearFunc, { 0b1111, 0 },
            GetFlags().with(CommandFlag::apply), kTileSlopeFlat);
        if (canBuild.error != Status::ok)
        {
            canBuild.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return canBuild;
        }

        res.cost = MazeCalculateCost(canBuild.cost, *ride, _loc);

        auto startLoc = _loc.ToTileStart();

        auto* trackElement = TileElementInsert<TrackElement>(_loc, 0b1111);
        Guard::Assert(trackElement != nullptr);

        trackElement->SetClearanceZ(clearanceHeight);
        trackElement->SetTrackType(TrackElemType::maze);
        trackElement->SetRideType(ride->type);
        trackElement->SetRideIndex(_rideIndex);
        trackElement->SetMazeEntry(_mazeEntry);
        trackElement->SetGhost(flags.has(CommandFlag::ghost));

        MapInvalidateTileFull(startLoc);

        ride->mazeTiles++;
        ride->getStation().SetBaseZ(trackElement->GetBaseZ());
        ride->getStation().Start = { 0, 0 };

        if (ride->mazeTiles == 1)
        {
            ride->overallView = startLoc;
        }

        return res;
    }
} // namespace OpenRCT2::GameActions
