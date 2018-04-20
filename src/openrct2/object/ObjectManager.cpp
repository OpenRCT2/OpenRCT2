#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <algorithm>
#include <array>
#include <memory>
#include <unordered_set>
#include "../Context.h"
#include "../core/Console.hpp"
#include "../core/Memory.hpp"
#include "../localisation/StringIds.h"
#include "FootpathItemObject.h"
#include "LargeSceneryObject.h"
#include "Object.h"
#include "ObjectManager.h"
#include "ObjectRepository.h"
#include "SceneryGroupObject.h"
#include "SmallSceneryObject.h"
#include "WallObject.h"

#include "ObjectList.h"

class ObjectManager final : public IObjectManager
{
private:
    std::shared_ptr<IObjectRepository> _objectRepository;
    std::vector<Object *> _loadedObjects;

public:
    explicit ObjectManager(std::shared_ptr<IObjectRepository> objectRepository)
        : _objectRepository(objectRepository)
    {
        Guard::ArgumentNotNull(objectRepository);

        _loadedObjects.resize(OBJECT_ENTRY_COUNT);

        UpdateSceneryGroupIndexes();
        reset_type_to_ride_entry_index_map();
    }

    ~ObjectManager() override
    {
        UnloadAll();
    }

    Object * GetLoadedObject(size_t index) override
    {
        if (index >= OBJECT_ENTRY_COUNT)
        {
#ifdef DEBUG
            log_warning("Object index %u exceeds maximum of %d.", index, OBJECT_ENTRY_COUNT);
#endif
            return nullptr;
        }
        if (index >= _loadedObjects.size())
        {
            return nullptr;
        }
        return _loadedObjects[index];
    }

    Object * GetLoadedObject(sint32 objectType, size_t index) override
    {
        if (index >= (size_t)object_entry_group_counts[objectType])
        {
#ifdef DEBUG
            log_warning("Object index %u exceeds maximum for type %d.", index, objectType);
#endif
            return nullptr;
        }

        size_t objectIndex = index;
        for (sint32 i = 0; i < objectType; i++)
        {
            objectIndex += object_entry_group_counts[i];
        }
        return GetLoadedObject(objectIndex);
    }

    Object * GetLoadedObject(const rct_object_entry * entry) override
    {
        Object * loadedObject = nullptr;
        const ObjectRepositoryItem * ori = _objectRepository->FindObject(entry);
        if (ori != nullptr)
        {
            loadedObject = ori->LoadedObject;
        }
        return loadedObject;
    }

    uint8 GetLoadedObjectEntryIndex(const Object * object) override
    {
        uint8 result = UINT8_MAX;
        size_t index = GetLoadedObjectIndex(object);
        if (index != SIZE_MAX)
        {
            get_type_entry_index(index, nullptr, &result);
        }
        return result;
    }

    Object * LoadObject(const rct_object_entry * entry) override
    {
        Object * loadedObject = nullptr;
        const ObjectRepositoryItem * ori = _objectRepository->FindObject(entry);
        if (ori != nullptr)
        {
            loadedObject = ori->LoadedObject;
            if (loadedObject == nullptr)
            {
                uint8 objectType = object_entry_get_type(entry);
                sint32 slot = FindSpareSlot(objectType);
                if (slot != -1)
                {
                    loadedObject = GetOrLoadObject(ori);
                    if (loadedObject != nullptr)
                    {
                        if (_loadedObjects.size() <= (size_t)slot)
                        {
                            _loadedObjects.resize(slot + 1);
                        }
                        _loadedObjects[slot] = loadedObject;
                        UpdateSceneryGroupIndexes();
                        reset_type_to_ride_entry_index_map();
                    }
                }
            }
        }
        return loadedObject;
    }

