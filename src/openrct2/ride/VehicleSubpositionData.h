/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "Track.h"

#include <cstdint>

constexpr const size_t VehicleTrackSubpositionSizeDefault = EnumValue(OpenRCT2::TrackElemType::Count)
    * kNumOrthogonalDirections;

struct VehicleInfo;

enum class VehicleTrackSubposition : uint8_t
{
    Default,
    // Going out means "moving away from the start". Viewed from Station 1, this is the left hand side of the track.
    ChairliftGoingOut,
    ChairliftGoingBack,
    // End and start bullwheel as viewed from Station 1.
    ChairliftEndBullwheel,
    ChairliftStartBullwheel,
    GoKartsLeftLane,
    GoKartsRightLane,
    GoKartsMovingToRightLane,
    GoKartsMovingToLeftLane,
    MiniGolfStart9 = 9,
    MiniGolfPathA9 = 9,
    MiniGolfBallPathA10,
    MiniGolfPathB11,
    MiniGolfBallPathB12,
    MiniGolfPathC13,
    MiniGolfBallPathC14,
    ReverserRCFrontBogie,
    ReverserRCRearBogie,

    Count,
};

struct VehicleInfoList
{
    uint16_t size;
    const VehicleInfo* info;
};

extern const VehicleInfoList* const* const gTrackVehicleInfo[EnumValue(VehicleTrackSubposition::Count)];
