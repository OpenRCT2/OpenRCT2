/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <array>
#include <cstdint>
#include <limits>

using ObjectEntryIndex = uint16_t;
constexpr const ObjectEntryIndex OBJECT_ENTRY_INDEX_NULL = std::numeric_limits<ObjectEntryIndex>::max();

// First 0xF of RCTObjectEntry->flags
enum class ObjectType : uint8_t
{
    Ride,
    SmallScenery,
    LargeScenery,
    Walls,
    Banners,
    Paths,
    PathBits,
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
    ObjectType::PathBits,
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

// Object types that can be saved in a park file.
constexpr std::array<ObjectType, 16> TransientObjectTypes = {
    ObjectType::Ride,         ObjectType::SmallScenery, ObjectType::LargeScenery,    ObjectType::Walls,
    ObjectType::Banners,      ObjectType::Paths,        ObjectType::PathBits,        ObjectType::SceneryGroup,
    ObjectType::ParkEntrance, ObjectType::Water,        ObjectType::TerrainSurface,  ObjectType::TerrainEdge,
    ObjectType::Station,      ObjectType::Music,        ObjectType::FootpathSurface, ObjectType::FootpathRailings,
};

// Object types that cannot be saved in a park file.
constexpr std::array<ObjectType, 2> IntransientObjectTypes = { ObjectType::ScenarioText, ObjectType::Audio };

bool ObjectTypeIsTransient(ObjectType type);
bool ObjectTypeIsIntransient(ObjectType type);
