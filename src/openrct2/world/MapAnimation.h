/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _MAP_ANIMATION_H_
#define _MAP_ANIMATION_H_

#include "../common.h"

#pragma pack(push, 1)
/**
 * Animated object
 * size: 0x06
 */
struct rct_map_animation {
    uint8_t baseZ;
    uint8_t type;
    uint16_t x;
    uint16_t y;
};
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

extern uint16_t gNumMapAnimations;
extern rct_map_animation gAnimatedObjects[MAX_ANIMATED_OBJECTS];

void map_animation_create(int32_t type, int32_t x, int32_t y, int32_t z);
void map_animation_invalidate_all();

#endif
