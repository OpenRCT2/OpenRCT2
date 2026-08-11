/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

extern const uint8_t* kTwistTimeToSpriteMaps[];
extern const uint8_t* kEnterpriseTimeToSpriteMaps[];
extern const uint8_t* kMerryGoRoundTimeToSpriteMaps[];

struct TopSpinTimeToSpriteMap
{
    uint8_t arm_rotation;
    uint8_t bank_rotation;
};

extern const TopSpinTimeToSpriteMap* kTopSpinTimeToSpriteMaps[];

extern const uint8_t MotionSimulatorTimeToSpriteMap[];
extern const int32_t MotionSimulatorTimeToSpriteMapCount;

extern const uint8_t kSpaceRingsTimeToSpriteMap[];

/** rct2: 0x0099F9D0 */
extern const int8_t* kSwingingTimeToSpriteMaps[];