    bool LoadObjects(const rct_object_entry * entries, size_t count) override
    {
        // Find all the required objects
        bool missingObjects;
        auto requiredObjects = GetRequiredObjects(entries, &missingObjects);
        if (missingObjects)
        {
            return false;
        }

        // Create a new list of loaded objects
        size_t numNewLoadedObjects = 0;
        auto loadedObjects = LoadObjects(requiredObjects, &numNewLoadedObjects);

        if (!std::get<0>(loadedObjects))
        {
            UnloadAll();
            return false;
        }
        else
        {
            SetNewLoadedObjectList(std::get<1>(loadedObjects));
            UpdateSceneryGroupIndexes();
            reset_type_to_ride_entry_index_map();
            log_verbose("%u / %u new objects loaded", numNewLoadedObjects, requiredObjects.size());
            return true;
        }
    }

    void UnloadObjects(const rct_object_entry * entries, size_t count) override
    {
        // TODO there are two performance issues here:
        //        - FindObject for every entry which is a dictionary lookup
        //        - GetLoadedObjectIndex for every entry which enumerates _loadedList

        size_t numObjectsUnloaded = 0;
        for (size_t i = 0; i < count; i++)
        {
            const rct_object_entry * entry = &entries[i];
            const ObjectRepositoryItem * ori = _objectRepository->FindObject(entry);
            if (ori != nullptr)
            {
                Object * loadedObject = ori->LoadedObject;
                if (loadedObject != nullptr)
                {
                    UnloadObject(loadedObject);
                    numObjectsUnloaded++;
                }
            }
        }

        if (numObjectsUnloaded > 0)
        {
            UpdateSceneryGroupIndexes();
            reset_type_to_ride_entry_index_map();
        }
    }

    void UnloadAll() override
    {
        for (auto object : _loadedObjects)
        {
            UnloadObject(object);
        }
        UpdateSceneryGroupIndexes();
        reset_type_to_ride_entry_index_map();
    }

    void ResetObjects() override
    {
        for (auto loadedObject : _loadedObjects)
        {
            if (loadedObject != nullptr)
            {
                loadedObject->Unload();
                loadedObject->Load();
            }
        }
        UpdateSceneryGroupIndexes();
        reset_type_to_ride_entry_index_map();
    }

    std::vector<const ObjectRepositoryItem *> GetPackableObjects() override
    {
        std::vector<const ObjectRepositoryItem *> objects;
        size_t numObjects = _objectRepository->GetNumObjects();
        for (size_t i = 0; i < numObjects; i++)
        {
            const ObjectRepositoryItem * item = &_objectRepository->GetObjects()[i];
            if (item->LoadedObject != nullptr &&
                item->LoadedObject->GetLegacyData() != nullptr &&
                IsObjectCustom(item))
            {
                objects.push_back(item);
            }
        }
        return objects;
    }

    static rct_string_id GetObjectSourceGameString(const rct_object_entry * entry)
    {
        switch (object_entry_get_source_game(entry))
        {
        case OBJECT_SOURCE_RCT1:
            return STR_SCENARIO_CATEGORY_RCT1;
        case OBJECT_SOURCE_ADDED_ATTRACTIONS:
            return STR_SCENARIO_CATEGORY_RCT1_AA;
        case OBJECT_SOURCE_LOOPY_LANDSCAPES:
            return STR_SCENARIO_CATEGORY_RCT1_LL;
        case OBJECT_SOURCE_RCT2:
            return STR_ROLLERCOASTER_TYCOON_2_DROPDOWN;
        case OBJECT_SOURCE_WACKY_WORLDS:
            return STR_OBJECT_FILTER_WW;
        case OBJECT_SOURCE_TIME_TWISTER:
            return STR_OBJECT_FILTER_TT;
        case OBJECT_SOURCE_OPENRCT2_OFFICIAL:
            return STR_OBJECT_FILTER_OPENRCT2_OFFICIAL;
        default:
            return STR_OBJECT_FILTER_CUSTOM;
        }
    }

private:
    sint32 FindSpareSlot(uint8 objectType)
    {
        sint32 firstIndex = GetIndexFromTypeEntry(objectType, 0);
        sint32 endIndex = firstIndex + object_entry_group_counts[objectType];
        for (sint32 i = firstIndex; i < endIndex; i++)
        {
            if (_loadedObjects.size() > (size_t)i && _loadedObjects[i] == nullptr)
            {
                return i;
            }
        }
        return -1;
    }

