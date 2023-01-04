/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideCreateAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectManager.h"
#include "../rct1/RCT1.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../scenario/Scenario.h"
#include "../world/Park.h"

#include <algorithm>

RideCreateAction::RideCreateAction(
    int32_t rideType, ObjectEntryIndex subType, int32_t colour1, int32_t colour2, ObjectEntryIndex entranceObjectIndex)
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

int32_t RideCreateAction::GetRideType() const
{
    return _rideType;
}

int32_t RideCreateAction::GetRideObject() const
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
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_INVALID_RIDE_TYPE);
    }

    int32_t rideEntryIndex = ride_get_entry_index(_rideType, _subType);
    if (rideEntryIndex >= MAX_RIDE_OBJECTS)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_INVALID_RIDE_TYPE);
    }

    const auto& colourPresets = GetRideTypeDescriptor(_rideType).ColourPresets;
    if (_colour1 >= colourPresets.count)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_NONE);
    }

    rct_ride_entry* rideEntry = get_ride_entry(rideEntryIndex);
    if (rideEntry == nullptr)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_NONE);
    }

    vehicle_colour_preset_list* presetList = rideEntry->vehicle_preset_list;
    if ((presetList->count > 0 && presetList->count != 255) && _colour2 >= presetList->count)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_NONE);
    }

    auto res = GameActions::Result();
    res.SetData(RideId{ rideIndex });

    return res;
}

