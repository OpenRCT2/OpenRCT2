/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetSettingAction.h"

#include "../Context.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"

RideSetSettingAction::RideSetSettingAction(ride_id_t rideIndex, RideSetSetting setting, uint8_t value)
    : _rideIndex(rideIndex)
    , _setting(setting)
    , _value(value)
{
}

void RideSetSettingAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("ride", _rideIndex);
    visitor.Visit("setting", _setting);
    visitor.Visit("value", _value);
}

uint16_t RideSetSettingAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void RideSetSettingAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_rideIndex) << DS_TAG(_setting) << DS_TAG(_value);
}

GameActions::Result::Ptr RideSetSettingAction::Query() const
{
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid ride: #%d.", static_cast<int32_t>(_rideIndex));
        return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE);
    }

    switch (_setting)
    {
        case RideSetSetting::Mode:
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
            {
                return MakeResult(
                    GameActions::Status::Disallowed, STR_CANT_CHANGE_OPERATING_MODE, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING);
            }

            if (ride->status != RIDE_STATUS_CLOSED && ride->status != RIDE_STATUS_SIMULATING)
            {
                return MakeResult(GameActions::Status::Disallowed, STR_CANT_CHANGE_OPERATING_MODE, STR_MUST_BE_CLOSED_FIRST);
            }

            if (!ride_is_mode_valid(ride) && !gCheatsShowAllOperatingModes)
            {
                log_warning("Invalid ride mode: %u", _value);
                return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE);
            }
            break;
        case RideSetSetting::Departure:
            break;
        case RideSetSetting::MinWaitingTime:
            if (_value > 250)
            {
                log_warning("Invalid minimum waiting time: %u", _value);
                return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE);
            }
            break;
        case RideSetSetting::MaxWaitingTime:
            if (_value > 250)
            {
                log_warning("Invalid maximum waiting time: %u", _value);
                return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE);
            }
            break;
        case RideSetSetting::Operation:
            if (!ride_is_valid_operation_option(ride))
            {
                log_warning("Invalid operation option value: %u", _value);
                return MakeResult(GameActions::Status::InvalidParameters, GetOperationErrorMessage(ride));
            }
            break;
        case RideSetSetting::InspectionInterval:
            if (_value > RIDE_INSPECTION_NEVER)
            {
                log_warning("Invalid inspection interval: %u", _value);
                return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE);
            }
            break;
        case RideSetSetting::Music:
            break;
        case RideSetSetting::MusicType:
        {
            auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
            auto musicObj = objManager.GetLoadedObject(ObjectType::Music, _value);
            if (musicObj == nullptr)
            {
                log_warning("Invalid music style: %u", _value);
                return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE);
            }
            break;
        }
        case RideSetSetting::LiftHillSpeed:
            if (!ride_is_valid_lift_hill_speed(ride))
            {
                log_warning("Invalid lift hill speed: %u", _value);
                return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE);
            }
            break;
        case RideSetSetting::NumCircuits:
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT && _value > 1)
            {
                return MakeResult(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE,
                    STR_MULTICIRCUIT_NOT_POSSIBLE_WITH_CABLE_LIFT_HILL);
            }

            if (!ride_is_valid_num_circuits())
            {
                log_warning("Invalid number of circuits: %u", _value);
                return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE);
            }
            break;
        case RideSetSetting::RideType:
            if (!gCheatsAllowArbitraryRideTypeChanges)
            {
                log_warning("Arbitrary ride type changes not allowed.");
                return MakeResult(GameActions::Status::Disallowed, STR_CANT_CHANGE_OPERATING_MODE);
            }
            break;
        default:
            log_warning("Invalid RideSetSetting: %u", static_cast<uint8_t>(_setting));
            return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE);
    }

    return std::make_unique<GameActions::Result>();
}

GameActions::Result::Ptr RideSetSettingAction::Execute() const
{
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid ride: #%d.", static_cast<int32_t>(_rideIndex));
        return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE);
    }

    switch (_setting)
    {
        case RideSetSetting::Mode:
            invalidate_test_results(ride);
            ride_clear_for_construction(ride);
            ride_remove_peeps(ride);

            ride->mode = static_cast<RideMode>(_value);
            ride->UpdateMaxVehicles();
            ride->UpdateNumberOfCircuits();
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
            gfx_invalidate_screen();
            break;
    }

    auto res = std::make_unique<GameActions::Result>();
    if (!ride->overall_view.isNull())
    {
        auto location = ride->overall_view.ToTileCentre();
        res->Position = { location, tile_element_height(location) };
    }
    window_invalidate_by_number(WC_RIDE, _rideIndex);
    return res;
}

bool RideSetSettingAction::ride_is_mode_valid(Ride* ride) const
{
    return ride->GetRideTypeDescriptor().RideModes & (1ULL << _value);
}

bool RideSetSettingAction::ride_is_valid_lift_hill_speed(Ride* ride) const
{
    int32_t minSpeed = gCheatsFastLiftHill ? 0 : ride->GetRideTypeDescriptor().LiftData.minimum_speed;
    int32_t maxSpeed = gCheatsFastLiftHill ? 255 : ride->GetRideTypeDescriptor().LiftData.maximum_speed;
    return _value >= minSpeed && _value <= maxSpeed;
}

bool RideSetSettingAction::ride_is_valid_num_circuits() const
{
    int32_t minNumCircuits = 1;
    int32_t maxNumCircuits = gCheatsFastLiftHill ? 255 : 20;
    return _value >= minNumCircuits && _value <= maxNumCircuits;
}

bool RideSetSettingAction::ride_is_valid_operation_option(Ride* ride) const
{
    const auto& operatingSettings = ride->GetRideTypeDescriptor().OperatingSettings;
    uint8_t minValue = operatingSettings.MinValue;
    uint8_t maxValue = operatingSettings.MaxValue;
    if (gCheatsFastLiftHill)
    {
        minValue = 0;
        maxValue = 255;
    }

    return _value >= minValue && _value <= maxValue;
}

rct_string_id RideSetSettingAction::GetOperationErrorMessage(Ride* ride) const
{
    switch (ride->mode)
    {
        case RideMode::StationToStation:
            return STR_CANT_CHANGE_SPEED;
        case RideMode::Race:
            return STR_CANT_CHANGE_NUMBER_OF_LAPS;
        case RideMode::Dodgems:
            return STR_CANT_CHANGE_TIME_LIMIT;
        case RideMode::Swing:
            return STR_CANT_CHANGE_NUMBER_OF_SWINGS;
        case RideMode::Rotation:
        case RideMode::ForwardRotation:
        case RideMode::BackwardRotation:
            return STR_CANT_CHANGE_NUMBER_OF_ROTATIONS;
        default:
            if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
            {
                return STR_CANT_CHANGE_THIS;
            }
            else
            {
                return STR_CANT_CHANGE_LAUNCH_SPEED;
            }
    }
}
