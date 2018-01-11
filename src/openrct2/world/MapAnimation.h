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

#ifndef _MAP_ANIMATION_H_
#define _MAP_ANIMATION_H_

#include "../common.h"

#pragma pack(push, 1)
/**
 * Animated object
 * size: 0x06
 */
typedef struct rct_map_animation {
    uint8 baseZ;
    uint8 type;
    uint16 x;
    uint16 y;
} rct_map_animation;
assert_struct_size(rct_map_animation, 6);
#pragma pack(pop)

enum {
    MAP_ANIMATION_TYPE_RIDE_ENTRANCE,
    MAP_ANIMATION_TYPE_QUEUE_BANNER,
    MAP_ANIMATION_TYPE_SMALL_SCENERY,
    MAP_ANIMATION_TYPE_PARK_ENTRANCE,
    MAP_ANIMATION_TYPE_TRACK_WATERFALL,
    MAP_ANIMATION_TYPE_TRACK_RAPIDS,
    MAP_ANIMATION_TYPE_TRACK_ONRIDEPHOTO,
    MAP_ANIMATION_TYPE_TRACK_WHIRLPOOL,
    MAP_ANIMATION_TYPE_TRACK_SPINNINGTUNNEL,
    MAP_ANIMATION_TYPE_REMOVE,
    MAP_ANIMATION_TYPE_BANNER,
    MAP_ANIMATION_TYPE_LARGE_SCENERY,
    MAP_ANIMATION_TYPE_WALL_DOOR,
    MAP_ANIMATION_TYPE_WALL,
    MAP_ANIMATION_TYPE_COUNT
};

#define MAX_ANIMATED_OBJECTS 2000

#ifdef __cplusplus
extern "C" {
#endif

extern uint16 gNumMapAnimations;
extern rct_map_animation gAnimatedObjects[MAX_ANIMATED_OBJECTS];

void map_animation_create(sint32 type, sint32 x, sint32 y, sint32 z);
void map_animation_invalidate_all();

#ifdef __cplusplus
}
#endif

#endif
