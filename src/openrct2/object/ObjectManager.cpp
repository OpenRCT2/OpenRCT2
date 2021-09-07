/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectManager.h"

#include "../Context.h"
#include "../ParkImporter.h"
#include "../core/Console.hpp"
#include "../core/Memory.hpp"
#include "../localisation/StringIds.h"
#include "../util/Util.h"
#include "FootpathItemObject.h"
#include "LargeSceneryObject.h"
#include "Object.h"
#include "ObjectList.h"
#include "ObjectRepository.h"
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

enum class ObjectTypeEntryIndex : int32_t
{
    Rides = 0,
    SmallScenery = Rides + MAX_RIDE_OBJECTS,
    LargeScenery = SmallScenery + MAX_SMALL_SCENERY_OBJECTS,
    WallScenery = LargeScenery + MAX_LARGE_SCENERY_OBJECTS,
    Banners = WallScenery + MAX_WALL_SCENERY_OBJECTS,
    Paths = Banners + MAX_BANNER_OBJECTS,
    PathAdditions = Paths + MAX_PATH_OBJECTS,
    SceneryGroups = PathAdditions + MAX_PATH_ADDITION_OBJECTS,
    ParkEntrances = SceneryGroups + MAX_SCENERY_GROUP_OBJECTS,
    Water = ParkEntrances + MAX_PARK_ENTRANCE_OBJECTS,
    ScenarioTexts = Water + MAX_WATER_OBJECTS,
    TerrainSurfaces = ScenarioTexts + MAX_SCENARIO_TEXT_OBJECTS,
    TerrainEdges = TerrainSurfaces + MAX_TERRAIN_SURFACE_OBJECTS,
    Stations = TerrainEdges + MAX_TERRAIN_EDGE_OBJECTS,
    Music = Stations + MAX_STATION_OBJECTS,
    FootpathSurfaces = Music + MAX_FOOTPATH_SURFACE_OBJECTS,
    FootpathRailings = FootpathSurfaces + MAX_FOOTPATH_SURFACE_OBJECTS,
    End = FootpathRailings + MAX_FOOTPATH_RAILINGS_OBJECTS,
};
static_assert(EnumValue(ObjectTypeEntryIndex::End) == OBJECT_ENTRY_COUNT);

static constexpr ObjectTypeEntryIndex ObjectTypeEntryIndices[] = {
    ObjectTypeEntryIndex::Rides,
    ObjectTypeEntryIndex::SmallScenery,
    ObjectTypeEntryIndex::LargeScenery,
    ObjectTypeEntryIndex::WallScenery,
    ObjectTypeEntryIndex::Banners,
    ObjectTypeEntryIndex::Paths,
    ObjectTypeEntryIndex::PathAdditions,
    ObjectTypeEntryIndex::SceneryGroups,
    ObjectTypeEntryIndex::ParkEntrances,
    ObjectTypeEntryIndex::Water,
    ObjectTypeEntryIndex::ScenarioTexts,
    ObjectTypeEntryIndex::TerrainSurfaces,
    ObjectTypeEntryIndex::TerrainEdges,
    ObjectTypeEntryIndex::Stations,
    ObjectTypeEntryIndex::Music,
    ObjectTypeEntryIndex::FootpathSurfaces,
    ObjectTypeEntryIndex::FootpathRailings,
};
static_assert(std::size(ObjectTypeEntryIndices) == EnumValue(ObjectType::Count));

class ObjectManager final : public IObjectManager
{
private:
    IObjectRepository& _objectRepository;

    std::vector<Object*> _loadedObjects;
    std::array<std::vector<ObjectEntryIndex>, RIDE_TYPE_COUNT> _rideTypeToObjectMap;

    // Used to return a safe empty vector back from GetAllRideEntries, can be removed when std::span is available
    std::vector<ObjectEntryIndex> _nullRideTypeEntries;

public:
    explicit ObjectManager(IObjectRepository& objectRepository)
        : _objectRepository(objectRepository)
    {
        _loadedObjects.resize(OBJECT_ENTRY_COUNT);

        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
    }

