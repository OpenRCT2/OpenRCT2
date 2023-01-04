/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideEntranceExitPlaceAction.h"

#include "../actions/RideEntranceExitRemoveAction.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/Station.h"
#include "../world/ConstructionClearance.h"
#include "../world/MapAnimation.h"

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

GameActions::Result RideEntranceExitPlaceAction::Query() const
{
    const auto errorTitle = _isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                    : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;

    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid game command for ride %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, errorTitle, STR_NONE);
    }

    if (_stationNum.ToUnderlying() >= OpenRCT2::Limits::MaxStationsPerRide)
    {
        log_warning("Invalid station number for ride. stationNum: %u", _stationNum.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, errorTitle, STR_NONE);
    }

    if (ride->status != RideStatus::Closed && ride->status != RideStatus::Simulating)
    {
        return GameActions::Result(GameActions::Status::NotClosed, errorTitle, STR_MUST_BE_CLOSED_FIRST);
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
    {
        return GameActions::Result(GameActions::Status::Disallowed, errorTitle, STR_NOT_ALLOWED_TO_MODIFY_STATION);
    }

    const auto& station = ride->GetStation(_stationNum);
    const auto location = _isExit ? station.Exit : station.Entrance;

    if (!location.IsNull())
    {
        auto rideEntranceExitRemove = RideEntranceExitRemoveAction(location.ToCoordsXY(), _rideIndex, _stationNum, _isExit);
        rideEntranceExitRemove.SetFlags(GetFlags());

        auto result = GameActions::QueryNested(&rideEntranceExitRemove);
        if (result.Error != GameActions::Status::Ok)
        {
            result.ErrorTitle = errorTitle;
            return result;
        }
    }

    auto z = ride->GetStation(_stationNum).GetBaseZ();
    if (!LocationValid(_loc) || (!gCheatsSandboxMode && !MapIsLocationOwned({ _loc, z })))
    {
        return GameActions::Result(GameActions::Status::NotOwned, errorTitle, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (!MapCheckCapacityAndReorganise(_loc))
    {
        return GameActions::Result(GameActions::Status::NoFreeElements, errorTitle, STR_TILE_ELEMENT_LIMIT_REACHED);
    }
    auto clear_z = z + (_isExit ? RideExitHeight : RideEntranceHeight);
    auto canBuild = MapCanConstructWithClearAt({ _loc, z, clear_z }, &MapPlaceNonSceneryClearFunc, { 0b1111, 0 }, GetFlags());
    if (canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = errorTitle;
        return canBuild;
    }

    const auto clearanceData = canBuild.GetData<ConstructClearResult>();
    if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER)
    {
        return GameActions::Result(GameActions::Status::Disallowed, errorTitle, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
    }

    if (z > MaxRideEntranceOrExitHeight)
    {
        return GameActions::Result(GameActions::Status::Disallowed, errorTitle, STR_TOO_HIGH);
    }

    auto res = GameActions::Result();
    res.Position = { _loc.ToTileCentre(), z };
    res.Expenditure = ExpenditureType::RideConstruction;
    res.Cost += canBuild.Cost;
    return res;
}

GameActions::Result RideEntranceExitPlaceAction::Execute() const
{
    // Remember when in unknown station num mode rideIndex is unknown and z is set
    // When in known station num mode rideIndex is known and z is unknown
    const auto errorTitle = _isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                    : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid game command for ride %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, errorTitle, STR_NONE);
    }

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        ride_clear_for_construction(ride);
        ride->RemovePeeps();
    }

    auto& station = ride->GetStation(_stationNum);
    const auto location = _isExit ? station.Exit : station.Entrance;
    if (!location.IsNull())
    {
        auto rideEntranceExitRemove = RideEntranceExitRemoveAction(location.ToCoordsXY(), _rideIndex, _stationNum, _isExit);
        rideEntranceExitRemove.SetFlags(GetFlags());

        auto result = GameActions::ExecuteNested(&rideEntranceExitRemove);
        if (result.Error != GameActions::Status::Ok)
        {
            result.ErrorTitle = errorTitle;
            return result;
        }
    }

    auto z = station.GetBaseZ();
    if (!(GetFlags() & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        FootpathRemoveLitter({ _loc, z });
        WallRemoveAtZ({ _loc, z });
    }

    auto clear_z = z + (_isExit ? RideExitHeight : RideEntranceHeight);
    auto canBuild = MapCanConstructWithClearAt(
        { _loc, z, clear_z }, &MapPlaceNonSceneryClearFunc, { 0b1111, 0 }, GetFlags() | GAME_COMMAND_FLAG_APPLY);
    if (canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = errorTitle;
        return canBuild;
    }

    auto res = GameActions::Result();
    res.Position = { _loc.ToTileCentre(), z };
    res.Expenditure = ExpenditureType::RideConstruction;
    res.Cost += canBuild.Cost;

    auto* entranceElement = TileElementInsert<EntranceElement>(CoordsXYZ{ _loc, z }, 0b1111);
    Guard::Assert(entranceElement != nullptr);

    entranceElement->SetDirection(_direction);
    entranceElement->SetClearanceZ(clear_z);
    entranceElement->SetEntranceType(_isExit ? ENTRANCE_TYPE_RIDE_EXIT : ENTRANCE_TYPE_RIDE_ENTRANCE);
    entranceElement->SetStationIndex(_stationNum);
    entranceElement->SetRideIndex(_rideIndex);
    entranceElement->SetGhost(GetFlags() & GAME_COMMAND_FLAG_GHOST);

    if (_isExit)
    {
        station.Exit = TileCoordsXYZD(CoordsXYZD{ _loc, z, entranceElement->GetDirection() });
    }
    else
    {
        station.Entrance = TileCoordsXYZD(CoordsXYZD{ _loc, z, entranceElement->GetDirection() });
        station.LastPeepInQueue = EntityId::GetNull();
        station.QueueLength = 0;

        MapAnimationCreate(MAP_ANIMATION_TYPE_RIDE_ENTRANCE, { _loc, z });
    }

    FootpathQueueChainReset();

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        MazeEntranceHedgeRemoval({ _loc, entranceElement->as<TileElement>() });
    }

    FootpathConnectEdges(_loc, entranceElement->as<TileElement>(), GetFlags());
    FootpathUpdateQueueChains();

    MapInvalidateTileFull(_loc);

    return res;
}

