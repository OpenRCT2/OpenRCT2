/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Vehicle.h"
#include "../VehicleSubpositionData.h"

#include <cstddef>

using enum VehiclePitch;
using enum VehicleRoll;
namespace OpenRCT2::Subposition
{

#define CREATE_VEHICLE_INFO(VAR, ...)                                                                                          \
    static constexpr VehicleInfo VAR##_data[] = __VA_ARGS__;                                                                   \
    static constexpr VehicleInfoList VAR = { static_cast<uint16_t>(std::size(VAR##_data)), VAR##_data };

#define MINI_GOLF_STATE(STATE) { kLocationNull, EnumValue(MiniGolfState::STATE), 0, 0, flat, unbanked }
#define MINI_GOLF_ANIMATION(VALUE)                                                                                             \
    { kLocationNull, EnumValue(MiniGolfState::Unk4), EnumValue(MiniGolfAnimation::VALUE), 0, flat, unbanked }

    CREATE_VEHICLE_INFO(
        TrackVehicleInfo_000000,
        {
            { 0, 0, 0, 0, flat, unbanked },
        })
} // namespace OpenRCT2::Subposition