    ~ObjectManager() override
    {
        UnloadAll();
    }

    Object* GetLoadedObject(size_t index) override
    {
        if (index >= _loadedObjects.size())
        {
            return nullptr;
        }
        return _loadedObjects[index];
    }

    Object* GetLoadedObject(ObjectType objectType, ObjectEntryIndex index) override
    {
        if (index >= static_cast<size_t>(object_entry_group_counts[EnumValue(objectType)]))
        {
#ifdef DEBUG
            log_warning("Object index %u exceeds maximum for type %d.", index, objectType);
#endif
            return nullptr;
        }

        auto objectIndex = GetIndexFromTypeEntry(objectType, index);
        return GetLoadedObject(objectIndex);
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
            get_type_entry_index(index, nullptr, &result);
        }
        return result;
    }

    Object* LoadObject(std::string_view identifier) override
    {
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(identifier);
        return RepositoryItemToObject(ori);
    }

    Object* LoadObject(const rct_object_entry* entry) override
    {
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(entry);
        return RepositoryItemToObject(ori);
    }

    void LoadObjects(const rct_object_entry* entries, size_t count) override
    {
        // Find all the required objects
        auto requiredObjects = GetRequiredObjects(entries, count);

        // Load the required objects
        LoadObjects(requiredObjects);

        // Load defaults.
        LoadDefaultObjects();

        // Update indices.
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
    }

    void UnloadObjects(const std::vector<rct_object_entry>& entries) override
    {
        // TODO there are two performance issues here:
        //        - FindObject for every entry which is a dictionary lookup
        //        - GetLoadedObjectIndex for every entry which enumerates _loadedList

        size_t numObjectsUnloaded = 0;
        for (const auto& entry : entries)
        {
            const ObjectRepositoryItem* ori = _objectRepository.FindObject(&entry);
            if (ori != nullptr)
            {
                Object* loadedObject = ori->LoadedObject.get();
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

    void UnloadAll() override
    {
        for (auto* object : _loadedObjects)
        {
            UnloadObject(object);
        }
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
    }

    void ResetObjects() override
    {
        for (auto& loadedObject : _loadedObjects)
        {
            if (loadedObject != nullptr)
            {
                loadedObject->Unload();
                loadedObject->Load();
            }
        }
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
    }

    std::vector<const ObjectRepositoryItem*> GetPackableObjects() override
    {
        std::vector<const ObjectRepositoryItem*> objects;
        size_t numObjects = _objectRepository.GetNumObjects();
        for (size_t i = 0; i < numObjects; i++)
        {
            const ObjectRepositoryItem* item = &_objectRepository.GetObjects()[i];
            if (item->LoadedObject != nullptr && IsObjectCustom(item) && item->LoadedObject->GetLegacyData() != nullptr
                && !item->LoadedObject->IsJsonObject())
            {
                objects.push_back(item);
            }
        }
        return objects;
    }

    void LoadDefaultObjects() override
    {
        // We currently will load new object types here that apply to all
        // loaded RCT1 and RCT2 save files.

        // Surfaces
        LoadObject("rct2.surface.grass");
        LoadObject("rct2.surface.sand");
        LoadObject("rct2.surface.dirt");
        LoadObject("rct2.surface.rock");
        LoadObject("rct2.surface.martian");
        LoadObject("rct2.surface.chequerboard");
        LoadObject("rct2.surface.grassclumps");
        LoadObject("rct2.surface.ice");
        LoadObject("rct2.surface.gridred");
        LoadObject("rct2.surface.gridyellow");
        LoadObject("rct2.surface.gridpurple");
        LoadObject("rct2.surface.gridgreen");
        LoadObject("rct2.surface.sandred");
        LoadObject("rct2.surface.sandbrown");
        LoadObject("rct1.aa.surface.roofred");
        LoadObject("rct1.ll.surface.roofgrey");
        LoadObject("rct1.ll.surface.rust");
        LoadObject("rct1.ll.surface.wood");

        // Edges
        LoadObject("rct2.edge.rock");
        LoadObject("rct2.edge.woodred");
        LoadObject("rct2.edge.woodblack");
        LoadObject("rct2.edge.ice");
        LoadObject("rct1.edge.brick");
        LoadObject("rct1.edge.iron");
        LoadObject("rct1.aa.edge.grey");
        LoadObject("rct1.aa.edge.yellow");
        LoadObject("rct1.aa.edge.red");
        LoadObject("rct1.ll.edge.purple");
        LoadObject("rct1.ll.edge.green");
        LoadObject("rct1.ll.edge.stonebrown");
        LoadObject("rct1.ll.edge.stonegrey");
        LoadObject("rct1.ll.edge.skyscrapera");
        LoadObject("rct1.ll.edge.skyscraperb");

        // Stations
        LoadObject("rct2.station.plain");
        LoadObject("rct2.station.wooden");
        LoadObject("rct2.station.canvastent");
        LoadObject("rct2.station.castlegrey");
        LoadObject("rct2.station.castlebrown");
        LoadObject("rct2.station.jungle");
        LoadObject("rct2.station.log");
        LoadObject("rct2.station.classical");
        LoadObject("rct2.station.abstract");
        LoadObject("rct2.station.snow");
        LoadObject("rct2.station.pagoda");
        LoadObject("rct2.station.space");
        LoadObject("openrct2.station.noentrance");

        // Music
        auto baseIndex = GetIndexFromTypeEntry(ObjectType::Music, 0);
        LoadObject(baseIndex + MUSIC_STYLE_DODGEMS_BEAT, "rct2.music.dodgems");
        LoadObject(baseIndex + MUSIC_STYLE_FAIRGROUND_ORGAN, "rct2.music.fairground");
        LoadObject(baseIndex + MUSIC_STYLE_ROMAN_FANFARE, "rct2.music.roman");
        LoadObject(baseIndex + MUSIC_STYLE_ORIENTAL, "rct2.music.oriental");
        LoadObject(baseIndex + MUSIC_STYLE_MARTIAN, "rct2.music.martian");
        LoadObject(baseIndex + MUSIC_STYLE_JUNGLE_DRUMS, "rct2.music.jungle");
        LoadObject(baseIndex + MUSIC_STYLE_EGYPTIAN, "rct2.music.egyptian");
        LoadObject(baseIndex + MUSIC_STYLE_TOYLAND, "rct2.music.toyland");
        LoadObject(baseIndex + MUSIC_STYLE_SPACE, "rct2.music.space");
        LoadObject(baseIndex + MUSIC_STYLE_HORROR, "rct2.music.horror");
        LoadObject(baseIndex + MUSIC_STYLE_TECHNO, "rct2.music.techno");
        LoadObject(baseIndex + MUSIC_STYLE_GENTLE, "rct2.music.gentle");
        LoadObject(baseIndex + MUSIC_STYLE_SUMMER, "rct2.music.summer");
        LoadObject(baseIndex + MUSIC_STYLE_WATER, "rct2.music.water");
        LoadObject(baseIndex + MUSIC_STYLE_WILD_WEST, "rct2.music.wildwest");
        LoadObject(baseIndex + MUSIC_STYLE_JURASSIC, "rct2.music.jurassic");
        LoadObject(baseIndex + MUSIC_STYLE_ROCK, "rct2.music.rock1");
        LoadObject(baseIndex + MUSIC_STYLE_RAGTIME, "rct2.music.ragtime");
        LoadObject(baseIndex + MUSIC_STYLE_FANTASY, "rct2.music.fantasy");
        LoadObject(baseIndex + MUSIC_STYLE_ROCK_STYLE_2, "rct2.music.rock2");
        LoadObject(baseIndex + MUSIC_STYLE_ICE, "rct2.music.ice");
        LoadObject(baseIndex + MUSIC_STYLE_SNOW, "rct2.music.snow");
        LoadObject(baseIndex + MUSIC_STYLE_CUSTOM_MUSIC_1, "rct2.music.custom1");
        LoadObject(baseIndex + MUSIC_STYLE_CUSTOM_MUSIC_2, "rct2.music.custom2");
        LoadObject(baseIndex + MUSIC_STYLE_MEDIEVAL, "rct2.music.medieval");
        LoadObject(baseIndex + MUSIC_STYLE_URBAN, "rct2.music.urban");
        LoadObject(baseIndex + MUSIC_STYLE_ORGAN, "rct2.music.organ");
        LoadObject(baseIndex + MUSIC_STYLE_MECHANICAL, "rct2.music.mechanical");
        LoadObject(baseIndex + MUSIC_STYLE_MODERN, "rct2.music.modern");
        LoadObject(baseIndex + MUSIC_STYLE_PIRATES, "rct2.music.pirate");
        LoadObject(baseIndex + MUSIC_STYLE_ROCK_STYLE_3, "rct2.music.rock3");
        LoadObject(baseIndex + MUSIC_STYLE_CANDY_STYLE, "rct2.music.candy");
    }

    static rct_string_id GetObjectSourceGameString(const ObjectSourceGame sourceGame)
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

    const std::vector<ObjectEntryIndex>& GetAllRideEntries(uint8_t rideType) override
    {
        if (rideType >= RIDE_TYPE_COUNT)
        {
            // Return an empty vector
            return _nullRideTypeEntries;
        }
        return _rideTypeToObjectMap[rideType];
    }

private:
    Object* LoadObject(int32_t slot, std::string_view identifier)
    {
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(identifier);
        return RepositoryItemToObject(ori, slot);
    }

    Object* RepositoryItemToObject(const ObjectRepositoryItem* ori, std::optional<int32_t> slot = {})
    {
        if (ori == nullptr)
            return nullptr;

        Object* loadedObject = ori->LoadedObject.get();
        if (loadedObject != nullptr)
            return loadedObject;

        ObjectType objectType = ori->ObjectEntry.GetType();
        if (slot)
        {
            if (_loadedObjects.size() > static_cast<size_t>(*slot) && _loadedObjects[*slot] != nullptr)
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
                if (_loadedObjects.size() <= static_cast<size_t>(*slot))
                {
                    _loadedObjects.resize(*slot + 1);
                }
                loadedObject = object;
                _loadedObjects[*slot] = object;
                UpdateSceneryGroupIndexes();
                ResetTypeToRideEntryIndexMap();
            }
        }

        return loadedObject;
    }

    std::optional<int32_t> FindSpareSlot(ObjectType objectType)
    {
        size_t firstIndex = GetIndexFromTypeEntry(objectType, 0);
        size_t endIndex = firstIndex + object_entry_group_counts[EnumValue(objectType)];
        for (size_t i = firstIndex; i < endIndex; i++)
        {
            if (_loadedObjects.size() <= i)
            {
                _loadedObjects.resize(i + 1);
                return static_cast<int32_t>(i);
            }
            else if (_loadedObjects[i] == nullptr)
            {
                return static_cast<int32_t>(i);
            }
        }
        return {};
    }

    size_t GetLoadedObjectIndex(const Object* object)
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

    void UnloadObject(Object* object)
    {
        if (object == nullptr)
            return;

        object->Unload();

        // TODO try to prevent doing a repository search
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(object->GetObjectEntry());
        if (ori != nullptr)
        {
            _objectRepository.UnregisterLoadedObject(ori, object);
        }

        // Because it's possible to have the same loaded object for multiple
        // slots, we have to make sure find and set all of them to nullptr
        std::replace(_loadedObjects.begin(), _loadedObjects.end(), object, static_cast<Object*>(nullptr));
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
        for (auto* object : _loadedObjects)
        {
            if (object == nullptr)
                continue;

            totalObjectsLoaded++;
            if (exceptSet.find(object) == exceptSet.end())
            {
                UnloadObject(object);
                numObjectsUnloaded++;
            }
        }

        log_verbose("%u / %u objects unloaded", numObjectsUnloaded, totalObjectsLoaded);
    }

    template<typename T> void UpdateSceneryGroupIndexes(Object* object)
    {
        auto* sceneryEntry = static_cast<T*>(object->GetLegacyData());
        sceneryEntry->scenery_tab_id = GetPrimarySceneryGroupEntryIndex(object);
    }

    void UpdateSceneryGroupIndexes()
    {
        for (auto* loadedObject : _loadedObjects)
        {
            // The list can contain unused slots, skip them.
            if (loadedObject == nullptr)
                continue;

            switch (loadedObject->GetObjectType())
            {
                case ObjectType::SmallScenery:
                    UpdateSceneryGroupIndexes<SmallSceneryEntry>(loadedObject);
                    break;
                case ObjectType::LargeScenery:
                    UpdateSceneryGroupIndexes<LargeSceneryEntry>(loadedObject);
                    break;
                case ObjectType::Walls:
                    UpdateSceneryGroupIndexes<WallSceneryEntry>(loadedObject);
                    break;
                case ObjectType::Banners:
                    UpdateSceneryGroupIndexes<BannerSceneryEntry>(loadedObject);
                    break;
                case ObjectType::PathBits:
                    UpdateSceneryGroupIndexes<PathBitEntry>(loadedObject);
                    break;
                case ObjectType::SceneryGroup:
                {
                    auto sgObject = dynamic_cast<SceneryGroupObject*>(loadedObject);
                    sgObject->UpdateEntryIndexes();
                    break;
                }
                default:
                    // This switch only handles scenery ObjectTypes.
                    break;
            }
        }

        // HACK Scenery window will lose its tabs after changing the scenery group indexing
        //      for now just close it, but it will be better to later tell it to invalidate the tabs
        window_close_by_class(WC_SCENERY);
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

    std::vector<const ObjectRepositoryItem*> GetRequiredObjects(const rct_object_entry* entries, size_t count)
    {
        std::vector<const ObjectRepositoryItem*> requiredObjects;
        std::vector<rct_object_entry> missingObjects;

        for (size_t i = 0; i < count; i++)
        {
            const rct_object_entry* entry = &entries[i];
            const ObjectRepositoryItem* ori = nullptr;
            if (!object_entry_is_empty(entry))
            {
                ori = _objectRepository.FindObject(entry);
                if (ori == nullptr && entry->GetType() != ObjectType::ScenarioText)
                {
                    missingObjects.push_back(*entry);
                    ReportMissingObject(entry);
                }
            }
            requiredObjects.push_back(ori);
        }

        if (!missingObjects.empty())
        {
            throw ObjectLoadException(std::move(missingObjects));
        }

        return requiredObjects;
    }

    template<typename T, typename TFunc> static void ParallelFor(const std::vector<T>& items, TFunc func)
    {
        auto partitions = std::thread::hardware_concurrency();
        auto partitionSize = (items.size() + (partitions - 1)) / partitions;
        std::vector<std::thread> threads;
        for (size_t n = 0; n < partitions; n++)
        {
            auto begin = n * partitionSize;
            auto end = std::min(items.size(), begin + partitionSize);
            threads.emplace_back(
                [func](size_t pbegin, size_t pend) {
                    for (size_t i = pbegin; i < pend; i++)
                    {
                        func(i);
                    }
                },
                begin, end);
        }
        for (auto& t : threads)
        {
            t.join();
        }
    }

    void LoadObjects(std::vector<const ObjectRepositoryItem*>& requiredObjects)
    {
        std::vector<Object*> objects;
        std::vector<Object*> newLoadedObjects;
        std::vector<rct_object_entry> badObjects;
        objects.resize(OBJECT_ENTRY_COUNT);
        newLoadedObjects.reserve(OBJECT_ENTRY_COUNT);

        // Read objects
        std::mutex commonMutex;
        ParallelFor(requiredObjects, [this, &commonMutex, requiredObjects, &objects, &badObjects, &newLoadedObjects](size_t i) {
            auto* requiredObject = requiredObjects[i];
            Object* object = nullptr;
            if (requiredObject != nullptr)
            {
                auto* loadedObject = requiredObject->LoadedObject.get();
                if (loadedObject == nullptr)
                {
                    // Object requires to be loaded, if the object successfully loads it will register it
                    // as a loaded object otherwise placed into the badObjects list.
                    auto newObject = _objectRepository.LoadObject(requiredObject);
                    std::lock_guard<std::mutex> guard(commonMutex);
                    if (newObject == nullptr)
                    {
                        badObjects.push_back(requiredObject->ObjectEntry);
                        ReportObjectLoadProblem(&requiredObject->ObjectEntry);
                    }
                    else
                    {
                        object = newObject.get();
                        newLoadedObjects.push_back(object);
                        // Connect the ori to the registered object
                        _objectRepository.RegisterLoadedObject(requiredObject, std::move(newObject));
                    }
                }
                else
                {
                    object = loadedObject;
                }
            }
            objects[i] = object;
        });

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
            UnloadAll();
        }
        else
        {
            UnloadObjectsExcept(objects);
        }

        _loadedObjects = std::move(objects);

        log_verbose("%u / %u new objects loaded", newLoadedObjects.size(), requiredObjects.size());
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
        const auto maxRideObjects = object_entry_group_counts[EnumValue(ObjectType::Ride)];
        for (ObjectEntryIndex i = 0; i < maxRideObjects; i++)
        {
            auto* rideObject = static_cast<RideObject*>(GetLoadedObject(ObjectType::Ride, i));
            if (rideObject == nullptr)
                continue;

            const auto* entry = static_cast<rct_ride_entry*>(rideObject->GetLegacyData());
            if (entry == nullptr)
                continue;

            for (auto rideType : entry->ride_type)
            {
                if (rideType < _rideTypeToObjectMap.size())
                {
                    auto& v = _rideTypeToObjectMap[rideType];
                    v.push_back(static_cast<ObjectEntryIndex>(i));
                }
            }
        }
    }

    static void ReportMissingObject(const rct_object_entry* entry)
    {
        utf8 objName[DAT_NAME_LENGTH + 1] = { 0 };
        std::copy_n(entry->name, DAT_NAME_LENGTH, objName);
        Console::Error::WriteLine("[%s] Object not found.", objName);
    }

    void ReportObjectLoadProblem(const rct_object_entry* entry)
    {
        utf8 objName[DAT_NAME_LENGTH + 1] = { 0 };
        std::copy_n(entry->name, DAT_NAME_LENGTH, objName);
        Console::Error::WriteLine("[%s] Object could not be loaded.", objName);
    }

    static int32_t GetIndexFromTypeEntry(ObjectType objectType, ObjectEntryIndex entryIndex)
    {
        auto typeEntryIndex = ObjectTypeEntryIndices[EnumValue(objectType)];
        return EnumValue(typeEntryIndex) + entryIndex;
    }
};

std::unique_ptr<IObjectManager> CreateObjectManager(IObjectRepository& objectRepository)
{
    return std::make_unique<ObjectManager>(objectRepository);
}

Object* object_manager_get_loaded_object(const ObjectEntryDescriptor& entry)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    Object* loadedObject = objectManager.GetLoadedObject(entry);
    return loadedObject;
}

ObjectEntryIndex object_manager_get_loaded_object_entry_index(const Object* loadedObject)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    auto entryIndex = objectManager.GetLoadedObjectEntryIndex(loadedObject);
    return entryIndex;
}

ObjectEntryIndex object_manager_get_loaded_object_entry_index(const ObjectEntryDescriptor& entry)
{
    return object_manager_get_loaded_object_entry_index(object_manager_get_loaded_object(entry));
}

Object* object_manager_load_object(const rct_object_entry* entry)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    Object* loadedObject = objectManager.LoadObject(entry);
    return loadedObject;
}

void object_manager_unload_objects(const std::vector<rct_object_entry>& entries)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    objectManager.UnloadObjects(entries);
}

void object_manager_unload_all_objects()
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    objectManager.UnloadAll();
}

rct_string_id object_manager_get_source_game_string(const ObjectSourceGame sourceGame)
{
    return ObjectManager::GetObjectSourceGameString(sourceGame);
}
