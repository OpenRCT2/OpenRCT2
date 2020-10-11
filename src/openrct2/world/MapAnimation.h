/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Location.hpp"

#include <cstdint>
#include <vector>

struct MapAnimation
{
    uint8_t type{};
    CoordsXYZ location{};
};

enum
{
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

void map_animation_create(int32_t type, const CoordsXYZ& loc);
void map_animation_invalidate_all();
const std::vector<MapAnimation>& GetMapAnimations();
void AutoCreateMapAnimations();
