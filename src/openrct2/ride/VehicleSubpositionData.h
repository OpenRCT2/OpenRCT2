/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "../world/Location.hpp"
#include "ted/TrackElemType.h"

#include <cstddef>
#include <cstdint>

constexpr const size_t VehicleTrackSubpositionSizeDefault = EnumValue(OpenRCT2::TrackElemType::count)
    * kNumOrthogonalDirections;

struct VehicleInfo;

enum class VehicleTrackSubposition : uint8_t
{
    standard,
    // Going out means "moving away from the start". Viewed from Station 1, this is the left hand side of the track.
    chairliftGoingOut,
    chairliftGoingBack,
    // End and start bullwheel as viewed from Station 1.
    chairliftEndBullwheel,
    chairliftStartBullwheel,
    goKartsLeftLane,
    goKartsRightLane,
    goKartsMovingToRightLane,
    goKartsMovingToLeftLane,
    miniGolfStart9 = 9,
    miniGolfPathA9 = 9,
    miniGolfBallPathA10,
    miniGolfPathB11,
    miniGolfBallPathB12,
    miniGolfPathC13,
    miniGolfBallPathC14,
    reverserRCFrontBogie,
    reverserRCRearBogie,

    count,
};

struct VehicleInfoList
{
    uint16_t size;
    const VehicleInfo* info;
};

extern const VehicleInfoList* const* const gTrackVehicleInfo[EnumValue(VehicleTrackSubposition::count)];
