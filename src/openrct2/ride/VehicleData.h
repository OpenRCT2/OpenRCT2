/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _VEHICLE_DATA_H
#define _VEHICLE_DATA_H

#include "../common.h"

extern const uint8 * Rotation1TimeToSpriteMaps[];
extern const uint8 * Rotation2TimeToSpriteMaps[];
extern const uint8 * Rotation3TimeToSpriteMaps[];

struct top_spin_time_to_sprite_map {
    uint8 arm_rotation;
    uint8 bank_rotation;
};

extern const top_spin_time_to_sprite_map * TopSpinTimeToSpriteMaps[];

extern const uint8 MotionSimulatorTimeToSpriteMap[];
extern const sint32 MotionSimulatorTimeToSpriteMapCount;

extern const sint32 dword_9A2930[];
extern const sint32 dword_9A2970[];

extern const sint32 SpriteDirectionToSoundDirection[];

#endif
