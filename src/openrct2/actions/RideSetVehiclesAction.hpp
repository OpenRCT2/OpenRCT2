/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Research.h"
#include "../ride/Ride.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"
#include "GameAction.h"

enum class RideSetVehicleType : uint8_t
{
    NumTrains,
    NumCarsPerTrain,
    RideEntry
};

DEFINE_GAME_ACTION(RideSetVehicleAction, GAME_COMMAND_SET_RIDE_VEHICLES, GameActionResult)
{
private:
    NetworkRideId_t _rideIndex{ -1 };
    uint8_t _type;
    uint8_t _value;
    uint8_t _colour;

    constexpr static rct_string_id SetVehicleTypeErrorTitle[] = { STR_RIDE_SET_VEHICLE_SET_NUM_TRAINS_FAIL,
                                                                  STR_RIDE_SET_VEHICLE_SET_NUM_CARS_PER_TRAIN_FAIL,
                                                                  STR_RIDE_SET_VEHICLE_TYPE_FAIL };

public:
    RideSetVehicleAction()
    {
    }

    RideSetVehicleAction(ride_id_t rideIndex, RideSetVehicleType type, uint8_t value, uint8_t colour = 0)
        : _rideIndex(rideIndex)
        , _type(static_cast<uint8_t>(type))
        , _value(value)
        , _colour(colour)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_rideIndex) << DS_TAG(_type) << DS_TAG(_value) << DS_TAG(_colour);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_type > sizeof(SetVehicleTypeErrorTitle))
        {
            log_warning("Invalid type. type = %d", _type);
        }
        auto errTitle = SetVehicleTypeErrorTitle[_type];

        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid game command, ride_id = %u", uint32_t(_rideIndex));
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, errTitle);
        }

        if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::BROKEN, errTitle, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING);
        }

        if (ride->status != RIDE_STATUS_CLOSED && ride->status != RIDE_STATUS_SIMULATING)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::NOT_CLOSED, errTitle, STR_MUST_BE_CLOSED_FIRST);
        }

        switch (static_cast<RideSetVehicleType>(_type))
        {
            case RideSetVehicleType::NumTrains:
            case RideSetVehicleType::NumCarsPerTrain:
                break;
            case RideSetVehicleType::RideEntry:
            {
                if (!ride_is_vehicle_type_valid(ride))
                {
                    log_error("Invalid vehicle type. type = %d", _value);
                    return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, errTitle);
                }
                auto rideEntry = get_ride_entry(_value);
                if (rideEntry == nullptr)
                {
                    log_warning("Invalid ride entry, ride->subtype = %d", ride->subtype);
                    return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, errTitle);
                }

                // Validate preset
                vehicle_colour_preset_list* presetList = rideEntry->vehicle_preset_list;
                if (_colour >= presetList->count && _colour != 255 && _colour != 0)
                {
                    log_error("Unknown vehicle colour preset. colour = %d", _colour);
                    return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, errTitle);
                }
                break;
            }

            default:
                log_error("Unknown vehicle command. type = %d", _type);
                return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, errTitle);
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto errTitle = SetVehicleTypeErrorTitle[_type];
        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid game command, ride_id = %u", uint32_t(_rideIndex));
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, errTitle);
        }

        switch (static_cast<RideSetVehicleType>(_type))
        {
            case RideSetVehicleType::NumTrains:
                ride_clear_for_construction(ride);
                ride_remove_peeps(ride);
                ride->vehicle_change_timeout = 100;

                ride->proposed_num_vehicles = _value;
                break;
            case RideSetVehicleType::NumCarsPerTrain:
            {
                ride_clear_for_construction(ride);
                ride_remove_peeps(ride);
                ride->vehicle_change_timeout = 100;

                invalidate_test_results(ride);
                auto rideEntry = get_ride_entry(ride->subtype);
                if (rideEntry == nullptr)
                {
                    log_warning("Invalid ride entry, ride->subtype = %d", ride->subtype);
                    return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, errTitle);
                }
                auto clampValue = _value;
                if (!gCheatsDisableTrainLengthLimit)
                {
                    clampValue = std::clamp(clampValue, rideEntry->min_cars_in_train, rideEntry->max_cars_in_train);
                }
                ride->proposed_num_cars_per_train = clampValue;
                break;
            }
            case RideSetVehicleType::RideEntry:
            {
                ride_clear_for_construction(ride);
                ride_remove_peeps(ride);
                ride->vehicle_change_timeout = 100;

                invalidate_test_results(ride);
                ride->subtype = _value;
                auto rideEntry = get_ride_entry(ride->subtype);
                if (rideEntry == nullptr)
                {
                    log_warning("Invalid ride entry, ride->subtype = %d", ride->subtype);
                    return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, errTitle);
                }

                ride_set_vehicle_colours_to_random_preset(ride, _colour);
                if (!gCheatsDisableTrainLengthLimit)
                {
                    ride->proposed_num_cars_per_train = std::clamp(
                        ride->proposed_num_cars_per_train, rideEntry->min_cars_in_train, rideEntry->max_cars_in_train);
                }
                break;
            }

            default:
                log_error("Unknown vehicle command. type = %d", _type);
                return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, errTitle);
        }

        ride->num_circuits = 1;
        ride->UpdateMaxVehicles();

        auto res = std::make_unique<GameActionResult>();
        if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
        {
            res->Position.x = ride->overall_view.x * 32 + 16;
            res->Position.y = ride->overall_view.y * 32 + 16;
            res->Position.z = tile_element_height(res->Position);
        }

        auto intent = Intent(INTENT_ACTION_RIDE_PAINT_RESET_VEHICLE);
        intent.putExtra(INTENT_EXTRA_RIDE_ID, _rideIndex);
        context_broadcast_intent(&intent);

        gfx_invalidate_screen();
        return res;
    }

private:
    bool ride_is_vehicle_type_valid(Ride * ride) const
    {
        bool selectionShouldBeExpanded;
        int32_t rideTypeIterator, rideTypeIteratorMax;

        if (gCheatsShowVehiclesFromOtherTrackTypes
            && !(
                ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) || ride->type == RIDE_TYPE_MAZE
                || ride->type == RIDE_TYPE_MINI_GOLF))
        {
            selectionShouldBeExpanded = true;
            rideTypeIterator = 0;
            rideTypeIteratorMax = RIDE_TYPE_COUNT - 1;
        }
        else
        {
            selectionShouldBeExpanded = false;
            rideTypeIterator = ride->type;
            rideTypeIteratorMax = ride->type;
        }

        for (; rideTypeIterator <= rideTypeIteratorMax; rideTypeIterator++)
        {
            if (selectionShouldBeExpanded)
            {
                if (ride_type_has_flag(rideTypeIterator, RIDE_TYPE_FLAG_FLAT_RIDE))
                    continue;
                if (rideTypeIterator == RIDE_TYPE_MAZE || rideTypeIterator == RIDE_TYPE_MINI_GOLF)
                    continue;
            }

            uint8_t* rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(rideTypeIterator);
            for (uint8_t* currentRideEntryIndex = rideEntryIndexPtr; *currentRideEntryIndex != RIDE_ENTRY_INDEX_NULL;
                 currentRideEntryIndex++)
            {
                uint8_t rideEntryIndex = *currentRideEntryIndex;
                if (rideEntryIndex == _value)
                {
                    if (!ride_entry_is_invented(rideEntryIndex) && !gCheatsIgnoreResearchStatus)
                    {
                        return false;
                    }

                    return true;
                }
            }
        }

        return false;
    }
};