GameActions::Result RideEntranceExitPlaceAction::TrackPlaceQuery(const CoordsXYZ& loc, const bool isExit)
{
    const auto errorTitle = isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                   : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;

    if (!gCheatsSandboxMode && !MapIsLocationOwned(loc))
    {
        return GameActions::Result(GameActions::Status::NotOwned, errorTitle, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (!MapCheckCapacityAndReorganise(loc))
    {
        return GameActions::Result(GameActions::Status::NoFreeElements, errorTitle, STR_TILE_ELEMENT_LIMIT_REACHED);
    }
    int16_t baseZ = loc.z;
    int16_t clearZ = baseZ + (isExit ? RideExitHeight : RideEntranceHeight);
    auto canBuild = MapCanConstructWithClearAt({ loc, baseZ, clearZ }, &MapPlaceNonSceneryClearFunc, { 0b1111, 0 }, 0);
    if (canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = errorTitle;
        return canBuild;
    }

    const auto clearanceData = canBuild.GetData<ConstructClearResult>();
    if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER)
    {
        return GameActions::Result(GameActions::Status::Disallowed, errorTitle, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
    }

    if (baseZ > MaxRideEntranceOrExitHeight)
    {
        return GameActions::Result(GameActions::Status::Disallowed, errorTitle, STR_TOO_HIGH);
    }
    auto res = GameActions::Result();
    res.Position = { loc.ToTileCentre(), TileElementHeight(loc) };
    res.Expenditure = ExpenditureType::RideConstruction;
    res.Cost += canBuild.Cost;
    return res;
}
