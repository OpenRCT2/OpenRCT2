/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetSettingAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ui/WindowManager.h"

using namespace OpenRCT2;

RideSetSettingAction::RideSetSettingAction(RideId rideIndex, RideSetSetting setting, uint8_t value)
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

GameActions::Result RideSetSettingAction::Query() const
{
    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %u.", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_RIDE_NOT_FOUND);
    }

    switch (_setting)
    {
        case RideSetSetting::Mode:
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_CANT_CHANGE_OPERATING_MODE, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING);
            }

            if (ride->status != RideStatus::Closed && ride->status != RideStatus::Simulating)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_CANT_CHANGE_OPERATING_MODE, STR_MUST_BE_CLOSED_FIRST);
            }

            if (!RideIsModeValid(*ride) && !GetGameState().Cheats.showAllOperatingModes)
            {
                LOG_ERROR("Invalid ride mode: %u", _value);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_VALUE_OUT_OF_RANGE);
            }
            break;
        case RideSetSetting::Departure:
            break;
        case RideSetSetting::MinWaitingTime:
            if (_value > 250)
            {
                LOG_ERROR("Invalid minimum waiting time: %u", _value);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_VALUE_OUT_OF_RANGE);
            }
            break;
        case RideSetSetting::MaxWaitingTime:
            if (_value > 250)
            {
                LOG_ERROR("Invalid maximum waiting time: %u", _value);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_VALUE_OUT_OF_RANGE);
            }
            break;
        case RideSetSetting::Operation:
            if (!RideIsValidOperationOption(*ride))
            {
                LOG_ERROR("Invalid operation option value: %u", _value);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, GetOperationErrorMessage(*ride));
            }
            break;
        case RideSetSetting::InspectionInterval:
            if (_value > RIDE_INSPECTION_NEVER)
            {
                LOG_ERROR("Invalid inspection interval: %u", _value);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_VALUE_OUT_OF_RANGE);
            }
            break;
        case RideSetSetting::Music:
            break;
        case RideSetSetting::MusicType:
        {
            auto& objManager = GetContext()->GetObjectManager();
            auto musicObj = objManager.GetLoadedObject(ObjectType::Music, _value);
            if (musicObj == nullptr)
            {
                LOG_ERROR("Invalid music style: %u", _value);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_VALUE_OUT_OF_RANGE);
            }
            break;
        }
        case RideSetSetting::LiftHillSpeed:
            if (!RideIsValidLiftHillSpeed(*ride))
            {
                LOG_ERROR("Invalid lift hill speed: %u", _value);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_VALUE_OUT_OF_RANGE);
            }
            break;
        case RideSetSetting::NumCircuits:
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT && _value > 1)
            {
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE,
                    STR_MULTICIRCUIT_NOT_POSSIBLE_WITH_CABLE_LIFT_HILL);
            }

            if (!RideIsValidNumCircuits())
            {
                LOG_ERROR("Invalid number of circuits: %u", _value);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_VALUE_OUT_OF_RANGE);
            }
            break;
        case RideSetSetting::RideType:
            if (!GetGameState().Cheats.allowArbitraryRideTypeChanges)
            {
                LOG_ERROR("Arbitrary ride type changes not allowed.");
                return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_CHANGE_OPERATING_MODE, kStringIdNone);
            }
            break;
        default:
            LOG_ERROR("Invalid ride setting %u", static_cast<uint8_t>(_setting));
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    return GameActions::Result();
}

GameActions::Result RideSetSettingAction::Execute() const
{
    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_CHANGE_OPERATING_MODE, STR_ERR_RIDE_NOT_FOUND);
    }

    switch (_setting)
    {
        case RideSetSetting::Mode:
            InvalidateTestResults(*ride);
            RideClearForConstruction(*ride);
            ride->RemovePeeps();

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
            InvalidateTestResults(*ride);
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
                ride->music_tune_id = kTuneIDNull;
            }
            break;
        case RideSetSetting::LiftHillSpeed:
            if (_value != ride->lift_hill_speed)
            {
                ride->lift_hill_speed = _value;
                InvalidateTestResults(*ride);
            }
            break;
        case RideSetSetting::NumCircuits:
            if (_value != ride->num_circuits)
            {
                ride->num_circuits = _value;
                InvalidateTestResults(*ride);
            }

            break;
        case RideSetSetting::RideType:
            ride->type = _value;
            ride->UpdateRideTypeForAllPieces();
            GfxInvalidateScreen();
            break;
    }

    auto res = GameActions::Result();
    if (!ride->overall_view.IsNull())
    {
        auto location = ride->overall_view.ToTileCentre();
        res.Position = { location, TileElementHeight(location) };
    }
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::Ride, _rideIndex.ToUnderlying());
    return res;
}

bool RideSetSettingAction::RideIsModeValid(const Ride& ride) const
{
    return ride.GetRideTypeDescriptor().RideModes & (1uLL << _value);
}

bool RideSetSettingAction::RideIsValidLiftHillSpeed(const Ride& ride) const
{
    auto& gameState = GetGameState();
    int32_t minSpeed = gameState.Cheats.unlockOperatingLimits ? 0 : ride.GetRideTypeDescriptor().LiftData.minimum_speed;
    int32_t maxSpeed = gameState.Cheats.unlockOperatingLimits ? 255 : ride.GetRideTypeDescriptor().LiftData.maximum_speed;
    return _value >= minSpeed && _value <= maxSpeed;
}

bool RideSetSettingAction::RideIsValidNumCircuits() const
{
    int32_t minNumCircuits = 1;
    int32_t maxNumCircuits = GetGameState().Cheats.unlockOperatingLimits ? 255 : Limits::kMaxCircuitsPerRide;
    return _value >= minNumCircuits && _value <= maxNumCircuits;
}

bool RideSetSettingAction::RideIsValidOperationOption(const Ride& ride) const
{
    const auto& operatingSettings = ride.GetRideTypeDescriptor().OperatingSettings;
    uint8_t minValue = operatingSettings.MinValue;
    uint8_t maxValue = operatingSettings.MaxValue;
    if (GetGameState().Cheats.unlockOperatingLimits)
    {
        minValue = 0;
        maxValue = 255;
    }

    return _value >= minValue && _value <= maxValue;
}

StringId RideSetSettingAction::GetOperationErrorMessage(const Ride& ride) const
{
    switch (ride.mode)
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
            if (ride.GetRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
            {
                return STR_CANT_CHANGE_THIS;
            }
            return STR_CANT_CHANGE_LAUNCH_SPEED;
    }
}
