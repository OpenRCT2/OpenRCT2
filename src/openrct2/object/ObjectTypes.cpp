/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
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
        ObjectType::scenarioMeta,
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
        ObjectType::scenarioMeta,
        ObjectType::audio,
    };

    static_assert(kNumTransientObjectTypes + kNumIntransientObjectTypes == static_cast<size_t>(ObjectType::count));

    bool ObjectTypeIsTransient(ObjectType type)
    {
        return std::find(kTransientObjectTypes.begin(), kTransientObjectTypes.end(), type) != std::end(kTransientObjectTypes);
    }

    bool ObjectTypeIsIntransient(ObjectType type)
    {
        return std::find(kIntransientObjectTypes.begin(), kIntransientObjectTypes.end(), type)
            != std::end(kIntransientObjectTypes);
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
} // namespace OpenRCT2