GameActions::Result RideCreateAction::Execute() const
{
    rct_ride_entry* rideEntry;
    auto res = GameActions::Result();

    int32_t rideEntryIndex = ride_get_entry_index(_rideType, _subType);
    auto rideIndex = GetNextFreeRideId();

    auto ride = GetOrAllocateRide(rideIndex);
    rideEntry = get_ride_entry(rideEntryIndex);
    if (rideEntry == nullptr)
    {
        log_warning("Invalid request for ride %u", rideIndex);
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, STR_UNKNOWN_OBJECT_TYPE);
    }

    ride->id = rideIndex;
    ride->type = _rideType;
    ride->subtype = rideEntryIndex;
    ride->SetColourPreset(_colour1);
    ride->overall_view.SetNull();
    ride->SetNameToDefault();

    for (auto& station : ride->GetStations())
    {
        station.Start.SetNull();
        station.Entrance.SetNull();
        station.Exit.SetNull();
        station.TrainAtStation = RideStation::NO_TRAIN;
        station.QueueTime = 0;
    }

    std::fill(std::begin(ride->vehicles), std::end(ride->vehicles), EntityId::GetNull());

    ride->status = RideStatus::Closed;
    ride->lifecycle_flags = 0;
    ride->vehicle_change_timeout = 0;
    ride->num_stations = 0;
    ride->NumTrains = 1;
    if (gCheatsDisableTrainLengthLimit)
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
    ride->max_trains = OpenRCT2::Limits::MaxTrainsPerRide;
    ride->num_cars_per_train = 1;
    ride->proposed_num_cars_per_train = rideEntry->max_cars_in_train;
    ride->min_waiting_time = 10;
    ride->max_waiting_time = 60;
    ride->depart_flags = RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH | 3;

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_ALLOW_MUSIC))
    {
        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        ride->music = objManager.GetLoadedObjectEntryIndex(rtd.DefaultMusic);
        if (ride->music != OBJECT_ENTRY_INDEX_NULL)
        {
            if (rtd.HasFlag(RIDE_TYPE_FLAG_MUSIC_ON_DEFAULT))
            {
                ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
            }
        }
    }

    const auto& operatingSettings = rtd.OperatingSettings;
    ride->operation_option = (operatingSettings.MinValue * 3 + operatingSettings.MaxValue) / 4;

    ride->lift_hill_speed = rtd.LiftData.minimum_speed;

    ride->measurement = {};
    ride->excitement = RIDE_RATING_UNDEFINED;
    ride->cur_num_customers = 0;
    ride->num_customers_timeout = 0;
    ride->chairlift_bullwheel_rotation = 0;

    for (auto& price : ride->price)
    {
        price = 0;
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        for (auto i = 0; i < RCT2::ObjectLimits::MaxShopItemsPerRideEntry; i++)
        {
            ride->price[i] = rtd.DefaultPrices[i];
        }

        if (rideEntry->shop_item[0] == ShopItem::None)
        {
            if (!ParkRidePricesUnlocked())
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

        if (gScenarioObjective.Type == OBJECTIVE_BUILD_THE_BEST)
        {
            ride->price[0] = 0;
        }

        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET))
        {
            if (shop_item_has_common_price(ShopItem::Admission))
            {
                money32 price = ride_get_common_price(ride);
                if (price != MONEY32_UNDEFINED)
                {
                    ride->price[0] = static_cast<money16>(price);
                }
            }
        }

        for (auto i = 0; i < RCT2::ObjectLimits::MaxShopItemsPerRideEntry; i++)
        {
            if (rideEntry->shop_item[i] != ShopItem::None)
            {
                if (shop_item_has_common_price(rideEntry->shop_item[i]))
                {
                    money32 price = shop_item_get_common_price(ride, rideEntry->shop_item[i]);
                    if (price != MONEY32_UNDEFINED)
                    {
                        ride->price[i] = static_cast<money16>(price);
                    }
                }
            }
        }

        // Set the on-ride photo price, whether the ride has one or not (except shops).
        if (!rtd.HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY) && shop_item_has_common_price(ShopItem::Photo))
        {
            money32 price = shop_item_get_common_price(ride, ShopItem::Photo);
            if (price != MONEY32_UNDEFINED)
            {
                ride->price[1] = static_cast<money16>(price);
            }
        }
    }

    std::fill(std::begin(ride->num_customers), std::end(ride->num_customers), 0);
    ride->value = RIDE_VALUE_UNDEFINED;
    ride->satisfaction = 255;
    ride->satisfaction_time_out = 0;
    ride->satisfaction_next = 0;
    ride->popularity = 255;
    ride->popularity_time_out = 0;
    ride->popularity_next = 0;
    ride->window_invalidate_flags = 0;
    ride->total_customers = 0;
    ride->total_profit = 0;
    ride->num_riders = 0;
    ride->slide_in_use = 0;
    ride->maze_tiles = 0;
    ride->build_date = gDateMonthsElapsed;
    ride->music_tune_id = TUNE_ID_NULL;

    ride->breakdown_reason = 255;
    ride->upkeep_cost = MONEY16_UNDEFINED;
    ride->reliability = RIDE_INITIAL_RELIABILITY;
    ride->unreliability_factor = 1;
    ride->inspection_interval = RIDE_INSPECTION_EVERY_30_MINUTES;
    ride->last_inspection = 0;
    ride->downtime = 0;
    std::fill_n(ride->downtime_history, sizeof(ride->downtime_history), 0x00);
    ride->no_primary_items_sold = 0;
    ride->no_secondary_items_sold = 0;
    ride->last_crash_type = RIDE_CRASH_TYPE_NONE;
    ride->income_per_hour = MONEY64_UNDEFINED;
    ride->profit = MONEY64_UNDEFINED;
    ride->connected_message_throttle = 0;

    ride->entrance_style = OBJECT_ENTRY_INDEX_NULL;
    if (rtd.HasFlag(RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT))
    {
        ride->entrance_style = _entranceObjectIndex;
    }

    ride->num_block_brakes = 0;
    ride->guests_favourite = 0;

    ride->num_circuits = 1;
    ride->mode = ride->GetDefaultMode();
    ride->MinCarsPerTrain = rideEntry->min_cars_in_train;
    ride->MaxCarsPerTrain = rideEntry->max_cars_in_train;
    ride_set_vehicle_colours_to_random_preset(ride, _colour2);
    window_invalidate_by_class(WindowClass::RideList);

    res.Expenditure = ExpenditureType::RideConstruction;
    res.SetData(RideId{ rideIndex });

    return res;
}
