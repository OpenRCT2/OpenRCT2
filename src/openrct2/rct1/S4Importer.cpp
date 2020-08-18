/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Cheats.h"
#include "../Context.h"
#include "../Editor.h"
#include "../Game.h"
#include "../GameState.h"
#include "../ParkImporter.h"
#include "../actions/WallPlaceAction.hpp"
#include "../audio/audio.h"
#include "../core/Collections.hpp"
#include "../core/Console.hpp"
#include "../core/FileStream.hpp"
#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../object/Object.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../peep/Peep.h"
#include "../peep/Staff.h"
#include "../ride/RideData.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../scenario/ScenarioSources.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/Wall.h"
#include "RCT1.h"
#include "Tables.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

static constexpr const ObjectEntryIndex OBJECT_ENTRY_INDEX_IGNORE = 254;

using namespace OpenRCT2;

class EntryList
{
private:
    std::vector<const char*> _entries;

public:
    size_t GetCount() const
    {
        return _entries.size();
    }

    const std::vector<const char*>& GetEntries() const
    {
        return _entries;
    }

    ObjectEntryIndex GetOrAddEntry(const char* entryName)
    {
        auto entryIndex = Collections::IndexOf(_entries, entryName, true);
        if (entryIndex == SIZE_MAX)
        {
            entryIndex = _entries.size();
            _entries.push_back(entryName);
        }
        return static_cast<ObjectEntryIndex>(entryIndex);
    }

    void AddRange(std::initializer_list<const char*> initializerList)
    {
        for (auto entry : initializerList)
        {
            GetOrAddEntry(entry);
        }
    }
};

class S4Importer final : public IParkImporter
{
private:
    std::string _s4Path;
    rct1_s4 _s4 = {};
    uint8_t _gameVersion = 0;
    uint8_t _parkValueConversionFactor = 0;
    bool _isScenario = false;

    // Lists of dynamic object entries
    EntryList _rideEntries;
    EntryList _smallSceneryEntries;
    EntryList _largeSceneryEntries;
    EntryList _wallEntries;
    EntryList _pathEntries;
    EntryList _pathAdditionEntries;
    EntryList _sceneryGroupEntries;
    EntryList _waterEntry;

    // Lookup tables for converting from RCT1 hard coded types to the new dynamic object entries
    ObjectEntryIndex _rideTypeToRideEntryMap[RCT1_RIDE_TYPE_COUNT]{};
    ObjectEntryIndex _vehicleTypeToRideEntryMap[RCT1_VEHICLE_TYPE_COUNT]{};
    ObjectEntryIndex _smallSceneryTypeToEntryMap[256]{};
    ObjectEntryIndex _largeSceneryTypeToEntryMap[256]{};
    ObjectEntryIndex _wallTypeToEntryMap[256]{};
    ObjectEntryIndex _pathTypeToEntryMap[24]{};
    ObjectEntryIndex _pathAdditionTypeToEntryMap[16]{};
    ObjectEntryIndex _sceneryThemeTypeToEntryMap[24]{};

    // Research
    std::bitset<MAX_RIDE_OBJECTS> _researchRideEntryUsed{};
    std::bitset<RCT1_RIDE_TYPE_COUNT> _researchRideTypeUsed{};

    // Scenario repository - used for determining scenario name
    IScenarioRepository* _scenarioRepository = GetScenarioRepository();

public:
    ParkLoadResult Load(const utf8* path) override
    {
        const utf8* extension = Path::GetExtension(path);
        if (String::Equals(extension, ".sc4", true))
        {
            return LoadScenario(path);
        }
        else if (String::Equals(extension, ".sv4", true))
        {
            return LoadSavedGame(path);
        }
        else
        {
            throw std::runtime_error("Invalid RCT1 park extension.");
        }
    }

    ParkLoadResult LoadSavedGame(const utf8* path, bool skipObjectCheck = false) override
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        auto result = LoadFromStream(&fs, false, skipObjectCheck, path);
        return result;
    }

    ParkLoadResult LoadScenario(const utf8* path, bool skipObjectCheck = false) override
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        auto result = LoadFromStream(&fs, true, skipObjectCheck, path);
        return result;
    }

    ParkLoadResult LoadFromStream(
        IStream* stream, bool isScenario, [[maybe_unused]] bool skipObjectCheck, const utf8* path) override
    {
        _s4 = *ReadAndDecodeS4(stream, isScenario);
        _s4Path = path;
        _isScenario = isScenario;
        _gameVersion = sawyercoding_detect_rct1_version(_s4.game_version) & FILE_VERSION_MASK;

        // Only determine what objects we required to import this saved game
        InitialiseEntryMaps();
        CreateAvailableObjectMappings();
        return ParkLoadResult(GetRequiredObjects());
    }

    void Import() override
    {
        Initialise();

        CreateAvailableObjectMappings();
        LoadObjects();

        ImportRides();
        ImportRideMeasurements();
        ImportSprites();
        ImportTileElements();
        ImportPeepSpawns();
        ImportFinance();
        ImportResearch();
        ImportParkName();
        ImportParkFlags();
        ImportClimate();
        ImportScenarioNameDetails();
        ImportScenarioObjective();
        ImportSavedView();
        FixLandOwnership();
        FixUrbanPark();
        CountBlockSections();
        SetDefaultNames();
        determine_ride_entrance_and_exit_locations();

        game_convert_news_items_to_utf8();
        map_count_remaining_land_rights();
        research_determine_first_of_type();
    }

    bool GetDetails(scenario_index_entry* dst) override
    {
        *dst = {};

        source_desc desc;
        // If no entry is found, this is a custom scenario.
        bool isOfficial = ScenarioSources::TryGetById(_s4.scenario_slot_index, &desc);

        dst->category = desc.category;
        dst->source_game = ScenarioSource{ desc.source };
        dst->source_index = desc.index;
        dst->sc_id = desc.id;

        dst->objective_type = _s4.scenario_objective_type;
        dst->objective_arg_1 = _s4.scenario_objective_years;
        // RCT1 used another way of calculating park value.
        if (_s4.scenario_objective_type == OBJECTIVE_PARK_VALUE_BY)
            dst->objective_arg_2 = CorrectRCT1ParkValue(_s4.scenario_objective_currency);
        else
            dst->objective_arg_2 = _s4.scenario_objective_currency;
        dst->objective_arg_3 = _s4.scenario_objective_num_guests;
        // This does not seem to be saved in the objective arguments, so look up the ID from the available rides instead.
        if (_s4.scenario_objective_type == OBJECTIVE_BUILD_THE_BEST)
        {
            dst->objective_arg_3 = GetBuildTheBestRideId();
        }

        auto name = rct2_to_utf8(_s4.scenario_name, RCT2_LANGUAGE_ID_ENGLISH_UK);
        std::string details;

        // TryGetById won't set this property if the scenario is not recognised,
        // but localisation needs it.
        if (!isOfficial)
        {
            desc.title = name.c_str();
        }

        String::Set(dst->internal_name, sizeof(dst->internal_name), desc.title);

        rct_string_id localisedStringIds[3];
        if (language_get_localised_scenario_strings(desc.title, localisedStringIds))
        {
            if (localisedStringIds[0] != STR_NONE)
            {
                name = String::ToStd(language_get_string(localisedStringIds[0]));
            }
            if (localisedStringIds[2] != STR_NONE)
            {
                details = String::ToStd(language_get_string(localisedStringIds[2]));
            }
        }

        String::Set(dst->name, sizeof(dst->name), name.c_str());
        String::Set(dst->details, sizeof(dst->details), details.c_str());

        return true;
    }

    int32_t CorrectRCT1ParkValue(money32 oldParkValue)
    {
        if (oldParkValue == MONEY32_UNDEFINED)
        {
            return MONEY32_UNDEFINED;
        }

        if (_parkValueConversionFactor == 0)
        {
            if (_s4.park_value != 0)
            {
                // Use the ratio between the old and new park value to calcute the ratio to
                // use for the park value history and the goal.
                auto& park = GetContext()->GetGameState()->GetPark();
                _parkValueConversionFactor = (park.CalculateParkValue() * 10) / _s4.park_value;
            }
            else
            {
                // In new games, the park value isn't set.
                _parkValueConversionFactor = 100;
            }
        }

        return (oldParkValue * _parkValueConversionFactor) / 10;
    }

