/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectTypes.h"

#include "../util/Util.h"
#include "Object.h"

#include <algorithm>

// Object types that can be saved in a park file.
static constexpr std::array<ObjectType, kNumTransientObjectTypes> kTransientObjectTypes = {
    ObjectType::Ride,         ObjectType::SmallScenery, ObjectType::LargeScenery,    ObjectType::Walls,
    ObjectType::Banners,      ObjectType::Paths,        ObjectType::PathAdditions,   ObjectType::SceneryGroup,
    ObjectType::ParkEntrance, ObjectType::Water,        ObjectType::TerrainSurface,  ObjectType::TerrainEdge,
    ObjectType::Station,      ObjectType::Music,        ObjectType::FootpathSurface, ObjectType::FootpathRailings,
};

// Object types that cannot be saved in a park file.
static constexpr std::array<ObjectType, kNumIntransientObjectTypes> kIntransientObjectTypes = {
    ObjectType::ScenarioText,
    ObjectType::Audio,
};

bool ObjectTypeIsTransient(ObjectType type)
{
    return std::find(kTransientObjectTypes.begin(), kTransientObjectTypes.end(), type) != std::end(kTransientObjectTypes);
}

bool ObjectTypeIsIntransient(ObjectType type)
{
    return std::find(kIntransientObjectTypes.begin(), kIntransientObjectTypes.end(), type) != std::end(kIntransientObjectTypes);
}

size_t GetObjectTypeLimit(ObjectType type)
{
    auto index = EnumValue(type);
    if (index >= EnumValue(ObjectType::Count))
        return 0;
    return static_cast<size_t>(object_entry_group_counts[index]);
}

const std::array<ObjectType, kNumTransientObjectTypes>& getTransientObjectTypes()
{
    return kTransientObjectTypes;
}

const std::array<ObjectType, kNumIntransientObjectTypes>& getIntransientObjectTypes()
{
    return kIntransientObjectTypes;
}
