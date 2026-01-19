/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    TrackDesignAction::TrackDesignAction(
        const CoordsXYZD& location, const TrackDesign& td, bool placeScenery, RideInspection inspectionInterval)
        : _loc(location)
        , _td(td)
        , _placeScenery(placeScenery)
        , _inspectionInterval(inspectionInterval)
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
        stream << DS_TAG(_inspectionInterval);
    }

    Result TrackDesignAction::Query(GameState_t& gameState) const
    {
        auto res = Result();
        res.position.x = _loc.x + 16;
        res.position.y = _loc.y + 16;
        res.position.z = _loc.z;
        res.expenditure = ExpenditureType::rideConstruction;

        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_OFF_EDGE_OF_MAP);
        }

        if (_inspectionInterval > RideInspection::never)
        {
            LOG_ERROR("Invalid inspection interval: %u", EnumValue(_inspectionInterval));
            return Result(Status::invalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_VALUE_OUT_OF_RANGE);
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
        auto rideCreateAction = RideCreateAction(
            _td.trackAndVehicle.rtdIndex, entryIndex, 0, 0, gameState.lastEntranceStyle, _inspectionInterval);
        rideCreateAction.SetFlags(GetFlags());
        auto r = ExecuteNested(&rideCreateAction, gameState);
        if (r.error != Status::ok)
        {
            return Result(Status::noFreeElements, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, kStringIdNone);
        }

        const auto rideIndex = r.getData<RideId>();
        auto ride = GetRide(rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %d", rideIndex);
            return Result(Status::unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_RIDE_NOT_FOUND);
        }

        bool placeScenery = _placeScenery;

        CommandFlags flags = {};
        flags.set(CommandFlag::ghost, GetFlags().has(CommandFlag::ghost));
        flags.set(CommandFlag::replay, GetFlags().has(CommandFlag::replay));

        auto queryRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
        if (_trackDesignPlaceStateSceneryUnavailable)
        {
            placeScenery = false;
            queryRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
        }

        auto gameAction = RideDemolishAction(ride->id, RideModifyType::demolish);
        gameAction.SetFlags(GetFlags());

        ExecuteNested(&gameAction, gameState);

        if (queryRes.error != Status::ok)
        {
            res.error = queryRes.error;
            res.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            res.errorMessage = queryRes.errorMessage;
            res.errorMessageArgs = queryRes.errorMessageArgs;
            return res;
        }

        res.cost = queryRes.cost;
        res.setData(RideId{ RideId::GetNull() });

        return res;
    }

    Result TrackDesignAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.position.x = _loc.x + 16;
        res.position.y = _loc.y + 16;
        res.position.z = _loc.z;
        res.expenditure = ExpenditureType::rideConstruction;

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
        auto rideCreateAction = RideCreateAction(
            _td.trackAndVehicle.rtdIndex, entryIndex, 0, 0, gameState.lastEntranceStyle, _inspectionInterval);
        rideCreateAction.SetFlags(GetFlags());
        auto r = ExecuteNested(&rideCreateAction, gameState);
        if (r.error != Status::ok)
        {
            return Result(Status::noFreeElements, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, kStringIdNone);
        }

        const auto rideIndex = r.getData<RideId>();
        auto ride = GetRide(rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %d", rideIndex);
            return Result(Status::unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_RIDE_NOT_FOUND);
        }

        // Query first, this is required again to determine if scenery is available.
        auto flags = GetFlags().without(CommandFlag::apply);

        bool placeScenery = _placeScenery;

        auto queryRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
        if (_trackDesignPlaceStateSceneryUnavailable)
        {
            placeScenery = false;
            queryRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
        }

        if (queryRes.error != Status::ok)
        {
            auto gameAction = RideDemolishAction(ride->id, RideModifyType::demolish);
            gameAction.SetFlags(GetFlags());
            ExecuteNested(&gameAction, gameState);

            res.error = queryRes.error;
            res.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            res.errorMessage = queryRes.errorMessage;
            res.errorMessageArgs = queryRes.errorMessageArgs;

            return res;
        }

        // Execute.
        flags.set(CommandFlag::apply);

        auto execRes = TrackDesignPlace(_td, flags, placeScenery, *ride, _loc);
        if (execRes.error != Status::ok)
        {
            auto gameAction = RideDemolishAction(ride->id, RideModifyType::demolish);
            gameAction.SetFlags(GetFlags());
            ExecuteNested(&gameAction, gameState);

            res.error = execRes.error;
            res.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            res.errorMessage = execRes.errorMessage;
            res.errorMessageArgs = execRes.errorMessageArgs;

            return res;
        }

        if (entryIndex != kObjectEntryIndexNull)
        {
            auto colour = RideGetUnusedPresetVehicleColour(entryIndex);
            auto rideSetVehicleAction = RideSetVehicleAction(ride->id, RideSetVehicleType::RideEntry, entryIndex, colour);
            ExecuteNested(&rideSetVehicleAction, gameState);
        }

        SetOperatingSettingNested(ride->id, RideSetSetting::Mode, static_cast<uint8_t>(_td.operation.rideMode), flags);
        auto rideSetVehicleAction2 = RideSetVehicleAction(
            ride->id, RideSetVehicleType::NumTrains, _td.trackAndVehicle.numberOfTrains);
        ExecuteNested(&rideSetVehicleAction2, gameState);

        auto rideSetVehicleAction3 = RideSetVehicleAction(
            ride->id, RideSetVehicleType::NumCarsPerTrain, _td.trackAndVehicle.numberOfCarsPerTrain);
        ExecuteNested(&rideSetVehicleAction3, gameState);

        SetOperatingSettingNested(ride->id, RideSetSetting::Departure, _td.operation.departFlags, flags);
        SetOperatingSettingNested(ride->id, RideSetSetting::MinWaitingTime, _td.operation.minWaitingTime, flags);
        SetOperatingSettingNested(ride->id, RideSetSetting::MaxWaitingTime, _td.operation.maxWaitingTime, flags);
        SetOperatingSettingNested(ride->id, RideSetSetting::Operation, _td.operation.operationSetting, flags);
        SetOperatingSettingNested(ride->id, RideSetSetting::LiftHillSpeed, _td.operation.liftHillSpeed, flags);

        auto numCircuits = std::max<uint8_t>(1, _td.operation.numCircuits);
        SetOperatingSettingNested(ride->id, RideSetSetting::NumCircuits, numCircuits, flags);

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

        for (int32_t count = 1; count == 1 || r.error != Status::ok; ++count)
        {
            auto name = count == 1 ? _td.gameStateData.name : (_td.gameStateData.name + " " + std::to_string(count));
            auto gameAction = RideSetNameAction(ride->id, name);
            gameAction.SetFlags(GetFlags());
            r = ExecuteNested(&gameAction, gameState);
        }
        res.cost = execRes.cost;
        res.setData(RideId{ ride->id });

        return res;
    }
} // namespace OpenRCT2::GameActions
