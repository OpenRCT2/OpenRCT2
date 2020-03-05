/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

#define MAX_RIDE_OBJECTS 128
#define MAX_SMALL_SCENERY_OBJECTS 252
#define MAX_LARGE_SCENERY_OBJECTS 128
#define MAX_WALL_SCENERY_OBJECTS 128
#define MAX_BANNER_OBJECTS 32
#define MAX_PATH_OBJECTS 16
#define MAX_PATH_ADDITION_OBJECTS 15
#define MAX_SCENERY_GROUP_OBJECTS 19
#define MAX_PARK_ENTRANCE_OBJECTS 1
#define MAX_WATER_OBJECTS 1
#define MAX_SCENARIO_TEXT_OBJECTS 1
#define MAX_TERRAIN_SURFACE_OBJECTS 14
#define MAX_TERRAIN_EDGE_OBJECTS 255
#define MAX_STATION_OBJECTS 255
#define MAX_MUSIC_OBJECTS 0

// clang-format off
constexpr const uint16_t OBJECT_ENTRY_COUNT =
    MAX_RIDE_OBJECTS +
    MAX_SMALL_SCENERY_OBJECTS +
    MAX_LARGE_SCENERY_OBJECTS +
    MAX_WALL_SCENERY_OBJECTS +
    MAX_BANNER_OBJECTS +
    MAX_PATH_OBJECTS +
    MAX_PATH_ADDITION_OBJECTS +
    MAX_SCENERY_GROUP_OBJECTS +
    MAX_PARK_ENTRANCE_OBJECTS +
    MAX_WATER_OBJECTS +
    MAX_SCENARIO_TEXT_OBJECTS;
// clang-format on
static_assert(OBJECT_ENTRY_COUNT == 721);

#define DAT_NAME_LENGTH 8
