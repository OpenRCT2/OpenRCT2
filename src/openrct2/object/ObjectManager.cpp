/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectManager.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../ParkImporter.h"
#include "../audio/audio.h"
#include "../core/Console.hpp"
#include "../core/EnumUtils.hpp"
#include "../core/JobPool.h"
#include "../core/Memory.hpp"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "../ride/RideAudio.h"
#include "BannerSceneryEntry.h"
#include "LargeSceneryObject.h"
#include "Object.h"
#include "ObjectLimits.h"
#include "ObjectList.h"
#include "ObjectRepository.h"
#include "PathAdditionObject.h"
#include "RideObject.h"
#include "SceneryGroupObject.h"
#include "SmallSceneryObject.h"
#include "WallObject.h"

#include <algorithm>
#include <array>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>

using namespace OpenRCT2;

/**
 * Represents an object that is to be loaded or is loaded and ready
 * to be placed in an object list.
 */
struct ObjectToLoad
{
    const ObjectRepositoryItem* RepositoryItem{};
    Object* LoadedObject{};
    ObjectEntryIndex Index{};
};

class ObjectManager final : public IObjectManager
{
private:
    IObjectRepository& _objectRepository;

    std::array<std::vector<Object*>, EnumValue(ObjectType::Count)> _loadedObjects;
    std::array<std::vector<ObjectEntryIndex>, RIDE_TYPE_COUNT> _rideTypeToObjectMap;

    // Used to return a safe empty vector back from GetAllRideEntries, can be removed when std::span is available
    std::vector<ObjectEntryIndex> _nullRideTypeEntries;

public:
    explicit ObjectManager(IObjectRepository& objectRepository)
        : _objectRepository(objectRepository)
    {
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
    }

    ~ObjectManager() override
    {
        UnloadAll();
    }

    Object* GetLoadedObject(ObjectType objectType, size_t index) override
    {
        // This is sometimes done deliberately (to avoid boilerplate), so no need to log_warn for this.
        if (index == OBJECT_ENTRY_INDEX_NULL)
        {
            return nullptr;
        }

        if (index >= static_cast<size_t>(getObjectEntryGroupCount(objectType)))
        {
#ifdef DEBUG
            if (index != OBJECT_ENTRY_INDEX_NULL)
            {
                LOG_WARNING("Object index %u exceeds maximum for type %d.", index, objectType);
            }
#endif
            return nullptr;
        }

        const auto& list = GetObjectList(objectType);
        if (index >= list.size())
        {
            return nullptr;
        }

        return list[index];
    }

    Object* GetLoadedObject(const ObjectEntryDescriptor& entry) override
    {
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(entry);
        if (ori == nullptr)
            return nullptr;

        return ori->LoadedObject.get();
    }

    ObjectEntryIndex GetLoadedObjectEntryIndex(std::string_view identifier) override
    {
        const auto* obj = GetLoadedObject(ObjectEntryDescriptor(identifier));
        if (obj != nullptr)
        {
            return GetLoadedObjectEntryIndex(obj);
        }
        return OBJECT_ENTRY_INDEX_NULL;
    }

    ObjectEntryIndex GetLoadedObjectEntryIndex(const ObjectEntryDescriptor& descriptor) override
    {
        auto obj = GetLoadedObject(descriptor);
        if (obj != nullptr)
        {
            return GetLoadedObjectEntryIndex(obj);
        }
        return OBJECT_ENTRY_INDEX_NULL;
    }

    ObjectEntryIndex GetLoadedObjectEntryIndex(const Object* object) override
    {
        ObjectEntryIndex result = OBJECT_ENTRY_INDEX_NULL;
        size_t index = GetLoadedObjectIndex(object);
        if (index != SIZE_MAX)
        {
            ObjectGetTypeEntryIndex(index, nullptr, &result);
        }
        return result;
    }

