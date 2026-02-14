/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BrakeBoosterMode.h"

#include "../world/tile_element/TrackElement.h"
#include "RideData.h"

using namespace OpenRCT2;

constexpr uint16_t kUncappedSpeed = std::numeric_limits<uint16_t>::max();
constexpr uint16_t kLSMLaunchBrakeSpeed = 8;
constexpr uint16_t kLSMLaunchBrakePower = 2;
constexpr uint16_t kStandardBrakePower = 16;

BrakeBoosterMode BrakeBoosterModeFromTrackType(const TrackElemType trackType, const RideTypeDescriptor& rtd)
{
    if (TrackTypeIsBooster(trackType))
    {
        return BrakeBoosterMode::forwardsBoosterBackwardsNone;
    }
    else if (TrackTypeIsBrakes(trackType))
    {
        return BrakeBoosterMode::forwardsBrakeBackwardsBrake;
    }
    else if (trackType == TrackElemType::poweredLift)
    {
        return BrakeBoosterMode::forwardsLSMBackwardsNone;
    }
    else if (trackType == TrackElemType::flat && rtd.flags.has(RtdFlag::hasLsmBehaviourOnFlat))
    {
        return BrakeBoosterMode::forwardsLSMBackwardsBrake;
    }
    return BrakeBoosterMode::none;
}

TrackPieceBehaviourPair SpeedPowerModeFromBoosterMode(uint8_t trackSpeed, BrakeBoosterMode mode, const RideTypeDescriptor& rtd)
{
    SpeedPowerMode forwards{};
    SpeedPowerMode backwards{};
    switch (mode)
    {
        case BrakeBoosterMode::none:
            return {};
        case BrakeBoosterMode::forwardsBrakeBackwardsBrake:
            forwards = { trackSpeed, kStandardBrakePower, DirectionalBoosterMode::brake };
            backwards = { trackSpeed, kStandardBrakePower, DirectionalBoosterMode::brake };
            break;
        case BrakeBoosterMode::forwardsBoosterBackwardsNone:
            forwards = { static_cast<uint16_t>(rtd.GetUnifiedBoosterSpeed(trackSpeed)), rtd.BoosterSettings.boosterPower,
                         DirectionalBoosterMode::booster };
            break;
        case BrakeBoosterMode::forwardsLSMBackwardsNone:
            forwards = { kUncappedSpeed, rtd.BoosterSettings.lsmLaunchPower, DirectionalBoosterMode::booster };
            break;
        case BrakeBoosterMode::forwardsLSMBackwardsBrake:
            forwards = { kUncappedSpeed, rtd.BoosterSettings.lsmLaunchPower, DirectionalBoosterMode::booster };
            backwards = { kLSMLaunchBrakeSpeed, kLSMLaunchBrakePower, DirectionalBoosterMode::brake };
            break;
        default:
            break;
    }
    return { forwards, backwards };
}