/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackDesignAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../management/Finance.h"
#include "../management/Research.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../ride/RideConstruction.h"
#include "../ride/TrackDesign.h"
#include "RideCreateAction.h"
#include "RideDemolishAction.h"
#include "RideSetNameAction.h"
#include "RideSetSettingAction.h"
#include "RideSetVehicleAction.h"

using namespace OpenRCT2;

TrackDesignAction::TrackDesignAction(const CoordsXYZD& location, const TrackDesign& td)
    : _loc(location)
    , _td(td)
{
}

void TrackDesignAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    // TODO visit the track design (it has a lot of sub fields)
}

uint16_t TrackDesignAction::GetActionFlags() const
{
    return GameActionBase::GetActionFlags();
}

void TrackDesignAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc);
    _td.Serialise(stream);
}

GameActions::Result TrackDesignAction::Query() const
{
    auto res = GameActions::Result();
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.Expenditure = ExpenditureType::RideConstruction;

    if (!LocationValid(_loc))
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_OFF_EDGE_OF_MAP);
    }

    auto& gameState = GetGameState();
    auto& objManager = GetContext()->GetObjectManager();
    auto entryIndex = objManager.GetLoadedObjectEntryIndex(_td.trackAndVehicle.vehicleObject);
    if (entryIndex == kObjectEntryIndexNull)
    {
        // Force a fallback if the entry is not invented yet a track design of it is selected,
        // which can happen in select-by-track-type mode
        if (!RideEntryIsInvented(entryIndex) && !gameState.Cheats.ignoreResearchStatus)
        {
            entryIndex = kObjectEntryIndexNull;
        }
    }

    // Colours do not matter as will be overwritten
    auto rideCreateAction = RideCreateAction(_td.trackAndVehicle.rtdIndex, entryIndex, 0, 0, gameState.LastEntranceStyle);
    rideCreateAction.SetFlags(GetFlags());
    auto r = GameActions::ExecuteNested(&rideCreateAction);
    if (r.Error != GameActions::Status::Ok)
    {
        return GameActions::Result(GameActions::Status::NoFreeElements, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, kStringIdNone);
    }

    const auto rideIndex = r.GetData<RideId>();
    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %d", rideIndex);
        return GameActions::Result(
            GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_RIDE_NOT_FOUND);
    }

    bool placeScenery = true;

    uint32_t flags = 0;
    if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
        flags |= GAME_COMMAND_FLAG_GHOST;
    if (GetFlags() & GAME_COMMAND_FLAG_REPLAY)
        flags |= GAME_COMMAND_FLAG_REPLAY;

    auto queryRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
    if (_trackDesignPlaceStateSceneryUnavailable)
    {
        placeScenery = false;
        queryRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
    }

    auto gameAction = RideDemolishAction(ride->id, RIDE_MODIFY_DEMOLISH);
    gameAction.SetFlags(GetFlags());

    GameActions::ExecuteNested(&gameAction);

    if (queryRes.Error != GameActions::Status::Ok)
    {
        res.Error = queryRes.Error;
        res.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
        res.ErrorMessage = queryRes.ErrorMessage;
        res.ErrorMessageArgs = queryRes.ErrorMessageArgs;
        return res;
    }

    res.Cost = queryRes.Cost;
    res.SetData(RideId{ RideId::GetNull() });

    return res;
}