    ObjectList GetLoadedObjects() override
    {
        ObjectList objectList;
        for (auto objectType : getAllObjectTypes())
        {
            auto maxObjectsOfType = static_cast<ObjectEntryIndex>(getObjectEntryGroupCount(objectType));
            for (ObjectEntryIndex i = 0; i < maxObjectsOfType; i++)
            {
                auto obj = GetLoadedObject(objectType, i);
                if (obj != nullptr)
                {
                    objectList.SetObject(i, obj->GetDescriptor());
                }
            }
        }
        return objectList;
    }

    Object* LoadObject(std::string_view identifier) override
    {
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(identifier);
        return RepositoryItemToObject(ori);
    }

    Object* LoadObject(const RCTObjectEntry* entry) override
    {
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(entry);
        return RepositoryItemToObject(ori);
    }

    Object* LoadObject(const ObjectEntryDescriptor& descriptor) override
    {
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(descriptor);
        return RepositoryItemToObject(ori);
    }

    Object* LoadObject(const ObjectEntryDescriptor& descriptor, ObjectEntryIndex slot) override
    {
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(descriptor);
        return RepositoryItemToObject(ori, slot);
    }

    void LoadObjects(const ObjectList& objectList, const bool reportProgress) override
    {
        // Find all the required objects
        auto requiredObjects = GetRequiredObjects(objectList);

        // Load the required objects
        LoadObjects(requiredObjects, reportProgress);

        // Update indices.
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
    }

