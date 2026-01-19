/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideEntranceExitPlaceAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../actions/RideEntranceExitRemoveAction.h"
#include "../core/Guard.hpp"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/Station.h"
#include "../world/ConstructionClearance.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Wall.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/Slope.h"

namespace OpenRCT2::GameActions
{
    RideEntranceExitPlaceAction::RideEntranceExitPlaceAction(
        const CoordsXY& loc, Direction direction, RideId rideIndex, StationIndex stationNum, bool isExit)
        : _loc(loc)
        , _direction(direction)
        , _rideIndex(rideIndex)
        , _stationNum(stationNum)
        , _isExit(isExit)
    {
    }

    void RideEntranceExitPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
        visitor.Visit("direction", _direction);
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("station", _stationNum);
        visitor.Visit("isExit", _isExit);
    }

    uint16_t RideEntranceExitPlaceAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags();
    }

    void RideEntranceExitPlaceAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_direction) << DS_TAG(_rideIndex) << DS_TAG(_stationNum) << DS_TAG(_isExit);
    }

    Result RideEntranceExitPlaceAction::Query(GameState_t& gameState) const
    {
        const auto errorTitle = _isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                        : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;

        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, errorTitle, STR_ERR_RIDE_NOT_FOUND);
        }

        if (_stationNum.ToUnderlying() >= Limits::kMaxStationsPerRide)
        {
            LOG_ERROR("Invalid station number for ride. stationNum: %u", _stationNum.ToUnderlying());
            return Result(Status::invalidParameters, errorTitle, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        if (ride->status != RideStatus::closed && ride->status != RideStatus::simulating)
        {
            return Result(Status::notClosed, errorTitle, STR_MUST_BE_CLOSED_FIRST);
        }

        if (ride->lifecycleFlags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
        {
            return Result(Status::disallowed, errorTitle, STR_NOT_ALLOWED_TO_MODIFY_STATION);
        }

        const auto& station = ride->getStation(_stationNum);
        const auto location = _isExit ? station.Exit : station.Entrance;

        if (!location.IsNull())
        {
            auto rideEntranceExitRemove = RideEntranceExitRemoveAction(location.ToCoordsXY(), _rideIndex, _stationNum, _isExit);
            rideEntranceExitRemove.SetFlags(GetFlags());

            auto result = QueryNested(&rideEntranceExitRemove, gameState);
            if (result.error != Status::ok)
            {
                result.errorTitle = errorTitle;
                return result;
            }
        }

        auto z = ride->getStation(_stationNum).GetBaseZ();
        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, errorTitle, STR_OFF_EDGE_OF_MAP);
        }
        if (!gameState.cheats.sandboxMode && !MapIsLocationOwned({ _loc, z }))
        {
            return Result(Status::notOwned, errorTitle, STR_LAND_NOT_OWNED_BY_PARK);
        }

        if (!MapCheckCapacityAndReorganise(_loc))
        {
            return Result(Status::noFreeElements, errorTitle, STR_TILE_ELEMENT_LIMIT_REACHED);
        }
        auto clear_z = z + (_isExit ? RideExitHeight : RideEntranceHeight);
        auto canBuild = MapCanConstructWithClearAt(
            { _loc, z, clear_z }, MapPlaceNonSceneryClearFunc, { 0b1111, 0 }, GetFlags(), kTileSlopeFlat);
        if (canBuild.error != Status::ok)
        {
            canBuild.errorTitle = errorTitle;
            return canBuild;
        }

        const auto clearanceData = canBuild.getData<ConstructClearResult>();
        if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER)
        {
            return Result(Status::disallowed, errorTitle, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
        }

        if (z > MaxRideEntranceOrExitHeight)
        {
            return Result(Status::disallowed, errorTitle, STR_TOO_HIGH);
        }

        auto res = Result();
        res.position = { _loc.ToTileCentre(), z };
        res.expenditure = ExpenditureType::rideConstruction;
        res.cost += canBuild.cost;
        return res;
    }

    Result RideEntranceExitPlaceAction::Execute(GameState_t& gameState) const
    {
        // Remember when in unknown station num mode rideIndex is unknown and z is set
        // When in known station num mode rideIndex is known and z is unknown
        const auto errorTitle = _isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                        : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, errorTitle, STR_ERR_RIDE_NOT_FOUND);
        }

        if (!GetFlags().has(CommandFlag::ghost))
        {
            RideClearForConstruction(*ride);
            ride->removePeeps();
        }

        auto& station = ride->getStation(_stationNum);
        const auto location = _isExit ? station.Exit : station.Entrance;
        if (!location.IsNull())
        {
            auto rideEntranceExitRemove = RideEntranceExitRemoveAction(location.ToCoordsXY(), _rideIndex, _stationNum, _isExit);
            rideEntranceExitRemove.SetFlags(GetFlags());

            auto result = ExecuteNested(&rideEntranceExitRemove, gameState);
            if (result.error != Status::ok)
            {
                result.errorTitle = errorTitle;
                return result;
            }
        }

        auto z = station.GetBaseZ();
        if (!GetFlags().has(CommandFlag::allowDuringPaused) && !GetFlags().has(CommandFlag::ghost)
            && !gameState.cheats.disableClearanceChecks)
        {
            FootpathRemoveLitter({ _loc, z });
            WallRemoveAtZ({ _loc, z });
        }

        auto clear_z = z + (_isExit ? RideExitHeight : RideEntranceHeight);
        auto canBuild = MapCanConstructWithClearAt(
            { _loc, z, clear_z }, MapPlaceNonSceneryClearFunc, { 0b1111, 0 }, GetFlags().with(CommandFlag::apply),
            kTileSlopeFlat);
        if (canBuild.error != Status::ok)
        {
            canBuild.errorTitle = errorTitle;
            return canBuild;
        }

        auto res = Result();
        res.position = { _loc.ToTileCentre(), z };
        res.expenditure = ExpenditureType::rideConstruction;
        res.cost += canBuild.cost;

        auto* entranceElement = TileElementInsert<EntranceElement>(CoordsXYZ{ _loc, z }, 0b1111);
        Guard::Assert(entranceElement != nullptr);

        entranceElement->SetDirection(_direction);
        entranceElement->SetClearanceZ(clear_z);
        entranceElement->SetEntranceType(_isExit ? ENTRANCE_TYPE_RIDE_EXIT : ENTRANCE_TYPE_RIDE_ENTRANCE);
        entranceElement->SetStationIndex(_stationNum);
        entranceElement->SetRideIndex(_rideIndex);
        entranceElement->SetGhost(GetFlags().has(CommandFlag::ghost));

        if (_isExit)
        {
            station.Exit = TileCoordsXYZD(CoordsXYZD{ _loc, z, entranceElement->GetDirection() });
        }
        else
        {
            station.Entrance = TileCoordsXYZD(CoordsXYZD{ _loc, z, entranceElement->GetDirection() });
            station.LastPeepInQueue = EntityId::GetNull();
            station.QueueLength = 0;

            MapAnimations::MarkTileForInvalidation(TileCoordsXY(_loc));
        }

        FootpathQueueChainReset();

        if (!GetFlags().has(CommandFlag::ghost))
        {
            MazeEntranceHedgeRemoval({ _loc, entranceElement->as<TileElement>() });
        }

        FootpathConnectEdges(_loc, entranceElement->as<TileElement>(), GetFlags());
        FootpathUpdateQueueChains();

        MapInvalidateTileFull(_loc);

        return res;
    }

    Result RideEntranceExitPlaceAction::TrackPlaceQuery(GameState_t& gameState, const CoordsXYZ& loc, const bool isExit)
    {
        const auto errorTitle = isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                       : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;

        if (!gameState.cheats.sandboxMode && !MapIsLocationOwned(loc))
        {
            return Result(Status::notOwned, errorTitle, STR_LAND_NOT_OWNED_BY_PARK);
        }

        if (!MapCheckCapacityAndReorganise(loc))
        {
            return Result(Status::noFreeElements, errorTitle, STR_TILE_ELEMENT_LIMIT_REACHED);
        }
        int16_t baseZ = loc.z;
        int16_t clearZ = baseZ + (isExit ? RideExitHeight : RideEntranceHeight);
        auto canBuild = MapCanConstructWithClearAt(
            { loc, baseZ, clearZ }, MapPlaceNonSceneryClearFunc, { 0b1111, 0 }, {}, kTileSlopeFlat);
        if (canBuild.error != Status::ok)
        {
            canBuild.errorTitle = errorTitle;
            return canBuild;
        }

        const auto clearanceData = canBuild.getData<ConstructClearResult>();
        if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER)
        {
            return Result(Status::disallowed, errorTitle, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
        }

        if (baseZ > MaxRideEntranceOrExitHeight)
        {
            return Result(Status::disallowed, errorTitle, STR_TOO_HIGH);
        }
        auto res = Result();
        res.position = { loc.ToTileCentre(), TileElementHeight(loc) };
        res.expenditure = ExpenditureType::rideConstruction;
        res.cost += canBuild.cost;
        return res;
    }
} // namespace OpenRCT2::GameActions