GameActions::Result TrackDesignAction::Execute() const
{
    auto res = GameActions::Result();
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.Expenditure = ExpenditureType::RideConstruction;

    auto& gameState = GetGameState();
    auto& objManager = GetContext()->GetObjectManager();
    auto entryIndex = objManager.GetLoadedObjectEntryIndex(_td.trackAndVehicle.vehicleObject);
    if (entryIndex != kObjectEntryIndexNull)
    {
        // Force a fallback if the entry is not invented yet a track design using it is selected.
        // This can happen on rides with multiple vehicles where some have been invented and some haven’t.
        if (!RideEntryIsInvented(entryIndex) && !gameState.Cheats.ignoreResearchStatus)
        {
            entryIndex = kObjectEntryIndexNull;
        }
    }

    // Colours do not matter as will be overwritten
    auto rideCreateAction = RideCreateAction(_td.trackAndVehicle.rtdIndex, entryIndex, 0, 0, gameState.LastEntranceStyle);
    rideCreateAction.SetFlags(GetFlags());
    auto r = GameActions::ExecuteNested(&rideCreateAction);
    if (r.Error != GameActions::Status::Ok)
    {
        return GameActions::Result(GameActions::Status::NoFreeElements, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, kStringIdNone);
    }

    const auto rideIndex = r.GetData<RideId>();
    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %d", rideIndex);
        return GameActions::Result(
            GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_RIDE_NOT_FOUND);
    }

    // Query first, this is required again to determine if scenery is available.
    uint32_t flags = GetFlags() & ~GAME_COMMAND_FLAG_APPLY;

    bool placeScenery = true;

    auto queryRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
    if (_trackDesignPlaceStateSceneryUnavailable)
    {
        placeScenery = false;
        queryRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
    }

    if (queryRes.Error != GameActions::Status::Ok)
    {
        auto gameAction = RideDemolishAction(ride->id, RIDE_MODIFY_DEMOLISH);
        gameAction.SetFlags(GetFlags());
        GameActions::ExecuteNested(&gameAction);

        res.Error = queryRes.Error;
        res.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
        res.ErrorMessage = queryRes.ErrorMessage;
        res.ErrorMessageArgs = queryRes.ErrorMessageArgs;

        return res;
    }

    // Execute.
    flags |= GAME_COMMAND_FLAG_APPLY;

    auto execRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
    if (execRes.Error != GameActions::Status::Ok)
    {
        auto gameAction = RideDemolishAction(ride->id, RIDE_MODIFY_DEMOLISH);
        gameAction.SetFlags(GetFlags());
        GameActions::ExecuteNested(&gameAction);

        res.Error = execRes.Error;
        res.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
        res.ErrorMessage = execRes.ErrorMessage;
        res.ErrorMessageArgs = execRes.ErrorMessageArgs;

        return res;
    }

    if (entryIndex != kObjectEntryIndexNull)
    {
        auto colour = RideGetUnusedPresetVehicleColour(entryIndex);
        auto rideSetVehicleAction = RideSetVehicleAction(ride->id, RideSetVehicleType::RideEntry, entryIndex, colour);
        GameActions::ExecuteNested(&rideSetVehicleAction);
    }

    SetOperatingSettingNested(ride->id, RideSetSetting::Mode, static_cast<uint8_t>(_td.operation.rideMode), flags);
    auto rideSetVehicleAction2 = RideSetVehicleAction(
        ride->id, RideSetVehicleType::NumTrains, _td.trackAndVehicle.numberOfTrains);
    GameActions::ExecuteNested(&rideSetVehicleAction2);

    auto rideSetVehicleAction3 = RideSetVehicleAction(
        ride->id, RideSetVehicleType::NumCarsPerTrain, _td.trackAndVehicle.numberOfCarsPerTrain);
    GameActions::ExecuteNested(&rideSetVehicleAction3);

    SetOperatingSettingNested(ride->id, RideSetSetting::Departure, _td.operation.departFlags, flags);
    SetOperatingSettingNested(ride->id, RideSetSetting::MinWaitingTime, _td.operation.minWaitingTime, flags);
    SetOperatingSettingNested(ride->id, RideSetSetting::MaxWaitingTime, _td.operation.maxWaitingTime, flags);
    SetOperatingSettingNested(ride->id, RideSetSetting::Operation, _td.operation.operationSetting, flags);
    SetOperatingSettingNested(ride->id, RideSetSetting::LiftHillSpeed, _td.operation.liftHillSpeed, flags);

    auto numCircuits = std::max<uint8_t>(1, _td.operation.numCircuits);
    SetOperatingSettingNested(ride->id, RideSetSetting::NumCircuits, numCircuits, flags);
    ride->SetToDefaultInspectionInterval();
    ride->lifecycle_flags |= RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN;
    ride->vehicleColourSettings = _td.appearance.vehicleColourSettings;

    ride->entrance_style = objManager.GetLoadedObjectEntryIndex(_td.appearance.stationObjectIdentifier);
    if (ride->entrance_style == kObjectEntryIndexNull)
    {
        ride->entrance_style = gameState.LastEntranceStyle;
    }

    for (size_t i = 0; i < std::min(std::size(ride->track_colour), std::size(_td.appearance.trackColours)); i++)
    {
        ride->track_colour[i] = _td.appearance.trackColours[i];
    }

    for (size_t i = 0; i < Limits::kMaxVehicleColours; i++)
    {
        ride->vehicle_colours[i] = _td.appearance.vehicleColours[i];
    }

    for (int32_t count = 1; count == 1 || r.Error != GameActions::Status::Ok; ++count)
    {
        auto name = count == 1 ? _td.gameStateData.name : (_td.gameStateData.name + " " + std::to_string(count));
        auto gameAction = RideSetNameAction(ride->id, name);
        gameAction.SetFlags(GetFlags());
        r = GameActions::ExecuteNested(&gameAction);
    }
    res.Cost = execRes.Cost;
    res.SetData(RideId{ ride->id });

    return res;
}
