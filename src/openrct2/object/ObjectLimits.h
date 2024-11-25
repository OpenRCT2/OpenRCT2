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

// Maximums based on number of values that can be represented in bit group.
// Subtract 1 to reserve the NULL entry identifier.
constexpr uint16_t kMaxRideObjects = 2047;
constexpr uint16_t kMaxSmallSceneryObjects = 2047;
constexpr uint16_t kMaxLargeSceneryObjects = 2047;
constexpr uint16_t kMaxWallSceneryObjects = 2047;
constexpr uint16_t kMaxBannerObjects = 255;
constexpr uint16_t kMaxPathObjects = 255;
constexpr uint16_t kMaxPathAdditionObjects = 255;
constexpr uint16_t kMaxSceneryGroupObjects = 255;
constexpr uint16_t kMaxParkEntranceObjects = 255;
constexpr uint16_t kMaxWaterObjects = 1;
constexpr uint16_t kMaxScenarioTextObjects = 1;
constexpr uint16_t kMaxTerrainSurfaceObjects = 255;
constexpr uint16_t kMaxTerrainEdgeObjects = 255;
constexpr uint16_t kMaxStationObjects = 255;
constexpr uint16_t kMaxMusicObjects = 255;
constexpr uint16_t kMaxFootpathSurfaceObjects = 255;
constexpr uint16_t kMaxFootpathRailingsObjects = 255;
constexpr uint16_t kMaxAudioObjects = 255;
constexpr uint16_t kMaxPeepNamesObjects = 1;
constexpr uint16_t kMaxPeepAnimationsObjects = 255;

constexpr uint8_t kDatNameLength = 8;
