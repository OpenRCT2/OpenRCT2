/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../rct1/RCT1.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../scenario/Scenario.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"

#include <algorithm>

using namespace OpenRCT2;

RideCreateAction::RideCreateAction(
    ride_type_t rideType, ObjectEntryIndex subType, colour_t colour1, colour_t colour2, ObjectEntryIndex entranceObjectIndex)
    : _rideType(rideType)
    , _subType(subType)
    , _entranceObjectIndex(entranceObjectIndex)
    , _colour1(colour1)
    , _colour2(colour2)
{
}

void RideCreateAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("rideType", _rideType);
    visitor.Visit("rideObject", _subType);
    visitor.Visit("entranceObject", _entranceObjectIndex);
    visitor.Visit("colour1", _colour1);
    visitor.Visit("colour2", _colour2);
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
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void RideCreateAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_rideType) << DS_TAG(_subType) << DS_TAG(_entranceObjectIndex) << DS_TAG(_colour1) << DS_TAG(_colour2);
}

GameActions::Result RideCreateAction::Query() const
{
    auto rideIndex = GetNextFreeRideId();
    if (rideIndex.IsNull())
    {
        // No more free slots available.
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_TOO_MANY_RIDES);
    }

    if (_rideType >= RIDE_TYPE_COUNT)
    {
        LOG_ERROR("Invalid ride type %d", _rideType);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_INVALID_RIDE_TYPE);
    }

    int32_t rideEntryIndex = RideGetEntryIndex(_rideType, _subType);
    if (rideEntryIndex >= kMaxRideObjects)
    {
        LOG_ERROR("Ride entry not found for rideType %d, subType %d", _rideType, _subType);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_INVALID_RIDE_TYPE);
    }

    const auto& colourPresets = GetRideTypeDescriptor(_rideType).ColourPresets;
    if (_colour1 >= colourPresets.count)
    {
        LOG_ERROR("Can't create ride, invalid colour preset %d", _colour1);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_ERR_INVALID_COLOUR);
    }

    const auto* rideEntry = GetRideEntryByIndex(rideEntryIndex);
    if (rideEntry == nullptr)
    {
        LOG_ERROR("Ride entry not found for rideEntryIndex %d", rideEntryIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_UNKNOWN_OBJECT_TYPE);
    }

    const auto* presetList = rideEntry->vehicle_preset_list;
    if ((presetList->count > 0 && presetList->count != 255) && _colour2 >= presetList->count)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, kStringIdNone);
    }

    auto res = GameActions::Result();
    res.SetData(RideId{ rideIndex });

    return res;
}

