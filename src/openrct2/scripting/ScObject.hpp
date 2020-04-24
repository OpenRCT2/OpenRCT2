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

#    include "../Context.h"
#    include "../common.h"
#    include "../object/ObjectManager.h"
#    include "../object/RideObject.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

#    include <optional>

namespace OpenRCT2::Scripting
{
    class ScObject
    {
    private:
        uint8_t _type{};
        int32_t _index{};

    public:
        ScObject(uint8_t type, int32_t index)
            : _type(type)
            , _index(index)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScObject::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScObject::index_get, nullptr, "index");
            dukglue_register_property(ctx, &ScObject::identifier_get, nullptr, "identifier");
            dukglue_register_property(ctx, &ScObject::name_get, nullptr, "name");
        }

        static std::optional<uint8_t> StringToObjectType(const std::string_view& type)
        {
            for (uint8_t i = 0; i < OBJECT_TYPE_COUNT; i++)
            {
                auto s = ObjectTypeToString(i);
                if (s == type)
                {
                    return i;
                }
            }
            return std::nullopt;
        }

        static std::string_view ObjectTypeToString(uint8_t type)
        {
            static const char* Types[] = { "ride",     "small_scenery",     "large_scenery", "wall",          "banner",
                                           "footpath", "footpath_addition", "scenery_group", "park_entrance", "water",
                                           "stex",     "terrain_surface",   "terrain_edge",  "station",       "music" };
            if (type >= std::size(Types))
                return "unknown";
            return Types[type];
        }

    private:
        std::string type_get()
        {
            return std::string(ObjectTypeToString(_type));
        }

        int32_t index_get()
        {
            return _index;
        }

        std::string identifier_get()
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return obj->GetIdentifier();
            }
            return {};
        }

        std::string name_get()
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return obj->GetName();
            }
            return {};
        }

    protected:
        Object* GetObject()
        {
            auto& objManager = GetContext()->GetObjectManager();
            return objManager.GetLoadedObject(_type, _index);
        }
    };

    class ScRideObject : public ScObject
    {
    public:
        ScRideObject(uint8_t type, int32_t index)
            : ScObject(type, index)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScObject, ScRideObject>(ctx);
            dukglue_register_property(ctx, &ScRideObject::description_get, nullptr, "description");
            dukglue_register_property(ctx, &ScRideObject::capacity_get, nullptr, "capacity");
        }

    private:
        std::string description_get()
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return obj->GetDescription();
            }
            return {};
        }

        std::string capacity_get()
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return obj->GetCapacity();
            }
            return {};
        }

    protected:
        RideObject* GetObject()
        {
            return static_cast<RideObject*>(ScObject::GetObject());
        }
    };
} // namespace OpenRCT2::Scripting

#endif
