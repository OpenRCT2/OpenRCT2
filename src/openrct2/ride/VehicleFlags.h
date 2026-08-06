/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"

#include <cstdint>

namespace OpenRCT2
{
    enum class VehicleFlag : uint8_t
    {
        onLiftHill,
        collisionDisabled,
        waitingOnAdjacentStation,
        poweredCarInReverse,
        readyToDepart,
        testing,
        // When go-karts are colliding, they have a higher chance of changing lanes
        currentlyColliding,
        // Used on vehicles when a safety cut-out stops them, such as RCs on a lift hill and powered rides
        stoppedBySafetyCutout,
        carIsBroken,
        trainIsBroken,
        stoppedOnHoldingBrake,
        // Used on vehicles that can run inverted for extended periods of time, i.e. the Flying, Lay-down and Multi-Dimension
        // trains
        carIsInverted,
        // Set when the vehicle travels backwards through the station for the first time
        reverseInclineCompletedLap,
        spinningIsLocked, // After passing a rotation toggle track piece this will enable
        // OpenRCT2 Flag: Used to override UpdateMotion to move the position of an individual car on a train
        moveSingleCar,
        crashed,       // Car displays as smoke plume
        carIsReversed, // Car is displayed running backwards
    };
    using VehicleFlags = FlagHolder<uint32_t, VehicleFlag>;
} // namespace OpenRCT2
