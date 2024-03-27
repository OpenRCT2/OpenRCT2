/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
constexpr ObjectEntryIndex OBJECT_ENTRY_INDEX_NULL = std::numeric_limits<ObjectEntryIndex>::max();

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

    Count,
    None = 255
};

constexpr std::array ObjectTypes = {
    ObjectType::Ride,
    ObjectType::SmallScenery,
    ObjectType::LargeScenery,
    ObjectType::Walls,
    ObjectType::Banners,
    ObjectType::Paths,
    ObjectType::PathAdditions,
    ObjectType::SceneryGroup,
    ObjectType::ParkEntrance,
    ObjectType::Water,
    ObjectType::ScenarioText,
    ObjectType::TerrainSurface,
    ObjectType::TerrainEdge,
    ObjectType::Station,
    ObjectType::Music,
    ObjectType::FootpathSurface,
    ObjectType::FootpathRailings,
    ObjectType::Audio,
};
// Not using EnumValue to reduce header includes
static_assert(ObjectTypes.size() == static_cast<uint8_t>(ObjectType::Count));

static constexpr size_t kNumTransientObjectTypes = 16;
static constexpr size_t kNumIntransientObjectTypes = 2;

bool ObjectTypeIsTransient(ObjectType type);
bool ObjectTypeIsIntransient(ObjectType type);
std::span<const ObjectType> getTransientObjectTypes();
std::span<const ObjectType> getIntransientObjectTypes();