    size_t GetLoadedObjectIndex(const Object * object)
    {
        Guard::ArgumentNotNull(object, GUARD_LINE);

        auto result = std::numeric_limits<size_t>().max();
        auto it = std::find(_loadedObjects.begin(), _loadedObjects.end(), object);
        if (it != _loadedObjects.end())
        {
            result = std::distance(_loadedObjects.begin(), it);
        }
        return result;
    }

    void SetNewLoadedObjectList(const std::vector<Object *> &newLoadedObjects)
    {
        if (newLoadedObjects.size() == 0)
        {
            UnloadAll();
        }
        else
        {
            UnloadObjectsExcept(newLoadedObjects);
        }
        _loadedObjects = newLoadedObjects;
    }

    void UnloadObject(Object * object)
    {
        if (object != nullptr)
        {
            // TODO try to prevent doing a repository search
            const ObjectRepositoryItem * ori = _objectRepository->FindObject(object->GetObjectEntry());
            if (ori != nullptr)
            {
                _objectRepository->UnregisterLoadedObject(ori, object);
            }

            // Because it's possible to have the same loaded object for multiple
            // slots, we have to make sure find and set all of them to nullptr
            for (auto &obj : _loadedObjects)
            {
                if (obj == object)
                {
                    obj = nullptr;
                }
            }

            object->Unload();
            delete object;
        }
    }

    void UnloadObjectsExcept(const std::vector<Object *> &newLoadedObjects)
    {
        // Build a hash set for quick checking
        auto exceptSet = std::unordered_set<Object *>();
        for (auto object : newLoadedObjects)
        {
            if (object != nullptr)
            {
                exceptSet.insert(object);
            }
        }

        // Unload objects that are not in the hash set
        size_t totalObjectsLoaded = 0;
        size_t numObjectsUnloaded = 0;
        for (auto object : _loadedObjects)
        {
            if (object != nullptr)
            {
                totalObjectsLoaded++;
                if (exceptSet.find(object) == exceptSet.end())
                {
                    UnloadObject(object);
                    numObjectsUnloaded++;
                }
            }
        }

        log_verbose("%u / %u objects unloaded", numObjectsUnloaded, totalObjectsLoaded);
    }

    void UpdateSceneryGroupIndexes()
    {
        for (auto loadedObject : _loadedObjects)
        {
            if (loadedObject != nullptr)
            {
                rct_scenery_entry * sceneryEntry;
                switch (loadedObject->GetObjectType()) {
                case OBJECT_TYPE_SMALL_SCENERY:
                    sceneryEntry = (rct_scenery_entry *)loadedObject->GetLegacyData();
                    sceneryEntry->small_scenery.scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                    break;
                case OBJECT_TYPE_LARGE_SCENERY:
                    sceneryEntry = (rct_scenery_entry *)loadedObject->GetLegacyData();
                    sceneryEntry->large_scenery.scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                    break;
                case OBJECT_TYPE_WALLS:
                    sceneryEntry = (rct_scenery_entry *)loadedObject->GetLegacyData();
                    sceneryEntry->wall.scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                    break;
                case OBJECT_TYPE_BANNERS:
                    sceneryEntry = (rct_scenery_entry *)loadedObject->GetLegacyData();
                    sceneryEntry->banner.scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                    break;
                case OBJECT_TYPE_PATH_BITS:
                    sceneryEntry = (rct_scenery_entry *)loadedObject->GetLegacyData();
                    sceneryEntry->path_bit.scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                    break;
                case OBJECT_TYPE_SCENERY_GROUP:
                    auto sgObject = dynamic_cast<SceneryGroupObject *>(loadedObject);
                    sgObject->UpdateEntryIndexes();
                    break;
                }
            }
        }

        // HACK Scenery window will lose its tabs after changing the scenery group indexing
        //      for now just close it, but it will be better to later tell it to invalidate the tabs
        window_close_by_class(WC_SCENERY);
    }

