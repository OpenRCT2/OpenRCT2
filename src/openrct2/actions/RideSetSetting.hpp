/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "GameAction.h"

enum class RideSetSetting : uint8_t
{
    Mode,
    Departure,
    MinWaitingTime,
    MaxWaitingTime,
    Operation,
    InspectionInterval,
    Music,
    MusicType,
    LiftHillSpeed,
    NumCircuits,
    RideType,
};

DEFINE_GAME_ACTION(RideSetSettingAction, GAME_COMMAND_SET_RIDE_SETTING, GameActionResult)
{
private:
    NetworkRideId_t _rideIndex{ -1 };
    uint8_t _setting{ 0 };
    uint8_t _value{ 0 };

public:
    RideSetSettingAction()
    {
    }
    RideSetSettingAction(ride_id_t rideIndex, RideSetSetting setting, uint8_t value)
        : _rideIndex(rideIndex)
        , _setting(static_cast<uint8_t>(setting))
        , _value(value)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideIndex) << DS_TAG(_setting) << DS_TAG(_value);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_rideIndex >= MAX_RIDES || _rideIndex < 0)
        {
            log_warning("Invalid game command for ride %u", uint32_t(_rideIndex));
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
        }

        Ride* ride = get_ride(_rideIndex);
        if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid ride: #%u.", _rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
        }

        switch (static_cast<RideSetSetting>(_setting))
        {
            case RideSetSetting::Mode:
                if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                {
                    return MakeResult(
                        GA_ERROR::DISALLOWED, STR_CANT_CHANGE_OPERATING_MODE, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING);
                }

                if (ride->status != RIDE_STATUS_CLOSED)
                {
                    return MakeResult(GA_ERROR::DISALLOWED, STR_CANT_CHANGE_OPERATING_MODE, STR_MUST_BE_CLOSED_FIRST);
                }

                if (!ride_is_mode_valid(ride))
                {
                    log_warning("Invalid ride mode: %u", _value);
                    return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
                }
                break;
            case RideSetSetting::Departure:
                break;
            case RideSetSetting::MinWaitingTime:
                if (_value > 250)
                {
                    log_warning("Invalid minimum waiting time: %u", _value);
                    return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
                }
                break;
            case RideSetSetting::MaxWaitingTime:
                if (_value > 250)
                {
                    log_warning("Invalid maximum waiting time: %u", _value);
                    return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
                }
                break;
            case RideSetSetting::Operation:
                if (!ride_is_valid_operation_option(ride))
                {
                    log_warning("Invalid operation option value: %u", _value);
                    return MakeResult(GA_ERROR::INVALID_PARAMETERS, GetOperationErrorMessage(ride));
                }
                break;
            case RideSetSetting::InspectionInterval:
                if (_value > RIDE_INSPECTION_NEVER)
                {
                    log_warning("Invalid inspection interval: %u", _value);
                    return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
                }
                break;
            case RideSetSetting::Music:
                break;
            case RideSetSetting::MusicType:
                if (_value >= MUSIC_STYLE_COUNT)
                {
                    log_warning("Invalid music style: %u", _value);
                    return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
                }
                break;
            case RideSetSetting::LiftHillSpeed:
                if (!ride_is_valid_lift_hill_speed(ride))
                {
                    log_warning("Invalid lift hill speed: %u", _value);
                    return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
                }
                break;
            case RideSetSetting::NumCircuits:
                if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT && _value > 1)
                {
                    return MakeResult(
                        GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE,
                        STR_MULTICIRCUIT_NOT_POSSIBLE_WITH_CABLE_LIFT_HILL);
                }

                if (!ride_is_valid_num_circuits())
                {
                    log_warning("Invalid number of circuits: %u", _value);
                    return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
                }
                break;
            case RideSetSetting::RideType:
                if (!gCheatsAllowArbitraryRideTypeChanges)
                {
                    log_warning("Arbitary ride type changes not allowed.");
                    return MakeResult(GA_ERROR::DISALLOWED, STR_CANT_CHANGE_OPERATING_MODE);
                }
                break;
            default:
                log_warning("Invalid RideSetSetting: %u", _setting);
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
                break;
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        Ride* ride = get_ride(_rideIndex);
        if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid ride: #%u.", _rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_OPERATING_MODE);
        }

        switch (static_cast<RideSetSetting>(_setting))
        {
            case RideSetSetting::Mode:
                invalidate_test_results(ride);
                ride_clear_for_construction(ride);
                ride_remove_peeps(ride);

                ride->mode = _value;
                ride_update_max_vehicles(ride);
                break;
            case RideSetSetting::Departure:
                ride->depart_flags = _value;
                break;
            case RideSetSetting::MinWaitingTime:
                ride->min_waiting_time = _value;
                ride->max_waiting_time = std::max(_value, ride->max_waiting_time);
                break;
            case RideSetSetting::MaxWaitingTime:
                ride->max_waiting_time = _value;
                ride->min_waiting_time = std::min(_value, ride->min_waiting_time);
                break;
            case RideSetSetting::Operation:
                invalidate_test_results(ride);
                ride->operation_option = _value;
                break;
            case RideSetSetting::InspectionInterval:

                if (_value == RIDE_INSPECTION_NEVER)
                {
                    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
                }

                ride->inspection_interval = _value;
                break;
            case RideSetSetting::Music:
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_MUSIC;
                if (_value)
                {
                    ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
                }
                break;
            case RideSetSetting::MusicType:
                if (_value != ride->music)
                {
                    ride->music = _value;
                    ride->music_tune_id = 0xFF;
                }
                break;
            case RideSetSetting::LiftHillSpeed:
                if (_value != ride->lift_hill_speed)
                {
                    ride->lift_hill_speed = _value;
                    invalidate_test_results(ride);
                }
                break;
            case RideSetSetting::NumCircuits:
                if (_value != ride->num_circuits)
                {
                    ride->num_circuits = _value;
                    invalidate_test_results(ride);
                }

                break;
            case RideSetSetting::RideType:
                ride->type = _value;
                break;
        }

        auto res = std::make_unique<GameActionResult>();
        if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
        {
            res->Position.x = ride->overall_view.x * 32 + 16;
            res->Position.y = ride->overall_view.y * 32 + 16;
            res->Position.z = tile_element_height(res->Position.x, res->Position.y);
        }
        window_invalidate_by_number(WC_RIDE, _rideIndex);
        return res;
    }

