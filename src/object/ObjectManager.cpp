#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
#include <unordered_set>
#include "../core/Console.hpp"
#include "../core/Memory.hpp"
#include "Object.h"
#include "ObjectManager.h"
#include "ObjectRepository.h"

extern "C"
{
    #include "../object_list.h"
}

class ObjectManager : public IObjectManager
{
private:
    IObjectRepository * _objectRepository;
    Object * *          _loadedObjects = nullptr;

public:
    ObjectManager(IObjectRepository * objectRepository)
    {
        _objectRepository = objectRepository;
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
                        reset_type_to_ride_entry_index_map();
                    }
                }
            }
        }
        return loadedObject;
    }

    bool LoadObjects(const rct_object_entry * entries, size_t count) override
    {
        IObjectRepository * objectRepository = GetObjectRepository();

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
            reset_type_to_ride_entry_index_map();
            // Console::WriteLine("%u / %u new objects loaded", numNewLoadedObjects, numRequiredObjects);
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
                size_t index = GetLoadedObjectIndex(loadedObject);
                
                UnloadObject(loadedObject);
                _loadedObjects[index] = nullptr;

                numObjectsUnloaded++;
            }
        }

        if (numObjectsUnloaded > 0)
        {
            UpdateLegacyLoadedObjectList();
            reset_type_to_ride_entry_index_map();
        }
    }

    void UnloadAll() override
    {
        if (_loadedObjects != nullptr)
        {
            for (int i = 0; i < OBJECT_ENTRY_COUNT; i++)
            {
                UnloadObject(_loadedObjects[i]);
                _loadedObjects[i] = nullptr;
            }
        }
        UpdateLegacyLoadedObjectList();
        reset_type_to_ride_entry_index_map();
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
        for (int i = 0; i < OBJECT_ENTRY_COUNT; i++)
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
        for (int i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            Object * object = _loadedObjects[i];
            if (object != nullptr)
            {
                totalObjectsLoaded++;
                if (exceptSet.find(object) == exceptSet.end())
                {
                    UnloadObject(object);
                    _loadedObjects[i] = nullptr;
                    numObjectsUnloaded++;
                }
            }
        }

        // Console::WriteLine("%u / %u objects unloaded", numObjectsUnloaded, totalObjectsLoaded);
    }

    void UpdateLegacyLoadedObjectList()
    {
        for (int i = 0; i < OBJECT_ENTRY_COUNT; i++)
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

    bool GetRequiredObjects(const rct_object_entry * entries,
                            const ObjectRepositoryItem * * requiredObjects,
                            size_t * outNumRequiredObjects)
    {
        bool missingObjects = false;
        size_t numRequiredObjects = 0;
        for (int i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            const rct_object_entry * entry = &entries[i];
            const ObjectRepositoryItem * ori = nullptr;
            if (check_object_entry(entry))
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
        for (int i = 0; i < OBJECT_ENTRY_COUNT; i++)
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
        Console::Error::WriteFormat("[%s]: Object not found.", objName);
        Console::Error::WriteLine();
    }

    static void ReportObjectLoadProblem(const rct_object_entry * entry)
    {
        utf8 objName[9] = { 0 };
        Memory::Copy(objName, entry->name, 8);
        Console::Error::WriteFormat("[%s]: Object could not be loaded.", objName);
        Console::Error::WriteLine();
    }

    static sint32 GetIndexFromTypeEntry(uint8 objectType, uint8 entryIndex)
    {
        int result = 0;
        for (uint8 i = 0; i < objectType; i++)
        {
            result += object_entry_group_counts[i];
        }
        result += entryIndex;
        return result;
    }
};

ObjectManager * _objectManager;

IObjectManager * GetObjectManager()
{
    if (_objectManager == nullptr)
    {
        IObjectRepository * objectRepository = GetObjectRepository();
        _objectManager = new ObjectManager(objectRepository);
    }
    return _objectManager;
}

extern "C"
{
    void * object_manager_get_loaded_object(const rct_object_entry * entry)
    {
        IObjectManager * objectManager = GetObjectManager();
        Object * loadedObject = objectManager->GetLoadedObject(entry);
        return (void *)loadedObject;
    }

    uint8 object_manager_get_loaded_object_entry_index(const void * loadedObject)
    {
        IObjectManager * objectManager = GetObjectManager();
        const Object * object = (const Object *)loadedObject;
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
        objectManager->UnloadAll();
    }
}
