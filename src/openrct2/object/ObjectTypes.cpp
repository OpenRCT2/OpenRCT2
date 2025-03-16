/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectTypes.h"

#include "../core/EnumMap.hpp"
#include "../core/EnumUtils.hpp"
#include "Object.h"

#include <algorithm>

constexpr std::array kAllObjectTypes = {
    ObjectType::ride,
    ObjectType::smallScenery,
    ObjectType::largeScenery,
    ObjectType::walls,
    ObjectType::banners,
    ObjectType::paths,
    ObjectType::pathAdditions,
    ObjectType::sceneryGroup,
    ObjectType::parkEntrance,
    ObjectType::water,
    ObjectType::scenarioText,
    ObjectType::terrainSurface,
    ObjectType::terrainEdge,
    ObjectType::station,
    ObjectType::music,
    ObjectType::footpathSurface,
    ObjectType::footpathRailings,
    ObjectType::audio,
    ObjectType::peepNames,
    ObjectType::peepAnimations,
    ObjectType::climate,
};

static_assert(kAllObjectTypes.size() == EnumValue(ObjectType::count));

// Object types that can be saved in a park file.
static constexpr std::array<const ObjectType, kNumTransientObjectTypes> kTransientObjectTypes = {
    ObjectType::ride,         ObjectType::smallScenery,   ObjectType::largeScenery,    ObjectType::walls,
    ObjectType::banners,      ObjectType::paths,          ObjectType::pathAdditions,   ObjectType::sceneryGroup,
    ObjectType::parkEntrance, ObjectType::water,          ObjectType::terrainSurface,  ObjectType::terrainEdge,
    ObjectType::station,      ObjectType::music,          ObjectType::footpathSurface, ObjectType::footpathRailings,
    ObjectType::peepNames,    ObjectType::peepAnimations, ObjectType::climate,
};

// Object types that cannot be saved in a park file.
static constexpr std::array<const ObjectType, kNumIntransientObjectTypes> kIntransientObjectTypes = {
    ObjectType::scenarioText,
    ObjectType::audio,
};

static_assert(kNumTransientObjectTypes + kNumIntransientObjectTypes == static_cast<size_t>(ObjectType::count));

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

static const EnumMap<ObjectType> kObjectTypeMap = {
    { "ride", ObjectType::ride },
    { "scenery_small", ObjectType::smallScenery },
    { "scenery_large", ObjectType::largeScenery },
    { "scenery_wall", ObjectType::walls },
    { "footpath_banner", ObjectType::banners },
    { "footpath_item", ObjectType::pathAdditions },
    { "scenery_group", ObjectType::sceneryGroup },
    { "park_entrance", ObjectType::parkEntrance },
    { "water", ObjectType::water },
    { "scenario_text", ObjectType::scenarioText },
    { "terrain_surface", ObjectType::terrainSurface },
    { "terrain_edge", ObjectType::terrainEdge },
    { "station", ObjectType::station },
    { "music", ObjectType::music },
    { "footpath_surface", ObjectType::footpathSurface },
    { "footpath_railings", ObjectType::footpathRailings },
    { "audio", ObjectType::audio },
    { "peep_names", ObjectType::peepNames },
    { "peep_animations", ObjectType::peepAnimations },
    { "climate", ObjectType::climate },
};

std::string_view objectTypeToString(ObjectType type)
{
    auto result = kObjectTypeMap.find(type);
    return result != kObjectTypeMap.end() ? result->first : "";
}

ObjectType objectTypeFromString(std::string_view string)
{
    auto result = kObjectTypeMap.find(string);
    return result != kObjectTypeMap.end() ? result->second : ObjectType::none;
}
