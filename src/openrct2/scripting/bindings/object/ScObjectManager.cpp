/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScObjectManager.h"

    #include "../../../Context.h"
    #include "../../../object/ObjectList.h"
    #include "../../../ride/RideData.h"
    #include "../../../windows/Intent.h"
    #include "../../ScriptEngine.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

void ScObjectManager::Register(JSContext* ctx)
{
    RegisterBaseStr(ctx, "ObjectManager");
}

JSValue ScObjectManager::New(JSContext* ctx)
{
    static constexpr JSCFunctionListEntry funcs[] = {
        JS_CGETSET_DEF("installedObjects", ScObjectManager::installedObjects_get, nullptr),
        JS_CFUNC_DEF("getInstalledObject", 1, ScObjectManager::getInstalledObject),
        JS_CFUNC_DEF("load", 2, ScObjectManager::load),
        JS_CFUNC_DEF("unload", 1, ScObjectManager::unload),
        JS_CFUNC_DEF("getObject", 2, ScObjectManager::getObject),
        JS_CFUNC_DEF("getAllObjects", 1, ScObjectManager::getAllObjects),
    };
    return MakeWithOpaque(ctx, funcs, nullptr);
}

JSValue ScObjectManager::installedObjects_get(JSContext* ctx, JSValue thisVal)
{
    JSValue result = JS_NewArray(ctx);

    auto context = GetContext();
    auto& objectManager = context->GetObjectRepository();
    auto count = static_cast<int64_t>(objectManager.GetNumObjects());
    for (int64_t i = 0; i < count; i++)
    {
        JSValue installedObject = gScInstalledObject.New(ctx, i);
        JS_SetPropertyInt64(ctx, result, i, installedObject);
    }

    return result;
}

