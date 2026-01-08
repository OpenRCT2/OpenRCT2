/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideCreateAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../localisation/Localisation.Date.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"

#include <algorithm>

namespace OpenRCT2::GameActions
{
    RideCreateAction::RideCreateAction(
        ride_type_t rideType, ObjectEntryIndex subType, uint8_t trackColourPreset, uint8_t vehicleColourPreset,
        ObjectEntryIndex entranceObjectIndex, RideInspection inspectionInterval)
        : _rideType(rideType)
        , _subType(subType)
        , _entranceObjectIndex(entranceObjectIndex)
        , _trackColourPreset(trackColourPreset)
        , _vehicleColourPreset(vehicleColourPreset)
        , _inspectionInterval(inspectionInterval)
    {
    }

    void RideCreateAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("rideType", _rideType);
        visitor.Visit("rideObject", _subType);
        visitor.Visit("entranceObject", _entranceObjectIndex);
        visitor.Visit("colour1", _trackColourPreset);
        visitor.Visit("colour2", _vehicleColourPreset);
        visitor.Visit("inspectionInterval", _inspectionInterval);
    }

    ride_type_t RideCreateAction::GetRideType() const
    {
        return _rideType;
    }

    ObjectEntryIndex RideCreateAction::GetRideObject() const
    {
        return _subType;
    }

    uint16_t RideCreateAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void RideCreateAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideType) << DS_TAG(_subType) << DS_TAG(_entranceObjectIndex) << DS_TAG(_trackColourPreset)
               << DS_TAG(_vehicleColourPreset) << DS_TAG(_inspectionInterval);
    }

    Result RideCreateAction::Query(GameState_t& gameState) const
    {
        if (_inspectionInterval > RideInspection::never)
        {
            LOG_ERROR("Invalid inspection interval: %u", EnumValue(_inspectionInterval));
            return Result(Status::invalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        auto rideIndex = GetNextFreeRideId();
        if (rideIndex.IsNull())
        {
            // No more free slots available.
            return Result(Status::noFreeElements, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_TOO_MANY_RIDES);
        }

        if (_rideType >= RIDE_TYPE_COUNT)
        {
            LOG_ERROR("Invalid ride type %d", _rideType);
            return Result(Status::invalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_INVALID_RIDE_TYPE);
        }

        int32_t rideEntryIndex = RideGetEntryIndex(_rideType, _subType);
        if (rideEntryIndex >= kMaxRideObjects)
        {
            LOG_ERROR("Ride entry not found for rideType %d, subType %d", _rideType, _subType);
            return Result(Status::invalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_INVALID_RIDE_TYPE);
        }

        const auto& colourPresets = GetRideTypeDescriptor(_rideType).ColourPresets;
        if (_trackColourPreset >= colourPresets.count)
        {
            LOG_ERROR("Can't create ride, invalid colour preset %d", _trackColourPreset);
            return Result(Status::invalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_ERR_INVALID_COLOUR);
        }

        const auto* rideEntry = GetRideEntryByIndex(rideEntryIndex);
        if (rideEntry == nullptr)
        {
            LOG_ERROR("Ride entry not found for rideEntryIndex %d", rideEntryIndex);
            return Result(Status::invalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_UNKNOWN_OBJECT_TYPE);
        }

        const auto* presetList = rideEntry->vehicle_preset_list;
        if ((presetList->count > 0 && presetList->count != 255) && _vehicleColourPreset >= presetList->count)
        {
            return Result(Status::invalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, kStringIdNone);
        }

        auto res = Result();
        res.setData(RideId{ rideIndex });

        return res;
    }

    Result RideCreateAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();

        int32_t rideEntryIndex = RideGetEntryIndex(_rideType, _subType);
        auto rideIndex = GetNextFreeRideId();

        auto ride = RideAllocateAtIndex(rideIndex);
        const auto* rideEntry = GetRideEntryByIndex(rideEntryIndex);
        if (rideEntry == nullptr)
        {
            LOG_ERROR(
                "Ride entry not found for index rideEntryIndex %u (from rideType %u, subType %u)", rideEntryIndex, _rideType,
                _subType);
            return Result(Status::unknown, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_UNKNOWN_OBJECT_TYPE);
        }

        ride->type = _rideType;
        ride->subtype = rideEntryIndex;
        ride->setColourPreset(_trackColourPreset, _vehicleColourPreset);
        ride->overallView.SetNull();
        ride->setNameToDefault();

        // Default initialize all stations.
        RideStation station{};
        station.Start.SetNull();
        station.Entrance.SetNull();
        station.Exit.SetNull();
        std::ranges::fill(ride->getStations(), station);

        ride->status = RideStatus::closed;
        ride->numTrains = 1;

        if (gameState.cheats.disableTrainLengthLimit)
        {
            // Reduce amount of proposed trains to prevent 32 trains from always spawning when limits are disabled
            if (rideEntry->cars_per_flat_ride == kNoFlatRideCars)
            {
                ride->proposedNumTrains = 12;
            }
            else
            {
                ride->proposedNumTrains = rideEntry->cars_per_flat_ride;
            }
        }
        else
        {
            ride->proposedNumTrains = 32;
        }
        ride->maxTrains = Limits::kMaxTrainsPerRide;
        ride->numCarsPerTrain = 1;
        ride->proposedNumCarsPerTrain = rideEntry->max_cars_in_train;
        ride->minWaitingTime = 10;
        ride->maxWaitingTime = 60;
        ride->departFlags = RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH | 3;

        const auto& rtd = ride->getRideTypeDescriptor();
        if (rtd.HasFlag(RtdFlag::allowMusic))
        {
            auto& objManager = GetContext()->GetObjectManager();
            ride->music = objManager.GetLoadedObjectEntryIndex(rtd.DefaultMusic);
            if (ride->music != kObjectEntryIndexNull)
            {
                if (rtd.HasFlag(RtdFlag::hasMusicByDefault))
                {
                    ride->lifecycleFlags |= RIDE_LIFECYCLE_MUSIC;
                }
            }
        }

        const auto& operatingSettings = rtd.OperatingSettings;
        ride->operationOption = (operatingSettings.MinValue * 3 + operatingSettings.MaxValue) / 4;

        ride->liftHillSpeed = rtd.LiftData.minimum_speed;

        ride->ratings.setNull();

        if (!(gameState.park.flags & PARK_FLAGS_NO_MONEY))
        {
            for (auto i = 0; i < RCT2::ObjectLimits::kMaxShopItemsPerRideEntry; i++)
            {
                ride->price[i] = rtd.DefaultPrices[i];
            }

            if (rideEntry->shop_item[0] == ShopItem::none)
            {
                if (!Park::RidePricesUnlocked() || gameState.park.entranceFee > 0)
                {
                    ride->price[0] = 0;
                }
            }
            else
            {
                ride->price[0] = GetShopItemDescriptor(rideEntry->shop_item[0]).DefaultPrice;
            }
            if (rideEntry->shop_item[1] != ShopItem::none)
            {
                ride->price[1] = GetShopItemDescriptor(rideEntry->shop_item[1]).DefaultPrice;
            }

            if (gameState.scenarioOptions.objective.Type == Scenario::ObjectiveType::buildTheBest)
            {
                ride->price[0] = 0;
            }

            if (rtd.specialType == RtdSpecialType::toilet)
            {
                if (ShopItemHasCommonPrice(ShopItem::admission))
                {
                    auto price = RideGetCommonPrice(*ride);
                    if (price != kMoney64Undefined)
                    {
                        ride->price[0] = price;
                    }
                }
            }

            for (auto i = 0; i < RCT2::ObjectLimits::kMaxShopItemsPerRideEntry; i++)
            {
                if (rideEntry->shop_item[i] != ShopItem::none)
                {
                    if (ShopItemHasCommonPrice(rideEntry->shop_item[i]))
                    {
                        auto price = ShopItemGetCommonPrice(ride, rideEntry->shop_item[i]);
                        if (price != kMoney64Undefined)
                        {
                            ride->price[i] = price;
                        }
                    }
                }
            }

            // Set the on-ride photo price, whether the ride has one or not (except shops).
            if (!rtd.HasFlag(RtdFlag::isShopOrFacility) && ShopItemHasCommonPrice(ShopItem::photo))
            {
                auto price = ShopItemGetCommonPrice(ride, ShopItem::photo);
                if (price != kMoney64Undefined)
                {
                    ride->price[1] = price;
                }
            }
        }

        ride->value = kRideValueUndefined;
        ride->satisfaction = 255;
        ride->popularity = 255;
        ride->buildDate = GetDate().GetMonthsElapsed();
        ride->musicTuneId = kTuneIDNull;

        ride->breakdownReason = BREAKDOWN_NONE;
        ride->upkeepCost = kMoney64Undefined;
        ride->reliability = kRideInitialReliability;
        ride->unreliabilityFactor = 1;
        ride->inspectionInterval = _inspectionInterval;
        ride->lastCrashType = RIDE_CRASH_TYPE_NONE;
        ride->incomePerHour = kMoney64Undefined;
        ride->profit = kMoney64Undefined;

        ride->entranceStyle = kObjectEntryIndexNull;
        if (rtd.HasFlag(RtdFlag::hasEntranceAndExit))
        {
            ride->entranceStyle = _entranceObjectIndex;
        }

        ride->numCircuits = 1;
        ride->mode = ride->getDefaultMode();
        ride->minCarsPerTrain = rideEntry->min_cars_in_train;
        ride->maxCarsPerTrain = rideEntry->max_cars_in_train;
        RideSetVehicleColoursToRandomPreset(*ride, _vehicleColourPreset);

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::rideList);

        res.expenditure = ExpenditureType::rideConstruction;
        res.setData(RideId{ rideIndex });

        return res;
    }
} // namespace OpenRCT2::GameActions
