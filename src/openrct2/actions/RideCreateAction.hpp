#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#pragma once

#include "../core/MemoryStream.h"
#include "../localisation/string_ids.h"
#include "GameAction.h"

extern "C"
{
#include "../localisation/date.h"
#include "../cheats.h"
#include "../interface/window.h"
#include "../world/park.h"
#include "../rct1.h"
#include "../ride/ride_data.h"
#include "../ride/ride.h"
}

struct RideCreateGameActionResult : public GameActionResult {
    RideCreateGameActionResult() {}
    RideCreateGameActionResult(GA_ERROR error, rct_string_id message) : GameActionResult(error, message) {}

    sint32& RideIndex()
    {
        return reinterpret_cast<sint32&>(Results[0]);
    }

    uint32& RideColor()
    {
        return reinterpret_cast<uint32&>(Results[1]);
    }
};

struct RideCreateAction : public GameAction<GAME_COMMAND_CREATE_RIDE, GA_FLAGS::ALLOW_WHILE_PAUSED>
{
public:
    sint32 rideType;
    sint32 rideSubType;

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << rideType << rideSubType;
    }

    GameActionResult Query() const override
    {
        if (rideType >= RIDE_TYPE_COUNT)
        {
            return RideCreateGameActionResult(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_RIDE_TYPE);
        }

        return GameActionResult();
    }

    GameActionResult Execute() const override
    {
        RideCreateGameActionResult res;

        rct_ride *ride;
        rct_ride_entry *rideEntry;
        sint32 rideIndex, rideEntryIndex;
        sint32 subType = rideSubType;

        if (subType == RIDE_ENTRY_INDEX_NULL)
        {
            uint8 *availableRideEntries = get_ride_entry_indices_for_ride_type(rideType);
            for (uint8 *rei = availableRideEntries; *rei != RIDE_ENTRY_INDEX_NULL; rei++) {
                rideEntry = get_ride_entry(*rei);

                // Can happen in select-by-track-type mode
                if (!ride_entry_is_invented(*rei))
                {
                    continue;
                }

                if (!(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) || rideTypeShouldLoseSeparateFlag(rideEntry)) {
                    subType = *rei;
                    break;
                }
            }
            if (subType == RIDE_ENTRY_INDEX_NULL)
            {
                subType = availableRideEntries[0];
                if (subType == RIDE_ENTRY_INDEX_NULL) {
                    return RideCreateGameActionResult(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_RIDE_TYPE);
                }
            }
        }

        rideEntryIndex = subType;
        rideIndex = ride_get_empty_slot();

        if (subType >= 128)
        {
            return RideCreateGameActionResult(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_RIDE_TYPE);
        }

        if (rideIndex == -1) 
        {
            return RideCreateGameActionResult(GA_ERROR::DISALLOWED, STR_TOO_MANY_RIDES);
        }

        res.RideIndex() = rideIndex;
        res.RideColor() = ride_get_random_colour_preset_index(rideType) | (ride_get_unused_preset_vehicle_colour(rideType, subType) << 8);

        ride = get_ride(rideIndex);
        rideEntry = get_ride_entry(rideEntryIndex);
        if (rideEntry == (rct_ride_entry *)-1)
        {
            log_warning("Invalid request for ride %u", rideIndex);
            return RideCreateGameActionResult(GA_ERROR::UNKNOWN, STR_UNKNOWN_OBJECT_TYPE);
        }

        ride->type = rideType;
        ride->subtype = rideEntryIndex;
        ride_set_colour_preset(ride, res.RideColor() & 0xFF);
        ride->overall_view = 0xFFFF;

        // Ride name
        if (rideEntryIndex == RIDE_ENTRY_INDEX_NULL)
        {
            ride_set_name_to_track_default(ride, rideEntry);
        }
        else
        {
            ride_set_name_to_default(ride, rideEntry);
        }

        for (size_t i = 0; i < RCT12_MAX_STATIONS_PER_RIDE; i++) {
            ride->station_starts[i] = 0xFFFF;
            ride->entrances[i] = 0xFFFF;
            ride->exits[i] = 0xFFFF;
            ride->train_at_station[i] = 255;
            ride->queue_time[i] = 0;
        }

        for (size_t i = 0; i < MAX_VEHICLES_PER_RIDE; i++) {
            ride->vehicles[i] = SPRITE_INDEX_NULL;
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
        if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_MUSIC_ON_DEFAULT) {
            ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
        }
        ride->music = RideData4[ride->type].default_music;

        const rct_ride_properties rideProperties = RideProperties[ride->type];
        ride->operation_option = (rideProperties.min_value * 3 + rideProperties.max_value) / 4;

        ride->lift_hill_speed = RideLiftData[ride->type].minimum_speed;

        ride->measurement_index = 255;
        ride->excitement = (ride_rating)-1;
        ride->cur_num_customers = 0;
        ride->num_customers_timeout = 0;
        ride->chairlift_bullwheel_rotation = 0;

        ride->price = 0;
        ride->price_secondary = 0;
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
            ride->price = RideData4[ride->type].price;
            ride->price_secondary = RideData4[ride->type].price_secondary;

            if (rideEntry->shop_item == SHOP_ITEM_NONE) {
                if (!(gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) && !gCheatsUnlockAllPrices) {
                    ride->price = 0;
                }
            }
            else {
                ride->price = DefaultShopItemPrice[rideEntry->shop_item];
            }
            if (rideEntry->shop_item_secondary != SHOP_ITEM_NONE) {
                ride->price_secondary = DefaultShopItemPrice[rideEntry->shop_item_secondary];
            }

            if (gScenarioObjectiveType == OBJECTIVE_BUILD_THE_BEST) {
                ride->price = 0;
            }

            if (ride->type == RIDE_TYPE_TOILETS) {
                if (shop_item_has_common_price(SHOP_ITEM_ADMISSION)) {
                    money32 price = ride_get_common_price(ride);
                    if (price != MONEY32_UNDEFINED) {
                        ride->price = (money16)price;
                    }
                }
            }

            if (rideEntry->shop_item != SHOP_ITEM_NONE) {
                if (shop_item_has_common_price(rideEntry->shop_item)) {
                    money32 price = shop_item_get_common_price(ride, rideEntry->shop_item);
                    if (price != MONEY32_UNDEFINED) {
                        ride->price = (money16)price;
                    }
                }
            }

            if (rideEntry->shop_item_secondary != SHOP_ITEM_NONE) {
                if (shop_item_has_common_price(rideEntry->shop_item_secondary)) {
                    money32 price = shop_item_get_common_price(ride, rideEntry->shop_item_secondary);
                    if (price != MONEY32_UNDEFINED) {
                        ride->price_secondary = (money16)price;
                    }
                }
            }

            // Set the on-ride photo price, whether the ride has one or not (except shops).
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP) && shop_item_has_common_price(SHOP_ITEM_PHOTO)) {
                money32 price = shop_item_get_common_price(ride, SHOP_ITEM_PHOTO);
                if (price != MONEY32_UNDEFINED) {
                    ride->price_secondary = (money16)price;
                }
            }
        }

        memset(ride->num_customers, 0, sizeof(ride->num_customers));
        ride->value = 0xFFFF;
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
        ride->upkeep_cost = (money16)-1;
        ride->reliability = 0x64FF;
        ride->unreliability_factor = 1;
        ride->inspection_interval = RIDE_INSPECTION_EVERY_30_MINUTES;
        ride->last_inspection = 0;
        ride->downtime = 0;
        memset(ride->downtime_history, 0, sizeof(ride->downtime_history));
        ride->no_primary_items_sold = 0;
        ride->no_secondary_items_sold = 0;
        ride->last_crash_type = RIDE_CRASH_TYPE_NONE;
        ride->income_per_hour = MONEY32_UNDEFINED;
        ride->profit = MONEY32_UNDEFINED;
        ride->connected_message_throttle = 0;
        ride->entrance_style = RIDE_ENTRANCE_STYLE_PLAIN;
        ride->num_block_brakes = 0;
        ride->guests_favourite = 0;

        ride->num_circuits = 1;
        ride->mode = ride_get_default_mode(ride);
        ride->min_max_cars_per_train = (rideEntry->min_cars_in_train << 4) | rideEntry->max_cars_in_train;
        ride_set_vehicle_colours_to_random_preset(ride, 0xFF & (res.RideColor() >> 8));
        window_invalidate_by_class(WC_RIDE_LIST);

        res.ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        res.Position.x = (uint16)0x8000;

        return res;
    }
};

