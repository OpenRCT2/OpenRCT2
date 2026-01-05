/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScObject.hpp"

namespace OpenRCT2::Scripting
{
    static const EnumMap<ObjectType> kObjectTypeMap = {
        { "ride", ObjectType::ride },
        { "small_scenery", ObjectType::smallScenery },
        { "large_scenery", ObjectType::largeScenery },
        { "wall", ObjectType::walls },
        { "banner", ObjectType::banners },
        { "footpath", ObjectType::paths },
        { "footpath_addition", ObjectType::pathAdditions },
        { "scenery_group", ObjectType::sceneryGroup },
        { "park_entrance", ObjectType::parkEntrance },
        { "water", ObjectType::water },
        { "scenario_meta", ObjectType::scenarioMeta },
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
} // namespace OpenRCT2::Scripting

#endif
