/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

extern const uint8_t* Rotation1TimeToSpriteMaps[];
extern const uint8_t* Rotation2TimeToSpriteMaps[];
extern const uint8_t* Rotation3TimeToSpriteMaps[];

struct TopSpinTimeToSpriteMap
{
    uint8_t arm_rotation;
    uint8_t bank_rotation;
};

extern const TopSpinTimeToSpriteMap* TopSpinTimeToSpriteMaps[];

extern const uint8_t MotionSimulatorTimeToSpriteMap[];
extern const int32_t MotionSimulatorTimeToSpriteMapCount;

extern const int32_t SubpositionTranslationDistances[];
extern const int32_t AccelerationFromPitch[];

extern const int32_t SpriteDirectionToSoundDirection[];