    void UnloadObjects(const std::vector<ObjectEntryDescriptor>& entries) override
    {
        // TODO there are two performance issues here:
        //        - FindObject for every entry which is a dictionary lookup
        //        - GetLoadedObjectIndex for every entry which enumerates _loadedList

        size_t numObjectsUnloaded = 0;
        for (const auto& descriptor : entries)
        {
            const auto* ori = _objectRepository.FindObject(descriptor);
            if (ori != nullptr)
            {
                auto* loadedObject = ori->LoadedObject.get();
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
            ResetTypeToRideEntryIndexMap();
        }
    }

    void UnloadAllTransient() override
    {
        UnloadAll(true);
    }

    void UnloadAll() override
    {
        UnloadAll(false);
    }

    void ResetObjects() override
    {
        for (auto& list : _loadedObjects)
        {
            for (auto* loadedObject : list)
            {
                if (loadedObject != nullptr)
                {
                    loadedObject->Unload();
                    loadedObject->Load();
                }
            }
        }
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();

        // We will need to replay the title music if the title music object got reloaded
        OpenRCT2::Audio::StopTitleMusic();
        OpenRCT2::Audio::PlayTitleMusic();
        OpenRCT2::RideAudio::StopAllChannels();
    }

    std::vector<const ObjectRepositoryItem*> GetPackableObjects() override
    {
        std::vector<const ObjectRepositoryItem*> objects;
        size_t numObjects = _objectRepository.GetNumObjects();
        for (size_t i = 0; i < numObjects; i++)
        {
            const ObjectRepositoryItem* item = &_objectRepository.GetObjects()[i];
            if (item->LoadedObject != nullptr && IsObjectCustom(item))
            {
                objects.push_back(item);
            }
        }
        return objects;
    }

    static StringId GetObjectSourceGameString(const ObjectSourceGame sourceGame)
    {
        switch (sourceGame)
        {
            case ObjectSourceGame::RCT1:
                return STR_SCENARIO_CATEGORY_RCT1;
            case ObjectSourceGame::AddedAttractions:
                return STR_SCENARIO_CATEGORY_RCT1_AA;
            case ObjectSourceGame::LoopyLandscapes:
                return STR_SCENARIO_CATEGORY_RCT1_LL;
            case ObjectSourceGame::RCT2:
                return STR_ROLLERCOASTER_TYCOON_2_DROPDOWN;
            case ObjectSourceGame::WackyWorlds:
                return STR_OBJECT_FILTER_WW;
            case ObjectSourceGame::TimeTwister:
                return STR_OBJECT_FILTER_TT;
            case ObjectSourceGame::OpenRCT2Official:
                return STR_OBJECT_FILTER_OPENRCT2_OFFICIAL;
            default:
                return STR_OBJECT_FILTER_CUSTOM;
        }
    }

    const std::vector<ObjectEntryIndex>& GetAllRideEntries(ride_type_t rideType) override
    {
        if (rideType >= RIDE_TYPE_COUNT)
        {
            // Return an empty vector
            return _nullRideTypeEntries;
        }
        return _rideTypeToObjectMap[rideType];
    }

private:
    std::vector<Object*>& GetObjectList(ObjectType type)
    {
        auto typeIndex = EnumValue(type);
        return _loadedObjects[typeIndex];
    }

    void UnloadAll(bool onlyTransient)
    {
        for (auto type : getAllObjectTypes())
        {
            if (!onlyTransient || !IsIntransientObjectType(type))
            {
                auto& list = GetObjectList(type);
                for (auto* loadedObject : list)
                {
                    UnloadObject(loadedObject);
                }
                list.clear();
            }
        }
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
    }

    Object* LoadObject(ObjectEntryIndex slot, std::string_view identifier)
    {
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(identifier);
        return RepositoryItemToObject(ori, slot);
    }

    Object* RepositoryItemToObject(const ObjectRepositoryItem* ori, std::optional<ObjectEntryIndex> slot = {})
    {
        if (ori == nullptr)
            return nullptr;

        Object* loadedObject = ori->LoadedObject.get();
        if (loadedObject != nullptr)
            return loadedObject;

        ObjectType objectType = ori->Type;
        if (slot)
        {
            auto& list = GetObjectList(objectType);
            if (list.size() > *slot && list[*slot] != nullptr)
            {
                // Slot already taken
                return nullptr;
            }
        }
        else
        {
            slot = FindSpareSlot(objectType);
        }
        if (slot)
        {
            auto* object = GetOrLoadObject(ori);
            if (object != nullptr)
            {
                auto& list = GetObjectList(objectType);
                if (list.size() <= *slot)
                {
                    list.resize(*slot + 1);
                }
                loadedObject = object;
                list[*slot] = object;
                UpdateSceneryGroupIndexes();
                ResetTypeToRideEntryIndexMap();
            }
        }
        return loadedObject;
    }

    std::optional<ObjectEntryIndex> FindSpareSlot(ObjectType objectType)
    {
        auto& list = GetObjectList(objectType);
        auto it = std::find(list.begin(), list.end(), nullptr);
        if (it != list.end())
        {
            return static_cast<ObjectEntryIndex>(std::distance(list.begin(), it));
        }

        auto maxSize = getObjectEntryGroupCount(objectType);
        if (list.size() < static_cast<size_t>(maxSize))
        {
            list.emplace_back();
            return static_cast<ObjectEntryIndex>(list.size() - 1);
        }
        return std::nullopt;
    }

    size_t GetLoadedObjectIndex(const Object* object)
    {
        Guard::ArgumentNotNull(object, GUARD_LINE);

        auto result = std::numeric_limits<size_t>().max();
        auto& list = GetObjectList(object->GetObjectType());
        auto it = std::find(list.begin(), list.end(), object);
        if (it != list.end())
        {
            result = std::distance(list.begin(), it);
        }
        return result;
    }

    void UnloadObject(Object* object)
    {
        if (object == nullptr)
            return;

        // Because it's possible to have the same loaded object for multiple
        // slots, we have to make sure find and set all of them to nullptr
        auto& list = GetObjectList(object->GetObjectType());
        std::replace(list.begin(), list.end(), object, static_cast<Object*>(nullptr));

        object->Unload();

        // TODO try to prevent doing a repository search
        const auto* ori = _objectRepository.FindObject(object->GetDescriptor());
        if (ori != nullptr)
        {
            _objectRepository.UnregisterLoadedObject(ori, object);
        }
    }

    void UnloadObjectsExcept(const std::vector<Object*>& newLoadedObjects)
    {
        // Build a hash set for quick checking
        auto exceptSet = std::unordered_set<Object*>();
        for (auto& object : newLoadedObjects)
        {
            if (object != nullptr)
            {
                exceptSet.insert(object);
            }
        }

        // Unload objects that are not in the hash set
        size_t totalObjectsLoaded = 0;
        size_t numObjectsUnloaded = 0;
        for (auto type : getAllObjectTypes())
        {
            if (!IsIntransientObjectType(type))
            {
                auto& list = GetObjectList(type);
                for (auto& object : list)
                {
                    if (object == nullptr)
                        continue;

                    totalObjectsLoaded++;
                    if (exceptSet.find(object) == exceptSet.end())
                    {
                        UnloadObject(object);
                        object = nullptr;
                        numObjectsUnloaded++;
                    }
                }
            }
        }

        LOG_VERBOSE("%u / %u objects unloaded", numObjectsUnloaded, totalObjectsLoaded);
    }

    template<typename T>
    void UpdateSceneryGroupIndexes(ObjectType type)
    {
        auto& list = GetObjectList(type);
        for (auto* loadedObject : list)
        {
            if (loadedObject != nullptr)
            {
                auto* sceneryEntry = static_cast<T*>(loadedObject->GetLegacyData());
                sceneryEntry->scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
            }
        }
    }

    void UpdateSceneryGroupIndexes()
    {
        UpdateSceneryGroupIndexes<SmallSceneryEntry>(ObjectType::SmallScenery);
        UpdateSceneryGroupIndexes<LargeSceneryEntry>(ObjectType::LargeScenery);
        UpdateSceneryGroupIndexes<WallSceneryEntry>(ObjectType::Walls);
        UpdateSceneryGroupIndexes<BannerSceneryEntry>(ObjectType::Banners);
        UpdateSceneryGroupIndexes<PathAdditionEntry>(ObjectType::PathAdditions);

        auto& list = GetObjectList(ObjectType::SceneryGroup);
        for (auto* loadedObject : list)
        {
            auto sgObject = static_cast<SceneryGroupObject*>(loadedObject);
            if (sgObject != nullptr)
            {
                sgObject->UpdateEntryIndexes();
            }
        }

        // HACK Scenery window will lose its tabs after changing the scenery group indexing
        //      for now just close it, but it will be better to later tell it to invalidate the tabs
        WindowCloseByClass(WindowClass::Scenery);
    }

    ObjectEntryIndex GetPrimarySceneryGroupEntryIndex(Object* loadedObject)
    {
        auto* sceneryObject = dynamic_cast<SceneryObject*>(loadedObject);
        const auto& primarySGEntry = sceneryObject->GetPrimarySceneryGroup();
        Object* sgObject = GetLoadedObject(primarySGEntry);

        auto entryIndex = OBJECT_ENTRY_INDEX_NULL;
        if (sgObject != nullptr)
        {
            entryIndex = GetLoadedObjectEntryIndex(sgObject);
        }
        return entryIndex;
    }

    std::vector<ObjectToLoad> GetRequiredObjects(const ObjectList& objectList)
    {
        std::vector<ObjectToLoad> requiredObjects;
        std::vector<ObjectEntryDescriptor> missingObjects;

        for (auto objectType : getAllObjectTypes())
        {
            auto& descriptors = objectList.GetList(objectType);
            auto maxSize = static_cast<size_t>(getObjectEntryGroupCount(objectType));
            auto listSize = static_cast<ObjectEntryIndex>(std::min(descriptors.size(), maxSize));
            for (ObjectEntryIndex i = 0; i < listSize; i++)
            {
                const auto& entry = objectList.GetObject(objectType, i);
                if (entry.HasValue())
                {
                    const auto* ori = _objectRepository.FindObject(entry);
                    if (ori == nullptr && entry.GetType() == ObjectType::ScenarioText)
                    {
                        continue;
                    }

                    if (ori == nullptr)
                    {
                        missingObjects.push_back(entry);
                        ReportMissingObject(entry);
                    }

                    ObjectToLoad otl;
                    otl.RepositoryItem = ori;
                    otl.Index = i;
                    requiredObjects.push_back(otl);
                }
            }
        }

        if (!missingObjects.empty())
        {
            throw ObjectLoadException(std::move(missingObjects));
        }

        return requiredObjects;
    }

    void ReportProgress(size_t numLoaded, size_t numRequired)
    {
        constexpr auto kObjectLoadMinProgress = 10;
        constexpr auto kObjectLoadMaxProgress = 90;
        constexpr auto kObjectLoadProgressRange = kObjectLoadMaxProgress - kObjectLoadMinProgress;

        const auto currentProgress = kObjectLoadMinProgress + (numLoaded * kObjectLoadProgressRange / numRequired);
        OpenRCT2::GetContext()->SetProgress(static_cast<uint32_t>(currentProgress), 100, STR_STRING_M_PERCENT);
    }

    void LoadObjects(std::vector<ObjectToLoad>& requiredObjects, bool reportProgress)
    {
        std::vector<Object*> objects;
        std::vector<Object*> newLoadedObjects;
        std::vector<ObjectEntryDescriptor> badObjects;

        // Create a list of objects that are currently not loaded but required.
        std::vector<const ObjectRepositoryItem*> objectsToLoad;
        for (auto& requiredObject : requiredObjects)
        {
            auto* repositoryItem = requiredObject.RepositoryItem;
            if (repositoryItem == nullptr)
            {
                continue;
            }

            auto* loadedObject = repositoryItem->LoadedObject.get();
            if (loadedObject == nullptr)
            {
                objectsToLoad.push_back(repositoryItem);
            }
        }

        // De-duplicate the list, since loading happens in parallel we can't have it race the repository item.
        std::sort(objectsToLoad.begin(), objectsToLoad.end());
        objectsToLoad.erase(std::unique(objectsToLoad.begin(), objectsToLoad.end()), objectsToLoad.end());

        // Prepare for loading objects multi-threaded
        auto numProcessed = 0;
        auto numRequired = objectsToLoad.size();
        std::mutex commonMutex;
        auto loadSingleObject = [&](const ObjectRepositoryItem* requiredObject) {
            // Object requires to be loaded, if the object successfully loads it will register it
            // as a loaded object otherwise placed into the badObjects list.
            auto newObject = _objectRepository.LoadObject(requiredObject);

            std::lock_guard<std::mutex> guard(commonMutex);
            if (newObject == nullptr)
            {
                badObjects.push_back(ObjectEntryDescriptor(requiredObject->ObjectEntry));
                ReportObjectLoadProblem(&requiredObject->ObjectEntry);
            }
            else
            {
                newLoadedObjects.push_back(newObject.get());
                // Connect the ori to the registered object
                _objectRepository.RegisterLoadedObject(requiredObject, std::move(newObject));
            }

            numProcessed++;
        };

        auto completionFn = [&]() {
            if (reportProgress && (numProcessed % 100) == 0)
                ReportProgress(numProcessed, numRequired);
        };

        // Dispatch loading the objects
        JobPool jobs{};
        for (auto* object : objectsToLoad)
        {
            jobs.AddTask([object, &loadSingleObject]() { loadSingleObject(object); }, completionFn);
        }

        // Wait until all jobs are fully completed
        jobs.Join();

        // Assign the loaded objects to the required objects
        for (auto& requiredObject : requiredObjects)
        {
            auto* repositoryItem = requiredObject.RepositoryItem;
            if (repositoryItem == nullptr)
            {
                continue;
            }
            auto* loadedObject = repositoryItem->LoadedObject.get();
            if (loadedObject == nullptr)
            {
                continue;
            }
            requiredObject.LoadedObject = loadedObject;
            objects.push_back(loadedObject);
        }

        // Load objects
        for (auto* obj : newLoadedObjects)
        {
            obj->Load();
        }

        if (!badObjects.empty())
        {
            // Unload all the new objects we loaded
            for (auto* object : newLoadedObjects)
            {
                UnloadObject(object);
            }
            throw ObjectLoadException(std::move(badObjects));
        }

        // Unload objects which are not in the required list.
        if (objects.empty())
        {
            UnloadAllTransient();
        }
        else
        {
            UnloadObjectsExcept(objects);
        }

        // Set the new object lists
        for (auto type : getAllObjectTypes())
        {
            if (!IsIntransientObjectType(type))
            {
                auto& list = GetObjectList(type);
                list.clear();
            }
        }
        for (auto& otl : requiredObjects)
        {
            auto objectType = otl.LoadedObject->GetObjectType();
            auto& list = GetObjectList(objectType);
            if (list.size() <= otl.Index)
            {
                list.resize(otl.Index + 1);
            }
            list[otl.Index] = otl.LoadedObject;
        }

        LOG_VERBOSE("%u / %u new objects loaded", newLoadedObjects.size(), requiredObjects.size());
    }

    Object* GetOrLoadObject(const ObjectRepositoryItem* ori)
    {
        auto* loadedObject = ori->LoadedObject.get();
        if (loadedObject != nullptr)
            return loadedObject;

        // Try to load object
        auto object = _objectRepository.LoadObject(ori);
        if (object != nullptr)
        {
            loadedObject = object.get();

            object->Load();

            // Connect the ori to the registered object
            _objectRepository.RegisterLoadedObject(ori, std::move(object));
        }

        return loadedObject;
    }

    void ResetTypeToRideEntryIndexMap()
    {
        // Clear all ride objects
        for (auto& v : _rideTypeToObjectMap)
        {
            v.clear();
        }

        // Build object lists
        const auto maxRideObjects = static_cast<size_t>(getObjectEntryGroupCount(ObjectType::Ride));
        for (size_t i = 0; i < maxRideObjects; i++)
        {
            auto* rideObject = static_cast<RideObject*>(GetLoadedObject(ObjectType::Ride, i));
            if (rideObject == nullptr)
                continue;

            const auto& entry = rideObject->GetEntry();

            for (auto rideType : entry.ride_type)
            {
                if (rideType < _rideTypeToObjectMap.size())
                {
                    auto& v = _rideTypeToObjectMap[rideType];
                    v.push_back(static_cast<ObjectEntryIndex>(i));
                }
            }
        }
    }

    static void ReportMissingObject(const ObjectEntryDescriptor& entry)
    {
        std::string name(entry.GetName());
        Console::Error::WriteLine("[%s] Object not found.", name.c_str());
    }

    void ReportObjectLoadProblem(const RCTObjectEntry* entry)
    {
        utf8 objName[kDatNameLength + 1] = { 0 };
        std::copy_n(entry->name, kDatNameLength, objName);
        Console::Error::WriteLine("[%s] Object could not be loaded.", objName);
    }
};

std::unique_ptr<IObjectManager> CreateObjectManager(IObjectRepository& objectRepository)
{
    return std::make_unique<ObjectManager>(objectRepository);
}

Object* ObjectManagerGetLoadedObject(const ObjectEntryDescriptor& entry)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    Object* loadedObject = objectManager.GetLoadedObject(entry);
    return loadedObject;
}

ObjectEntryIndex ObjectManagerGetLoadedObjectEntryIndex(const Object* loadedObject)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    auto entryIndex = objectManager.GetLoadedObjectEntryIndex(loadedObject);
    return entryIndex;
}

ObjectEntryIndex ObjectManagerGetLoadedObjectEntryIndex(const ObjectEntryDescriptor& entry)
{
    return ObjectManagerGetLoadedObjectEntryIndex(ObjectManagerGetLoadedObject(entry));
}

Object* ObjectManagerLoadObject(const RCTObjectEntry* entry)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    Object* loadedObject = objectManager.LoadObject(entry);
    return loadedObject;
}

void ObjectManagerUnloadObjects(const std::vector<ObjectEntryDescriptor>& entries)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    objectManager.UnloadObjects(entries);
}

void ObjectManagerUnloadAllObjects()
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    objectManager.UnloadAllTransient();
}

StringId ObjectManagerGetSourceGameString(const ObjectSourceGame sourceGame)
{
    return ObjectManager::GetObjectSourceGameString(sourceGame);
}
