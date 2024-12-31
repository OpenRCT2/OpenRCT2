/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScObjectManager.h"

    #include "../../../object/ObjectList.h"
    #include "../../../ride/RideData.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

void ScObjectManager::Register(duk_context* ctx)
{
    dukglue_register_property(ctx, &ScObjectManager::installedObjects_get, nullptr, "installedObjects");
    dukglue_register_method(ctx, &ScObjectManager::installedObject_get, "getInstalledObject");
    dukglue_register_method(ctx, &ScObjectManager::load, "load");
    dukglue_register_method(ctx, &ScObjectManager::unload, "unload");
    dukglue_register_method(ctx, &ScObjectManager::getObject, "getObject");
    dukglue_register_method(ctx, &ScObjectManager::getAllObjects, "getAllObjects");
}

std::vector<std::shared_ptr<ScInstalledObject>> ScObjectManager::installedObjects_get() const
{
    std::vector<std::shared_ptr<ScInstalledObject>> result;

    auto context = GetContext();
    auto& objectManager = context->GetObjectRepository();
    auto count = objectManager.GetNumObjects();
    for (size_t i = 0; i < count; i++)
    {
        auto installedObject = std::make_shared<ScInstalledObject>(i);
        result.push_back(installedObject);
    }

    return result;
}

std::shared_ptr<ScInstalledObject> ScObjectManager::installedObject_get(const std::string& identifier) const
{
    auto context = GetContext();
    auto& objectRepository = context->GetObjectRepository();
    auto object = objectRepository.FindObject(identifier);
    return object != nullptr ? std::make_shared<ScInstalledObject>(object->Id) : nullptr;
}

DukValue ScObjectManager::load(const DukValue& p1, const DukValue& p2)
{
    auto context = GetContext();
    auto& scriptEngine = context->GetScriptEngine();
    auto& objectRepository = context->GetObjectRepository();
    auto& objectManager = context->GetObjectManager();
    auto ctx = scriptEngine.GetContext();

    if (p1.is_array())
    {
        // load(identifiers)
        std::vector<ObjectEntryDescriptor> descriptors;
        for (const auto& item : p1.as_array())
        {
            if (item.type() != DukValue::STRING)
                throw DukException() << "Expected string for 'identifier'.";

            const auto& identifier = item.as_string();
            descriptors.push_back(ObjectEntryDescriptor::Parse(identifier));
        }

        duk_push_array(ctx);
        duk_uarridx_t index = 0;
        for (const auto& descriptor : descriptors)
        {
            auto obj = objectManager.LoadObject(descriptor);
            if (obj != nullptr)
            {
                MarkAsResearched(obj);
                auto objIndex = objectManager.GetLoadedObjectEntryIndex(obj);
                auto scLoadedObject = CreateScObject(scriptEngine.GetContext(), obj->GetObjectType(), objIndex);
                scLoadedObject.push();
                duk_put_prop_index(ctx, -2, index);
            }
            else
            {
                duk_push_null(ctx);
                duk_put_prop_index(ctx, -2, index);
            }
            index++;
        }
        RefreshResearchedItems();
        return DukValue::take_from_stack(ctx);
    }
    else
    {
        // load(identifier, index?)
        if (p1.type() != DukValue::STRING)
            throw DukException() << "Expected string for 'identifier'.";

        const auto& identifier = p1.as_string();
        auto descriptor = ObjectEntryDescriptor::Parse(identifier);

        auto installedObject = objectRepository.FindObject(descriptor);
        if (installedObject != nullptr)
        {
            if (p2.type() != DukValue::UNDEFINED)
            {
                if (p2.type() != DukValue::NUMBER)
                    throw DukException() << "Expected number for 'index'.";

                auto index = static_cast<size_t>(p2.as_uint());
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
                        return CreateScObject(scriptEngine.GetContext(), obj->GetObjectType(), objIndex);
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
                    return CreateScObject(scriptEngine.GetContext(), obj->GetObjectType(), objIndex);
                }
            }
        }
    }
    return ToDuk(ctx, nullptr);
}