GameActions::Result RideCreateAction::Execute() const
{
    auto res = GameActions::Result();

    int32_t rideEntryIndex = RideGetEntryIndex(_rideType, _subType);
    auto rideIndex = GetNextFreeRideId();

    auto ride = RideAllocateAtIndex(rideIndex);
    const auto* rideEntry = GetRideEntryByIndex(rideEntryIndex);
    if (rideEntry == nullptr)
    {
        LOG_ERROR(
            "Ride entry not found for index rideEntryIndex %u (from rideType %u, subType %u)", rideEntryIndex, _rideType,
            _subType);
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_UNKNOWN_OBJECT_TYPE);
    }

    ride->type = _rideType;
    ride->subtype = rideEntryIndex;
    ride->SetColourPreset(_colour1);
    ride->overall_view.SetNull();
    ride->SetNameToDefault();

    // Default initialize all stations.
    RideStation station{};
    station.Start.SetNull();
    station.Entrance.SetNull();
    station.Exit.SetNull();
    std::ranges::fill(ride->GetStations(), station);

    ride->status = RideStatus::Closed;
    ride->NumTrains = 1;

    auto& gameState = GetGameState();
    if (gameState.Cheats.disableTrainLengthLimit)
    {
        // Reduce amount of proposed trains to prevent 32 trains from always spawning when limits are disabled
        if (rideEntry->cars_per_flat_ride == NoFlatRideCars)
        {
            ride->ProposedNumTrains = 12;
        }
        else
        {
            ride->ProposedNumTrains = rideEntry->cars_per_flat_ride;
        }
    }
    else
    {
        ride->ProposedNumTrains = 32;
    }
    ride->max_trains = OpenRCT2::Limits::kMaxTrainsPerRide;
    ride->num_cars_per_train = 1;
    ride->proposed_num_cars_per_train = rideEntry->max_cars_in_train;
    ride->min_waiting_time = 10;
    ride->max_waiting_time = 60;
    ride->depart_flags = RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH | 3;

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.HasFlag(RtdFlag::allowMusic))
    {
        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        ride->music = objManager.GetLoadedObjectEntryIndex(rtd.DefaultMusic);
        if (ride->music != kObjectEntryIndexNull)
        {
            if (rtd.HasFlag(RtdFlag::hasMusicByDefault))
            {
                ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
            }
        }
    }

    const auto& operatingSettings = rtd.OperatingSettings;
    ride->operation_option = (operatingSettings.MinValue * 3 + operatingSettings.MaxValue) / 4;

    ride->lift_hill_speed = rtd.LiftData.minimum_speed;

    ride->ratings.setNull();

    if (!(gameState.Park.Flags & PARK_FLAGS_NO_MONEY))
    {
        for (auto i = 0; i < RCT2::ObjectLimits::kMaxShopItemsPerRideEntry; i++)
        {
            ride->price[i] = rtd.DefaultPrices[i];
        }

        if (rideEntry->shop_item[0] == ShopItem::None)
        {
            if (!Park::RidePricesUnlocked() || gameState.Park.EntranceFee > 0)
            {
                ride->price[0] = 0;
            }
        }
        else
        {
            ride->price[0] = GetShopItemDescriptor(rideEntry->shop_item[0]).DefaultPrice;
        }
        if (rideEntry->shop_item[1] != ShopItem::None)
        {
            ride->price[1] = GetShopItemDescriptor(rideEntry->shop_item[1]).DefaultPrice;
        }

        if (gameState.ScenarioObjective.Type == OBJECTIVE_BUILD_THE_BEST)
        {
            ride->price[0] = 0;
        }

        if (rtd.specialType == RtdSpecialType::toilet)
        {
            if (ShopItemHasCommonPrice(ShopItem::Admission))
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
            if (rideEntry->shop_item[i] != ShopItem::None)
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
        if (!rtd.HasFlag(RtdFlag::isShopOrFacility) && ShopItemHasCommonPrice(ShopItem::Photo))
        {
            auto price = ShopItemGetCommonPrice(ride, ShopItem::Photo);
            if (price != kMoney64Undefined)
            {
                ride->price[1] = price;
            }
        }
    }

    ride->value = RIDE_VALUE_UNDEFINED;
    ride->satisfaction = 255;
    ride->popularity = 255;
    ride->build_date = GetDate().GetMonthsElapsed();
    ride->music_tune_id = kTuneIDNull;

    ride->breakdown_reason = 255;
    ride->upkeep_cost = kMoney64Undefined;
    ride->reliability = kRideInitialReliability;
    ride->unreliability_factor = 1;
    ride->inspection_interval = RIDE_INSPECTION_EVERY_30_MINUTES;
    ride->last_crash_type = RIDE_CRASH_TYPE_NONE;
    ride->income_per_hour = kMoney64Undefined;
    ride->profit = kMoney64Undefined;

    ride->entrance_style = kObjectEntryIndexNull;
    if (rtd.HasFlag(RtdFlag::hasEntranceAndExit))
    {
        ride->entrance_style = _entranceObjectIndex;
    }

    ride->num_circuits = 1;
    ride->mode = ride->GetDefaultMode();
    ride->MinCarsPerTrain = rideEntry->min_cars_in_train;
    ride->MaxCarsPerTrain = rideEntry->max_cars_in_train;
    RideSetVehicleColoursToRandomPreset(*ride, _colour2);

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::RideList);

    res.Expenditure = ExpenditureType::RideConstruction;
    res.SetData(RideId{ rideIndex });

    return res;
}
