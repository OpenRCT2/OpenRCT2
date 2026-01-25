/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../Context.h"
    #include "../../../object/ObjectRepository.h"
    #include "../../ScriptEngine.h"

    #include <optional>
    #include <string_view>

namespace OpenRCT2::Scripting
{
    std::string_view objectTypeToString(ObjectType type);
    ObjectType objectTypeFromString(std::string_view string);

    inline std::string_view ObjectSourceGameToString(ObjectSourceGame sourceGame)
    {
        static constexpr std::string_view values[] = { "custom", "wacky_worlds",      "time_twister",     "openrct2_official",
                                                       "rct1",   "added_attractions", "loopy_landscapes", "unknown",
                                                       "rct2" };
        if (EnumValue(sourceGame) >= std::size(values))
            return "unknown";
        return values[EnumValue(sourceGame)];
    }

    class ScInstalledObject;
    extern ScInstalledObject gScInstalledObject;

    class ScInstalledObject final : public ScBase
    {
    private:
        struct InstalledObjectData
        {
            size_t _index{};
        };

    public:
        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "InstalledObject", Finalize);
        }

        JSValue New(JSContext* ctx, size_t index)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("path", ScInstalledObject::path_get, nullptr),
                JS_CGETSET_DEF("generation", ScInstalledObject::generation_get, nullptr),
                JS_CGETSET_DEF("identifier", ScInstalledObject::identifier_get, nullptr),
                JS_CGETSET_DEF("type", ScInstalledObject::type_get, nullptr),
                JS_CGETSET_DEF("sourceGames", ScInstalledObject::sourceGames_get, nullptr),
                JS_CGETSET_DEF("legacyIdentifier", ScInstalledObject::legacyIdentifier_get, nullptr),
                JS_CGETSET_DEF("authors", ScInstalledObject::authors_get, nullptr),
                JS_CGETSET_DEF("name", ScInstalledObject::name_get, nullptr),
            };
            return MakeWithOpaque(ctx, funcs, new InstalledObjectData{ index });
        }

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            InstalledObjectData* data = gScInstalledObject.GetOpaque<InstalledObjectData*>(thisVal);
            if (data)
                delete data;
        }

        static JSValue path_get(JSContext* ctx, JSValue thisVal)
        {
            auto installedObject = GetInstalledObject(thisVal);
            if (installedObject != nullptr)
            {
                return JSFromStdString(ctx, installedObject->Path);
            }
            return JSFromStdString(ctx, "");
        }

        static JSValue generation_get(JSContext* ctx, JSValue thisVal)
        {
            auto installedObject = GetInstalledObject(thisVal);
            if (installedObject != nullptr)
            {
                if (installedObject->Generation == ObjectGeneration::DAT)
                    return JSFromStdString(ctx, "dat");
                else
                    return JSFromStdString(ctx, "json");
            }
            return JSFromStdString(ctx, "");
        }

        static JSValue sourceGames_get(JSContext* ctx, JSValue thisVal)
        {
            auto installedObject = GetInstalledObject(thisVal);

            JSValue array = JS_NewArray(ctx);
            if (installedObject != nullptr)
            {
                int64_t index = 0;
                for (const auto& sourceGame : installedObject->Sources)
                {
                    JSValue sourceGameStr = JSFromStdString(ctx, std::string(ObjectSourceGameToString(sourceGame)));
                    JS_SetPropertyInt64(ctx, array, index++, sourceGameStr);
                }
            }
            return array;
        }

        static JSValue type_get(JSContext* ctx, JSValue thisVal)
        {
            auto installedObject = GetInstalledObject(thisVal);
            if (installedObject != nullptr)
            {
                return JSFromStdString(ctx, std::string(objectTypeToString(installedObject->Type)));
            }
            return JSFromStdString(ctx, "");
        }

        static JSValue identifier_get(JSContext* ctx, JSValue thisVal)
        {
            auto installedObject = GetInstalledObject(thisVal);
            if (installedObject != nullptr)
            {
                if (installedObject->Generation == ObjectGeneration::DAT)
                {
                    return JSFromStdString(ctx, ObjectEntryDescriptor(installedObject->ObjectEntry).ToString());
                }
                else
                {
                    return JSFromStdString(ctx, installedObject->Identifier);
                }
            }
            return JSFromStdString(ctx, "");
        }

        static JSValue legacyIdentifier_get(JSContext* ctx, JSValue thisVal)
        {
            auto installedObject = GetInstalledObject(thisVal);
            if (installedObject != nullptr)
            {
                if (!installedObject->ObjectEntry.IsEmpty())
                {
                    auto str = installedObject->ObjectEntry.GetName();
                    if (str.find('\0') != std::string::npos)
                        str = {};
                    return JSFromStdString(ctx, str);
                }
            }
            return JS_NULL;
        }

        static JSValue authors_get(JSContext* ctx, JSValue thisVal)
        {
            auto installedObject = GetInstalledObject(thisVal);

            JSValue array = JS_NewArray(ctx);
            if (installedObject != nullptr)
            {
                uint32_t index = 0;
                for (const auto& author : installedObject->Authors)
                {
                    JSValue authorStr = JSFromStdString(ctx, author);
                    JS_SetPropertyInt64(ctx, array, index++, authorStr);
                }
            }
            return array;
        }

        static JSValue name_get(JSContext* ctx, JSValue thisVal)
        {
            auto installedObject = GetInstalledObject(thisVal);
            if (installedObject != nullptr)
            {
                return JSFromStdString(ctx, installedObject->Name);
            }
            return JSFromStdString(ctx, "");
        }

        static const ObjectRepositoryItem* GetInstalledObject(JSValue thisVal)
        {
            size_t index = gScInstalledObject.GetOpaque<InstalledObjectData*>(thisVal)->_index;
            auto context = GetContext();
            auto& objectRepository = context->GetObjectRepository();
            auto numObjects = objectRepository.GetNumObjects();
            if (index < numObjects)
            {
                auto* objects = objectRepository.GetObjects();
                return &objects[index];
            }
            return nullptr;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
