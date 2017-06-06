#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _VEHICLE_DATA_H
#define _VEHICLE_DATA_H

#include "../common.h"

extern const uint8 * Rotation1TimeToSpriteMaps[];
extern const uint8 * Rotation2TimeToSpriteMaps[];
extern const uint8 * Rotation3TimeToSpriteMaps[];

typedef struct top_spin_time_to_sprite_map {
    uint8 arm_rotation;
    uint8 bank_rotation;
} top_spin_time_to_sprite_map;

extern const top_spin_time_to_sprite_map * TopSpinTimeToSpriteMaps[];

extern const uint8 MotionSimulatorTimeToSpriteMap[];
extern const sint32 MotionSimulatorTimeToSpriteMapCount;

extern const sint32 dword_9A2930[];
extern const sint32 dword_9A2970[];

extern const sint32 word_9A3684[];

#endif
