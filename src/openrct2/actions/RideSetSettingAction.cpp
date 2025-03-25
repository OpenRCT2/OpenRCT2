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
            if (ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_CANT_CHANGE_OPERATING_MODE, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING);
            }

            if (ride->status != RideStatus::closed && ride->status != RideStatus::simulating)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_CANT_CHANGE_OPERATING_MODE, STR_MUST_BE_CLOSED_FIRST);
            }

            if (!RideIsModeValid(*ride) && !getGameState().cheats.showAllOperatingModes)
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
            auto musicObj = objManager.GetLoadedObject<MusicObject>(_value);
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
            if (ride->lifecycleFlags & RIDE_LIFECYCLE_CABLE_LIFT && _value > 1)
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
            if (!getGameState().cheats.allowArbitraryRideTypeChanges)
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
            ride->removePeeps();

            ride->mode = static_cast<RideMode>(_value);
            ride->updateMaxVehicles();
            ride->updateNumberOfCircuits();
            break;
        case RideSetSetting::Departure:
            ride->departFlags = _value;
            break;
        case RideSetSetting::MinWaitingTime:
            ride->minWaitingTime = _value;
            ride->maxWaitingTime = std::max(_value, ride->maxWaitingTime);
            break;
        case RideSetSetting::MaxWaitingTime:
            ride->maxWaitingTime = _value;
            ride->minWaitingTime = std::min(_value, ride->minWaitingTime);
            break;
        case RideSetSetting::Operation:
            InvalidateTestResults(*ride);
            ride->operationOption = _value;
            break;
        case RideSetSetting::InspectionInterval:

            if (_value == RIDE_INSPECTION_NEVER)
            {
                ride->lifecycleFlags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
            }

            ride->inspectionInterval = _value;
            break;
        case RideSetSetting::Music:
            ride->lifecycleFlags &= ~RIDE_LIFECYCLE_MUSIC;
            if (_value)
            {
                ride->lifecycleFlags |= RIDE_LIFECYCLE_MUSIC;
            }
            break;
        case RideSetSetting::MusicType:
            if (_value != ride->music)
            {
                ride->music = _value;
                ride->musicTuneId = kTuneIDNull;
            }
            break;
        case RideSetSetting::LiftHillSpeed:
            if (_value != ride->liftHillSpeed)
            {
                ride->liftHillSpeed = _value;
                InvalidateTestResults(*ride);
            }
            break;
        case RideSetSetting::NumCircuits:
            if (_value != ride->numCircuits)
            {
                ride->numCircuits = _value;
                InvalidateTestResults(*ride);
            }

            break;
        case RideSetSetting::RideType:
            ride->type = _value;
            ride->updateRideTypeForAllPieces();
            GfxInvalidateScreen();
            break;
    }

    auto res = GameActions::Result();
    if (!ride->overallView.IsNull())
    {
        auto location = ride->overallView.ToTileCentre();
        res.Position = { location, TileElementHeight(location) };
    }
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::Ride, _rideIndex.ToUnderlying());
    return res;
}

bool RideSetSettingAction::RideIsModeValid(const Ride& ride) const
{
    return ride.getRideTypeDescriptor().RideModes & (1uLL << _value);
}

bool RideSetSettingAction::RideIsValidLiftHillSpeed(const Ride& ride) const
{
    auto& gameState = getGameState();
    int32_t minSpeed = gameState.cheats.unlockOperatingLimits ? 0 : ride.getRideTypeDescriptor().LiftData.minimum_speed;
    int32_t maxSpeed = gameState.cheats.unlockOperatingLimits ? 255 : ride.getRideTypeDescriptor().LiftData.maximum_speed;
    return _value >= minSpeed && _value <= maxSpeed;
}

bool RideSetSettingAction::RideIsValidNumCircuits() const
{
    int32_t minNumCircuits = 1;
    int32_t maxNumCircuits = getGameState().cheats.unlockOperatingLimits ? 255 : Limits::kMaxCircuitsPerRide;
    return _value >= minNumCircuits && _value <= maxNumCircuits;
}

bool RideSetSettingAction::RideIsValidOperationOption(const Ride& ride) const
{
    const auto& operatingSettings = ride.getRideTypeDescriptor().OperatingSettings;
    uint8_t minValue = operatingSettings.MinValue;
    uint8_t maxValue = operatingSettings.MaxValue;
    if (getGameState().cheats.unlockOperatingLimits)
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
        case RideMode::stationToStation:
            return STR_CANT_CHANGE_SPEED;
        case RideMode::race:
            return STR_CANT_CHANGE_NUMBER_OF_LAPS;
        case RideMode::dodgems:
            return STR_CANT_CHANGE_TIME_LIMIT;
        case RideMode::swing:
            return STR_CANT_CHANGE_NUMBER_OF_SWINGS;
        case RideMode::rotation:
        case RideMode::forwardRotation:
        case RideMode::backwardRotation:
            return STR_CANT_CHANGE_NUMBER_OF_ROTATIONS;
        default:
            if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
            {
                return STR_CANT_CHANGE_THIS;
            }
            return STR_CANT_CHANGE_LAUNCH_SPEED;
    }
}
