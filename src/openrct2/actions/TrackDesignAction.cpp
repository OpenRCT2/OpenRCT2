/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackDesignAction.h"

#include "../Context.h"
#include "../management/Finance.h"
#include "../management/Research.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../rct12/RCT12.h"
#include "../ride/RideConstruction.h"
#include "../ride/TrackDesign.h"
#include "RideCreateAction.h"
#include "RideDemolishAction.h"
#include "RideSetNameAction.h"
#include "RideSetSettingAction.h"
#include "RideSetVehicleAction.h"

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
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    auto entryIndex = objManager.GetLoadedObjectEntryIndex(_td.vehicle_object);
    if (entryIndex == OBJECT_ENTRY_INDEX_NULL)
    {
        // Force a fallback if the entry is not invented yet a td6 of it is selected,
        // which can happen in select-by-track-type mode
        if (!ride_entry_is_invented(entryIndex) && !gCheatsIgnoreResearchStatus)
        {
            entryIndex = OBJECT_ENTRY_INDEX_NULL;
        }
    }

    // Colours do not matter as will be overwritten
    auto rideCreateAction = RideCreateAction(_td.type, entryIndex, 0, 0, gLastEntranceStyle);
    rideCreateAction.SetFlags(GetFlags());
    auto r = GameActions::ExecuteNested(&rideCreateAction);
    if (r.Error != GameActions::Status::Ok)
    {
        return GameActions::Result(GameActions::Status::NoFreeElements, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_NONE);
    }

    const auto rideIndex = r.GetData<RideId>();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid game command for track placement, ride id = %d", rideIndex);
        return GameActions::Result(GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    bool placeScenery = true;

    uint32_t flags = 0;
    if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
        flags |= GAME_COMMAND_FLAG_GHOST;
    if (GetFlags() & GAME_COMMAND_FLAG_REPLAY)
        flags |= GAME_COMMAND_FLAG_REPLAY;

    auto queryRes = TrackDesignPlace(const_cast<TrackDesign*>(&_td), flags, placeScenery, ride, _loc);
    if (_trackDesignPlaceStateSceneryUnavailable)
    {
        placeScenery = false;
        queryRes = TrackDesignPlace(const_cast<TrackDesign*>(&_td), flags, placeScenery, ride, _loc);
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

    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    auto entryIndex = objManager.GetLoadedObjectEntryIndex(_td.vehicle_object);
    if (entryIndex != OBJECT_ENTRY_INDEX_NULL)
    {
        // Force a fallback if the entry is not invented yet a track design using it is selected.
        // This can happen on rides with multiple vehicles where some have been invented and some haven’t.
        if (!ride_entry_is_invented(entryIndex) && !gCheatsIgnoreResearchStatus)
        {
            entryIndex = OBJECT_ENTRY_INDEX_NULL;
        }
    }

    // Colours do not matter as will be overwritten
    auto rideCreateAction = RideCreateAction(_td.type, entryIndex, 0, 0, gLastEntranceStyle);
    rideCreateAction.SetFlags(GetFlags());
    auto r = GameActions::ExecuteNested(&rideCreateAction);
    if (r.Error != GameActions::Status::Ok)
    {
        return GameActions::Result(GameActions::Status::NoFreeElements, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_NONE);
    }

    const auto rideIndex = r.GetData<RideId>();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid game command for track placement, ride id = %d", rideIndex);
        return GameActions::Result(GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    // Query first, this is required again to determine if scenery is available.
    bool placeScenery = true;

    uint32_t flags = 0;
    if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
        flags |= GAME_COMMAND_FLAG_GHOST;
    if (GetFlags() & GAME_COMMAND_FLAG_REPLAY)
        flags |= GAME_COMMAND_FLAG_REPLAY;

    auto queryRes = TrackDesignPlace(const_cast<TrackDesign*>(&_td), flags, placeScenery, ride, _loc);
    if (_trackDesignPlaceStateSceneryUnavailable)
    {
        placeScenery = false;
        queryRes = TrackDesignPlace(const_cast<TrackDesign*>(&_td), flags, placeScenery, ride, _loc);
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

    auto execRes = TrackDesignPlace(const_cast<TrackDesign*>(&_td), flags, placeScenery, ride, _loc);
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

    if (entryIndex != OBJECT_ENTRY_INDEX_NULL)
    {
        auto colour = ride_get_unused_preset_vehicle_colour(entryIndex);
        auto rideSetVehicleAction = RideSetVehicleAction(ride->id, RideSetVehicleType::RideEntry, entryIndex, colour);
        GameActions::ExecuteNested(&rideSetVehicleAction);
    }

    set_operating_setting_nested(ride->id, RideSetSetting::Mode, static_cast<uint8_t>(_td.ride_mode), GAME_COMMAND_FLAG_APPLY);
    auto rideSetVehicleAction2 = RideSetVehicleAction(ride->id, RideSetVehicleType::NumTrains, _td.number_of_trains);
    GameActions::ExecuteNested(&rideSetVehicleAction2);

    auto rideSetVehicleAction3 = RideSetVehicleAction(
        ride->id, RideSetVehicleType::NumCarsPerTrain, _td.number_of_cars_per_train);
    GameActions::ExecuteNested(&rideSetVehicleAction3);

    set_operating_setting_nested(ride->id, RideSetSetting::Departure, _td.depart_flags, GAME_COMMAND_FLAG_APPLY);
    set_operating_setting_nested(ride->id, RideSetSetting::MinWaitingTime, _td.min_waiting_time, GAME_COMMAND_FLAG_APPLY);
    set_operating_setting_nested(ride->id, RideSetSetting::MaxWaitingTime, _td.max_waiting_time, GAME_COMMAND_FLAG_APPLY);
    set_operating_setting_nested(ride->id, RideSetSetting::Operation, _td.operation_setting, GAME_COMMAND_FLAG_APPLY);
    set_operating_setting_nested(ride->id, RideSetSetting::LiftHillSpeed, _td.lift_hill_speed & 0x1F, GAME_COMMAND_FLAG_APPLY);

    uint8_t num_circuits = _td.num_circuits;
    if (num_circuits == 0)
    {
        num_circuits = 1;
    }
    set_operating_setting_nested(ride->id, RideSetSetting::NumCircuits, num_circuits, GAME_COMMAND_FLAG_APPLY);
    ride->SetToDefaultInspectionInterval();
    ride->lifecycle_flags |= RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN;
    ride->colour_scheme_type = _td.colour_scheme;

    auto stationIdentifier = GetStationIdentifierFromStyle(_td.entrance_style);
    ride->entrance_style = objManager.GetLoadedObjectEntryIndex(stationIdentifier);
    if (ride->entrance_style == OBJECT_ENTRY_INDEX_NULL)
    {
        ride->entrance_style = gLastEntranceStyle;
    }

    for (int32_t i = 0; i < RCT12::Limits::NumColourSchemes; i++)
    {
        ride->track_colour[i].main = _td.track_spine_colour[i];
        ride->track_colour[i].additional = _td.track_rail_colour[i];
        ride->track_colour[i].supports = _td.track_support_colour[i];
    }

    for (size_t i = 0; i <= OpenRCT2::Limits::MaxTrainsPerRide; i++)
    {
        auto tdIndex = i % std::size(_td.vehicle_colours);
        ride->vehicle_colours[i] = _td.vehicle_colours[tdIndex];
    }

    for (int32_t count = 1; count == 1 || r.Error != GameActions::Status::Ok; ++count)
    {
        auto name = count == 1 ? _td.name : (_td.name + " " + std::to_string(count));
        auto gameAction = RideSetNameAction(ride->id, name);
        gameAction.SetFlags(GetFlags());
        r = GameActions::ExecuteNested(&gameAction);
    }
    res.Cost = execRes.Cost;
    res.SetData(RideId{ ride->id });

    return res;
}
