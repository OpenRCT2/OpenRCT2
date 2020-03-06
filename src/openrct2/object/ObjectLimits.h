/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

constexpr const uint16_t MAX_RIDE_OBJECTS = 255;
constexpr const uint16_t MAX_SMALL_SCENERY_OBJECTS = 255;
constexpr const uint16_t MAX_LARGE_SCENERY_OBJECTS = 255;
constexpr const uint16_t MAX_WALL_SCENERY_OBJECTS = 255;
constexpr const uint16_t MAX_BANNER_OBJECTS = 255;
constexpr const uint16_t MAX_PATH_OBJECTS = 16;
constexpr const uint16_t MAX_PATH_ADDITION_OBJECTS = 15;
constexpr const uint16_t MAX_SCENERY_GROUP_OBJECTS = 19;
constexpr const uint16_t MAX_PARK_ENTRANCE_OBJECTS = 1;
constexpr const uint16_t MAX_WATER_OBJECTS = 1;
constexpr const uint16_t MAX_SCENARIO_TEXT_OBJECTS = 1;
constexpr const uint16_t MAX_TERRAIN_SURFACE_OBJECTS = 255;
constexpr const uint16_t MAX_TERRAIN_EDGE_OBJECTS = 255;
constexpr const uint16_t MAX_STATION_OBJECTS = 255;
constexpr const uint16_t MAX_MUSIC_OBJECTS = 255;

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

constexpr const uint8_t DAT_NAME_LENGTH = 8;