JSValue ScObjectManager::getInstalledObject(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
{
    JS_UNPACK_STR(identifier, ctx, argv[0]);

    auto context = GetContext();
    auto& objectRepository = context->GetObjectRepository();
    auto object = objectRepository.FindObject(identifier);
    return object != nullptr ? gScInstalledObject.New(ctx, object->Id) : JS_NULL;
}

JSValue ScObjectManager::load(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
{
    auto context = GetContext();
    auto& objectRepository = context->GetObjectRepository();
    auto& objectManager = context->GetObjectManager();

    if (JS_IsArray(argv[0]))
    {
        // load(identifiers)
        JS_UNPACK_ARRAY(array, ctx, argv[0]);
        std::vector<ObjectEntryDescriptor> descriptors;
        int64_t length;
        JS_GetLength(ctx, array, &length);

        for (int64_t i = 0; i < length; i++)
        {
            JSValue item = JS_GetPropertyInt64(ctx, array, i);
            JS_UNPACK_STR(identifier, ctx, item);
            descriptors.push_back(ObjectEntryDescriptor::Parse(identifier));
        }

        JSValue result = JS_NewArray(ctx);
        int64_t index = 0;
        for (const auto& descriptor : descriptors)
        {
            auto obj = objectManager.LoadObject(descriptor);
            if (obj != nullptr)
            {
                MarkAsResearched(obj);
                auto objIndex = objectManager.GetLoadedObjectEntryIndex(obj);
                auto scLoadedObject = CreateScObject(ctx, obj->GetObjectType(), objIndex);
                JS_SetPropertyInt64(ctx, result, index, scLoadedObject);
            }
            else
            {
                JS_SetPropertyInt64(ctx, result, index, JS_NULL);
            }
            index++;
        }
        RefreshResearchedItems();
        return result;
    }
    else
    {
        // load(identifier, index?)
        JS_UNPACK_STR(identifier, ctx, argv[0]);
        auto descriptor = ObjectEntryDescriptor::Parse(identifier);

        auto installedObject = objectRepository.FindObject(descriptor);
        if (installedObject != nullptr)
        {
            if (argc > 1 && !JS_IsUndefined(argv[1]))
            {
                JS_UNPACK_UINT32(index, ctx, argv[1]);

                auto limit = getObjectTypeLimit(installedObject->Type);
                if (index < limit)
                {
                    auto loadedObject = objectManager.GetLoadedObject(installedObject->Type, index);
                    if (loadedObject != nullptr)
                    {
                        objectManager.UnloadObjects({ loadedObject->GetDescriptor() });
                    }
                    auto obj = objectManager.LoadObject(descriptor, static_cast<ObjectEntryIndex>(index));
                    if (obj != nullptr)
                    {
                        MarkAsResearched(obj);
                        RefreshResearchedItems();
                        auto objIndex = objectManager.GetLoadedObjectEntryIndex(obj);
                        return CreateScObject(ctx, obj->GetObjectType(), objIndex);
                    }
                }
            }
            else
            {
                auto obj = objectManager.LoadObject(descriptor);
                if (obj != nullptr)
                {
                    MarkAsResearched(obj);
                    RefreshResearchedItems();
                    auto objIndex = objectManager.GetLoadedObjectEntryIndex(obj);
                    return CreateScObject(ctx, obj->GetObjectType(), objIndex);
                }
            }
        }
    }
    return JS_NULL;
}

JSValue ScObjectManager::unload(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
{
    // TODO: shouldn't this throw JS_THROW_IF_GAME_STATE_NOT_MUTABLE()?
    auto context = GetContext();
    auto& objectManager = context->GetObjectManager();

    if (JS_IsString(argv[0]))
    {
        JS_UNPACK_STR(szP1, ctx, argv[0]);
        auto objType = objectTypeFromString(szP1);
        if (objType != ObjectType::none)
        {
            // unload(type, index)
            JS_UNPACK_UINT32(objIndex, ctx, argv[1]);
            auto obj = objectManager.GetLoadedObject(objType, objIndex);
            if (obj != nullptr)
            {
                objectManager.UnloadObjects({ obj->GetDescriptor() });
            }
        }
        else
        {
            // unload(identifier)
            objectManager.UnloadObjects({ ObjectEntryDescriptor::Parse(szP1) });
        }
    }
    else if (JS_IsArray(argv[0]))
    {
        // unload(identifiers)
        JS_UNPACK_ARRAY(array, ctx, argv[0]);
        int64_t length;
        JS_GetLength(ctx, array, &length);

        std::vector<ObjectEntryDescriptor> descriptors;
        for (int64_t i = 0; i < length; i++)
        {
            JSValue item = JS_GetPropertyInt64(ctx, array, i);
            if (JS_IsString(item))
            {
                JS_UNPACK_STR(identifier, ctx, item);
                descriptors.push_back(ObjectEntryDescriptor::Parse(identifier));
            }
        }
        objectManager.UnloadObjects(descriptors);
    }
    auto intent = Intent(INTENT_ACTION_REFRESH_SCENERY);
    ContextBroadcastIntent(&intent);

    return JS_UNDEFINED;
}

JSValue ScObjectManager::getObject(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
{
    JS_UNPACK_STR(typez, ctx, argv[0]);
    JS_UNPACK_INT32(index, ctx, argv[1]);
    auto& objManager = GetContext()->GetObjectManager();

    auto type = objectTypeFromString(typez);
    if (type != ObjectType::none)
    {
        auto obj = objManager.GetLoadedObject(type, index);
        if (obj != nullptr)
        {
            return CreateScObject(ctx, type, index);
        }
    }
    else
    {
        return JS_ThrowPlainError(ctx, "Invalid object type.");
    }
    return JS_NULL;
}

JSValue ScObjectManager::getAllObjects(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
{
    JS_UNPACK_STR(typez, ctx, argv[0]);

    auto& objManager = GetContext()->GetObjectManager();
    auto type = objectTypeFromString(typez);

    if (type != ObjectType::none)
    {
        JSValue result = JS_NewArray(ctx);
        size_t count = getObjectEntryGroupCount(type);
        int64_t resultIndex = 0;
        for (size_t i = 0; i < count; i++)
        {
            auto obj = objManager.GetLoadedObject(type, i);
            if (obj != nullptr)
            {
                JSValue scObj = CreateScObject(ctx, type, static_cast<int32_t>(i));
                JS_SetPropertyInt64(ctx, result, resultIndex++, scObj);
            }
        }
        return result;
    }
    return JS_ThrowPlainError(ctx, "Invalid object type.");
}

void ScObjectManager::MarkAsResearched(const Object* object)
{
    // Defaults selected items to researched (if in-game)
    auto objectType = object->GetObjectType();
    auto entryIndex = ObjectManagerGetLoadedObjectEntryIndex(object);
    if (objectType == ObjectType::ride)
    {
        const auto* rideEntry = GetRideEntryByIndex(entryIndex);
        auto rideType = rideEntry->GetFirstNonNullRideType();
        auto category = static_cast<ResearchCategory>(GetRideTypeDescriptor(rideType).Category);
        ResearchInsertRideEntry(rideType, entryIndex, category, true);
    }
    else if (objectType == ObjectType::sceneryGroup)
    {
        ResearchInsertSceneryGroupEntry(entryIndex, true);
    }
}

void ScObjectManager::RefreshResearchedItems()
{
    // Same thing object selection window and inventions window does
    gSilentResearch = true;
    ResearchResetCurrentItem();
    gSilentResearch = false;
}

JSValue ScObjectManager::CreateScObject(JSContext* ctx, ObjectType type, int32_t index)
{
    switch (type)
    {
        case ObjectType::ride:
        {
            return ScRideObject::New(ctx, type, index);
        }
        case ObjectType::smallScenery:
        {
            return ScSmallSceneryObject::New(ctx, type, index);
        }
        case ObjectType::largeScenery:
        {
            return ScLargeSceneryObject::New(ctx, type, index);
        }
        case ObjectType::walls:
        {
            return ScWallObject::New(ctx, type, index);
        }
        case ObjectType::pathAdditions:
        {
            return ScFootpathAdditionObject::New(ctx, type, index);
        }
        case ObjectType::banners:
        {
            return ScBannerObject::New(ctx, type, index);
        }
        case ObjectType::sceneryGroup:
        {
            return ScSceneryGroupObject::New(ctx, type, index);
        }
        default:
        {
            return ScObject::New(ctx, type, index);
        }
    }
}

#endif