    uint8 GetPrimarySceneryGroupEntryIndex(Object * loadedObject)
    {
        auto sceneryObject = dynamic_cast<SceneryObject *>(loadedObject);
        const rct_object_entry * primarySGEntry = sceneryObject->GetPrimarySceneryGroup();
        Object * sgObject = GetLoadedObject(primarySGEntry);

        uint8 entryIndex = 255;
        if (sgObject != nullptr)
        {
            entryIndex = GetLoadedObjectEntryIndex(sgObject);
        }
        return entryIndex;
    }

    rct_object_entry* DuplicateObjectEntry(const rct_object_entry* original)
    {
        rct_object_entry * duplicate = Memory::Allocate<rct_object_entry>(sizeof(rct_object_entry));
        duplicate->checksum = original->checksum;
        strncpy(duplicate->name, original->name, 8);
        duplicate->flags = original->flags;
        return duplicate;
    }

    std::vector<rct_object_entry> GetInvalidObjects(const rct_object_entry * entries) override
    {
        std::vector<rct_object_entry> invalidEntries;
        invalidEntries.reserve(OBJECT_ENTRY_COUNT);
        for (sint32 i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            auto entry = entries[i];
            const ObjectRepositoryItem * ori = nullptr;
            if (object_entry_is_empty(&entry))
            {
                entry = { 0 };
                continue;
            }

            ori = _objectRepository->FindObject(&entry);
            if (ori == nullptr)
            {
                if (object_entry_get_type(&entry) != OBJECT_TYPE_SCENARIO_TEXT)
                {
                    invalidEntries.push_back(entry);
                    ReportMissingObject(&entry);
                }
                else
                {
                    log_info("Ignoring missing STEX entry.");
                    entry = { 0 };
                    continue;
                }
            }
            else
            {
                Object * loadedObject = nullptr;
                loadedObject = ori->LoadedObject;
                if (loadedObject == nullptr)
                {
                    loadedObject = _objectRepository->LoadObject(ori);
                    if (loadedObject == nullptr)
                    {
                        invalidEntries.push_back(entry);
                        ReportObjectLoadProblem(&entry);
                    }
                    delete loadedObject;
                }
            }
        }
        return invalidEntries;
    }

    std::vector<const ObjectRepositoryItem *> GetRequiredObjects(const rct_object_entry * entries, bool * missingObjects)
    {
        std::vector<const ObjectRepositoryItem *> requiredObjects;
        *missingObjects = false;
        for (sint32 i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            const rct_object_entry * entry = &entries[i];
            const ObjectRepositoryItem * ori = nullptr;
            if (!object_entry_is_empty(entry))
            {
                ori = _objectRepository->FindObject(entry);
                if (ori == nullptr && object_entry_get_type(entry) != OBJECT_TYPE_SCENARIO_TEXT)
                {
                    *missingObjects = true;
                    ReportMissingObject(entry);
                }
            }
            requiredObjects.push_back(ori);
        }
        return requiredObjects;
    }

