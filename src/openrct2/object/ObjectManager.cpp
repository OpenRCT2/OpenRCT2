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

#include <array>
#include <memory>
#include <unordered_set>
#include "../core/Console.hpp"
#include "../core/Memory.hpp"
#include "../localisation/string_ids.h"
#include "FootpathItemObject.h"
#include "LargeSceneryObject.h"
#include "Object.h"
#include "ObjectManager.h"
#include "ObjectRepository.h"
#include "SceneryGroupObject.h"
#include "SmallSceneryObject.h"
#include "WallObject.h"

extern "C"
{
    #include "../object_list.h"
}

class ObjectManager final : public IObjectManager
{
private:
    IObjectRepository *         _objectRepository;
    Object * *                  _loadedObjects = nullptr;

public:
    explicit ObjectManager(IObjectRepository * objectRepository)
    {
        Guard::ArgumentNotNull(objectRepository);

        _objectRepository = objectRepository;
        _loadedObjects = Memory::AllocateArray<Object *>(OBJECT_ENTRY_COUNT);
        for (size_t i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            _loadedObjects[i] = nullptr;
        }

        UpdateLegacyLoadedObjectList();
        UpdateSceneryGroupIndexes();
        reset_type_to_ride_entry_index_map();
    }

    ~ObjectManager() override
    {
        SetNewLoadedObjectList(nullptr);
    }

