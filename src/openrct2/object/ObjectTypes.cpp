/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectTypes.h"

#include "../core/EnumUtils.hpp"
#include "Object.h"

#include <algorithm>

constexpr std::array kAllObjectTypes = {
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
    ObjectType::PeepNames,
    ObjectType::PeepAnimations,
};

static_assert(kAllObjectTypes.size() == EnumValue(ObjectType::Count));

// Object types that can be saved in a park file.
static constexpr std::array<const ObjectType, kNumTransientObjectTypes> kTransientObjectTypes = {
    ObjectType::Ride,         ObjectType::SmallScenery,   ObjectType::LargeScenery,    ObjectType::Walls,
    ObjectType::Banners,      ObjectType::Paths,          ObjectType::PathAdditions,   ObjectType::SceneryGroup,
    ObjectType::ParkEntrance, ObjectType::Water,          ObjectType::TerrainSurface,  ObjectType::TerrainEdge,
    ObjectType::Station,      ObjectType::Music,          ObjectType::FootpathSurface, ObjectType::FootpathRailings,
    ObjectType::PeepNames,    ObjectType::PeepAnimations,
};

// Object types that cannot be saved in a park file.
static constexpr std::array<const ObjectType, kNumIntransientObjectTypes> kIntransientObjectTypes = {
    ObjectType::ScenarioText,
    ObjectType::Audio,
};

static_assert(kNumTransientObjectTypes + kNumIntransientObjectTypes == static_cast<size_t>(ObjectType::Count));

bool ObjectTypeIsTransient(ObjectType type)
{
    return std::find(kTransientObjectTypes.begin(), kTransientObjectTypes.end(), type) != std::end(kTransientObjectTypes);
}

bool ObjectTypeIsIntransient(ObjectType type)
{
    return std::find(kIntransientObjectTypes.begin(), kIntransientObjectTypes.end(), type) != std::end(kIntransientObjectTypes);
}

std::span<const ObjectType> getAllObjectTypes()
{
    return kAllObjectTypes;
}

std::span<const ObjectType> getTransientObjectTypes()
{
    return kTransientObjectTypes;
}

std::span<const ObjectType> getIntransientObjectTypes()
{
    return kIntransientObjectTypes;
}