    std::pair<bool, std::vector<Object *>> LoadObjects(std::vector<const ObjectRepositoryItem *> &requiredObjects, size_t * outNewObjectsLoaded)
    {
        size_t newObjectsLoaded = 0;
        std::vector<Object *> loadedObjects;
        loadedObjects.reserve(OBJECT_ENTRY_COUNT);
        for (auto ori : requiredObjects)
        {
            Object * loadedObject = nullptr;
            if (ori != nullptr)
            {
                loadedObject = ori->LoadedObject;
                if (loadedObject == nullptr)
                {
                    loadedObject = GetOrLoadObject(ori);
                    if (loadedObject == nullptr)
                    {
                        ReportObjectLoadProblem(&ori->ObjectEntry);
                        return std::make_pair(false, std::vector<Object *>());
                    } else {
                        newObjectsLoaded++;
                    }
                }
            }
            loadedObjects.push_back(loadedObject);
        }
        if (outNewObjectsLoaded != nullptr)
        {
            *outNewObjectsLoaded = newObjectsLoaded;
        }
        return std::make_pair(true, loadedObjects);
    }

    Object * GetOrLoadObject(const ObjectRepositoryItem * ori)
    {
        Object * loadedObject = ori->LoadedObject;
        if (loadedObject == nullptr)
        {
            // Try to load object
            loadedObject = _objectRepository->LoadObject(ori);
            if (loadedObject != nullptr)
            {
                loadedObject->Load();

                // Connect the ori to the registered object
                _objectRepository->RegisterLoadedObject(ori, loadedObject);
            }
        }
        return loadedObject;
    }

    static void ReportMissingObject(const rct_object_entry * entry)
    {
        utf8 objName[DAT_NAME_LENGTH + 1] = { 0 };
        std::copy_n(entry->name, DAT_NAME_LENGTH, objName);
        Console::Error::WriteLine("[%s] Object not found.", objName);
    }

    void ReportObjectLoadProblem(const rct_object_entry * entry)
    {
        utf8 objName[DAT_NAME_LENGTH + 1] = { 0 };
        std::copy_n(entry->name, DAT_NAME_LENGTH, objName);
        Console::Error::WriteLine("[%s] Object could not be loaded.", objName);
    }

    static sint32 GetIndexFromTypeEntry(uint8 objectType, uint8 entryIndex)
    {
        sint32 result = 0;
        for (uint8 i = 0; i < objectType; i++)
        {
            result += object_entry_group_counts[i];
        }
        result += entryIndex;
        return result;
    }
};

std::unique_ptr<IObjectManager> CreateObjectManager(std::shared_ptr<IObjectRepository> objectRepository)
{
    return std::make_unique<ObjectManager>(objectRepository);
}

void * object_manager_get_loaded_object_by_index(size_t index)
{
    auto objectManager = OpenRCT2::GetContext()->GetObjectManager();
    Object * loadedObject = objectManager->GetLoadedObject(index);
    return (void *)loadedObject;
}

void * object_manager_get_loaded_object(const rct_object_entry * entry)
{
    auto objectManager = OpenRCT2::GetContext()->GetObjectManager();
    Object * loadedObject = objectManager->GetLoadedObject(entry);
    return (void *)loadedObject;
}

uint8 object_manager_get_loaded_object_entry_index(const void * loadedObject)
{
    auto objectManager = OpenRCT2::GetContext()->GetObjectManager();
    const Object * object = static_cast<const Object *>(loadedObject);
    uint8 entryIndex = objectManager->GetLoadedObjectEntryIndex(object);
    return entryIndex;
}

void * object_manager_load_object(const rct_object_entry * entry)
{
    auto objectManager = OpenRCT2::GetContext()->GetObjectManager();
    Object * loadedObject = objectManager->LoadObject(entry);
    return (void *)loadedObject;
}

void object_manager_unload_objects(const rct_object_entry * entries, size_t count)
{
    auto objectManager = OpenRCT2::GetContext()->GetObjectManager();
    objectManager->UnloadObjects(entries, count);
}

void object_manager_unload_all_objects()
{
    auto objectManager = OpenRCT2::GetContext()->GetObjectManager();
    if (objectManager != nullptr)
    {
        objectManager->UnloadAll();
    }
}

rct_string_id object_manager_get_source_game_string(const rct_object_entry * entry)
{
    return ObjectManager::GetObjectSourceGameString(entry);
}