private:
    std::unique_ptr<rct1_s4> ReadAndDecodeS4(IStream* stream, bool isScenario)
    {
        auto s4 = std::make_unique<rct1_s4>();
        size_t dataSize = stream->GetLength() - stream->GetPosition();
        auto deleter_lambda = [dataSize](uint8_t* ptr) { Memory::FreeArray(ptr, dataSize); };
        auto data = std::unique_ptr<uint8_t, decltype(deleter_lambda)>(stream->ReadArray<uint8_t>(dataSize), deleter_lambda);
        auto decodedData = std::unique_ptr<uint8_t, decltype(&Memory::Free<uint8_t>)>(
            Memory::Allocate<uint8_t>(sizeof(rct1_s4)), &Memory::Free<uint8_t>);

        size_t decodedSize;
        int32_t fileType = sawyercoding_detect_file_type(data.get(), dataSize);
        if (isScenario && (fileType & FILE_VERSION_MASK) != FILE_VERSION_RCT1)
        {
            decodedSize = sawyercoding_decode_sc4(data.get(), decodedData.get(), dataSize, sizeof(rct1_s4));
        }
        else
        {
            decodedSize = sawyercoding_decode_sv4(data.get(), decodedData.get(), dataSize, sizeof(rct1_s4));
        }

        if (decodedSize == sizeof(rct1_s4))
        {
            std::memcpy(s4.get(), decodedData.get(), sizeof(rct1_s4));
            return s4;
        }
        else
        {
            throw std::runtime_error("Unable to decode park.");
        }
    }

    void Initialise()
    {
        // Avoid reusing the value used for last import
        _parkValueConversionFactor = 0;

        uint16_t mapSize = _s4.map_size == 0 ? 128 : _s4.map_size;

        String::Set(gScenarioFileName, sizeof(gScenarioFileName), GetRCT1ScenarioName().c_str());

        // Do map initialisation, same kind of stuff done when loading scenario editor
        auto context = OpenRCT2::GetContext();
        context->GetObjectManager().UnloadAll();
        context->GetGameState()->InitAll(mapSize);
        gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
        gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        gS6Info.category = SCENARIO_CATEGORY_OTHER;
    }

    std::string GetRCT1ScenarioName()
    {
        const scenario_index_entry* scenarioEntry = _scenarioRepository->GetByInternalName(_s4.scenario_name);
        if (scenarioEntry == nullptr)
        {
            return "";
        }
        else
        {
            return path_get_filename(scenarioEntry->path);
        }
    }

    void InitialiseEntryMaps()
    {
        std::fill(std::begin(_rideTypeToRideEntryMap), std::end(_rideTypeToRideEntryMap), OBJECT_ENTRY_INDEX_NULL);
        std::fill(std::begin(_vehicleTypeToRideEntryMap), std::end(_vehicleTypeToRideEntryMap), OBJECT_ENTRY_INDEX_NULL);
        std::fill(std::begin(_smallSceneryTypeToEntryMap), std::end(_smallSceneryTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
        std::fill(std::begin(_largeSceneryTypeToEntryMap), std::end(_largeSceneryTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
        std::fill(std::begin(_wallTypeToEntryMap), std::end(_wallTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
        std::fill(std::begin(_pathTypeToEntryMap), std::end(_pathTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
        std::fill(std::begin(_pathAdditionTypeToEntryMap), std::end(_pathAdditionTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
        std::fill(std::begin(_sceneryThemeTypeToEntryMap), std::end(_sceneryThemeTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
    }

    /**
     * Scans the map and research list for all the object types used and builds lists and
     * lookup tables for converting from hard coded RCT1 object types to dynamic object entries.
     */
    void CreateAvailableObjectMappings()
    {
        AddDefaultEntries();
        AddAvailableEntriesFromResearchList();
        AddAvailableEntriesFromMap();
        AddAvailableEntriesFromRides();
        AddAvailableEntriesFromSceneryGroups();
        AddEntryForWater();
    }

    void AddDefaultEntries()
    {
        // Add default scenery groups
        _sceneryGroupEntries.AddRange({
            "SCGTREES",
            "SCGSHRUB",
            "SCGGARDN",
            "SCGFENCE",
            "SCGWALLS",
            "SCGPATHX",
        });

        // Add default footpaths
        _pathEntries.AddRange({
            "TARMAC  ",
            "TARMACG ",
            "TARMACB ",
            "PATHCRZY",
            "PATHSPCE",
            "PATHDIRT",
            "PATHASH ",
            "ROAD    ",
        });
    }

    void AddAvailableEntriesFromResearchList()
    {
        size_t researchListCount;
        const rct1_research_item* researchList = GetResearchList(&researchListCount);
        std::bitset<RCT1_RIDE_TYPE_COUNT> rideTypeInResearch = GetRideTypesPresentInResearchList(
            researchList, researchListCount);
        for (size_t i = 0; i < researchListCount; i++)
        {
            const rct1_research_item* researchItem = &researchList[i];

            if (researchItem->flags == RCT1_RESEARCH_FLAGS_SEPARATOR)
            {
                if (researchItem->item == RCT1_RESEARCH_END)
                {
                    break;
                }
                if (researchItem->item == RCT1_RESEARCH_END_AVAILABLE || researchItem->item == RCT1_RESEARCH_END_RESEARCHABLE)
                {
                    continue;
                }
            }

            switch (researchItem->type)
            {
                case RCT1_RESEARCH_TYPE_THEME:
                    AddEntriesForSceneryTheme(researchItem->item);
                    break;
                case RCT1_RESEARCH_TYPE_RIDE:
                    AddEntryForRideType(researchItem->item);
                    break;
                case RCT1_RESEARCH_TYPE_VEHICLE:
                    // For some bizarre reason, RCT1 research lists contain vehicles that aren't actually researched.
                    // Extra bizarrely, this does not seem to apply to Loopy Landscapes saves/scenarios.
                    if (rideTypeInResearch[researchItem->related_ride] || _gameVersion == FILE_VERSION_RCT1_LL)
                    {
                        AddEntryForVehicleType(researchItem->related_ride, researchItem->item);
                    }
                    break;
            }
        }
    }

    void AddAvailableEntriesFromMap()
    {
        size_t maxTiles = 128 * 128;
        size_t tileIndex = 0;
        RCT12TileElement* tileElement = _s4.tile_elements;

        while (tileIndex < maxTiles)
        {
            switch (tileElement->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    uint8_t pathType = tileElement->AsPath()->GetRCT1PathType();
                    uint8_t pathAdditionsType = tileElement->AsPath()->GetAddition();

                    AddEntryForPath(pathType);
                    AddEntryForPathAddition(pathAdditionsType);
                    break;
                }
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                    AddEntryForSmallScenery(tileElement->AsSmallScenery()->GetEntryIndex());
                    break;
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                    AddEntryForLargeScenery(tileElement->AsLargeScenery()->GetEntryIndex());
                    break;
                case TILE_ELEMENT_TYPE_WALL:
                {
                    for (int32_t edge = 0; edge < 4; edge++)
                    {
                        int32_t type = tileElement->AsWall()->GetRCT1WallType(edge);

                        if (type != -1)
                        {
                            AddEntryForWall(type);
                        }
                    }
                    break;
                }
            }

            if ((tileElement++)->IsLastForTile())
            {
                tileIndex++;
            }
        }
    }

    void AddAvailableEntriesFromRides()
    {
        for (size_t i = 0; i < std::size(_s4.rides); i++)
        {
            auto ride = &_s4.rides[i];
            if (ride->type != RCT1_RIDE_TYPE_NULL)
            {
                if (RCT1::RideTypeUsesVehicles(ride->type))
                    AddEntryForVehicleType(ride->type, ride->vehicle_type);
                else
                    AddEntryForRideType(ride->type);
            }
        }
    }

    void AddAvailableEntriesFromSceneryGroups()
    {
        for (int32_t sceneryTheme = 0; sceneryTheme <= RCT1_SCENERY_THEME_PAGODA; sceneryTheme++)
        {
            if (sceneryTheme != 0 && _sceneryThemeTypeToEntryMap[sceneryTheme] == OBJECT_ENTRY_INDEX_NULL)
                continue;

            std::vector<const char*> objects = RCT1::GetSceneryObjects(sceneryTheme);
            for (const char* objectName : objects)
            {
                auto& objectRepository = OpenRCT2::GetContext()->GetObjectRepository();
                auto foundObject = objectRepository.FindObject(objectName);
                if (foundObject != nullptr)
                {
                    uint8_t objectType = foundObject->ObjectEntry.GetType();
                    switch (objectType)
                    {
                        case OBJECT_TYPE_SMALL_SCENERY:
                        case OBJECT_TYPE_LARGE_SCENERY:
                        case OBJECT_TYPE_WALLS:
                        case OBJECT_TYPE_PATHS:
                        case OBJECT_TYPE_PATH_BITS:
                            EntryList* entries = GetEntryList(objectType);

                            // Check if there are spare entries available
                            size_t maxEntries = static_cast<size_t>(object_entry_group_counts[objectType]);
                            if (entries != nullptr && entries->GetCount() < maxEntries)
                            {
                                entries->GetOrAddEntry(objectName);
                            }
                            break;
                    }
                }
            }
        }
    }

    void AddEntryForWater()
    {
        const char* entryName;

        if (_gameVersion < FILE_VERSION_RCT1_LL)
        {
            entryName = RCT1::GetWaterObject(RCT1_WATER_CYAN);
        }
        else
        {
            entryName = RCT1::GetWaterObject(_s4.water_colour);
        }

        _waterEntry.GetOrAddEntry(entryName);
    }

    void AddEntryForRideType(uint8_t rideType)
    {
        assert(rideType < std::size(_rideTypeToRideEntryMap));
        if (_rideTypeToRideEntryMap[rideType] == RIDE_ENTRY_INDEX_NULL)
        {
            const char* entryName = RCT1::GetRideTypeObject(rideType);
            if (!String::Equals(entryName, "        "))
            {
                auto entryIndex = _rideEntries.GetOrAddEntry(entryName);
                _rideTypeToRideEntryMap[rideType] = entryIndex;
            }
        }
    }

    void AddEntryForVehicleType(uint8_t rideType, uint8_t vehicleType)
    {
        assert(vehicleType < std::size(_vehicleTypeToRideEntryMap));
        if (_vehicleTypeToRideEntryMap[vehicleType] == RIDE_ENTRY_INDEX_NULL)
        {
            const char* entryName = RCT1::GetVehicleObject(vehicleType);
            if (!String::Equals(entryName, "        "))
            {
                auto entryIndex = _rideEntries.GetOrAddEntry(entryName);
                _vehicleTypeToRideEntryMap[vehicleType] = entryIndex;

                if (rideType != RIDE_TYPE_NULL)
                    AddEntryForRideType(rideType);
            }
        }
    }

    void AddEntryForSmallScenery(ObjectEntryIndex smallSceneryType)
    {
        assert(smallSceneryType < std::size(_smallSceneryTypeToEntryMap));
        if (_smallSceneryTypeToEntryMap[smallSceneryType] == OBJECT_ENTRY_INDEX_NULL)
        {
            const char* entryName = RCT1::GetSmallSceneryObject(smallSceneryType);
            auto entryIndex = _smallSceneryEntries.GetOrAddEntry(entryName);

            _smallSceneryTypeToEntryMap[smallSceneryType] = entryIndex;
        }
    }

    void AddEntryForLargeScenery(ObjectEntryIndex largeSceneryType)
    {
        assert(largeSceneryType < std::size(_largeSceneryTypeToEntryMap));
        if (_largeSceneryTypeToEntryMap[largeSceneryType] == OBJECT_ENTRY_INDEX_NULL)
        {
            const char* entryName = RCT1::GetLargeSceneryObject(largeSceneryType);
            auto entryIndex = _largeSceneryEntries.GetOrAddEntry(entryName);

            _largeSceneryTypeToEntryMap[largeSceneryType] = entryIndex;
        }
    }

    void AddEntryForWall(ObjectEntryIndex wallType)
    {
        assert(wallType < std::size(_wallTypeToEntryMap));
        if (_wallTypeToEntryMap[wallType] == OBJECT_ENTRY_INDEX_NULL)
        {
            const char* entryName = RCT1::GetWallObject(wallType);
            auto entryIndex = _wallEntries.GetOrAddEntry(entryName);

            _wallTypeToEntryMap[wallType] = entryIndex;
        }
    }

    void AddEntryForPath(ObjectEntryIndex pathType)
    {
        assert(pathType < std::size(_pathTypeToEntryMap));
        if (_pathTypeToEntryMap[pathType] == OBJECT_ENTRY_INDEX_NULL)
        {
            const char* entryName = RCT1::GetPathObject(pathType);
            if (!String::Equals(entryName, "        "))
            {
                auto entryIndex = _pathEntries.GetOrAddEntry(entryName);
                _pathTypeToEntryMap[pathType] = entryIndex;
            }
        }
    }

    void AddEntryForPathAddition(ObjectEntryIndex pathAdditionType)
    {
        if (pathAdditionType == RCT1_PATH_ADDITION_NONE)
            return;

        if (_pathAdditionTypeToEntryMap[pathAdditionType] == OBJECT_ENTRY_INDEX_NULL)
        {
            uint8_t normalisedPathAdditionType = RCT1::NormalisePathAddition(pathAdditionType);
            if (_pathAdditionTypeToEntryMap[normalisedPathAdditionType] == OBJECT_ENTRY_INDEX_NULL)
            {
                const char* entryName = RCT1::GetPathAddtionObject(normalisedPathAdditionType);
                auto entryIndex = _pathAdditionEntries.GetOrAddEntry(entryName);

                _pathAdditionTypeToEntryMap[normalisedPathAdditionType] = entryIndex;
            }

            _pathAdditionTypeToEntryMap[pathAdditionType] = _pathAdditionTypeToEntryMap[normalisedPathAdditionType];
        }
    }

    void AddEntriesForSceneryTheme(ObjectEntryIndex sceneryThemeType)
    {
        if (sceneryThemeType == RCT1_SCENERY_THEME_GENERAL || sceneryThemeType == RCT1_SCENERY_THEME_JUMPING_FOUNTAINS
            || sceneryThemeType == RCT1_SCENERY_THEME_GARDEN_CLOCK)
        {
            _sceneryThemeTypeToEntryMap[sceneryThemeType] = OBJECT_ENTRY_INDEX_IGNORE;
        }
        else
        {
            const char* entryName = RCT1::GetSceneryGroupObject(sceneryThemeType);
            if (_sceneryGroupEntries.GetCount() >= MAX_SCENERY_GROUP_OBJECTS)
            {
                Console::WriteLine("Warning: More than %d (max scenery groups) in RCT1 park.", MAX_SCENERY_GROUP_OBJECTS);
                Console::WriteLine("         [%s] scenery group not added.", entryName);
            }
            else
            {
                auto entryIndex = _sceneryGroupEntries.GetOrAddEntry(entryName);
                _sceneryThemeTypeToEntryMap[sceneryThemeType] = entryIndex;
            }
        }
    }

    void ImportRides()
    {
        for (int32_t i = 0; i < RCT12_MAX_RIDES_IN_PARK; i++)
        {
            if (_s4.rides[i].type != RIDE_TYPE_NULL)
            {
                ImportRide(GetOrAllocateRide(i), &_s4.rides[i], i);
            }
        }
    }

    void ImportRide(Ride* dst, rct1_ride* src, ride_id_t rideIndex)
    {
        *dst = {};
        dst->id = rideIndex;

        // This is a peculiarity of this exact version number, which only Heide-Park seems to use.
        if (_s4.game_version == 110018 && src->type == RCT1_RIDE_TYPE_INVERTED_ROLLER_COASTER)
        {
            dst->type = RIDE_TYPE_COMPACT_INVERTED_COASTER;
        }
        else
        {
            dst->type = RCT1::GetRideType(src->type, src->vehicle_type);
        }

        if (RCT1::RideTypeUsesVehicles(src->type))
        {
            dst->subtype = _vehicleTypeToRideEntryMap[src->vehicle_type];
        }
        else
        {
            dst->subtype = _rideTypeToRideEntryMap[src->type];
        }

        rct_ride_entry* rideEntry = get_ride_entry(dst->subtype);
        // This can happen with hacked parks
        if (rideEntry == nullptr)
        {
            log_warning("Discarding ride with invalid ride entry");
            dst->type = RIDE_TYPE_NULL;
            return;
        }

        // Ride name
        if (is_user_string_id(src->name))
        {
            dst->custom_name = GetUserString(src->name);
        }

        dst->status = src->status;

        // Flags
        dst->lifecycle_flags = src->lifecycle_flags;
        // These flags were not in the base game
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->lifecycle_flags &= ~RIDE_LIFECYCLE_MUSIC;
            dst->lifecycle_flags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE;
            dst->lifecycle_flags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
        }

        // Station
        if (src->overall_view.isNull())
        {
            dst->overall_view.setNull();
        }
        else
        {
            dst->overall_view = TileCoordsXY{ src->overall_view.x, src->overall_view.y }.ToCoordsXY();
        }

        for (int32_t i = 0; i < RCT12_MAX_STATIONS_PER_RIDE; i++)
        {
            if (src->station_starts[i].isNull())
            {
                dst->stations[i].Start.setNull();
            }
            else
            {
                auto tileStartLoc = TileCoordsXY{ src->station_starts[i].x, src->station_starts[i].y };
                dst->stations[i].Start = tileStartLoc.ToCoordsXY();
            }
            dst->stations[i].SetBaseZ(src->station_height[i] * RCT1_COORDS_Z_STEP);
            dst->stations[i].Length = src->station_length[i];
            dst->stations[i].Depart = src->station_light[i];

            dst->stations[i].TrainAtStation = src->station_depart[i];

            // Direction is fixed later.
            if (src->entrance[i].isNull())
                ride_clear_entrance_location(dst, i);
            else
                ride_set_entrance_location(dst, i, { src->entrance[i].x, src->entrance[i].y, src->station_height[i] / 2, 0 });

            if (src->exit[i].isNull())
                ride_clear_exit_location(dst, i);
            else
                ride_set_exit_location(dst, i, { src->exit[i].x, src->exit[i].y, src->station_height[i] / 2, 0 });

            dst->stations[i].QueueTime = src->queue_time[i];
            dst->stations[i].LastPeepInQueue = src->last_peep_in_queue[i];
            dst->stations[i].QueueLength = src->num_peeps_in_queue[i];

            dst->stations[i].SegmentTime = src->time[i];
            dst->stations[i].SegmentLength = src->length[i];
        }
        // All other values take 0 as their default. Since they're already memset to that, no need to do it again.
        for (int32_t i = RCT12_MAX_STATIONS_PER_RIDE; i < MAX_STATIONS; i++)
        {
            dst->stations[i].Start.setNull();
            dst->stations[i].TrainAtStation = RideStation::NO_TRAIN;
            ride_clear_entrance_location(dst, i);
            ride_clear_exit_location(dst, i);
            dst->stations[i].LastPeepInQueue = SPRITE_INDEX_NULL;
        }

        dst->num_stations = src->num_stations;

        // Vehicle links (indexes converted later)
        for (int32_t i = 0; i < RCT1_MAX_TRAINS_PER_RIDE; i++)
        {
            dst->vehicles[i] = src->vehicles[i];
        }
        for (int32_t i = RCT1_MAX_TRAINS_PER_RIDE; i <= MAX_VEHICLES_PER_RIDE; i++)
        {
            dst->vehicles[i] = SPRITE_INDEX_NULL;
        }

        dst->num_vehicles = src->num_trains;
        dst->num_cars_per_train = src->num_cars_per_train + rideEntry->zero_cars;
        dst->proposed_num_vehicles = src->num_trains;
        dst->max_trains = src->max_trains;
        dst->proposed_num_cars_per_train = src->num_cars_per_train + rideEntry->zero_cars;
        dst->special_track_elements = src->special_track_elements;
        dst->num_sheltered_sections = src->num_sheltered_sections;
        dst->sheltered_length = src->sheltered_length;

        // Operation
        dst->depart_flags = src->depart_flags;
        dst->min_waiting_time = src->min_waiting_time;
        dst->max_waiting_time = src->max_waiting_time;
        dst->operation_option = src->operation_option;
        dst->num_circuits = 1;
        dst->min_max_cars_per_train = (rideEntry->min_cars_in_train << 4) | rideEntry->max_cars_in_train;

        // RCT1 used 5mph / 8 km/h for every lift hill
        dst->lift_hill_speed = 5;

        if (_gameVersion == FILE_VERSION_RCT1)
        {
            // Original RCT had no music settings, take default style
            dst->music = RideTypeDescriptors[dst->type].DefaultMusic;

            // Only merry-go-round and dodgems had music and used
            // the same flag as synchronise stations for the option to enable it
            if (src->type == RCT1_RIDE_TYPE_MERRY_GO_ROUND || src->type == RCT1_RIDE_TYPE_DODGEMS)
            {
                if (src->depart_flags & RCT1_RIDE_DEPART_PLAY_MUSIC)
                {
                    dst->depart_flags &= ~RCT1_RIDE_DEPART_PLAY_MUSIC;
                    dst->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
                }
            }
        }
        else
        {
            dst->music = src->music;
        }

        if (src->operating_mode == RCT1_RIDE_MODE_POWERED_LAUNCH)
        {
            // Launched rides never passed through the station in RCT1.
            dst->mode = RIDE_MODE_POWERED_LAUNCH;
        }
        else
        {
            dst->mode = src->operating_mode;
        }

        SetRideColourScheme(dst, src);

        // Maintenance
        dst->build_date = static_cast<int32_t>(src->build_date);
        dst->inspection_interval = src->inspection_interval;
        dst->last_inspection = src->last_inspection;
        dst->reliability = src->reliability;
        dst->unreliability_factor = src->unreliability_factor;
        dst->downtime = src->downtime;
        dst->breakdown_reason = src->breakdown_reason;
        dst->mechanic_status = src->mechanic_status;
        dst->mechanic = src->mechanic;
        dst->breakdown_reason_pending = src->breakdown_reason_pending;
        dst->inspection_station = src->inspection_station;
        dst->broken_car = src->broken_car;
        dst->broken_vehicle = src->broken_vehicle;

        // Measurement data
        dst->excitement = src->excitement;
        dst->intensity = src->intensity;
        dst->nausea = src->nausea;

        dst->max_speed = src->max_speed;
        dst->average_speed = src->average_speed;

        dst->max_positive_vertical_g = src->max_positive_vertical_g;
        dst->max_negative_vertical_g = src->max_negative_vertical_g;
        dst->max_lateral_g = src->max_lateral_g;
        dst->previous_lateral_g = src->previous_lateral_g;
        dst->previous_vertical_g = src->previous_vertical_g;
        dst->turn_count_banked = src->turn_count_banked;
        dst->turn_count_default = src->turn_count_default;
        dst->turn_count_sloped = src->turn_count_sloped;
        dst->drops = src->num_drops;
        dst->start_drop_height = src->start_drop_height / 2;
        dst->highest_drop_height = src->highest_drop_height / 2;
        if (dst->type == RIDE_TYPE_MINI_GOLF)
            dst->holes = src->num_inversions & 0x1F;
        else
            dst->inversions = src->num_inversions & 0x1F;
        dst->sheltered_eighths = src->num_inversions >> 5;
        dst->boat_hire_return_direction = src->boat_hire_return_direction;
        dst->boat_hire_return_position = { src->boat_hire_return_position.x, src->boat_hire_return_position.y };
        dst->chairlift_bullwheel_rotation = src->chairlift_bullwheel_rotation;
        for (int i = 0; i < 2; i++)
        {
            dst->ChairliftBullwheelLocation[i] = { src->chairlift_bullwheel_location[i].x,
                                                   src->chairlift_bullwheel_location[i].y, src->chairlift_bullwheel_z[i] / 2 };
        }

        if (src->cur_test_track_location.isNull())
        {
            dst->CurTestTrackLocation.setNull();
        }
        else
        {
            dst->CurTestTrackLocation = { src->cur_test_track_location.x, src->cur_test_track_location.y,
                                          src->cur_test_track_z / 2 };
        }
        dst->testing_flags = src->testing_flags;
        dst->current_test_segment = src->current_test_segment;
        dst->current_test_station = STATION_INDEX_NULL;
        dst->average_speed_test_timeout = src->average_speed_test_timeout;
        dst->slide_in_use = src->slide_in_use;
        dst->slide_peep_t_shirt_colour = RCT1::GetColour(src->slide_peep_t_shirt_colour);
        dst->spiral_slide_progress = src->spiral_slide_progress;
        // Doubles as slide_peep
        dst->maze_tiles = src->maze_tiles;

        // Finance / customers
        dst->upkeep_cost = src->upkeep_cost;
        dst->price[0] = src->price;
        dst->price[1] = src->price_secondary;
        dst->income_per_hour = src->income_per_hour;
        dst->total_customers = src->total_customers;
        dst->profit = src->profit;
        dst->total_profit = src->total_profit;
        dst->value = src->value;

        dst->satisfaction = src->satisfaction;
        dst->satisfaction_time_out = src->satisfaction_time_out;
        dst->satisfaction_next = src->satisfaction_next;
        dst->popularity = src->popularity;
        dst->popularity_next = src->popularity_next;
        dst->popularity_time_out = src->popularity_time_out;

        dst->num_riders = src->num_riders;

        dst->music_tune_id = 255;
    }

    void SetRideColourScheme(Ride* dst, rct1_ride* src)
    {
        // Colours
        dst->colour_scheme_type = src->colour_scheme;
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->track_colour[0].main = RCT1::GetColour(src->track_primary_colour);
            dst->track_colour[0].additional = RCT1::GetColour(src->track_secondary_colour);
            dst->track_colour[0].supports = RCT1::GetColour(src->track_support_colour);

            // Balloons were always blue in the original RCT.
            if (src->type == RCT1_RIDE_TYPE_BALLOON_STALL)
            {
                dst->track_colour[0].main = COLOUR_LIGHT_BLUE;
            }
            else if (src->type == RCT1_RIDE_TYPE_RIVER_RAPIDS)
            {
                dst->track_colour[0].main = COLOUR_WHITE;
            }
        }
        else
        {
            for (int i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++)
            {
                dst->track_colour[i].main = RCT1::GetColour(src->track_colour_main[i]);
                dst->track_colour[i].additional = RCT1::GetColour(src->track_colour_additional[i]);
                dst->track_colour[i].supports = RCT1::GetColour(src->track_colour_supports[i]);
            }
            // Entrance styles were introduced with AA. They correspond directly with those in RCT2.
            dst->entrance_style = src->entrance_style;
        }

        if (_gameVersion < FILE_VERSION_RCT1_LL && dst->type == RIDE_TYPE_MERRY_GO_ROUND)
        {
            // The merry-go-round in pre-LL versions was always yellow with red
            dst->vehicle_colours[0].Body = COLOUR_YELLOW;
            dst->vehicle_colours[0].Trim = COLOUR_BRIGHT_RED;
        }
        else
        {
            for (int i = 0; i < RCT1_MAX_TRAINS_PER_RIDE; i++)
            {
                // RCT1 had no third colour
                RCT1::RCT1VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = RCT1::GetColourSchemeCopyDescriptor(
                    src->vehicle_type);
                if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_1)
                {
                    dst->vehicle_colours[i].Body = RCT1::GetColour(src->vehicle_colours[i].body);
                }
                else if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_2)
                {
                    dst->vehicle_colours[i].Body = RCT1::GetColour(src->vehicle_colours[i].trim);
                }
                else
                {
                    dst->vehicle_colours[i].Body = colourSchemeCopyDescriptor.colour1;
                }

                if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_1)
                {
                    dst->vehicle_colours[i].Trim = RCT1::GetColour(src->vehicle_colours[i].body);
                }
                else if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_2)
                {
                    dst->vehicle_colours[i].Trim = RCT1::GetColour(src->vehicle_colours[i].trim);
                }
                else
                {
                    dst->vehicle_colours[i].Trim = colourSchemeCopyDescriptor.colour2;
                }

                if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_1)
                {
                    dst->vehicle_colours[i].Ternary = RCT1::GetColour(src->vehicle_colours[i].body);
                }
                else if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_2)
                {
                    dst->vehicle_colours[i].Ternary = RCT1::GetColour(src->vehicle_colours[i].trim);
                }
                else
                {
                    dst->vehicle_colours[i].Ternary = colourSchemeCopyDescriptor.colour3;
                }
            }
        }

        // In RCT1 and AA, the maze was always hedges.
        // LL has 4 types, like RCT2. For LL, only guard against invalid values.
        if (dst->type == RIDE_TYPE_MAZE)
        {
            if (_gameVersion < FILE_VERSION_RCT1_LL || src->track_colour_supports[0] > 3)
                dst->track_colour[0].supports = MAZE_WALL_TYPE_HEDGE;
            else
                dst->track_colour[0].supports = src->track_colour_supports[0];
        }
    }

    void FixRideVehicleLinks(const uint16_t* spriteIndexMap)
    {
        for (auto& ride : GetRideManager())
        {
            for (uint8_t j = 0; j < std::size(ride.vehicles); j++)
            {
                uint16_t originalIndex = ride.vehicles[j];
                if (originalIndex != SPRITE_INDEX_NULL)
                {
                    ride.vehicles[j] = spriteIndexMap[originalIndex];
                }
            }
        }
    }

    void ImportRideMeasurements()
    {
        for (const auto& src : _s4.ride_measurements)
        {
            if (src.ride_index != RCT12_RIDE_ID_NULL)
            {
                auto ride = get_ride(src.ride_index);
                if (ride != nullptr)
                {
                    ride->measurement = std::make_unique<RideMeasurement>();
                    ImportRideMeasurement(*ride->measurement, src);
                }
            }
        }
    }

    void ImportRideMeasurement(RideMeasurement& dst, const RCT12RideMeasurement& src)
    {
        dst.flags = src.flags;
        dst.last_use_tick = src.last_use_tick;
        dst.num_items = src.num_items;
        dst.current_item = src.current_item;
        dst.vehicle_index = src.vehicle_index;
        dst.current_station = src.current_station;
        for (size_t i = 0; i < std::size(src.velocity); i++)
        {
            dst.velocity[i] = src.velocity[i] / 2;
            dst.altitude[i] = src.altitude[i] / 2;
            dst.vertical[i] = src.vertical[i] / 2;
            dst.lateral[i] = src.lateral[i] / 2;
        }
    }

    void ImportSprites()
    {
        ImportVehicles();
        ImportPeeps();
        ImportLitter();
        ImportMiscSprites();
    }

    void ImportVehicles()
    {
        std::vector<Vehicle*> vehicles;
        uint16_t spriteIndexMap[RCT1_MAX_SPRITES];
        for (int i = 0; i < RCT1_MAX_SPRITES; i++)
        {
            spriteIndexMap[i] = SPRITE_INDEX_NULL;
            if (_s4.sprites[i].unknown.sprite_identifier == SPRITE_IDENTIFIER_VEHICLE)
            {
                rct1_vehicle* srcVehicle = &_s4.sprites[i].vehicle;
                if (srcVehicle->x != LOCATION_NULL)
                {
                    // If vehicle is the first car on a train add to train list
                    auto isFirstCar = srcVehicle->type == static_cast<uint8_t>(Vehicle::Type::Head);
                    auto llt = isFirstCar ? EntityListId::TrainHead : EntityListId::Vehicle;

                    Vehicle* vehicle = reinterpret_cast<Vehicle*>(create_sprite(SPRITE_IDENTIFIER_VEHICLE, llt));
                    spriteIndexMap[i] = vehicle->sprite_index;
                    vehicles.push_back(vehicle);

                    ImportVehicle(vehicle, srcVehicle);
                }
            }
        }
        for (auto vehicle : vehicles)
        {
            FixVehicleLinks(vehicle, spriteIndexMap);
        }
        FixRideVehicleLinks(spriteIndexMap);
    }

    void ImportVehicle(Vehicle* dst, rct1_vehicle* src)
    {
        const auto* ride = get_ride(src->ride);
        if (ride == nullptr)
            return;

        uint8_t vehicleEntryIndex = RCT1::GetVehicleSubEntryIndex(src->vehicle_type);

        dst->sprite_identifier = SPRITE_IDENTIFIER_VEHICLE;
        dst->ride = src->ride;
        dst->ride_subtype = RCTEntryIndexToOpenRCT2EntryIndex(ride->subtype);

        dst->vehicle_type = vehicleEntryIndex;
        dst->type = src->type;
        dst->var_44 = src->var_44;
        dst->remaining_distance = src->remaining_distance;

        // Properties from vehicle entry
        dst->sprite_width = src->sprite_width;
        dst->sprite_height_negative = src->sprite_height_negative;
        dst->sprite_height_positive = src->sprite_height_positive;
        dst->sprite_direction = src->sprite_direction;

        dst->sprite_left = src->sprite_left;
        dst->sprite_top = src->sprite_top;
        dst->sprite_right = src->sprite_right;
        dst->sprite_bottom = src->sprite_bottom;

        dst->mass = src->mass;
        dst->num_seats = src->num_seats;
        dst->speed = src->speed;
        dst->powered_acceleration = src->powered_acceleration;
        dst->brake_speed = src->brake_speed;

        dst->velocity = src->velocity;
        dst->acceleration = src->acceleration;
        dst->SwingSprite = src->SwingSprite;
        dst->SwingPosition = src->SwingPosition;
        dst->SwingSpeed = src->SwingSpeed;
        dst->restraints_position = src->restraints_position;
        dst->spin_sprite = src->spin_sprite;
        dst->sound_vector_factor = src->sound_vector_factor;
        dst->spin_speed = src->spin_speed;
        dst->sound2_flags = src->sound2_flags;
        dst->sound1_id = SoundId::Null;
        dst->sound2_id = SoundId::Null;
        dst->var_C0 = src->var_C0;
        dst->var_C4 = src->var_C4;
        dst->animation_frame = src->animation_frame;
        dst->var_C8 = src->var_C8;
        dst->var_CA = src->var_CA;
        dst->var_CE = src->var_CE;
        dst->var_D3 = src->var_D3;
        dst->scream_sound_id = SoundId::Null;
        dst->vehicle_sprite_type = src->vehicle_sprite_type;
        dst->bank_rotation = src->bank_rotation;

        // Seat rotation was not in RCT1
        dst->target_seat_rotation = DEFAULT_SEAT_ROTATION;
        dst->seat_rotation = DEFAULT_SEAT_ROTATION;

        // Vehicle links (indexes converted later)
        dst->prev_vehicle_on_ride = src->prev_vehicle_on_ride;
        dst->next_vehicle_on_ride = src->next_vehicle_on_ride;
        dst->next_vehicle_on_train = src->next_vehicle_on_train;

        // Guests (indexes converted later)
        for (int i = 0; i < 32; i++)
        {
            uint16_t spriteIndex = src->peep[i];
            dst->peep[i] = spriteIndex;
            if (spriteIndex != SPRITE_INDEX_NULL)
            {
                dst->peep_tshirt_colours[i] = RCT1::GetColour(src->peep_tshirt_colours[i]);
            }
        }

        Vehicle::Status statusSrc = Vehicle::Status::MovingToEndOfStation;
        if (src->status <= static_cast<uint8_t>(Vehicle::Status::StoppedByBlockBrakes))
        {
            statusSrc = static_cast<Vehicle::Status>(src->status);
        }
        dst->status = statusSrc;
        dst->TrackSubposition = VehicleTrackSubposition{ src->TrackSubposition };
        dst->TrackLocation = { src->track_x, src->track_y, src->track_z };
        dst->current_station = src->current_station;
        if (src->boat_location.isNull() || ride->mode != RIDE_MODE_BOAT_HIRE || statusSrc != Vehicle::Status::TravellingBoat)
        {
            dst->BoatLocation.setNull();
            dst->track_type = src->track_type;
        }
        else
        {
            dst->BoatLocation = TileCoordsXY{ src->boat_location.x, src->boat_location.y }.ToCoordsXY();
            dst->track_type = 0;
        }
        dst->track_progress = src->track_progress;
        dst->vertical_drop_countdown = src->vertical_drop_countdown;
        dst->sub_state = src->sub_state;
        dst->update_flags = src->update_flags;

        SetVehicleColours(dst, src);

        dst->mini_golf_current_animation = src->mini_golf_current_animation;
        dst->mini_golf_flags = src->mini_golf_flags;

        dst->MoveTo({ src->x, src->y, src->z });

        dst->num_peeps = src->num_peeps;
        dst->next_free_seat = src->next_free_seat;
    }

    void SetVehicleColours(Vehicle* dst, rct1_vehicle* src)
    {
        rct1_ride* srcRide = &_s4.rides[src->ride];
        uint8_t vehicleTypeIndex = srcRide->vehicle_type;
        RCT1::RCT1VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = RCT1::GetColourSchemeCopyDescriptor(
            vehicleTypeIndex);

        // RCT1 had no third colour
        if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_1)
        {
            dst->colours.body_colour = RCT1::GetColour(src->colours.body_colour);
        }
        else if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_2)
        {
            dst->colours.body_colour = RCT1::GetColour(src->colours.trim_colour);
        }
        else
        {
            dst->colours.body_colour = colourSchemeCopyDescriptor.colour1;
        }

        if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_1)
        {
            dst->colours.trim_colour = RCT1::GetColour(src->colours.body_colour);
        }
        else if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_2)
        {
            dst->colours.trim_colour = RCT1::GetColour(src->colours.trim_colour);
        }
        else
        {
            dst->colours.trim_colour = colourSchemeCopyDescriptor.colour2;
        }

        if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_1)
        {
            dst->colours_extended = RCT1::GetColour(src->colours.body_colour);
        }
        else if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_2)
        {
            dst->colours_extended = RCT1::GetColour(src->colours.trim_colour);
        }
        else
        {
            dst->colours_extended = colourSchemeCopyDescriptor.colour3;
        }
    }

    void FixVehicleLinks(Vehicle* vehicle, const uint16_t* spriteIndexMap)
    {
        if (vehicle->prev_vehicle_on_ride != SPRITE_INDEX_NULL)
        {
            vehicle->prev_vehicle_on_ride = spriteIndexMap[vehicle->prev_vehicle_on_ride];
        }
        if (vehicle->next_vehicle_on_ride != SPRITE_INDEX_NULL)
        {
            vehicle->next_vehicle_on_ride = spriteIndexMap[vehicle->next_vehicle_on_ride];
        }
        if (vehicle->next_vehicle_on_train != SPRITE_INDEX_NULL)
        {
            vehicle->next_vehicle_on_train = spriteIndexMap[vehicle->next_vehicle_on_train];
        }
    }

    void FixVehiclePeepLinks(Vehicle* vehicle, const uint16_t* spriteIndexMap)
    {
        for (auto& peep : vehicle->peep)
        {
            peep = MapSpriteIndex(peep, spriteIndexMap);
        }
    }

    void ImportPeeps()
    {
        uint16_t spriteIndexMap[RCT1_MAX_SPRITES];
        for (size_t i = 0; i < RCT1_MAX_SPRITES; i++)
        {
            spriteIndexMap[i] = SPRITE_INDEX_NULL;
            if (_s4.sprites[i].unknown.sprite_identifier == SPRITE_IDENTIFIER_PEEP)
            {
                rct1_peep* srcPeep = &_s4.sprites[i].peep;
                Peep* peep = reinterpret_cast<Peep*>(create_sprite(SPRITE_IDENTIFIER_PEEP));
                spriteIndexMap[i] = peep->sprite_index;

                ImportPeep(peep, srcPeep);
            }
        }
        for (size_t i = 0; i < MAX_SPRITES; i++)
        {
            auto vehicle = GetEntity<Vehicle>(i);
            if (vehicle != nullptr)
            {
                FixVehiclePeepLinks(vehicle, spriteIndexMap);
            }
        }

        for (auto& ride : GetRideManager())
        {
            FixRidePeepLinks(&ride, spriteIndexMap);
        }

        {
            for (auto peep : EntityList<Guest>(EntityListId::Peep))
            {
                FixPeepNextInQueue(peep, spriteIndexMap);
            }
        }
        // Fix the news items in advance
        for (auto i = 0; i < News::MaxItems; i++)
        {
            rct12_news_item* newsItem = &_s4.messages[i];
            News::ItemType type = static_cast<News::ItemType>(newsItem->Type);

            if (type == News::ItemType::Peep || type == News::ItemType::PeepOnRide)
            {
                newsItem->Assoc = MapSpriteIndex(newsItem->Assoc, spriteIndexMap);
            }
        }

        // The RCT2/OpenRCT2 structures are bigger than in RCT1, so set them to zero
        std::fill(std::begin(gStaffModes), std::end(gStaffModes), StaffMode::None);
        std::fill(std::begin(gStaffPatrolAreas), std::end(gStaffPatrolAreas), 0);

        std::fill(std::begin(_s4.staff_modes), std::end(_s4.staff_modes), 0);

        for (auto peep : EntityList<Staff>(EntityListId::Peep))
        {
            ImportStaffPatrolArea(peep);
        }
        // Only the individual patrol areas have been converted, so generate the combined patrol areas of each staff type
        staff_update_greyed_patrol_areas();
    }

    void ImportPeep(Peep* dst, rct1_peep* src)
    {
        dst->sprite_identifier = SPRITE_IDENTIFIER_PEEP;
        // Peep vs. staff (including which kind)
        dst->SpriteType = RCT1::GetPeepSpriteType(src->sprite_type);
        dst->Action = static_cast<PeepActionType>(src->action);
        dst->SpecialSprite = src->special_sprite;
        dst->NextActionSpriteType = static_cast<PeepActionSpriteType>(src->next_action_sprite_type);
        dst->ActionSpriteImageOffset = src->action_sprite_image_offset;
        dst->WalkingFrameNum = src->no_action_frame_num;
        dst->ActionSpriteType = static_cast<PeepActionSpriteType>(src->action_sprite_type);
        dst->ActionFrame = src->action_frame;

        const rct_sprite_bounds* spriteBounds = g_peep_animation_entries[dst->SpriteType].sprite_bounds;
        dst->sprite_width = spriteBounds[dst->ActionSpriteType].sprite_width;
        dst->sprite_height_negative = spriteBounds[dst->ActionSpriteType].sprite_height_negative;
        dst->sprite_height_positive = spriteBounds[dst->ActionSpriteType].sprite_height_positive;

        dst->MoveTo({ src->x, src->y, src->z });
        dst->Invalidate2();

        dst->sprite_direction = src->sprite_direction;

        // Peep name
        if (is_user_string_id(src->name_string_idx))
        {
            dst->SetName(GetUserString(src->name_string_idx));
        }

        dst->OutsideOfPark = static_cast<bool>(src->outside_of_park);

        dst->State = static_cast<PeepState>(src->state);
        dst->SubState = src->sub_state;
        dst->NextLoc = { src->next_x, src->next_y, src->next_z * RCT1_COORDS_Z_STEP };
        dst->NextFlags = src->next_flags;
        dst->Var37 = src->var_37;
        dst->TimeToConsume = src->time_to_consume;
        dst->StepProgress = src->step_progress;
        dst->VandalismSeen = src->vandalism_seen;

        dst->AssignedPeepType = static_cast<PeepType>(src->type);

        dst->TshirtColour = RCT1::GetColour(src->tshirt_colour);
        dst->TrousersColour = RCT1::GetColour(src->trousers_colour);
        dst->UmbrellaColour = RCT1::GetColour(src->umbrella_colour);
        dst->HatColour = RCT1::GetColour(src->hat_colour);

        // Balloons were always blue in RCT1 without AA/LL
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->BalloonColour = COLOUR_LIGHT_BLUE;
        }
        else
        {
            dst->BalloonColour = RCT1::GetColour(src->balloon_colour);
        }

        dst->DestinationX = src->destination_x;
        dst->DestinationY = src->destination_y;
        dst->DestinationTolerance = src->destination_tolerance;
        dst->PeepDirection = src->direction;

        dst->Energy = src->energy;
        dst->EnergyTarget = src->energy_target;
        dst->Happiness = src->happiness;
        dst->HappinessTarget = src->happiness_target;
        dst->Nausea = src->nausea;
        dst->NauseaTarget = src->nausea_target;
        dst->Hunger = src->hunger;
        dst->Thirst = src->thirst;
        dst->Toilet = src->toilet;
        dst->Mass = src->mass;

        dst->LitterCount = src->litter_count;
        dst->DisgustingCount = src->disgusting_count;

        dst->Intensity = static_cast<IntensityRange>(src->intensity);
        dst->NauseaTolerance = src->nausea_tolerance;
        dst->WindowInvalidateFlags = 0;

        dst->CurrentRide = src->current_ride;
        dst->CurrentRideStation = src->current_ride_station;
        dst->CurrentTrain = src->current_train;
        dst->CurrentCar = src->current_car;
        dst->CurrentSeat = src->current_seat;
        dst->GuestTimeOnRide = src->time_on_ride;
        dst->DaysInQueue = src->days_in_queue;

        dst->InteractionRideIndex = src->interaction_ride_index;

        dst->Id = src->id;
        dst->CashInPocket = src->cash_in_pocket;
        dst->CashSpent = src->cash_spent;
        dst->TimeInPark = src->time_in_park;

        // This doubles as staff type
        dst->GuestNumRides = src->no_of_rides;

        dst->AmountOfDrinks = src->no_of_drinks;
        dst->AmountOfFood = src->no_of_food;
        dst->AmountOfSouvenirs = src->no_of_souvenirs;

        dst->PaidToEnter = src->paid_to_enter;
        dst->PaidOnRides = src->paid_on_rides;
        dst->PaidOnDrink = src->paid_on_drink;
        dst->PaidOnFood = src->paid_on_food;
        dst->PaidOnSouvenirs = src->paid_on_souvenirs;

        dst->VoucherRideId = src->voucher_arguments;
        dst->VoucherType = src->voucher_type;

        dst->SurroundingsThoughtTimeout = src->surroundings_thought_timeout;
        dst->Angriness = src->angriness;
        dst->TimeLost = src->time_lost;

        for (size_t i = 0; i < 32; i++)
        {
            dst->RidesBeenOn[i] = src->rides_been_on[i];
        }
        for (size_t i = 0; i < 16; i++)
        {
            dst->RideTypesBeenOn[i] = src->ride_types_been_on[i];
        }

        dst->Photo1RideRef = src->photo1_ride_ref;

        for (size_t i = 0; i < std::size(src->thoughts); i++)
        {
            auto srcThought = &src->thoughts[i];
            auto dstThought = &dst->Thoughts[i];
            dstThought->type = static_cast<PeepThoughtType>(srcThought->type);
            dstThought->item = srcThought->item;
            dstThought->freshness = srcThought->freshness;
            dstThought->fresh_timeout = srcThought->fresh_timeout;
        }

        dst->PreviousRide = src->previous_ride;
        dst->PreviousRideTimeOut = src->previous_ride_time_out;

        dst->PathCheckOptimisation = 0;
        dst->GuestHeadingToRideId = src->guest_heading_to_ride_id;
        // Doubles as staff orders
        dst->GuestIsLostCountdown = src->peep_is_lost_countdown;
        // The ID is fixed later
        dst->GuestNextInQueue = src->next_in_queue;

        dst->PeepFlags = 0;
        dst->PathfindGoal.x = 0xFF;
        dst->PathfindGoal.y = 0xFF;
        dst->PathfindGoal.z = 0xFF;
        dst->PathfindGoal.direction = INVALID_DIRECTION;

        // Guests' favourite ride was only saved in LL.
        // Set it to N/A if the save comes from the original or AA.
        if (_gameVersion == FILE_VERSION_RCT1_LL)
        {
            dst->FavouriteRide = src->favourite_ride;
            dst->FavouriteRideRating = src->favourite_ride_rating;
        }
        else
        {
            dst->FavouriteRide = RIDE_ID_NULL;
            dst->FavouriteRideRating = 0;
        }

        dst->ItemStandardFlags = src->item_standard_flags;

        if (dst->AssignedPeepType == PeepType::Guest)
        {
            if (dst->OutsideOfPark && dst->State != PEEP_STATE_LEAVING_PARK)
            {
                increment_guests_heading_for_park();
            }
            else
            {
                increment_guests_in_park();
            }
        }
    }

    void FixRidePeepLinks(Ride* ride, const uint16_t* spriteIndexMap)
    {
        for (auto& station : ride->stations)
        {
            station.LastPeepInQueue = MapSpriteIndex(station.LastPeepInQueue, spriteIndexMap);
        }
        ride->mechanic = MapSpriteIndex(ride->mechanic, spriteIndexMap);
        if (ride->type == RIDE_TYPE_SPIRAL_SLIDE)
        {
            ride->slide_peep = MapSpriteIndex(ride->slide_peep, spriteIndexMap);
        }
    }

    void FixPeepNextInQueue(Peep* peep, const uint16_t* spriteIndexMap)
    {
        peep->GuestNextInQueue = MapSpriteIndex(peep->GuestNextInQueue, spriteIndexMap);
    }

    void ImportStaffPatrolArea(Peep* staffmember)
    {
        // The patrol areas in RCT1 are encoded as follows, for coordinates x and y, separately for every staff member:
        // - Chop off the 7 lowest bits of the x and y coordinates, which leaves 5 bits per coordinate.
        //   This step also "produces" the 4x4 patrol squares.
        // - Append the two bitstrings to a 10-bit value like so: yyyyyxxxxx
        // - Use this 10-bit value as an index into an 8-bit array. The array is sized such that every 4x4 square
        //   used for the patrols on the map has a bit in that array. If a bit is 1, that square is part of the patrol.
        //   The correct bit position in that array is found like this: yyyyyxx|xxx
        //                                          index in the array ----^     ^--- bit position in the 8-bit value
        // We do the opposite in this function to recover the x and y values.

        int32_t peepOffset = staffmember->StaffId * RCT12_PATROL_AREA_SIZE;
        for (int32_t i = 0; i < RCT12_PATROL_AREA_SIZE; i++)
        {
            if (_s4.patrol_areas[peepOffset + i] == 0)
            {
                // No patrol for this area
                continue;
            }

            // Loop over the bits of the uint8_t
            for (int32_t j = 0; j < 8; j++)
            {
                int8_t bit = (_s4.patrol_areas[peepOffset + i] >> j) & 1;
                if (bit == 0)
                {
                    // No patrol for this area
                    continue;
                }
                // val contains the 5 highest bits of both the x and y coordinates
                int32_t val = j | (i << 3);
                int32_t x = val & 0x1F;
                x <<= 7;
                int32_t y = val & 0x3E0;
                y <<= 2;
                staff_set_patrol_area(staffmember->StaffId, { x, y }, true);
            }
        }
    }

    void ImportLitter()
    {
        for (auto& sprite : _s4.sprites)
        {
            if (sprite.unknown.sprite_identifier == SPRITE_IDENTIFIER_LITTER)
            {
                const auto* srcLitter = &sprite.litter;

                Litter* litter = reinterpret_cast<Litter*>(create_sprite(SPRITE_IDENTIFIER_LITTER));
                litter->sprite_identifier = srcLitter->sprite_identifier;
                litter->type = srcLitter->type;

                litter->x = srcLitter->x;
                litter->y = srcLitter->y;
                litter->z = srcLitter->z;
                litter->sprite_direction = srcLitter->sprite_direction;
                litter->sprite_width = srcLitter->sprite_width;
                litter->sprite_height_positive = srcLitter->sprite_height_positive;
                litter->sprite_height_negative = srcLitter->sprite_height_negative;

                litter->MoveTo({ srcLitter->x, srcLitter->y, srcLitter->z });
                litter->Invalidate2();
            }
        }
    }

    void ImportMiscSprites()
    {
        for (auto& sprite : _s4.sprites)
        {
            if (sprite.unknown.sprite_identifier == SPRITE_IDENTIFIER_MISC)
            {
                rct1_unk_sprite* src = &sprite.unknown;
                SpriteGeneric* dst = reinterpret_cast<SpriteGeneric*>(create_sprite(SPRITE_IDENTIFIER_MISC));
                if (dst == nullptr)
                {
                    log_warning("SV4 has too many misc entities. No more misc entities will be imported!");
                    break;
                }
                dst->sprite_identifier = src->sprite_identifier;
                dst->type = src->type;
                dst->flags = src->flags;
                dst->sprite_direction = src->sprite_direction;
                dst->sprite_width = src->sprite_width;
                dst->sprite_height_negative = src->sprite_height_negative;
                dst->sprite_height_positive = src->sprite_height_positive;

                dst->MoveTo({ src->x, src->y, src->z });

                switch (src->type)
                {
                    case SPRITE_MISC_STEAM_PARTICLE:
                        ImportSteamParticle(dst->As<SteamParticle>(), reinterpret_cast<RCT12SpriteSteamParticle*>(src));
                        break;
                    case SPRITE_MISC_MONEY_EFFECT:
                        ImportMoneyEffect(dst->As<MoneyEffect>(), reinterpret_cast<RCT12SpriteMoneyEffect*>(src));
                        break;
                    case SPRITE_MISC_CRASHED_VEHICLE_PARTICLE:
                        break;
                    case SPRITE_MISC_EXPLOSION_CLOUD:
                        break;
                    case SPRITE_MISC_CRASH_SPLASH:
                        break;
                    case SPRITE_MISC_EXPLOSION_FLARE:
                        break;
                    case SPRITE_MISC_JUMPING_FOUNTAIN_WATER:
                        ImportJumpingFountainWater(
                            dst->As<JumpingFountain>(), reinterpret_cast<RCT12SpriteJumpingFountain*>(src));
                        break;
                    case SPRITE_MISC_BALLOON:
                        ImportBalloon(dst->As<Balloon>(), reinterpret_cast<RCT12SpriteBalloon*>(src));
                        break;
                    case SPRITE_MISC_DUCK:
                        ImportDuck(dst->As<Duck>(), reinterpret_cast<RCT12SpriteDuck*>(src));
                        break;
                }

                dst->MoveTo({ src->x, src->y, src->z });
                dst->Invalidate2();
            }
        }
    }

    void ImportMoneyEffect(MoneyEffect* dst, RCT12SpriteMoneyEffect* src)
    {
        if (dst == nullptr)
            return;
        dst->MoveDelay = src->move_delay;
        dst->NumMovements = src->num_movements;
        dst->Value = src->value;
        dst->OffsetX = src->offset_x;
        dst->Wiggle = src->wiggle;
    }

    void ImportSteamParticle(SteamParticle* dst, RCT12SpriteSteamParticle* src)
    {
        if (dst == nullptr)
            return;
        dst->frame = src->frame;
    }

    void ImportJumpingFountainWater(JumpingFountain* dst, RCT12SpriteJumpingFountain* src)
    {
        if (dst == nullptr)
            return;
        dst->FountainFlags = src->fountain_flags;
        dst->Iteration = src->iteration;
        dst->NumTicksAlive = src->num_ticks_alive;
        dst->frame = src->frame;
    }

    void ImportBalloon(Balloon* dst, RCT12SpriteBalloon* src)
    {
        if (dst == nullptr)
            return;
        // Balloons were always blue in RCT1 without AA/LL
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->colour = COLOUR_LIGHT_BLUE;
        }
        else
        {
            dst->colour = RCT1::GetColour(src->colour);
        }
    }

    void ImportDuck(Duck* dst, RCT12SpriteDuck* src)
    {
        if (dst == nullptr)
            return;
        dst->frame = src->frame;
        dst->state = src->state;
    }

    uint16_t MapSpriteIndex(uint16_t originalSpriteIndex, const uint16_t* spriteIndexMap)
    {
        uint16_t newSpriteIndex = SPRITE_INDEX_NULL;
        if (originalSpriteIndex != SPRITE_INDEX_NULL)
        {
            if (originalSpriteIndex >= RCT1_MAX_SPRITES)
            {
                log_warning("Incorrect sprite index: %d", originalSpriteIndex);
            }
            else
            {
                newSpriteIndex = spriteIndexMap[originalSpriteIndex];
            }
        }
        return newSpriteIndex;
    }

    void ImportPeepSpawns()
    {
        gPeepSpawns.clear();
        for (size_t i = 0; i < RCT12_MAX_PEEP_SPAWNS; i++)
        {
            if (_s4.peep_spawn[i].x != RCT12_PEEP_SPAWN_UNDEFINED)
            {
                PeepSpawn spawn = { _s4.peep_spawn[i].x, _s4.peep_spawn[i].y, _s4.peep_spawn[i].z * 16,
                                    _s4.peep_spawn[i].direction };
                gPeepSpawns.push_back(spawn);
            }
        }
    }

    void ImportFinance()
    {
        gParkEntranceFee = _s4.park_entrance_fee;
        gLandPrice = _s4.land_price;
        gConstructionRightsPrice = _s4.construction_rights_price;

        gCash = _s4.cash;
        gBankLoan = _s4.loan;
        gMaxBankLoan = _s4.max_loan;
        // It's more like 1.33%, but we can only use integers. Can be fixed once we have our own save format.
        gBankLoanInterestRate = 1;
        gInitialCash = _s4.cash;

        gCompanyValue = _s4.company_value;
        gParkValue = CorrectRCT1ParkValue(_s4.park_value);
        gCurrentProfit = _s4.profit;

        for (size_t i = 0; i < RCT12_FINANCE_GRAPH_SIZE; i++)
        {
            gCashHistory[i] = _s4.cash_history[i];
            gParkValueHistory[i] = CorrectRCT1ParkValue(_s4.park_value_history[i]);
            gWeeklyProfitHistory[i] = _s4.weekly_profit_history[i];
        }

        for (size_t i = 0; i < RCT12_EXPENDITURE_TABLE_MONTH_COUNT; i++)
        {
            for (size_t j = 0; j < RCT12_EXPENDITURE_TYPE_COUNT; j++)
            {
                gExpenditureTable[i][j] = _s4.expenditure[i][j];
            }
        }
        gCurrentExpenditure = _s4.total_expenditure;

        gScenarioCompletedCompanyValue = _s4.completed_company_value;
        gTotalAdmissions = _s4.num_admissions;
        gTotalIncomeFromAdmissions = _s4.admission_total_income;

        // TODO marketing campaigns not working
        static_assert(
            std::numeric_limits<uint8_t>::max() > ADVERTISING_CAMPAIGN_COUNT,
            "Advertising enum bigger than capacity of iterator");
        for (uint8_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
        {
            if (_s4.marketing_status[i] & CAMPAIGN_ACTIVE_FLAG)
            {
                MarketingCampaign campaign;
                campaign.Type = i;
                campaign.WeeksLeft = _s4.marketing_status[i] & ~CAMPAIGN_ACTIVE_FLAG;
                if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
                {
                    campaign.RideId = _s4.marketing_assoc[i];
                }
                else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
                {
                    campaign.ShopItemType = _s4.marketing_assoc[i];
                }
                gMarketingCampaigns.push_back(campaign);
            }
        }
    }

    void LoadObjects()
    {
        auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
        objectManager.LoadDefaultObjects();

        LoadObjects(OBJECT_TYPE_RIDE, _rideEntries);
        LoadObjects(OBJECT_TYPE_SMALL_SCENERY, _smallSceneryEntries);
        LoadObjects(OBJECT_TYPE_LARGE_SCENERY, _largeSceneryEntries);
        LoadObjects(OBJECT_TYPE_WALLS, _wallEntries);
        LoadObjects(OBJECT_TYPE_PATHS, _pathEntries);
        LoadObjects(OBJECT_TYPE_PATH_BITS, _pathAdditionEntries);
        LoadObjects(OBJECT_TYPE_SCENERY_GROUP, _sceneryGroupEntries);
        LoadObjects(
            OBJECT_TYPE_BANNERS,
            std::vector<const char*>({
                "BN1     ",
                "BN2     ",
                "BN3     ",
                "BN4     ",
                "BN5     ",
                "BN6     ",
                "BN7     ",
                "BN8     ",
                "BN9     ",
            }));
        LoadObjects(OBJECT_TYPE_PARK_ENTRANCE, std::vector<const char*>({ "PKENT1  " }));
        LoadObjects(OBJECT_TYPE_WATER, _waterEntry);
    }

    void LoadObjects(uint8_t objectType, const EntryList& entries)
    {
        LoadObjects(objectType, entries.GetEntries());
    }

    void LoadObjects(uint8_t objectType, const std::vector<const char*>& entries)
    {
        auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();

        uint32_t entryIndex = 0;
        for (const char* objectName : entries)
        {
            rct_object_entry entry;
            entry.flags = 0x00008000 + objectType;
            std::copy_n(objectName, 8, entry.name);
            entry.checksum = 0;

            Object* object = objectManager.LoadObject(&entry);
            if (object == nullptr && objectType != OBJECT_TYPE_SCENERY_GROUP)
            {
                log_error("Failed to load %s.", objectName);
                throw std::runtime_error("Failed to load object.");
            }

            entryIndex++;
        }
    }

    void AppendRequiredObjects(std::vector<rct_object_entry>& entries, uint8_t objectType, const EntryList& entryList)
    {
        AppendRequiredObjects(entries, objectType, entryList.GetEntries());
    }

    void AppendRequiredObjects(
        std::vector<rct_object_entry>& entries, uint8_t objectType, const std::vector<const char*>& objectNames)
    {
        for (const auto objectName : objectNames)
        {
            rct_object_entry entry{};
            entry.flags = ((OBJECT_SOURCE_RCT2 << 4) & 0xF0) | (objectType & 0x0F);
            entry.SetName(objectName);
            entries.push_back(entry);
        }
    }

    std::vector<rct_object_entry> GetRequiredObjects()
    {
        std::vector<rct_object_entry> result;
        AppendRequiredObjects(result, OBJECT_TYPE_RIDE, _rideEntries);
        AppendRequiredObjects(result, OBJECT_TYPE_SMALL_SCENERY, _smallSceneryEntries);
        AppendRequiredObjects(result, OBJECT_TYPE_LARGE_SCENERY, _largeSceneryEntries);
        AppendRequiredObjects(result, OBJECT_TYPE_WALLS, _wallEntries);
        AppendRequiredObjects(result, OBJECT_TYPE_PATHS, _pathEntries);
        AppendRequiredObjects(result, OBJECT_TYPE_PATH_BITS, _pathAdditionEntries);
        AppendRequiredObjects(result, OBJECT_TYPE_SCENERY_GROUP, _sceneryGroupEntries);
        AppendRequiredObjects(
            result, OBJECT_TYPE_BANNERS,
            std::vector<const char*>({
                "BN1     ",
                "BN2     ",
                "BN3     ",
                "BN4     ",
                "BN5     ",
                "BN6     ",
                "BN7     ",
                "BN8     ",
                "BN9     ",
            }));
        AppendRequiredObjects(result, OBJECT_TYPE_PARK_ENTRANCE, std::vector<const char*>({ "PKENT1  " }));
        AppendRequiredObjects(result, OBJECT_TYPE_WATER, _waterEntry);
        return result;
    }

    void GetInvalidObjects(
        uint8_t objectType, const std::vector<const char*>& entries, std::vector<rct_object_entry>& missingObjects)
    {
        auto& objectRepository = OpenRCT2::GetContext()->GetObjectRepository();
        for (const char* objectName : entries)
        {
            rct_object_entry entry;
            entry.flags = 0x00008000 + objectType;
            std::copy_n(objectName, DAT_NAME_LENGTH, entry.name);
            entry.checksum = 0;

            const ObjectRepositoryItem* ori = objectRepository.FindObject(&entry);
            if (ori == nullptr)
            {
                missingObjects.push_back(entry);
                Console::Error::WriteLine("[%s] Object not found.", objectName);
            }
            else
            {
                Object* object = objectRepository.LoadObject(ori);
                if (object == nullptr && objectType != OBJECT_TYPE_SCENERY_GROUP)
                {
                    missingObjects.push_back(entry);
                    Console::Error::WriteLine("[%s] Object could not be loaded.", objectName);
                }
                delete object;
            }
        }
    }

    void ImportTileElements()
    {
        gMapBaseZ = 7;

        for (uint32_t index = 0, dstOffset = 0; index < RCT1_MAX_TILE_ELEMENTS; index++)
        {
            auto src = &_s4.tile_elements[index];
            auto dst = &gTileElements[index + dstOffset];
            if (src->base_height == RCT12_MAX_ELEMENT_HEIGHT)
            {
                std::memcpy(dst, src, sizeof(*src));
            }
            else
            {
                ImportTileElement(dst, src);
            }
        }

        ClearExtraTileEntries();
        FixWalls();
        FixEntrancePositions();
    }

    void ImportTileElement(TileElement* dst, const RCT12TileElement* src)
    {
        // Todo: allow for changing definition of OpenRCT2 tile element types - replace with a map
        uint8_t tileElementType = src->GetType();
        dst->ClearAs(tileElementType);
        dst->SetDirection(src->GetDirection());

        // All saved in "flags"
        dst->SetOccupiedQuadrants(src->GetOccupiedQuadrants());
        // Skipping IsGhost, which appears to use a different flag in RCT1.
        dst->SetLastForTile(src->IsLastForTile());

        dst->SetBaseZ(src->base_height * RCT1_COORDS_Z_STEP);
        dst->SetClearanceZ(src->clearance_height * RCT1_COORDS_Z_STEP);

        switch (tileElementType)
        {
            case TILE_ELEMENT_TYPE_SURFACE:
            {
                auto dst2 = dst->AsSurface();
                auto src2 = src->AsSurface();

                dst2->SetSlope(src2->GetSlope());
                dst2->SetSurfaceStyle(RCT1::GetTerrain(src2->GetSurfaceStyle()));
                dst2->SetEdgeStyle(RCT1::GetTerrainEdge(src2->GetEdgeStyle()));
                dst2->SetGrassLength(src2->GetGrassLength());
                dst2->SetOwnership(src2->GetOwnership());
                dst2->SetParkFences(src2->GetParkFences());
                dst2->SetWaterHeight(src2->GetWaterHeight());
                dst2->SetHasTrackThatNeedsWater(src2->HasTrackThatNeedsWater());

                break;
            }
            case TILE_ELEMENT_TYPE_PATH:
            {
                auto dst2 = dst->AsPath();
                auto src2 = src->AsPath();

                dst2->SetQueueBannerDirection(src2->GetQueueBannerDirection());
                dst2->SetSloped(src2->IsSloped());
                dst2->SetSlopeDirection(src2->GetSlopeDirection());
                dst2->SetRideIndex(src2->GetRideIndex());
                dst2->SetStationIndex(src2->GetStationIndex());
                dst2->SetWide(src2->IsWide());
                dst2->SetHasQueueBanner(src2->HasQueueBanner());
                dst2->SetEdges(src2->GetEdges());
                dst2->SetCorners(src2->GetCorners());
                dst2->SetAddition(src2->GetAddition());
                dst2->SetAdditionIsGhost(false);
                dst2->SetAdditionStatus(src2->GetAdditionStatus());

                // Type
                uint8_t pathType = src2->GetRCT1PathType();
                auto entryIndex = _pathTypeToEntryMap[pathType];

                dst2->SetDirection(0);
                dst2->SetIsBroken(false);
                dst2->SetIsBlockedByVehicle(false);

                dst2->SetSurfaceEntryIndex(entryIndex);
                dst2->SetShouldDrawPathOverSupports(true);
                if (RCT1::PathIsQueue(pathType))
                {
                    dst2->SetIsQueue(true);
                }
                if (_gameVersion != FILE_VERSION_RCT1_LL)
                {
                    dst2->SetRailingEntryIndex(0);
                }
                else
                {
                    ObjectEntryIndex railingsEntryIndex;
                    switch (src2->GetRCT1SupportType())
                    {
                        case RCT1_PATH_SUPPORT_TYPE_COATED_WOOD:
                            railingsEntryIndex = 3;
                            break;
                        case RCT1_PATH_SUPPORT_TYPE_SPACE:
                            railingsEntryIndex = 4;
                            break;
                        case RCT1_PATH_SUPPORT_TYPE_BAMBOO:
                            railingsEntryIndex = 5;
                            break;
                        case RCT1_PATH_SUPPORT_TYPE_TRUSS:
                        default:
                            railingsEntryIndex = 0;
                    }
                    dst2->SetRailingEntryIndex(railingsEntryIndex);
                }

                // Additions
                ObjectEntryIndex additionType = dst2->GetAddition();
                if (additionType != RCT1_PATH_ADDITION_NONE)
                {
                    ObjectEntryIndex normalisedType = RCT1::NormalisePathAddition(additionType);
                    entryIndex = _pathAdditionTypeToEntryMap[normalisedType];
                    if (additionType != normalisedType)
                    {
                        dst2->SetIsBroken(true);
                    }
                    dst2->SetAddition(entryIndex + 1);
                }
                break;
            }
            case TILE_ELEMENT_TYPE_TRACK:
            {
                auto dst2 = dst->AsTrack();
                auto src2 = src->AsTrack();

                dst2->SetTrackType(src2->GetTrackType());
                dst2->SetSequenceIndex(src2->GetSequenceIndex());
                dst2->SetRideIndex(src2->GetRideIndex());
                dst2->SetColourScheme(src2->GetColourScheme());
                dst2->SetHasChain(src2->HasChain());
                dst2->SetHasCableLift(false);
                dst2->SetInverted(src2->IsInverted());
                dst2->SetDoorAState(src2->GetDoorAState());
                dst2->SetDoorBState(src2->GetDoorBState());
                dst2->SetStationIndex(src2->GetStationIndex());
                dst2->SetHasGreenLight(src2->HasGreenLight());
                dst2->SetIsIndestructible(src2->IsIndestructible());
                dst2->SetSeatRotation(DEFAULT_SEAT_ROTATION);
                // Skipping IsHighlighted()

                auto trackType = dst2->GetTrackType();
                if (TrackTypeHasSpeedSetting(trackType))
                {
                    dst2->SetBrakeBoosterSpeed(src2->GetBrakeBoosterSpeed());
                }
                else if (trackType == TrackElemType::OnRidePhoto)
                {
                    dst2->SetPhotoTimeout(src2->GetPhotoTimeout());
                }

                // This has to be done last, since the maze entry shares fields with the colour and sequence fields.
                if (_s4.rides[src2->GetRideIndex()].type == RIDE_TYPE_MAZE)
                {
                    dst2->SetMazeEntry(src2->GetMazeEntry());
                }

                break;
            }
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            {
                auto dst2 = dst->AsSmallScenery();
                auto src2 = src->AsSmallScenery();

                auto entryIndex = _smallSceneryTypeToEntryMap[src2->GetEntryIndex()];
                dst2->SetEntryIndex(entryIndex);
                dst2->SetAge(src2->GetAge());
                dst2->SetSceneryQuadrant(src2->GetSceneryQuadrant());
                dst2->SetPrimaryColour(RCT1::GetColour(src2->GetPrimaryColour()));
                if (src2->NeedsSupports())
                    dst2->SetNeedsSupports();

                // Copied from [rct2: 0x006A2956]
                switch (src2->GetEntryIndex())
                {
                    case RCT1_SCENERY_GEOMETRIC_SCULPTURE_1:
                    case RCT1_SCENERY_GEOMETRIC_SCULPTURE_2:
                    case RCT1_SCENERY_GEOMETRIC_SCULPTURE_3:
                    case RCT1_SCENERY_GEOMETRIC_SCULPTURE_4:
                    case RCT1_SCENERY_GEOMETRIC_SCULPTURE_5:
                        dst2->SetSecondaryColour(COLOUR_WHITE);
                        break;
                    case RCT1_SCENERY_TULIPS_1:
                    case RCT1_SCENERY_TULIPS_2:
                        dst2->SetPrimaryColour(COLOUR_BRIGHT_RED);
                        dst2->SetSecondaryColour(COLOUR_YELLOW);
                        break;
                    case RCT1_SCENERY_SMALL_RED_GARDENS:
                        dst2->SetPrimaryColour(COLOUR_BRIGHT_RED);
                        break;
                }

                break;
            }
            case TILE_ELEMENT_TYPE_ENTRANCE:
            {
                auto dst2 = dst->AsEntrance();
                auto src2 = src->AsEntrance();

                dst2->SetEntranceType(src2->GetEntranceType());
                dst2->SetRideIndex(src2->GetRideIndex());
                dst2->SetStationIndex(src2->GetStationIndex());
                dst2->SetSequenceIndex(src2->GetSequenceIndex());

                if (src2->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
                {
                    auto pathType = src2->GetPathType();
                    if (pathType == 0)
                    {
                        pathType = RCT1_FOOTPATH_TYPE_TARMAC_GRAY;
                    }
                    auto entryIndex = _pathTypeToEntryMap[pathType];
                    dst2->SetPathType(entryIndex & 0x7F);
                }

                break;
            }
            case TILE_ELEMENT_TYPE_WALL:
            {
                auto dst2 = dst->AsWall();
                auto src2 = src->AsWall();

                dst2->SetSlope(src2->GetSlope());
                dst2->SetRawRCT1Data(src2->GetRawRCT1WallTypeData());

                break;
            }
            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            {
                auto dst2 = dst->AsLargeScenery();
                auto src2 = src->AsLargeScenery();

                auto type = src2->GetEntryIndex();
                dst2->SetEntryIndex(_largeSceneryTypeToEntryMap[type]);
                dst2->SetSequenceIndex(src2->GetSequenceIndex());
                dst2->SetPrimaryColour(RCT1::GetColour(src2->GetPrimaryColour()));
                dst2->SetSecondaryColour(RCT1::GetColour(src2->GetSecondaryColour()));

                break;
            }
            case TILE_ELEMENT_TYPE_BANNER:
            {
                auto dst2 = dst->AsBanner();
                auto src2 = src->AsBanner();

                auto index = src2->GetIndex();
                if (index != RCT12_BANNER_INDEX_NULL)
                    dst2->SetIndex(index);
                else
                    dst2->SetIndex(BANNER_INDEX_NULL);
                dst2->SetPosition(src2->GetPosition());
                dst2->SetAllowedEdges(src2->GetAllowedEdges());

                if (index < std::size(_s4.banners))
                {
                    auto srcBanner = &_s4.banners[index];
                    auto dstBanner = GetBanner(index);
                    ImportBanner(dstBanner, srcBanner);
                }
                break;
            }
            default:
                assert(false);
        }
    }

    void ImportResearch()
    {
        // All available objects must be loaded before this method is called as it
        // requires them to correctly insert objects into the research list

        research_reset_items();

        size_t researchListCount;
        const rct1_research_item* researchList = GetResearchList(&researchListCount);

        // Initialise the "seen" tables
        _researchRideEntryUsed.reset();
        _researchRideTypeUsed.reset();

        // The first six scenery groups are always available
        for (uint8_t i = 0; i < 6; i++)
        {
            research_insert_scenery_group_entry(i, true);
        }

        bool researched = true;
        std::bitset<RCT1_RIDE_TYPE_COUNT> rideTypeInResearch = GetRideTypesPresentInResearchList(
            researchList, researchListCount);
        std::vector<rct1_research_item> vehiclesWithMissingRideTypes;
        for (size_t i = 0; i < researchListCount; i++)
        {
            const rct1_research_item* researchItem = &researchList[i];
            if (researchItem->flags == RCT1_RESEARCH_FLAGS_SEPARATOR)
            {
                if (researchItem->item == RCT1_RESEARCH_END_AVAILABLE)
                {
                    researched = false;
                    continue;
                }
                // We don't import the random items yet.
                else if (researchItem->item == RCT1_RESEARCH_END_RESEARCHABLE || researchItem->item == RCT1_RESEARCH_END)
                {
                    break;
                }
            }

            switch (researchItem->type)
            {
                case RCT1_RESEARCH_TYPE_THEME:
                {
                    uint8_t rct1SceneryTheme = researchItem->item;
                    auto sceneryGroupEntryIndex = _sceneryThemeTypeToEntryMap[rct1SceneryTheme];
                    if (sceneryGroupEntryIndex != OBJECT_ENTRY_INDEX_IGNORE
                        && sceneryGroupEntryIndex != OBJECT_ENTRY_INDEX_NULL)
                    {
                        research_insert_scenery_group_entry(sceneryGroupEntryIndex, researched);
                    }
                    break;
                }
                case RCT1_RESEARCH_TYPE_RIDE:
                {
                    uint8_t rct1RideType = researchItem->item;
                    _researchRideTypeUsed[rct1RideType] = true;

                    auto ownRideEntryIndex = _rideTypeToRideEntryMap[rct1RideType];
                    Guard::Assert(ownRideEntryIndex != RIDE_ENTRY_INDEX_NULL, "ownRideEntryIndex was RIDE_ENTRY_INDEX_NULL");

                    bool foundOwnType = false;
                    // If the ride type does not use vehicles, no point looking for them in the research list.
                    if (RCT1::RideTypeUsesVehicles(rct1RideType))
                    {
                        // Add all vehicles for this ride type that are researched or before this research item
                        for (size_t j = 0; j < researchListCount; j++)
                        {
                            const rct1_research_item* researchItem2 = &researchList[j];
                            if (researchItem2->flags == RCT1_RESEARCH_FLAGS_SEPARATOR)
                            {
                                if (researchItem2->item == RCT1_RESEARCH_END_RESEARCHABLE
                                    || researchItem2->item == RCT1_RESEARCH_END)
                                {
                                    break;
                                }

                                continue;
                            }

                            if (researchItem2->type == RCT1_RESEARCH_TYPE_VEHICLE
                                && researchItem2->related_ride == rct1RideType)
                            {
                                auto rideEntryIndex2 = _vehicleTypeToRideEntryMap[researchItem2->item];
                                bool isOwnType = (ownRideEntryIndex == rideEntryIndex2);
                                if (isOwnType)
                                {
                                    foundOwnType = true;
                                }

                                // Only add the vehicles that were listed before this ride, otherwise we might
                                // change the research order
                                if (j < i && (researched || isOwnType))
                                {
                                    InsertResearchVehicle(researchItem2, researched);
                                }
                            }
                        }
                    }

                    if (!foundOwnType)
                    {
                        if (!_researchRideEntryUsed[ownRideEntryIndex])
                        {
                            _researchRideEntryUsed[ownRideEntryIndex] = true;
                            research_insert_ride_entry(ownRideEntryIndex, researched);
                        }
                    }

                    break;
                }
                case RCT1_RESEARCH_TYPE_VEHICLE:
                {
                    // Only add vehicle if the related ride has been seen, this to make sure that vehicles
                    // are researched only after the ride has been researched. Otherwise, remove them from the research list,
                    // so that they are automatically co-invented when their master ride is invented.
                    if (_researchRideTypeUsed[researchItem->related_ride])
                    {
                        InsertResearchVehicle(researchItem, researched);
                    }
                    else if (!rideTypeInResearch[researchItem->related_ride] && _gameVersion == FILE_VERSION_RCT1_LL)
                    {
                        vehiclesWithMissingRideTypes.push_back(*researchItem);
                    }

                    break;
                }
                case RCT1_RESEARCH_TYPE_SPECIAL:
                    // Not supported
                    break;
            }
        }
        for (const rct1_research_item& researchItem : vehiclesWithMissingRideTypes)
        {
            InsertResearchVehicle(&researchItem, false);
        }

        // Research funding / priority
        uint8_t activeResearchTypes = 0;
        if (_s4.research_priority & RCT1_RESEARCH_CATEGORY_ROLLERCOASTERS)
        {
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_ROLLERCOASTER);
        }
        if (_s4.research_priority & RCT1_RESEARCH_CATEGORY_THRILL_RIDES)
        {
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_THRILL);
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_WATER);
        }
        if (_s4.research_priority & RCT1_RESEARCH_CATEGORY_GENTLE_TRANSPORT_RIDES)
        {
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_GENTLE);
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_TRANSPORT);
        }
        if (_s4.research_priority & RCT1_RESEARCH_CATEGORY_SHOPS)
        {
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_SHOP);
        }
        if (_s4.research_priority & RCT1_RESEARCH_CATEGORY_SCENERY_THEMEING)
        {
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_SCENERY_GROUP);
        }
        gResearchPriorities = activeResearchTypes;
        gResearchFundingLevel = _s4.research_level;

        // This will mark items as researched/unresearched according to the research list.
        // This needs to be called before importing progress, as it will reset it.
        research_reset_current_item();

        // Research history
        gResearchProgress = _s4.research_progress;
        gResearchProgressStage = _s4.research_progress_stage;
        gResearchExpectedDay = _s4.next_research_expected_day;
        gResearchExpectedMonth = _s4.next_research_expected_month;

        if (_s4.last_research_flags == 0xFF)
        {
            gResearchLastItem = std::nullopt;
        }
        else
        {
            ResearchItem researchItem = {};
            ConvertResearchEntry(&researchItem, _s4.last_research_item, _s4.last_research_type);
            gResearchLastItem = researchItem;
        }

        if (_s4.next_research_flags == 0xFF)
        {
            gResearchNextItem = std::nullopt;
            gResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
            gResearchProgress = 0;
        }
        else
        {
            ResearchItem researchItem = {};
            ConvertResearchEntry(&researchItem, _s4.next_research_item, _s4.next_research_type);
            gResearchNextItem = researchItem;
        }
    }

    static std::bitset<RCT1_RIDE_TYPE_COUNT> GetRideTypesPresentInResearchList(
        const rct1_research_item* researchList, size_t researchListCount)
    {
        std::bitset<RCT1_RIDE_TYPE_COUNT> ret = {};

        for (size_t i = 0; i < researchListCount; i++)
        {
            const rct1_research_item* researchItem = &researchList[i];
            if (researchItem->flags == RCT1_RESEARCH_FLAGS_SEPARATOR)
            {
                if (researchItem->item == RCT1_RESEARCH_END_AVAILABLE || researchItem->item == RCT1_RESEARCH_END_RESEARCHABLE)
                {
                    continue;
                }
                else if (researchItem->item == RCT1_RESEARCH_END)
                {
                    break;
                }
            }

            if (researchItem->type == RCT1_RESEARCH_TYPE_RIDE)
            {
                ret[researchItem->item] = true;
            }
        }

        return ret;
    }

    void InsertResearchVehicle(const rct1_research_item* researchItem, bool researched)
    {
        uint8_t vehicle = researchItem->item;
        auto rideEntryIndex = _vehicleTypeToRideEntryMap[vehicle];

        if (!_researchRideEntryUsed[rideEntryIndex])
        {
            _researchRideEntryUsed[rideEntryIndex] = true;
            research_insert_ride_entry(rideEntryIndex, researched);
        }
    }

    void ImportParkName()
    {
        std::string parkName = std::string(_s4.scenario_name);
        if (is_user_string_id(static_cast<rct_string_id>(_s4.park_name_string_index)))
        {
            std::string userString = GetUserString(_s4.park_name_string_index);
            if (!userString.empty())
            {
                parkName = userString;
            }
        }

        auto& park = GetContext()->GetGameState()->GetPark();
        park.Name = parkName;
    }

    void ImportParkFlags()
    {
        // Date and srand
        gScenarioTicks = _s4.ticks;
        scenario_rand_seed(_s4.random_a, _s4.random_b);
        gDateMonthsElapsed = static_cast<int32_t>(_s4.month);
        gDateMonthTicks = _s4.day;

        // Park rating
        gParkRating = _s4.park_rating;
        for (size_t i = 0; i < 32; i++)
        {
            gParkRatingHistory[i] = _s4.park_rating_history[i];
        }

        // Awards
        for (int32_t i = 0; i < RCT12_MAX_AWARDS; i++)
        {
            rct12_award* src = &_s4.awards[i];
            Award* dst = &gCurrentAwards[i];
            dst->Time = src->time;
            dst->Type = src->type;
        }

        // Number of guests history
        for (size_t i = 0; i < 32; i++)
        {
            gGuestsInParkHistory[i] = _s4.guests_in_park_history[i];
        }

        // News items
        for (size_t i = 0; i < RCT12_MAX_NEWS_ITEMS; i++)
        {
            const rct12_news_item* src = &_s4.messages[i];
            News::Item* dst = &gNewsItems[i];

            dst->Type = static_cast<News::ItemType>(src->Type);
            dst->Flags = src->Flags;
            dst->Ticks = src->Ticks;
            dst->MonthYear = src->MonthYear;
            dst->Day = src->Day;
            std::copy(std::begin(src->Text), std::end(src->Text), dst->Text);

            if (dst->Type == News::ItemType::Research)
            {
                uint8_t researchItem = src->Assoc & 0x000000FF;
                uint8_t researchType = (src->Assoc & 0x00FF0000) >> 16;

                ResearchItem tmpResearchItem = {};
                ConvertResearchEntry(&tmpResearchItem, researchItem, researchType);
                dst->Assoc = tmpResearchItem.rawValue;
            }
            else
            {
                dst->Assoc = src->Assoc;
            }
        }

        // Initial guest status
        gGuestInitialCash = _s4.guest_initial_cash;
        gGuestInitialHunger = _s4.guest_initial_hunger;
        gGuestInitialThirst = _s4.guest_initial_thirst;
        gGuestInitialHappiness = _s4.guest_initial_happiness;

        _guestGenerationProbability = _s4.guest_generation_probability;

        // Staff colours
        gStaffHandymanColour = RCT1::GetColour(_s4.handman_colour);
        gStaffMechanicColour = RCT1::GetColour(_s4.mechanic_colour);
        gStaffSecurityColour = RCT1::GetColour(_s4.security_guard_colour);

        // Flags
        gParkFlags = _s4.park_flags;
        gParkFlags &= ~PARK_FLAGS_ANTI_CHEAT_DEPRECATED;
        // Loopy Landscape parks can set a flag to lock the entry price to free.
        // If this flag is not set, the player can ask money for both rides and entry.
        if (!(_s4.park_flags & RCT1_PARK_FLAGS_PARK_ENTRY_LOCKED_AT_FREE))
        {
            gParkFlags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
        }

        // RCT2 uses two flags for no money (due to the scenario editor). RCT1 used only one.
        // Copy its value to make no money scenarios such as Arid Heights work properly.
        if (_s4.park_flags & RCT1_PARK_FLAGS_NO_MONEY)
        {
            gParkFlags |= PARK_FLAGS_NO_MONEY_SCENARIO;
        }

        gParkSize = _s4.park_size;
        gTotalRideValueForMoney = _s4.total_ride_value_for_money;
    }

    void ConvertResearchEntry(ResearchItem* dst, uint8_t srcItem, uint8_t srcType)
    {
        dst->SetNull();
        if (srcType == RCT1_RESEARCH_TYPE_RIDE)
        {
            auto entryIndex = _rideTypeToRideEntryMap[srcItem];

            if (entryIndex != RIDE_ENTRY_INDEX_NULL)
            {
                rct_ride_entry* rideEntry = get_ride_entry(entryIndex);

                if (rideEntry != nullptr)
                {
                    auto rideType = ride_entry_get_first_non_null_ride_type(rideEntry);
                    dst->entryIndex = entryIndex;
                    dst->baseRideType = rideType;
                    dst->type = Research::EntryType::Ride;
                    dst->flags = 0;
                    dst->category = RideTypeDescriptors[rideType].Category;
                }
            }
        }
        else if (srcType == RCT1_RESEARCH_TYPE_VEHICLE)
        {
            auto entryIndex = _vehicleTypeToRideEntryMap[srcItem];

            if (entryIndex != RIDE_ENTRY_INDEX_NULL)
            {
                rct_ride_entry* rideEntry = get_ride_entry(entryIndex);

                if (rideEntry != nullptr)
                {
                    auto rideType = ride_entry_get_first_non_null_ride_type(rideEntry);
                    dst->entryIndex = entryIndex;
                    dst->baseRideType = rideType;
                    dst->type = Research::EntryType::Ride;
                    dst->flags = 0;
                    dst->category = RideTypeDescriptors[rideType].Category;
                }
            }
        }
        else if (srcType == RCT1_RESEARCH_TYPE_THEME)
        {
            auto entryIndex = _sceneryThemeTypeToEntryMap[srcItem];

            if (entryIndex != OBJECT_ENTRY_INDEX_IGNORE && entryIndex != OBJECT_ENTRY_INDEX_NULL)
            {
                dst->entryIndex = entryIndex;
                dst->type = Research::EntryType::Scenery;
                dst->category = RESEARCH_CATEGORY_SCENERY_GROUP;
                dst->flags = 0;
            }
        }
    }

    void ImportClimate()
    {
        gClimate = ClimateType{ _s4.climate };
        gClimateUpdateTimer = _s4.climate_timer;
        gClimateCurrent.Temperature = _s4.temperature;
        gClimateCurrent.Weather = _s4.weather;
        gClimateCurrent.WeatherEffect = WeatherEffectType::None;
        gClimateCurrent.WeatherGloom = _s4.weather_gloom;
        gClimateCurrent.Level = static_cast<WeatherLevel>(_s4.rain);
        gClimateNext.Temperature = _s4.target_temperature;
        gClimateNext.Weather = _s4.target_weather;
        gClimateNext.WeatherEffect = WeatherEffectType::None;
        gClimateNext.WeatherGloom = _s4.target_weather_gloom;
        gClimateNext.Level = static_cast<WeatherLevel>(_s4.target_rain);
    }

    void ImportScenarioNameDetails()
    {
        std::string name = String::ToStd(_s4.scenario_name);
        std::string details;

        int32_t scNumber = _s4.scenario_slot_index;
        if (scNumber != -1)
        {
            source_desc sourceDesc;
            if (ScenarioSources::TryGetById(scNumber, &sourceDesc))
            {
                rct_string_id localisedStringIds[3];
                if (language_get_localised_scenario_strings(sourceDesc.title, localisedStringIds))
                {
                    if (localisedStringIds[0] != STR_NONE)
                    {
                        name = String::ToStd(language_get_string(localisedStringIds[0]));
                    }
                    if (localisedStringIds[2] != STR_NONE)
                    {
                        details = String::ToStd(language_get_string(localisedStringIds[2]));
                    }
                }
            }
        }

        String::Set(gS6Info.name, sizeof(gS6Info.name), name.c_str());
        String::Set(gS6Info.details, sizeof(gS6Info.details), details.c_str());
        gScenarioName = name;
        gScenarioDetails = details;
    }

    void ImportScenarioObjective()
    {
        gScenarioObjective.Type = _s4.scenario_objective_type;
        gScenarioObjective.Year = _s4.scenario_objective_years;
        gScenarioObjective.NumGuests = _s4.scenario_objective_num_guests;

        // RCT1 used a different way of calculating the park value.
        // This is corrected here, but since scenario_objective_currency doubles as minimum excitement rating,
        // we need to check the goal to avoid affecting scenarios like Volcania.
        if (_s4.scenario_objective_type == OBJECTIVE_PARK_VALUE_BY)
            gScenarioObjective.Currency = CorrectRCT1ParkValue(_s4.scenario_objective_currency);
        else
            gScenarioObjective.Currency = _s4.scenario_objective_currency;

        // This does not seem to be saved in the objective arguments, so look up the ID from the available rides instead.
        if (_s4.scenario_objective_type == OBJECTIVE_BUILD_THE_BEST)
            gScenarioObjective.RideId = GetBuildTheBestRideId();
    }

    void ImportSavedView()
    {
        gSavedView = ScreenCoordsXY{ _s4.view_x, _s4.view_y };
        gSavedViewZoom = _s4.view_zoom;
        gSavedViewRotation = _s4.view_rotation;
    }

    void ClearExtraTileEntries()
    {
        // Reset the map tile pointers
        std::fill(std::begin(gTileElementTilePointers), std::end(gTileElementTilePointers), nullptr);

        // Get the first free map element
        TileElement* nextFreeTileElement = gTileElements;
        for (size_t i = 0; i < RCT1_MAX_MAP_SIZE * RCT1_MAX_MAP_SIZE; i++)
        {
            while (!(nextFreeTileElement++)->IsLastForTile())
                ;
        }

        TileElement* tileElement = gTileElements;
        TileElement** tilePointer = gTileElementTilePointers;

        // 128 rows of map data from RCT1 map
        for (int32_t x = 0; x < RCT1_MAX_MAP_SIZE; x++)
        {
            // Assign the first half of this row
            for (int32_t y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                *tilePointer++ = tileElement;
                while (!(tileElement++)->IsLastForTile())
                    ;
            }

            // Fill the rest of the row with blank tiles
            for (int32_t y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                nextFreeTileElement->ClearAs(TILE_ELEMENT_TYPE_SURFACE);
                nextFreeTileElement->SetLastForTile(true);
                nextFreeTileElement->AsSurface()->SetSlope(TILE_ELEMENT_SLOPE_FLAT);
                nextFreeTileElement->AsSurface()->SetSurfaceStyle(TERRAIN_GRASS);
                nextFreeTileElement->AsSurface()->SetEdgeStyle(TERRAIN_EDGE_ROCK);
                nextFreeTileElement->AsSurface()->SetGrassLength(GRASS_LENGTH_CLEAR_0);
                nextFreeTileElement->AsSurface()->SetOwnership(OWNERSHIP_UNOWNED);
                *tilePointer++ = nextFreeTileElement++;
            }
        }

        // 128 extra rows left to fill with blank tiles
        for (int32_t y = 0; y < 128 * 256; y++)
        {
            nextFreeTileElement->ClearAs(TILE_ELEMENT_TYPE_SURFACE);
            nextFreeTileElement->SetLastForTile(true);
            nextFreeTileElement->AsSurface()->SetSlope(TILE_ELEMENT_SLOPE_FLAT);
            nextFreeTileElement->AsSurface()->SetSurfaceStyle(TERRAIN_GRASS);
            nextFreeTileElement->AsSurface()->SetEdgeStyle(TERRAIN_EDGE_ROCK);
            nextFreeTileElement->AsSurface()->SetGrassLength(GRASS_LENGTH_CLEAR_0);
            nextFreeTileElement->AsSurface()->SetOwnership(OWNERSHIP_UNOWNED);
            *tilePointer++ = nextFreeTileElement++;
        }

        gNextFreeTileElement = nextFreeTileElement;
    }

    void FixWalls()
    {
        std::vector<TileElement> wallsOnTile = {};

        for (int32_t x = 0; x < RCT1_MAX_MAP_SIZE; x++)
        {
            for (int32_t y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                TileElement* tileElement = map_get_first_element_at(TileCoordsXY{ x, y }.ToCoordsXY());
                if (tileElement == nullptr)
                    continue;
                do
                {
                    if (tileElement->GetType() == TILE_ELEMENT_TYPE_WALL)
                    {
                        wallsOnTile.push_back(*tileElement);
                        tile_element_remove(tileElement);
                        tileElement--;
                    }
                } while (!(tileElement++)->IsLastForTile());

                for (auto originalTileElement : wallsOnTile)
                {
                    auto location = TileCoordsXYZ(x, y, 0).ToCoordsXYZ();

                    for (int32_t edge = 0; edge < 4; edge++)
                    {
                        int32_t type = originalTileElement.AsWall()->GetRCT1WallType(edge);
                        auto slope = originalTileElement.AsWall()->GetRCT1Slope();

                        if (type != -1)
                        {
                            colour_t colourA = RCT1::GetColour(originalTileElement.AsWall()->GetRCT1WallColour());
                            colour_t colourB = COLOUR_BLACK;
                            colour_t colourC = COLOUR_BLACK;
                            ConvertWall(&type, &colourA, &colourB);

                            type = _wallTypeToEntryMap[type];
                            auto baseZ = originalTileElement.GetBaseZ();
                            auto clearanceZ = originalTileElement.GetClearanceZ();
                            auto edgeSlope = LandSlopeToWallSlope[slope][edge & 3];
                            if (edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS))
                            {
                                clearanceZ += LAND_HEIGHT_STEP;
                            }
                            if (edgeSlope & EDGE_SLOPE_ELEVATED)
                            {
                                edgeSlope &= ~EDGE_SLOPE_ELEVATED;
                                baseZ += LAND_HEIGHT_STEP;
                                clearanceZ += LAND_HEIGHT_STEP;
                            }

                            auto element = tile_element_insert(location, originalTileElement.GetOccupiedQuadrants());
                            element->SetType(TILE_ELEMENT_TYPE_WALL);
                            element->SetDirection(edge);
                            element->SetBaseZ(baseZ);
                            element->SetClearanceZ(clearanceZ);

                            auto wallElement = element->AsWall();
                            wallElement->SetEntryIndex(type);
                            wallElement->SetPrimaryColour(colourA);
                            wallElement->SetSecondaryColour(colourB);
                            wallElement->SetTertiaryColour(colourC);
                            wallElement->SetBannerIndex(BANNER_INDEX_NULL);
                            wallElement->SetAcrossTrack(originalTileElement.AsWall()->IsAcrossTrack());
                            wallElement->SetAnimationIsBackwards(originalTileElement.AsWall()->AnimationIsBackwards());
                            wallElement->SetSlope(edgeSlope);
                        }
                    }
                }
                wallsOnTile.clear();
            }
        }
    }

    void ConvertWall(int32_t* type, colour_t* colourA, colour_t* colourB)
    {
        switch (*type)
        {
            case RCT1_WALL_TYPE_WOODEN_PANEL_FENCE:
                *colourA = COLOUR_DARK_BROWN;
                break;
            case RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE:
                *colourA = COLOUR_WHITE;
                break;
            case RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE:
                *colourA = COLOUR_SALMON_PINK;
                break;
            case RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_SNOW:
                *colourA = COLOUR_DARK_BROWN;
                break;
            case RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_GATE:
                *colourB = *colourA;
                *colourA = COLOUR_DARK_BROWN;
                break;
            case RCT1_WALL_TYPE_GLASS_SMOOTH:
            case RCT1_WALL_TYPE_GLASS_PANELS:
                *colourB = COLOUR_WHITE;
                break;
            case RCT1_WALL_TYPE_SMALL_GREY_CASTLE:
            case RCT1_WALL_TYPE_LARGE_GREY_CASTLE:
            case RCT1_WALL_TYPE_LARGE_GREY_CASTLE_CROSS:
            case RCT1_WALL_TYPE_LARGE_GREY_CASTLE_GATE:
            case RCT1_WALL_TYPE_LARGE_GREY_CASTLE_WINDOW:
            case RCT1_WALL_TYPE_MEDIUM_GREY_CASTLE:
                *colourA = COLOUR_GREY;
                break;
        }
    }

    void ImportBanner(Banner* dst, const RCT12Banner* src)
    {
        *dst = {};
        dst->type = RCTEntryIndexToOpenRCT2EntryIndex(src->type);

        dst->flags = 0;
        if (src->flags & BANNER_FLAG_NO_ENTRY)
        {
            dst->flags |= BANNER_FLAG_NO_ENTRY;
        }

        if (is_user_string_id(src->string_idx))
        {
            dst->text = GetUserString(src->string_idx);
        }

        dst->colour = RCT1::GetColour(src->colour);
        dst->text_colour = src->text_colour;
        dst->position.x = src->x;
        dst->position.y = src->y;
    }

    void FixEntrancePositions()
    {
        gParkEntrances.clear();
        tile_element_iterator it;
        tile_element_iterator_begin(&it);
        while (tile_element_iterator_next(&it) && gParkEntrances.size() < RCT12_MAX_PARK_ENTRANCES)
        {
            TileElement* element = it.element;

            if (element->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;
            if (element->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                continue;
            if ((element->AsEntrance()->GetSequenceIndex()) != 0)
                continue;

            CoordsXYZD entrance = { TileCoordsXY(it.x, it.y).ToCoordsXY(), element->GetBaseZ(), element->GetDirection() };
            gParkEntrances.push_back(entrance);
        }
    }

    EntryList* GetEntryList(uint8_t objectType)
    {
        switch (objectType)
        {
            case OBJECT_TYPE_RIDE:
                return &_rideEntries;
            case OBJECT_TYPE_SMALL_SCENERY:
                return &_smallSceneryEntries;
            case OBJECT_TYPE_LARGE_SCENERY:
                return &_largeSceneryEntries;
            case OBJECT_TYPE_WALLS:
                return &_wallEntries;
            case OBJECT_TYPE_PATHS:
                return &_pathEntries;
            case OBJECT_TYPE_PATH_BITS:
                return &_pathAdditionEntries;
            case OBJECT_TYPE_SCENERY_GROUP:
                return &_sceneryGroupEntries;
            case OBJECT_TYPE_WATER:
                return &_waterEntry;
        }
        return nullptr;
    }

    const rct1_research_item* GetResearchList(size_t* count)
    {
        // Loopy Landscapes stores research items in a different place
        if (_gameVersion == FILE_VERSION_RCT1_LL)
        {
            *count = std::size(_s4.research_items_LL);
            return _s4.research_items_LL;
        }
        else
        {
            *count = std::size(_s4.research_items);
            return _s4.research_items;
        }
    }

    std::string GetUserString(rct_string_id stringId)
    {
        const auto originalString = _s4.string_table[(stringId - USER_STRING_START) % 1024];
        std::string_view originalStringView(originalString, USER_STRING_MAX_LENGTH);
        auto asUtf8 = rct2_to_utf8(originalStringView, RCT2_LANGUAGE_ID_ENGLISH_UK);
        utf8_remove_format_codes(asUtf8.data(), /*allow colour*/ false);
        return asUtf8.data();
    }

    void FixLandOwnership()
    {
        switch (_s4.scenario_slot_index)
        {
            case SC_DYNAMITE_DUNES:
                FixLandOwnershipTiles({ { 97, 18 }, { 99, 19 }, { 83, 34 } });
                break;
            case SC_LEAFY_LAKE:
                FixLandOwnershipTiles({ { 49, 66 } });
                break;
            case SC_KATIES_DREAMLAND:
                FixLandOwnershipTiles({ { 74, 70 }, { 75, 70 }, { 76, 70 }, { 77, 73 }, { 80, 77 } });
                break;
            case SC_POKEY_PARK:
                FixLandOwnershipTiles({ { 64, 102 } });
                break;
            case SC_MYSTIC_MOUNTAIN:
                FixLandOwnershipTiles({ { 98, 69 }, { 98, 70 }, { 103, 64 }, { 53, 79 }, { 86, 93 }, { 87, 93 } });
                break;
            case SC_PACIFIC_PYRAMIDS:
                FixLandOwnershipTiles({ { 93, 105 }, { 63, 34 }, { 76, 25 }, { 85, 31 }, { 96, 47 }, { 96, 48 } });
                break;
            case SC_UTOPIA_PARK:
                FixLandOwnershipTiles({ { 85, 73 } });
                break;
            case SC_URBAN_PARK:
                FixLandOwnershipTiles({ { 64, 77 }, { 61, 66 }, { 61, 67 }, { 39, 20 } });
                break;
        }
    }

    /**
     * In Urban Park, the entrance and exit of the merry-go-round are the wrong way round. This code fixes that.
     * To avoid messing up saves (in which this problem is most likely solved by the user), only carry out this
     * fix when loading from a scenario.
     */
    void FixUrbanPark()
    {
        if (_s4.scenario_slot_index == SC_URBAN_PARK && _isScenario)
        {
            // First, make the queuing peep exit
            for (auto peep : EntityList<Guest>(EntityListId::Peep))
            {
                if (peep->State == PEEP_STATE_QUEUING_FRONT && peep->CurrentRide == 0)
                {
                    peep->RemoveFromQueue();
                    peep->SetState(PEEP_STATE_FALLING);
                    break;
                }
            }

            // Now, swap the entrance and exit.
            auto ride = get_ride(0);
            if (ride != nullptr)
            {
                auto entranceCoords = ride->stations[0].Exit;
                auto exitCoords = ride->stations[0].Entrance;
                ride->stations[0].Entrance = entranceCoords;
                ride->stations[0].Exit = exitCoords;

                auto entranceElement = map_get_ride_exit_element_at(entranceCoords.ToCoordsXYZD(), false);
                entranceElement->SetEntranceType(ENTRANCE_TYPE_RIDE_ENTRANCE);
                auto exitElement = map_get_ride_entrance_element_at(exitCoords.ToCoordsXYZD(), false);
                exitElement->SetEntranceType(ENTRANCE_TYPE_RIDE_EXIT);

                // Trigger footpath update
                footpath_queue_chain_reset();
                footpath_connect_edges(
                    entranceCoords.ToCoordsXY(), reinterpret_cast<TileElement*>(entranceElement),
                    GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
                footpath_update_queue_chains();
            }
        }
    }

    /**
     * Counts the block sections. The reason this iterates over the map is to avoid getting into infinite loops,
     * which can happen with hacked parks.
     */
    void CountBlockSections()
    {
        for (int32_t x = 0; x < RCT1_MAX_MAP_SIZE; x++)
        {
            for (int32_t y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                TileElement* tileElement = map_get_first_element_at(TileCoordsXY{ x, y }.ToCoordsXY());
                if (tileElement == nullptr)
                    continue;
                do
                {
                    if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
                    {
                        // Lift hill tops are the only pieces present in RCT1 that can count as a block brake.
                        if (!tileElement->AsTrack()->HasChain())
                            continue;

                        auto trackType = tileElement->AsTrack()->GetTrackType();
                        switch (trackType)
                        {
                            case TrackElemType::Up25ToFlat:
                            case TrackElemType::Up60ToFlat:
                            case TrackElemType::DiagUp25ToFlat:
                            case TrackElemType::DiagUp60ToFlat:
                                break;
                            default:
                                continue;
                        }

                        ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
                        auto ride = get_ride(rideIndex);
                        if (ride != nullptr)
                        {
                            ride->num_block_brakes++;
                        }
                    }
                } while (!(tileElement++)->IsLastForTile());
            }
        }
    }

    /**
     * This has to be done after importing tile elements, because it needs those to detect if a pre-existing ride
     * name should be considered reserved.
     */
    void SetDefaultNames()
    {
        for (auto& ride : GetRideManager())
        {
            if (ride.custom_name.empty())
            {
                ride.SetNameToDefault();
            }
        }
    }

    ObjectEntryIndex GetBuildTheBestRideId()
    {
        size_t researchListCount;
        const rct1_research_item* researchList = GetResearchList(&researchListCount);
        for (size_t i = 0; i < researchListCount; i++)
        {
            if (researchList[i].flags == 0xFF)
            {
                break;
            }

            if (researchList[i].type == RCT1_RESEARCH_TYPE_RIDE)
            {
                return RCT1::GetRideType(researchList[i].item, 0);
            }
        }

        return RIDE_TYPE_NULL;
    }
};

std::unique_ptr<IParkImporter> ParkImporter::CreateS4()
{
    return std::make_unique<S4Importer>();
}

void load_from_sv4(const utf8* path)
{
    auto& objectMgr = GetContext()->GetObjectManager();
    auto s4Importer = std::make_unique<S4Importer>();
    auto result = s4Importer->LoadSavedGame(path);
    objectMgr.LoadObjects(result.RequiredObjects.data(), result.RequiredObjects.size());
    s4Importer->Import();
}

void load_from_sc4(const utf8* path)
{
    auto& objectMgr = GetContext()->GetObjectManager();
    auto s4Importer = std::make_unique<S4Importer>();
    auto result = s4Importer->LoadScenario(path);
    objectMgr.LoadObjects(result.RequiredObjects.data(), result.RequiredObjects.size());
    s4Importer->Import();
}

int32_t WallElement::GetRCT1WallType(int32_t edge) const
{
    uint8_t var_05 = colour_3;
    uint16_t var_06 = colour_1 | (animation << 8);

    int32_t typeA = (var_05 >> (edge * 2)) & 3;
    int32_t typeB = (var_06 >> (edge * 4)) & 0x0F;

    if (typeB != 0x0F)
    {
        return typeA | (typeB << 2);
    }
    else
    {
        return -1;
    }
}

colour_t WallElement::GetRCT1WallColour() const
{
    return (((type & 0xC0) >> 3) | ((entryIndex & 0xE0) >> 5)) & 31;
}
