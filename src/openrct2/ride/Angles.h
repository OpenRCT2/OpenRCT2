/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

/*
 * VehiclePitch and VehicleRoll are both added to the local scope in VehicleSubpositionData.cpp thanks to 'using enum'
 * introduced in C++20. As such, enum items must be named to avoid name collisions between the two enums.
 */

enum class VehiclePitch : uint8_t
{
    flat = 0,
    up12,
    up25,
    up42,
    up60,
    down12,
    down25,
    down42,
    down60,
    up75,
    up90,
    up105,
    up120,
    up135,
    up150,
    up165,
    inverted,
    down75,
    down90,
    down105,
    down120,
    down135,
    down150,
    down165,
    corkscrewUpRight0,
    corkscrewUpRight1,
    corkscrewUpRight2,
    corkscrewUpRight3,
    corkscrewUpRight4,
    corkscrewDownLeft0,
    corkscrewDownLeft1,
    corkscrewDownLeft2,
    corkscrewDownLeft3,
    corkscrewDownLeft4,
    corkscrewUpLeft0,
    corkscrewUpLeft1,
    corkscrewUpLeft2,
    corkscrewUpLeft3,
    corkscrewUpLeft4,
    corkscrewDownRight0,
    corkscrewDownRight1,
    corkscrewDownRight2,
    corkscrewDownRight3,
    corkscrewDownRight4,
    upHalfHelixLarge,
    upHalfHelixSmall,
    downHalfHelixLarge,
    downHalfHelixSmall,
    upQuarterHelix,
    downQuarterHelix,
    up8,
    up16,
    up50,
    down8,
    down16,
    down50,
    uninvertingDown25, // uninverting pitches are used on inverted-to-upright flyer loops to draw inverted vehicles as upright
    uninvertingDown42,
    uninvertingDown60,
    curvedLiftHillUp,
    curvedLiftHillDown,
    pitchCount,
    nullPitch = 255,
};

enum class VehicleRoll : uint8_t
{
    unbanked = 0,
    left22,
    left45,
    right22,
    right45,
    left67,
    left90,
    left112,
    left135,
    left157,
    right67,
    right90,
    right112,
    right135,
    right157,
    uninvertingUnbanked, // uninverting roll values are used on inverted-to-upright flyer twists to draw inverted vehicles
    uninvertingLeft22,   // as upright
    uninvertingLeft45,
    uninvertingRight22,
    uninvertingRight45,
    rollCount,
    nullRoll = 255,
};

// Size: 0x09
struct VehicleInfo
{
    int16_t x;          // 0x00
    int16_t y;          // 0x02
    int16_t z;          // 0x04
    uint8_t direction;  // 0x06
    VehiclePitch pitch; // 0x07
    VehicleRoll roll;   // 0x08

    bool IsInvalid() const
    {
        return x == 0 && y == 0 && z == 0 && direction == 0 && pitch == VehiclePitch::flat && roll == VehicleRoll::unbanked;
    }
};