void ScObjectManager::unload(const DukValue& p1, const DukValue& p2)
{
    auto context = GetContext();
    auto& objectManager = context->GetObjectManager();

    if (p1.type() == DukValue::STRING)
    {
        const auto& szP1 = p1.as_string();
        auto objType = ScObject::StringToObjectType(szP1);
        if (objType)
        {
            // unload(type, index)
            if (p2.type() != DukValue::NUMBER)
                throw DukException() << "'index' is invalid.";

            auto objIndex = p2.as_uint();
            auto obj = objectManager.GetLoadedObject(*objType, objIndex);
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
    else if (p1.is_array())
    {
        // unload(identifiers)
        auto identifiers = p1.as_array();
        std::vector<ObjectEntryDescriptor> descriptors;
        for (const auto& identifier : identifiers)
        {
            if (identifier.type() == DukValue::STRING)
            {
                descriptors.push_back(ObjectEntryDescriptor::Parse(identifier.as_string()));
            }
        }
        objectManager.UnloadObjects(descriptors);
    }
}

DukValue ScObjectManager::getObject(const std::string& typez, int32_t index) const
{
    auto ctx = GetContext()->GetScriptEngine().GetContext();
    auto& objManager = GetContext()->GetObjectManager();

    auto type = ScObject::StringToObjectType(typez);
    if (type)
    {
        auto obj = objManager.GetLoadedObject(*type, index);
        if (obj != nullptr)
        {
            return CreateScObject(ctx, *type, index);
        }
    }
    else
    {
        duk_error(ctx, DUK_ERR_ERROR, "Invalid object type.");
    }
    return ToDuk(ctx, nullptr);
}

std::vector<DukValue> ScObjectManager::getAllObjects(const std::string& typez) const
{
    auto ctx = GetContext()->GetScriptEngine().GetContext();
    auto& objManager = GetContext()->GetObjectManager();

    std::vector<DukValue> result;
    auto type = ScObject::StringToObjectType(typez);
    if (type)
    {
        auto count = getObjectEntryGroupCount(*type);
        for (auto i = 0u; i < count; i++)
        {
            auto obj = objManager.GetLoadedObject(*type, i);
            if (obj != nullptr)
            {
                result.push_back(CreateScObject(ctx, *type, i));
            }
        }
    }
    else
    {
        duk_error(ctx, DUK_ERR_ERROR, "Invalid object type.");
    }
    return result;
}

void ScObjectManager::MarkAsResearched(const Object* object)
{
    // Defaults selected items to researched (if in-game)
    auto objectType = object->GetObjectType();
    auto entryIndex = ObjectManagerGetLoadedObjectEntryIndex(object);
    if (objectType == ObjectType::Ride)
    {
        const auto* rideEntry = GetRideEntryByIndex(entryIndex);
        auto rideType = rideEntry->GetFirstNonNullRideType();
        auto category = static_cast<ResearchCategory>(GetRideTypeDescriptor(rideType).Category);
        ResearchInsertRideEntry(rideType, entryIndex, category, true);
    }
    else if (objectType == ObjectType::SceneryGroup)
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

DukValue ScObjectManager::CreateScObject(duk_context* ctx, ObjectType type, int32_t index)
{
    switch (type)
    {
        case ObjectType::Ride:
            return GetObjectAsDukValue(ctx, std::make_shared<ScRideObject>(type, index));
        case ObjectType::SmallScenery:
            return GetObjectAsDukValue(ctx, std::make_shared<ScSmallSceneryObject>(type, index));
        case ObjectType::LargeScenery:
            return GetObjectAsDukValue(ctx, std::make_shared<ScLargeSceneryObject>(type, index));
        case ObjectType::Walls:
            return GetObjectAsDukValue(ctx, std::make_shared<ScWallObject>(type, index));
        case ObjectType::PathAdditions:
            return GetObjectAsDukValue(ctx, std::make_shared<ScFootpathAdditionObject>(type, index));
        case ObjectType::Banners:
            return GetObjectAsDukValue(ctx, std::make_shared<ScBannerObject>(type, index));
        case ObjectType::SceneryGroup:
            return GetObjectAsDukValue(ctx, std::make_shared<ScSceneryGroupObject>(type, index));
        default:
            return GetObjectAsDukValue(ctx, std::make_shared<ScObject>(type, index));
    }
}

#endif
