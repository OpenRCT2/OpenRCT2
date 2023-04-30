/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../object/ObjectRepository.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

#    include <optional>

namespace OpenRCT2::Scripting
{
    inline std::string_view ObjectTypeToString(uint8_t type)
    {
        static constexpr std::string_view Types[] = {
            "ride",
            "small_scenery",
            "large_scenery",
            "wall",
            "banner",
            "footpath",
            "footpath_addition",
            "scenery_group",
            "park_entrance",
            "water",
            "stex",
            "terrain_surface",
            "terrain_edge",
            "station",
            "music",
            "footpath_surface",
            "footpath_railings",
        };
        if (type >= std::size(Types))
            return "unknown";
        return Types[type];
    }

    inline std::string_view ObjectSourceGameToString(ObjectSourceGame sourceGame)
    {
        static constexpr std::string_view values[] = { "custom", "wacky_worlds",      "time_twister",     "openrct2_official",
                                                       "rct1",   "added_attractions", "loopy_landscapes", "unknown",
                                                       "rct2" };
        if (EnumValue(sourceGame) >= std::size(values))
            return "unknown";
        return values[EnumValue(sourceGame)];
    }

    class ScInstalledObject
    {
    protected:
        size_t _index{};

    public:
        ScInstalledObject(size_t index)
            : _index(index)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScInstalledObject::path_get, nullptr, "path");
            dukglue_register_property(ctx, &ScInstalledObject::generation_get, nullptr, "generation");
            dukglue_register_property(ctx, &ScInstalledObject::identifier_get, nullptr, "identifier");
            dukglue_register_property(ctx, &ScInstalledObject::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScInstalledObject::sourceGames_get, nullptr, "sourceGames");
            dukglue_register_property(ctx, &ScInstalledObject::legacyIdentifier_get, nullptr, "legacyIdentifier");
            dukglue_register_property(ctx, &ScInstalledObject::authors_get, nullptr, "authors");
            dukglue_register_property(ctx, &ScInstalledObject::name_get, nullptr, "name");
        }

    private:
        std::string path_get() const
        {
            auto installedObject = GetInstalledObject();
            if (installedObject != nullptr)
            {
                return installedObject->Path;
            }
            return {};
        }

        std::string generation_get() const
        {
            auto installedObject = GetInstalledObject();
            if (installedObject != nullptr)
            {
                if (installedObject->Generation == ObjectGeneration::DAT)
                    return "dat";
                else
                    return "json";
            }
            return {};
        }

        std::vector<std::string> sourceGames_get() const
        {
            std::vector<std::string> result;
            auto installedObject = GetInstalledObject();
            if (installedObject != nullptr)
            {
                for (const auto& sourceGame : installedObject->Sources)
                {
                    result.push_back(std::string(ObjectSourceGameToString(sourceGame)));
                }
            }
            return result;
        }

        std::string type_get() const
        {
            auto installedObject = GetInstalledObject();
            if (installedObject != nullptr)
            {
                return std::string(ObjectTypeToString(EnumValue(installedObject->Type)));
            }
            return {};
        }

        std::string identifier_get() const
        {
            auto installedObject = GetInstalledObject();
            if (installedObject != nullptr)
            {
                if (installedObject->Generation == ObjectGeneration::DAT)
                {
                    return ObjectEntryDescriptor(installedObject->ObjectEntry).ToString();
                }
                else
                {
                    return installedObject->Identifier;
                }
            }
            return {};
        }

        std::string legacyIdentifier_get() const
        {
            auto installedObject = GetInstalledObject();
            if (installedObject != nullptr)
            {
                if (!installedObject->ObjectEntry.IsEmpty())
                {
                    return std::string(installedObject->ObjectEntry.GetName());
                }
            }
            return {};
        }

        std::vector<std::string> authors_get() const
        {
            auto installedObject = GetInstalledObject();
            if (installedObject != nullptr)
            {
                return installedObject->Authors;
            }
            return {};
        }

        std::string name_get() const
        {
            auto installedObject = GetInstalledObject();
            if (installedObject != nullptr)
            {
                return installedObject->Name;
            }
            return {};
        }

        const ObjectRepositoryItem* GetInstalledObject() const
        {
            auto context = GetContext();
            auto& objectRepository = context->GetObjectRepository();
            auto numObjects = objectRepository.GetNumObjects();
            if (_index < numObjects)
            {
                auto* objects = objectRepository.GetObjects();
                return &objects[_index];
            }
            return nullptr;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
