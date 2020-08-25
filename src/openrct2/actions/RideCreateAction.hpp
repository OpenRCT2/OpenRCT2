/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/StringIds.h"
#include "../rct1/RCT1.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../scenario/Scenario.h"
#include "../world/Park.h"
#include "GameAction.h"

#include <algorithm>

class RideCreateGameActionResult final : public GameActionResult
{
public:
    RideCreateGameActionResult()
        : GameActionResult(GA_ERROR::OK, STR_NONE)
    {
    }
    RideCreateGameActionResult(GA_ERROR error, rct_string_id message)
        : GameActionResult(error, STR_CANT_CREATE_NEW_RIDE_ATTRACTION, message)
    {
    }

    ride_id_t rideIndex = RIDE_ID_NULL;
};

DEFINE_GAME_ACTION(RideCreateAction, GAME_COMMAND_CREATE_RIDE, RideCreateGameActionResult)
{
private:
    int32_t _rideType{ RIDE_ID_NULL };
    int32_t _subType{ RIDE_ENTRY_INDEX_NULL };
    uint8_t _colour1{ 0xFF };
    uint8_t _colour2{ 0xFF };

public:
    RideCreateAction() = default;

    RideCreateAction(int32_t rideType, int32_t subType, int32_t colour1, int32_t colour2)
        : _rideType(rideType)
        , _subType(subType)
        , _colour1(colour1)
        , _colour2(colour2)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit("rideType", _rideType);
        visitor.Visit("rideObject", _subType);
        visitor.Visit("colour1", _colour1);
        visitor.Visit("colour2", _colour2);
    }

    int32_t GetRideType() const
    {
        return _rideType;
    }

    int32_t GetRideObject() const
    {
        return _subType;
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideType) << DS_TAG(_subType) << DS_TAG(_colour1) << DS_TAG(_colour2);
    }

    GameActionResult::Ptr Query() const override
    {
        auto rideIndex = GetNextFreeRideId();
        if (rideIndex == RIDE_ID_NULL)
        {
            // No more free slots available.
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_TOO_MANY_RIDES);
        }

        if (_rideType >= RIDE_TYPE_COUNT)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_RIDE_TYPE);
        }

        int32_t rideEntryIndex = ride_get_entry_index(_rideType, _subType);
        if (rideEntryIndex >= MAX_RIDE_OBJECTS)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_RIDE_TYPE);
        }

        const auto& colourPresets = RideTypeDescriptors[_rideType].ColourPresets;
        if (_colour1 >= colourPresets.count)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        rct_ride_entry* rideEntry = get_ride_entry(rideEntryIndex);
        if (rideEntry == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        vehicle_colour_preset_list* presetList = rideEntry->vehicle_preset_list;
        if ((presetList->count > 0 && presetList->count != 255) && _colour2 >= presetList->count)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        rct_ride_entry* rideEntry;
        auto res = MakeResult();

        int32_t rideEntryIndex = ride_get_entry_index(_rideType, _subType);
        auto rideIndex = GetNextFreeRideId();

        res->rideIndex = rideIndex;

        auto ride = GetOrAllocateRide(rideIndex);
        rideEntry = get_ride_entry(rideEntryIndex);
        if (rideEntry == nullptr)
        {
            log_warning("Invalid request for ride %u", rideIndex);
            res->Error = GA_ERROR::UNKNOWN;
            res->ErrorMessage = STR_UNKNOWN_OBJECT_TYPE;
            return res;
        }

        ride->id = rideIndex;
        ride->type = _rideType;
        ride->subtype = rideEntryIndex;
        ride->SetColourPreset(_colour1);
        ride->overall_view.setNull();
        ride->SetNameToDefault();

        for (int32_t i = 0; i < MAX_STATIONS; i++)
        {
            ride->stations[i].Start.setNull();
            ride_clear_entrance_location(ride, i);
            ride_clear_exit_location(ride, i);
            ride->stations[i].TrainAtStation = RideStation::NO_TRAIN;
            ride->stations[i].QueueTime = 0;
        }

        for (auto& vehicle : ride->vehicles)
        {
            vehicle = SPRITE_INDEX_NULL;
        }

        ride->status = RIDE_STATUS_CLOSED;
        ride->lifecycle_flags = 0;
        ride->vehicle_change_timeout = 0;
        ride->num_stations = 0;
        ride->num_vehicles = 1;
        ride->proposed_num_vehicles = 32;
        ride->max_trains = 32;
        ride->num_cars_per_train = 1;
        ride->proposed_num_cars_per_train = 12;
        ride->min_waiting_time = 10;
        ride->max_waiting_time = 60;
        ride->depart_flags = RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH | 3;
        if (RideTypeDescriptors[ride->type].Flags & RIDE_TYPE_FLAG_MUSIC_ON_DEFAULT)
        {
            ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
        }
        ride->music = RideTypeDescriptors[ride->type].DefaultMusic;

        const auto& operatingSettings = RideTypeDescriptors[ride->type].OperatingSettings;
        ride->operation_option = (operatingSettings.MinValue * 3 + operatingSettings.MaxValue) / 4;

        ride->lift_hill_speed = RideTypeDescriptors[ride->type].LiftData.minimum_speed;

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
            for (auto i = 0; i < NUM_SHOP_ITEMS_PER_RIDE; i++)
            {
                ride->price[i] = RideTypeDescriptors[ride->type].DefaultPrices[i];
            }

            if (rideEntry->shop_item[0] == SHOP_ITEM_NONE)
            {
                if (!park_ride_prices_unlocked())
                {
                    ride->price[0] = 0;
                }
            }
            else
            {
                ride->price[0] = ShopItems[rideEntry->shop_item[0]].DefaultPrice;
            }
            if (rideEntry->shop_item[1] != SHOP_ITEM_NONE)
            {
                ride->price[1] = ShopItems[rideEntry->shop_item[1]].DefaultPrice;
            }

            if (gScenarioObjectiveType == OBJECTIVE_BUILD_THE_BEST)
            {
                ride->price[0] = 0;
            }

            if (ride->type == RIDE_TYPE_TOILETS)
            {
                if (shop_item_has_common_price(SHOP_ITEM_ADMISSION))
                {
                    money32 price = ride_get_common_price(ride);
                    if (price != MONEY32_UNDEFINED)
                    {
                        ride->price[0] = static_cast<money16>(price);
                    }
                }
            }

            for (auto i = 0; i < NUM_SHOP_ITEMS_PER_RIDE; i++)
            {
                if (rideEntry->shop_item[i] != SHOP_ITEM_NONE)
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
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP) && shop_item_has_common_price(SHOP_ITEM_PHOTO))
            {
                money32 price = shop_item_get_common_price(ride, SHOP_ITEM_PHOTO);
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
        ride->music_tune_id = 255;

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
        ride->income_per_hour = MONEY32_UNDEFINED;
        ride->profit = MONEY32_UNDEFINED;
        ride->connected_message_throttle = 0;
        ride->entrance_style = 0;
        ride->num_block_brakes = 0;
        ride->guests_favourite = 0;

        ride->num_circuits = 1;
        ride->mode = ride->GetDefaultMode();
        ride->min_max_cars_per_train = (rideEntry->min_cars_in_train << 4) | rideEntry->max_cars_in_train;
        ride_set_vehicle_colours_to_random_preset(ride, _colour2);
        window_invalidate_by_class(WC_RIDE_LIST);

        res->Expenditure = ExpenditureType::RideConstruction;

        return res;
    }
};
