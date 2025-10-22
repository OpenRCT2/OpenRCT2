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
#include "../config/Config.h"
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

namespace OpenRCT2::GameActions
{
    TrackDesignAction::TrackDesignAction(const CoordsXYZD& location, const TrackDesign& td, bool placeScenery)
        : _loc(location)
        , _td(td)
        , _placeScenery(placeScenery)
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
        stream << DS_TAG(_placeScenery);
    }

    Result TrackDesignAction::Query(GameState_t& gameState) const
    {
        auto res = Result();
        res.Position.x = _loc.x + 16;
        res.Position.y = _loc.y + 16;
        res.Position.z = _loc.z;
        res.Expenditure = ExpenditureType::rideConstruction;

        if (!LocationValid(_loc))
        {
            return Result(Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_OFF_EDGE_OF_MAP);
        }

        auto& objManager = GetContext()->GetObjectManager();
        auto entryIndex = objManager.GetLoadedObjectEntryIndex(_td.trackAndVehicle.vehicleObject);
        if (entryIndex == kObjectEntryIndexNull)
        {
            // Force a fallback if the entry is not invented yet a track design of it is selected,
            // which can happen in select-by-track-type mode
            if (!RideEntryIsInvented(entryIndex) && !gameState.cheats.ignoreResearchStatus)
            {
                entryIndex = kObjectEntryIndexNull;
            }
        }

        // Colours do not matter as will be overwritten
        auto rideCreateAction = RideCreateAction(_td.trackAndVehicle.rtdIndex, entryIndex, 0, 0, gameState.lastEntranceStyle);
        rideCreateAction.SetFlags(GetFlags());
        auto r = ExecuteNested(&rideCreateAction, gameState);
        if (r.Error != Status::Ok)
        {
            return Result(Status::NoFreeElements, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, kStringIdNone);
        }

        const auto rideIndex = r.GetData<RideId>();
        auto ride = GetRide(rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %d", rideIndex);
            return Result(Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_RIDE_NOT_FOUND);
        }

        bool placeScenery = _placeScenery;

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

        auto gameAction = RideDemolishAction(ride->id, RideModifyType::demolish);
        gameAction.SetFlags(GetFlags());

        ExecuteNested(&gameAction, gameState);

        if (queryRes.Error != Status::Ok)
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

    Result TrackDesignAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.Position.x = _loc.x + 16;
        res.Position.y = _loc.y + 16;
        res.Position.z = _loc.z;
        res.Expenditure = ExpenditureType::rideConstruction;

        auto& objManager = GetContext()->GetObjectManager();
        auto entryIndex = objManager.GetLoadedObjectEntryIndex(_td.trackAndVehicle.vehicleObject);
        if (entryIndex != kObjectEntryIndexNull)
        {
            // Force a fallback if the entry is not invented yet a track design using it is selected.
            // This can happen on rides with multiple vehicles where some have been invented and some haven’t.
            if (!RideEntryIsInvented(entryIndex) && !gameState.cheats.ignoreResearchStatus)
            {
                entryIndex = kObjectEntryIndexNull;
            }
        }

        // Colours do not matter as will be overwritten
        auto rideCreateAction = RideCreateAction(_td.trackAndVehicle.rtdIndex, entryIndex, 0, 0, gameState.lastEntranceStyle);
        rideCreateAction.SetFlags(GetFlags());
        auto r = ExecuteNested(&rideCreateAction, gameState);
        if (r.Error != Status::Ok)
        {
            return Result(Status::NoFreeElements, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, kStringIdNone);
        }

        const auto rideIndex = r.GetData<RideId>();
        auto ride = GetRide(rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %d", rideIndex);
            return Result(Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_RIDE_NOT_FOUND);
        }

        // Query first, this is required again to determine if scenery is available.
        uint32_t flags = GetFlags() & ~GAME_COMMAND_FLAG_APPLY;

        bool placeScenery = _placeScenery;

        auto queryRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
        if (_trackDesignPlaceStateSceneryUnavailable)
        {
            placeScenery = false;
            queryRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
        }

        if (queryRes.Error != Status::Ok)
        {
            auto gameAction = RideDemolishAction(ride->id, RideModifyType::demolish);
            gameAction.SetFlags(GetFlags());
            ExecuteNested(&gameAction, gameState);

            res.Error = queryRes.Error;
            res.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            res.ErrorMessage = queryRes.ErrorMessage;
            res.ErrorMessageArgs = queryRes.ErrorMessageArgs;

            return res;
        }

        // Execute.
        flags |= GAME_COMMAND_FLAG_APPLY;

        auto execRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
        if (execRes.Error != Status::Ok)
        {
            auto gameAction = RideDemolishAction(ride->id, RideModifyType::demolish);
            gameAction.SetFlags(GetFlags());
            ExecuteNested(&gameAction, gameState);

            res.Error = execRes.Error;
            res.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            res.ErrorMessage = execRes.ErrorMessage;
            res.ErrorMessageArgs = execRes.ErrorMessageArgs;

            return res;
        }

        if (entryIndex != kObjectEntryIndexNull)
        {
            auto colour = RideGetUnusedPresetVehicleColour(entryIndex);
            auto rideSetVehicleAction = GameActions::RideSetVehicleAction(
                ride->id, GameActions::RideSetVehicleType::RideEntry, entryIndex, colour);
            ExecuteNested(&rideSetVehicleAction, gameState);
        }

        SetOperatingSettingNested(ride->id, RideSetSetting::Mode, RideModes::ToIndex(_td.operation.rideMode), flags);
        auto rideSetVehicleAction2 = GameActions::RideSetVehicleAction(
            ride->id, GameActions::RideSetVehicleType::NumTrains, _td.trackAndVehicle.numberOfTrains);
        ExecuteNested(&rideSetVehicleAction2, gameState);

        auto rideSetVehicleAction3 = GameActions::RideSetVehicleAction(
            ride->id, GameActions::RideSetVehicleType::NumCarsPerTrain, _td.trackAndVehicle.numberOfCarsPerTrain);
        ExecuteNested(&rideSetVehicleAction3, gameState);

        SetOperatingSettingNested(ride->id, RideSetSetting::Departure, _td.operation.departFlags, flags);
        SetOperatingSettingNested(ride->id, RideSetSetting::MinWaitingTime, _td.operation.minWaitingTime, flags);
        SetOperatingSettingNested(ride->id, RideSetSetting::MaxWaitingTime, _td.operation.maxWaitingTime, flags);
        SetOperatingSettingNested(ride->id, RideSetSetting::Operation, _td.operation.operationSetting, flags);
        SetOperatingSettingNested(ride->id, RideSetSetting::LiftHillSpeed, _td.operation.liftHillSpeed, flags);

        auto numCircuits = std::max<uint8_t>(1, _td.operation.numCircuits);
        SetOperatingSettingNested(ride->id, RideSetSetting::NumCircuits, numCircuits, flags);

        uint8_t defaultInspectionInterval = Config::Get().general.defaultInspectionInterval;
        if (defaultInspectionInterval <= RIDE_INSPECTION_NEVER)
            SetOperatingSettingNested(ride->id, RideSetSetting::InspectionInterval, defaultInspectionInterval, flags);

        ride->lifecycleFlags |= RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN;
        ride->vehicleColourSettings = _td.appearance.vehicleColourSettings;

        ride->entranceStyle = objManager.GetLoadedObjectEntryIndex(_td.appearance.stationObjectIdentifier);
        if (ride->entranceStyle == kObjectEntryIndexNull)
        {
            ride->entranceStyle = gameState.lastEntranceStyle;
        }

        for (size_t i = 0; i < std::min(std::size(ride->trackColours), std::size(_td.appearance.trackColours)); i++)
        {
            ride->trackColours[i] = _td.appearance.trackColours[i];
        }

        for (size_t i = 0; i < Limits::kMaxVehicleColours; i++)
        {
            ride->vehicleColours[i] = _td.appearance.vehicleColours[i];
        }

        for (int32_t count = 1; count == 1 || r.Error != Status::Ok; ++count)
        {
            auto name = count == 1 ? _td.gameStateData.name : (_td.gameStateData.name + " " + std::to_string(count));
            auto gameAction = RideSetNameAction(ride->id, name);
            gameAction.SetFlags(GetFlags());
            r = ExecuteNested(&gameAction, gameState);
        }
        res.Cost = execRes.Cost;
        res.SetData(RideId{ ride->id });

        return res;
    }
} // namespace OpenRCT2::GameActions
