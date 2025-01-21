/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

using ObjectEntryIndex = uint16_t;
constexpr ObjectEntryIndex kObjectEntryIndexNull = std::numeric_limits<ObjectEntryIndex>::max();

// First 0xF of RCTObjectEntry->flags
enum class ObjectType : uint8_t
{
    Ride,
    SmallScenery,
    LargeScenery,
    Walls,
    Banners,
    Paths,
    PathAdditions,
    SceneryGroup,
    ParkEntrance,
    Water,
    ScenarioText,
    TerrainSurface,
    TerrainEdge,
    Station,
    Music,
    FootpathSurface,
    FootpathRailings,
    Audio,
    PeepNames,
    PeepAnimations,

    Count,
    None = 255
};

static constexpr size_t kNumTransientObjectTypes = 18;
static constexpr size_t kNumIntransientObjectTypes = 2;

bool ObjectTypeIsTransient(ObjectType type);
bool ObjectTypeIsIntransient(ObjectType type);

std::span<const ObjectType> getAllObjectTypes();
std::span<const ObjectType> getTransientObjectTypes();
std::span<const ObjectType> getIntransientObjectTypes();
