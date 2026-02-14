/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Track.h"
#include "Vehicle.h"

#include <tuple>

enum class BrakeBoosterMode : uint8_t
{
    none,
    forwardsBrakeBackwardsBrake,
    forwardsBoosterBackwardsNone,
    forwardsLSMBackwardsNone,
    forwardsLSMBackwardsBrake,
};

using TrackPieceBehaviourPair = std::tuple<SpeedPowerMode, SpeedPowerMode>;

BrakeBoosterMode BrakeBoosterModeFromTrackType(OpenRCT2::TrackElemType trackType, const RideTypeDescriptor& rtd);

TrackPieceBehaviourPair SpeedPowerModeFromBoosterMode(uint8_t trackSpeed, BrakeBoosterMode mode, const RideTypeDescriptor& rtd);