    Object * GetLoadedObject(size_t index) override
    {
        if (_loadedObjects == nullptr)
        {
            return nullptr;
        }
        return _loadedObjects[index];
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
                uint8 objectType = entry->flags & 0x0F;
                sint32 slot = FindSpareSlot(objectType);
                if (slot != -1)
                {
                    loadedObject = GetOrLoadObject(ori);
                    if (loadedObject != nullptr)
                    {
                        _loadedObjects[slot] = loadedObject;
                        UpdateLegacyLoadedObjectList();
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
        size_t numRequiredObjects;
        auto requiredObjects = new const ObjectRepositoryItem *[OBJECT_ENTRY_COUNT];
        if (!GetRequiredObjects(entries, requiredObjects, &numRequiredObjects))
        {
            delete[] requiredObjects;
            return false;
        }

        // Create a new list of loaded objects
        size_t numNewLoadedObjects;
        Object * * loadedObjects = LoadObjects(requiredObjects, &numNewLoadedObjects);

        delete[] requiredObjects;

        if (loadedObjects == nullptr)
        {
            UnloadAll();
            return false;
        }
        else
        {
            SetNewLoadedObjectList(loadedObjects);
            UpdateLegacyLoadedObjectList();
            UpdateSceneryGroupIndexes();
            reset_type_to_ride_entry_index_map();
            log_verbose("%u / %u new objects loaded", numNewLoadedObjects, numRequiredObjects);
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
            UpdateLegacyLoadedObjectList();
            UpdateSceneryGroupIndexes();
            reset_type_to_ride_entry_index_map();
        }
    }

    void UnloadAll() override
    {
        if (_loadedObjects != nullptr)
        {
            for (sint32 i = 0; i < OBJECT_ENTRY_COUNT; i++)
            {
                UnloadObject(_loadedObjects[i]);
            }
        }
        UpdateLegacyLoadedObjectList();
        UpdateSceneryGroupIndexes();
        reset_type_to_ride_entry_index_map();
    }

    void ResetObjects() override
    {
        if (_loadedObjects != nullptr)
        {
            for (size_t i = 0; i < OBJECT_ENTRY_COUNT; i++)
            {
                Object * loadedObject = _loadedObjects[i];
                if (loadedObject != nullptr)
                {
                    loadedObject->Unload();
                    loadedObject->Load();
                }
            }
            UpdateLegacyLoadedObjectList();
            UpdateSceneryGroupIndexes();
            reset_type_to_ride_entry_index_map();
        }
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
        uint8 source = (entry->flags & 0xF0) >> 4;
        switch (source)
        {
        case OBJECT_SOURCE_RCT2:
            return STR_ROLLERCOASTER_TYCOON_2_DROPDOWN;
        case OBJECT_SOURCE_WACKY_WORLDS:
            return STR_OBJECT_FILTER_WW;
        case OBJECT_SOURCE_TIME_TWISTER:
            return STR_OBJECT_FILTER_TT;
        default:
            return STR_OBJECT_FILTER_CUSTOM;
        }
    }

private:
    sint32 FindSpareSlot(uint8 objectType)
    {
        if (_loadedObjects != nullptr)
        {
            sint32 firstIndex = GetIndexFromTypeEntry(objectType, 0);
            sint32 endIndex = firstIndex + object_entry_group_counts[objectType];
            for (sint32 i = firstIndex; i < endIndex; i++)
            {
                if (_loadedObjects[i] == nullptr)
                {
                    return i;
                }
            }
        }
        return -1;
    }

    size_t GetLoadedObjectIndex(const Object * object)
    {
        Guard::ArgumentNotNull(object, GUARD_LINE);

        size_t result = SIZE_MAX;
        if (_loadedObjects != nullptr)
        {
            for (size_t i = 0; i < OBJECT_ENTRY_COUNT; i++)
            {
                if (_loadedObjects[i] == object)
                {
                    result = i;
                    break;
                }
            }
        }
        return result;
    }

    void SetNewLoadedObjectList(Object * * newLoadedObjects)
    {
        if (newLoadedObjects == nullptr)
        {
            UnloadAll();
        }
        else
        {
            UnloadObjectsExcept(newLoadedObjects);
        }
        Memory::Free(_loadedObjects);
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

            object->Unload();
            delete object;

            // Because it's possible to have the same loaded object for multiple
            // slots, we have to make sure find and set all of them to nullptr
            if (_loadedObjects != nullptr)
            {
                for (size_t i = 0; i < OBJECT_ENTRY_COUNT; i++)
                {
                    if (_loadedObjects[i] == object)
                    {
                        _loadedObjects[i] = nullptr;
                    }
                }
            }
        }
    }

    void UnloadObjectsExcept(Object * * newLoadedObjects)
    {
        if (_loadedObjects == nullptr)
        {
            return;
        }

        // Build a hash set for quick checking
        auto exceptSet = std::unordered_set<Object *>();
        for (sint32 i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            Object * object = newLoadedObjects[i];
            if (object != nullptr)
            {
                exceptSet.insert(object);
            }
        }

        // Unload objects that are not in the hash set
        size_t totalObjectsLoaded = 0;
        size_t numObjectsUnloaded = 0;
        for (sint32 i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            Object * object = _loadedObjects[i];
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

    void UpdateLegacyLoadedObjectList()
    {
        for (sint32 i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            Object * loadedObject = nullptr;
            if (_loadedObjects != nullptr)
            {
                loadedObject = _loadedObjects[i];
            }

            uint8 objectType, entryIndex;
            get_type_entry_index(i, &objectType, &entryIndex);

            rct_object_entry_extended * legacyEntry = &object_entry_groups[objectType].entries[entryIndex];
            void * * legacyChunk = &object_entry_groups[objectType].chunks[entryIndex];
            if (loadedObject == nullptr)
            {
                Memory::Set(legacyEntry, 0xFF, sizeof(rct_object_entry_extended));
                *legacyChunk = (void *)-1;
            }
            else
            {
                legacyEntry->entry = *loadedObject->GetObjectEntry();
                legacyEntry->chunk_size = 0;
                *legacyChunk = loadedObject->GetLegacyData();
            }
        }
    }

    void UpdateSceneryGroupIndexes()
    {
        if (_loadedObjects != nullptr)
        {
            for (size_t i = 0; i < OBJECT_ENTRY_COUNT; i++)
            {
                Object * loadedObject = _loadedObjects[i];
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
                    case OBJECT_TYPE_SCENERY_SETS:
                        auto sgObject = static_cast<SceneryGroupObject *>(loadedObject);
                        sgObject->UpdateEntryIndexes();
                        break;
                    }
                }
            }

            // HACK Scenery window will lose its tabs after changing the scenery group indexing
            //      for now just close it, but it will be better to later tell it to invalidate the tabs
            window_close_by_class(WC_SCENERY);
        }
    }

    uint8 GetPrimarySceneryGroupEntryIndex(Object * loadedObject)
    {
        auto sceneryObject = static_cast<SceneryObject *>(loadedObject);
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
            const rct_object_entry * entry = &entries[i];
            const ObjectRepositoryItem * ori = nullptr;
            if (!object_entry_is_empty(entry))
            {
                ori = _objectRepository->FindObject(entry);
                if (ori == nullptr)
                {
                    invalidEntries.push_back(*entry);
                    ReportMissingObject(entry);
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
                            invalidEntries.push_back(*entry);
                            ReportObjectLoadProblem(entry);
                        }
                        delete loadedObject;
                    }
                }
            }
        }
        return invalidEntries;
    }

    bool GetRequiredObjects(const rct_object_entry * entries,
                            const ObjectRepositoryItem * * requiredObjects,
                            size_t * outNumRequiredObjects)
    {
        bool missingObjects = false;
        size_t numRequiredObjects = 0;
        for (sint32 i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            const rct_object_entry * entry = &entries[i];
            const ObjectRepositoryItem * ori = nullptr;
            if (!object_entry_is_empty(entry))
            {
                ori = _objectRepository->FindObject(entry);
                if (ori == nullptr)
                {
                    missingObjects = true;
                    ReportMissingObject(entry);
                }
                numRequiredObjects++;
            }
            requiredObjects[i] = ori;
        }

        if (outNumRequiredObjects != nullptr)
        {
            *outNumRequiredObjects = numRequiredObjects;
        }
        return !missingObjects;
    }

    Object * * LoadObjects(const ObjectRepositoryItem * * requiredObjects, size_t * outNewObjectsLoaded)
    {
        size_t newObjectsLoaded = 0;
        Object * * loadedObjects = Memory::AllocateArray<Object *>(OBJECT_ENTRY_COUNT);
        for (sint32 i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            Object * loadedObject = nullptr;
            const ObjectRepositoryItem * ori = requiredObjects[i];
            if (ori != nullptr)
            {
                loadedObject = ori->LoadedObject;
                if (loadedObject == nullptr)
                {
                    loadedObject = GetOrLoadObject(ori);
                    if (loadedObject == nullptr)
                    {
                        ReportObjectLoadProblem(&ori->ObjectEntry);
                        Memory::Free(loadedObjects);
                        return nullptr;
                    } else {
                        newObjectsLoaded++;
                    }
                }
            }
            loadedObjects[i] = loadedObject;
        }
        if (outNewObjectsLoaded != nullptr)
        {
            *outNewObjectsLoaded = newObjectsLoaded;
        }
        return loadedObjects;
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
        utf8 objName[9] = { 0 };
        Memory::Copy(objName, entry->name, 8);
        Console::Error::WriteLine("[%s] Object not found.", objName);
    }

    void ReportObjectLoadProblem(const rct_object_entry * entry)
    {
        utf8 objName[9] = { 0 };
        Memory::Copy(objName, entry->name, 8);
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

static ObjectManager * _objectManager = nullptr;

IObjectManager * CreateObjectManager(IObjectRepository * objectRepository)
{
    _objectManager = new ObjectManager(objectRepository);
    return _objectManager;
}

IObjectManager * GetObjectManager()
{
    return _objectManager;
}

extern "C"
{
    void * object_manager_get_loaded_object_by_index(size_t index)
    {
        IObjectManager * objectManager = GetObjectManager();
        Object * loadedObject = objectManager->GetLoadedObject(index);
        return (void *)loadedObject;
    }

    void * object_manager_get_loaded_object(const rct_object_entry * entry)
    {
        IObjectManager * objectManager = GetObjectManager();
        Object * loadedObject = objectManager->GetLoadedObject(entry);
        return (void *)loadedObject;
    }

    uint8 object_manager_get_loaded_object_entry_index(const void * loadedObject)
    {
        IObjectManager * objectManager = GetObjectManager();
        const Object * object = static_cast<const Object *>(loadedObject);
        uint8 entryIndex = objectManager->GetLoadedObjectEntryIndex(object);
        return entryIndex;
    }

    void * object_manager_load_object(const rct_object_entry * entry)
    {
        IObjectManager * objectManager = GetObjectManager();
        Object * loadedObject = objectManager->LoadObject(entry);
        return (void *)loadedObject;
    }

    void object_manager_unload_objects(const rct_object_entry * entries, size_t count)
    {
        IObjectManager * objectManager = GetObjectManager();
        objectManager->UnloadObjects(entries, count);
    }

    void object_manager_unload_all_objects()
    {
        IObjectManager * objectManager = GetObjectManager();
        if (objectManager != nullptr)
        {
            objectManager->UnloadAll();
        }
    }

    rct_string_id object_manager_get_source_game_string(const rct_object_entry * entry)
    {
        return ObjectManager::GetObjectSourceGameString(entry);
    }
}