private:
    bool ride_is_mode_valid(Ride * ride) const
    {
        const uint8_t* availableModes = ride_seek_available_modes(ride);

        for (; *availableModes != 0xFF; availableModes++)
        {
            if (*availableModes == _value)
            {
                return true;
            }
        }

        return false;
    }

    bool ride_is_valid_lift_hill_speed(Ride * ride) const
    {
        int32_t minSpeed = gCheatsFastLiftHill ? 0 : RideLiftData[ride->type].minimum_speed;
        int32_t maxSpeed = gCheatsFastLiftHill ? 255 : RideLiftData[ride->type].maximum_speed;
        return _value >= minSpeed && _value <= maxSpeed;
    }

    bool ride_is_valid_num_circuits() const
    {
        int32_t minNumCircuits = 1;
        int32_t maxNumCircuits = gCheatsFastLiftHill ? 255 : 20;
        return _value >= minNumCircuits && _value <= maxNumCircuits;
    }

    bool ride_is_valid_operation_option(Ride * ride) const
    {
        uint8_t minValue = RideProperties[ride->type].min_value;
        uint8_t maxValue = RideProperties[ride->type].max_value;
        if (gCheatsFastLiftHill)
        {
            minValue = 0;
            maxValue = 255;
        }

        return _value >= minValue && _value <= maxValue;
    }

    rct_string_id GetOperationErrorMessage(Ride * ride) const
    {
        switch (ride->mode)
        {
            case RIDE_MODE_STATION_TO_STATION:
                return STR_CANT_CHANGE_SPEED;
            case RIDE_MODE_RACE:
                return STR_CANT_CHANGE_NUMBER_OF_LAPS;
            case RIDE_MODE_BUMPERCAR:
                return STR_CANT_CHANGE_TIME_LIMIT;
            case RIDE_MODE_SWING:
                return STR_CANT_CHANGE_NUMBER_OF_SWINGS;
            case RIDE_MODE_ROTATION:
            case RIDE_MODE_FORWARD_ROTATION:
            case RIDE_MODE_BACKWARD_ROTATION:
                return STR_CANT_CHANGE_NUMBER_OF_ROTATIONS;
            default:
                if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
                {
                    return STR_CANT_CHANGE_THIS;
                }
                else
                {
                    return STR_CANT_CHANGE_LAUNCH_SPEED;
                }
                break;
        }
    }
};
