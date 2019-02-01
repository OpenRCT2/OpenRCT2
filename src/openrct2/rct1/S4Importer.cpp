/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
#include "RCT1.h"
#include "Tables.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

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

    size_t GetOrAddEntry(const char* entryName)
    {
        size_t entryIndex = Collections::IndexOf(_entries, entryName, true);
        if (entryIndex == SIZE_MAX)
        {
            entryIndex = _entries.size();
            _entries.push_back(entryName);
        }
        return entryIndex;
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
    uint8_t _rideTypeToRideEntryMap[RCT1_RIDE_TYPE_COUNT]{};
    uint8_t _vehicleTypeToRideEntryMap[RCT1_VEHICLE_TYPE_COUNT]{};
    uint8_t _smallSceneryTypeToEntryMap[256]{};
    uint8_t _largeSceneryTypeToEntryMap[256]{};
    uint8_t _wallTypeToEntryMap[256]{};
    uint8_t _pathTypeToEntryMap[24]{};
    uint8_t _pathAdditionTypeToEntryMap[16]{};
    uint8_t _sceneryThemeTypeToEntryMap[24]{};

    // Research
    uint8_t _researchRideEntryUsed[MAX_RIDE_OBJECTS]{};
    uint8_t _researchRideTypeUsed[RCT1_RIDE_TYPE_COUNT]{};

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
        ImportMapAnimations();
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
        CountBlockSections();
        determine_ride_entrance_and_exit_locations();

        // Importing the strings is done later on, although that approach needs looking at.
        // game_convert_strings_to_utf8();
        game_convert_news_items_to_utf8();
        map_count_remaining_land_rights();
    }

    bool GetDetails(scenario_index_entry* dst) override
    {
        *dst = {};

        source_desc desc;
        // If no entry is found, this is a custom scenario.
        bool isOfficial = ScenarioSources::TryGetById(_s4.scenario_slot_index, &desc);

        dst->category = desc.category;
        dst->source_game = desc.source;
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
        _gameVersion = sawyercoding_detect_rct1_version(_s4.game_version) & FILE_VERSION_MASK;
        // Avoid reusing the value used for last import
        _parkValueConversionFactor = 0;

        InitialiseEntryMaps();
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
        std::fill(std::begin(_rideTypeToRideEntryMap), std::end(_rideTypeToRideEntryMap), 255);
        std::fill(std::begin(_vehicleTypeToRideEntryMap), std::end(_vehicleTypeToRideEntryMap), 255);
        std::fill(std::begin(_smallSceneryTypeToEntryMap), std::end(_smallSceneryTypeToEntryMap), 255);
        std::fill(std::begin(_largeSceneryTypeToEntryMap), std::end(_largeSceneryTypeToEntryMap), 255);
        std::fill(std::begin(_wallTypeToEntryMap), std::end(_wallTypeToEntryMap), 255);
        std::fill(std::begin(_pathTypeToEntryMap), std::end(_pathTypeToEntryMap), 255);
        std::fill(std::begin(_pathAdditionTypeToEntryMap), std::end(_pathAdditionTypeToEntryMap), 255);
        std::fill(std::begin(_sceneryThemeTypeToEntryMap), std::end(_sceneryThemeTypeToEntryMap), 255);
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
                {
                    uint8_t rideType = researchItem->item;

                    // Add all vehicles for this ride type
                    uint32_t numVehicles = 0;
                    for (size_t j = 0; j < researchListCount; j++)
                    {
                        const rct1_research_item* researchItem2 = &researchList[j];
                        if (researchItem2->flags == RCT1_RESEARCH_FLAGS_SEPARATOR)
                        {
                            if (researchItem2->item == RCT1_RESEARCH_END_RESEARCHABLE
                                || researchItem2->item == RCT1_RESEARCH_END_AVAILABLE)
                            {
                                continue;
                            }
                            else if (researchItem2->item == RCT1_RESEARCH_END)
                            {
                                break;
                            }
                        }

                        if (researchItem2->type == RCT1_RESEARCH_TYPE_VEHICLE && researchItem2->related_ride == rideType)
                        {
                            AddEntryForVehicleType(rideType, researchItem2->item);
                            numVehicles++;
                        }
                    }

                    // If no vehicles found so just add the default for this ride
                    if (numVehicles == 0)
                    {
                        AddEntryForRideType(rideType);
                    }
                    break;
                }
            }
        }

        // In addition to the research list, there is also a list of invented ride/vehicles in general.
        // This is especially useful if the research list is damaged or if the save is hacked.
        for (int32_t rideType = 0; rideType < RCT1_RIDE_TYPE_COUNT; rideType++)
        {
            int32_t quadIndex = rideType >> 5;
            int32_t bitIndex = rideType & 0x1F;
            bool invented = (_s4.available_rides[quadIndex] & ((uint32_t)1 << bitIndex));

            if (invented)
            {
                AddEntryForRideType(rideType);
            }
        }

        for (int32_t vehicleType = 0; vehicleType < RCT1_VEHICLE_TYPE_COUNT; vehicleType++)
        {
            int32_t quadIndex = vehicleType >> 5;
            int32_t bitIndex = vehicleType & 0x1F;
            bool invented = (_s4.available_vehicles[quadIndex] & ((uint32_t)1 << bitIndex));

            if (invented)
            {
                AddEntryForVehicleType(RIDE_TYPE_NULL, vehicleType);
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
            rct1_ride* ride = &_s4.rides[i];
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
            if (sceneryTheme != 0 && _sceneryThemeTypeToEntryMap[sceneryTheme] == 255)
                continue;

            std::vector<const char*> objects = RCT1::GetSceneryObjects(sceneryTheme);
            for (const char* objectName : objects)
            {
                auto& objectRepository = OpenRCT2::GetContext()->GetObjectRepository();
                auto foundObject = objectRepository.FindObject(objectName);
                if (foundObject != nullptr)
                {
                    uint8_t objectType = object_entry_get_type(&foundObject->ObjectEntry);
                    switch (objectType)
                    {
                        case OBJECT_TYPE_SMALL_SCENERY:
                        case OBJECT_TYPE_LARGE_SCENERY:
                        case OBJECT_TYPE_WALLS:
                        case OBJECT_TYPE_PATHS:
                        case OBJECT_TYPE_PATH_BITS:
                            EntryList* entries = GetEntryList(objectType);

                            // Check if there are spare entries available
                            size_t maxEntries = (size_t)object_entry_group_counts[objectType];
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

        if (_s4.game_version < FILE_VERSION_RCT1_LL)
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
                size_t entryIndex = _rideEntries.GetOrAddEntry(entryName);
                _rideTypeToRideEntryMap[rideType] = (uint8_t)entryIndex;
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
                size_t entryIndex = _rideEntries.GetOrAddEntry(entryName);
                _vehicleTypeToRideEntryMap[vehicleType] = (uint8_t)entryIndex;

                if (rideType != RIDE_TYPE_NULL)
                    _rideTypeToRideEntryMap[rideType] = (uint8_t)entryIndex;
            }
        }
    }

    void AddEntryForSmallScenery(uint8_t smallSceneryType)
    {
        assert(smallSceneryType < std::size(_smallSceneryTypeToEntryMap));
        if (_smallSceneryTypeToEntryMap[smallSceneryType] == 255)
        {
            const char* entryName = RCT1::GetSmallSceneryObject(smallSceneryType);
            size_t entryIndex = _smallSceneryEntries.GetOrAddEntry(entryName);

            _smallSceneryTypeToEntryMap[smallSceneryType] = (uint8_t)entryIndex;
        }
    }

    void AddEntryForLargeScenery(uint8_t largeSceneryType)
    {
        assert(largeSceneryType < std::size(_largeSceneryTypeToEntryMap));
        if (_largeSceneryTypeToEntryMap[largeSceneryType] == 255)
        {
            const char* entryName = RCT1::GetLargeSceneryObject(largeSceneryType);
            size_t entryIndex = _largeSceneryEntries.GetOrAddEntry(entryName);

            _largeSceneryTypeToEntryMap[largeSceneryType] = (uint8_t)entryIndex;
        }
    }

    void AddEntryForWall(uint8_t wallType)
    {
        assert(wallType < std::size(_wallTypeToEntryMap));
        if (_wallTypeToEntryMap[wallType] == 255)
        {
            const char* entryName = RCT1::GetWallObject(wallType);
            size_t entryIndex = _wallEntries.GetOrAddEntry(entryName);

            _wallTypeToEntryMap[wallType] = (uint8_t)entryIndex;
        }
    }

    void AddEntryForPath(uint8_t pathType)
    {
        assert(pathType < std::size(_pathTypeToEntryMap));
        if (_pathTypeToEntryMap[pathType] == 255)
        {
            const char* entryName = RCT1::GetPathObject(pathType);
            if (!String::Equals(entryName, "        "))
            {
                size_t entryIndex = _pathEntries.GetOrAddEntry(entryName);
                _pathTypeToEntryMap[pathType] = (uint8_t)entryIndex;
            }
        }
    }

    void AddEntryForPathAddition(uint8_t pathAdditionType)
    {
        if (pathAdditionType == RCT1_PATH_ADDITION_NONE)
            return;

        if (_pathAdditionTypeToEntryMap[pathAdditionType] == 255)
        {
            uint8_t normalisedPathAdditionType = RCT1::NormalisePathAddition(pathAdditionType);
            if (_pathAdditionTypeToEntryMap[normalisedPathAdditionType] == 255)
            {
                const char* entryName = RCT1::GetPathAddtionObject(normalisedPathAdditionType);
                size_t entryIndex = _pathAdditionEntries.GetOrAddEntry(entryName);

                _pathAdditionTypeToEntryMap[normalisedPathAdditionType] = (uint8_t)entryIndex;
            }

            _pathAdditionTypeToEntryMap[pathAdditionType] = _pathAdditionTypeToEntryMap[normalisedPathAdditionType];
        }
    }

    void AddEntriesForSceneryTheme(uint8_t sceneryThemeType)
    {
        if (sceneryThemeType == RCT1_SCENERY_THEME_GENERAL || sceneryThemeType == RCT1_SCENERY_THEME_JUMPING_FOUNTAINS
            || sceneryThemeType == RCT1_SCENERY_THEME_GARDEN_CLOCK)
        {
            _sceneryThemeTypeToEntryMap[sceneryThemeType] = 254;
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
                size_t entryIndex = _sceneryGroupEntries.GetOrAddEntry(entryName);
                _sceneryThemeTypeToEntryMap[sceneryThemeType] = (uint8_t)entryIndex;
            }
        }
    }

    void ImportRides()
    {
        for (int32_t i = 0; i < RCT12_MAX_RIDES_IN_PARK; i++)
        {
            if (_s4.rides[i].type != RIDE_TYPE_NULL)
            {
                ImportRide(get_ride(i), &_s4.rides[i], i);
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
            dst->type = RCT1::GetRideType(src->type);
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
        dst->name = 0;
        if (is_user_string_id(src->name))
        {
            std::string rideName = GetUserString(src->name);
            if (!rideName.empty())
            {
                rct_string_id rideNameStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, rideName.c_str());
                if (rideNameStringId != 0)
                {
                    dst->name = rideNameStringId;
                }
            }
        }
        if (dst->name == 0)
        {
            ride_set_name_to_default(dst, rideEntry);
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
        dst->overall_view = src->overall_view;
        for (int32_t i = 0; i < RCT12_MAX_STATIONS_PER_RIDE; i++)
        {
            dst->stations[i].Start = src->station_starts[i];
            dst->stations[i].Height = src->station_height[i] / 2;
            dst->stations[i].Length = src->station_length[i];
            dst->stations[i].Depart = src->station_light[i];

            dst->stations[i].TrainAtStation = src->station_depart[i];

            // Direction is fixed later.
            if (src->entrance[i].xy == RCT_XY8_UNDEFINED)
                ride_clear_entrance_location(dst, i);
            else
                ride_set_entrance_location(dst, i, { src->entrance[i].x, src->entrance[i].y, src->station_height[i] / 2, 0 });

            if (src->exit[i].xy == RCT_XY8_UNDEFINED)
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
            dst->stations[i].Start.xy = RCT_XY8_UNDEFINED;
            dst->stations[i].TrainAtStation = 255;
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
        for (int32_t i = RCT1_MAX_TRAINS_PER_RIDE; i < MAX_VEHICLES_PER_RIDE; i++)
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
            dst->music = RideData4[dst->type].default_music;

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
        dst->build_date = src->build_date;
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
        dst->inversions = src->num_inversions;
        dst->boat_hire_return_direction = src->boat_hire_return_direction;
        dst->boat_hire_return_position = src->boat_hire_return_position;
        dst->measurement_index = src->data_logging_index;
        dst->chairlift_bullwheel_rotation = src->chairlift_bullwheel_rotation;
        for (int i = 0; i < 2; i++)
        {
            dst->chairlift_bullwheel_location[i] = src->chairlift_bullwheel_location[i];
            dst->chairlift_bullwheel_z[i] = src->chairlift_bullwheel_z[i] / 2;
        }
        dst->cur_test_track_z = src->cur_test_track_z / 2;
        dst->cur_test_track_location = src->cur_test_track_location;
        dst->testing_flags = src->testing_flags;
        dst->current_test_segment = src->current_test_segment;
        dst->current_test_station = 0xFF;
        dst->average_speed_test_timeout = src->average_speed_test_timeout;
        dst->slide_in_use = src->slide_in_use;
        dst->slide_peep_t_shirt_colour = RCT1::GetColour(src->slide_peep_t_shirt_colour);
        dst->spiral_slide_progress = src->spiral_slide_progress;
        // Doubles as slide_peep
        dst->maze_tiles = src->maze_tiles;

        // Finance / customers
        dst->upkeep_cost = src->upkeep_cost;
        dst->price = src->price;
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
        uint8_t i;
        Ride* ride;
        FOR_ALL_RIDES (i, ride)
        {
            for (uint8_t j = 0; j < std::size(ride->vehicles); j++)
            {
                uint16_t originalIndex = ride->vehicles[j];
                if (originalIndex != SPRITE_INDEX_NULL)
                {
                    ride->vehicles[j] = spriteIndexMap[originalIndex];
                }
            }
        }
    }

    void ImportRideMeasurements()
    {
        for (int32_t i = 0; i < MAX_RIDE_MEASUREMENTS; i++)
        {
            rct_ride_measurement* dst = get_ride_measurement(i);
            rct_ride_measurement* src = &_s4.ride_measurements[i];
            ImportRideMeasurement(dst, src);
        }
    }

    void ImportRideMeasurement(rct_ride_measurement* dst, rct_ride_measurement* src)
    {
        *dst = *src;
        for (int32_t i = 0; i < RIDE_MEASUREMENT_MAX_ITEMS; i++)
        {
            dst->velocity[i] /= 2;
            dst->altitude[i] /= 2;
            dst->vertical[i] /= 2;
            dst->lateral[i] /= 2;
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
        std::vector<rct_vehicle*> vehicles;
        uint16_t spriteIndexMap[RCT1_MAX_SPRITES];
        for (int i = 0; i < RCT1_MAX_SPRITES; i++)
        {
            spriteIndexMap[i] = SPRITE_INDEX_NULL;
            if (_s4.sprites[i].unknown.sprite_identifier == SPRITE_IDENTIFIER_VEHICLE)
            {
                rct1_vehicle* srcVehicle = &_s4.sprites[i].vehicle;
                if (srcVehicle->x != LOCATION_NULL)
                {
                    rct_vehicle* vehicle = (rct_vehicle*)create_sprite(SPRITE_IDENTIFIER_VEHICLE);
                    spriteIndexMap[i] = vehicle->sprite_index;
                    vehicles.push_back(vehicle);

                    ImportVehicle(vehicle, srcVehicle);

                    // If vehicle is the first car on a train add to train list
                    if (!vehicle->is_child)
                    {
                        move_sprite_to_list((rct_sprite*)vehicle, SPRITE_LIST_TRAIN * 2);
                    }
                }
            }
        }
        for (auto vehicle : vehicles)
        {
            FixVehicleLinks(vehicle, spriteIndexMap);
        }
        FixRideVehicleLinks(spriteIndexMap);
    }

    void ImportVehicle(rct_vehicle* dst, rct1_vehicle* src)
    {
        Ride* ride = get_ride(src->ride);
        uint8_t vehicleEntryIndex = RCT1::GetVehicleSubEntryIndex(src->vehicle_type);

        dst->sprite_identifier = SPRITE_IDENTIFIER_VEHICLE;
        dst->ride = src->ride;
        dst->ride_subtype = ride->subtype;

        dst->vehicle_type = vehicleEntryIndex;
        dst->is_child = src->is_child;
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
        dst->swing_sprite = src->swing_sprite;
        dst->swinging_car_var_0 = src->swinging_car_var_0;
        dst->var_4E = src->var_4E;
        dst->restraints_position = src->restraints_position;
        dst->spin_sprite = src->spin_sprite;
        dst->sound_vector_factor = src->sound_vector_factor;
        dst->spin_speed = src->spin_speed;
        dst->sound2_flags = src->sound2_flags;
        dst->sound1_id = RCT12_SOUND_ID_NULL;
        dst->sound2_id = RCT12_SOUND_ID_NULL;
        dst->var_C0 = src->var_C0;
        dst->var_C4 = src->var_C4;
        dst->animation_frame = src->animation_frame;
        dst->var_C8 = src->var_C8;
        dst->var_CA = src->var_CA;
        dst->var_CE = src->var_CE;
        dst->var_D3 = src->var_D3;
        dst->scream_sound_id = 255;
        dst->vehicle_sprite_type = src->vehicle_sprite_type;
        dst->bank_rotation = src->bank_rotation;

        // Seat rotation was not in RCT1
        dst->target_seat_rotation = 4;
        dst->seat_rotation = 4;

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

        dst->var_CD = src->var_CD;
        dst->track_x = src->track_x;
        dst->track_y = src->track_y;
        dst->track_z = src->track_z;
        dst->current_station = src->current_station;
        dst->track_type = src->track_type;
        dst->track_progress = src->track_progress;
        dst->vertical_drop_countdown = src->vertical_drop_countdown;
        dst->status = src->status;
        dst->sub_state = src->sub_state;
        dst->update_flags = src->update_flags;

        SetVehicleColours(dst, src);

        dst->mini_golf_current_animation = src->mini_golf_current_animation;
        dst->mini_golf_flags = src->mini_golf_flags;

        sprite_move(src->x, src->y, src->z, (rct_sprite*)dst);
        invalidate_sprite_2((rct_sprite*)dst);

        dst->num_peeps = src->num_peeps;
        dst->next_free_seat = src->next_free_seat;
    }

    void SetVehicleColours(rct_vehicle* dst, rct1_vehicle* src)
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

    void FixVehicleLinks(rct_vehicle* vehicle, const uint16_t* spriteIndexMap)
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

    void FixVehiclePeepLinks(rct_vehicle* vehicle, const uint16_t* spriteIndexMap)
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
                rct_peep* peep = (rct_peep*)create_sprite(SPRITE_IDENTIFIER_PEEP);
                move_sprite_to_list((rct_sprite*)peep, SPRITE_LIST_PEEP * 2);
                spriteIndexMap[i] = peep->sprite_index;

                ImportPeep(peep, srcPeep);
            }
        }
        for (size_t i = 0; i < MAX_SPRITES; i++)
        {
            rct_sprite* sprite = get_sprite(i);
            if (sprite->generic.sprite_identifier == SPRITE_IDENTIFIER_VEHICLE)
            {
                rct_vehicle* vehicle = (rct_vehicle*)sprite;
                FixVehiclePeepLinks(vehicle, spriteIndexMap);
            }
        }

        int i;
        Ride* ride;
        rct_peep* peep;

        FOR_ALL_RIDES (i, ride)
        {
            FixRidePeepLinks(ride, spriteIndexMap);
        }

        FOR_ALL_GUESTS (i, peep)
        {
            FixPeepNextInQueue(peep, spriteIndexMap);
        }

        // Fix the news items in advance
        for (i = 0; i < MAX_NEWS_ITEMS; i++)
        {
            rct12_news_item* newsItem = &_s4.messages[i];

            if (newsItem->Type == NEWS_ITEM_PEEP || newsItem->Type == NEWS_ITEM_PEEP_ON_RIDE)
            {
                newsItem->Assoc = MapSpriteIndex(newsItem->Assoc, spriteIndexMap);
            }
        }

        // The RCT2/OpenRCT2 structures are bigger than in RCT1, so set them to zero
        std::fill(std::begin(gStaffModes), std::end(gStaffModes), 0);
        std::fill(std::begin(gStaffPatrolAreas), std::end(gStaffPatrolAreas), 0);

        std::copy(std::begin(_s4.staff_modes), std::end(_s4.staff_modes), gStaffModes);

        FOR_ALL_STAFF (i, peep)
        {
            ImportStaffPatrolArea(peep);
        }
        // Only the individual patrol areas have been converted, so generate the combined patrol areas of each staff type
        staff_update_greyed_patrol_areas();
    }

    void ImportPeep(rct_peep* dst, rct1_peep* src)
    {
        dst->sprite_identifier = SPRITE_IDENTIFIER_PEEP;
        // Peep vs. staff (including which kind)
        dst->sprite_type = RCT1::GetPeepSpriteType(src->sprite_type);
        dst->action = static_cast<PeepActionType>(src->action);
        dst->special_sprite = src->special_sprite;
        dst->next_action_sprite_type = static_cast<PeepActionSpriteType>(src->next_action_sprite_type);
        dst->action_sprite_image_offset = src->action_sprite_image_offset;
        dst->no_action_frame_num = src->no_action_frame_num;
        dst->action_sprite_type = static_cast<PeepActionSpriteType>(src->action_sprite_type);
        dst->action_frame = src->action_frame;

        const rct_sprite_bounds* spriteBounds = g_peep_animation_entries[dst->sprite_type].sprite_bounds;
        dst->sprite_width = spriteBounds[dst->action_sprite_type].sprite_width;
        dst->sprite_height_negative = spriteBounds[dst->action_sprite_type].sprite_height_negative;
        dst->sprite_height_positive = spriteBounds[dst->action_sprite_type].sprite_height_positive;

        sprite_move(src->x, src->y, src->z, (rct_sprite*)dst);
        invalidate_sprite_2((rct_sprite*)dst);

        dst->sprite_direction = src->sprite_direction;

        // Peep name
        dst->name_string_idx = src->name_string_idx;
        if (is_user_string_id(src->name_string_idx))
        {
            std::string peepName = GetUserString(src->name_string_idx);
            if (!peepName.empty())
            {
                rct_string_id peepNameStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, peepName.c_str());
                if (peepNameStringId != 0)
                {
                    dst->name_string_idx = peepNameStringId;
                }
            }
        }

        dst->outside_of_park = src->outside_of_park;

        dst->state = static_cast<PeepState>(src->state);
        dst->sub_state = src->sub_state;
        dst->next_x = src->next_x;
        dst->next_y = src->next_y;
        dst->next_z = src->next_z / 2;
        dst->next_flags = src->next_flags;
        dst->var_37 = src->var_37;
        dst->time_to_consume = src->time_to_consume;
        dst->step_progress = src->step_progress;
        dst->vandalism_seen = src->vandalism_seen;

        dst->type = static_cast<PeepType>(src->type);

        dst->tshirt_colour = RCT1::GetColour(src->tshirt_colour);
        dst->trousers_colour = RCT1::GetColour(src->trousers_colour);
        dst->umbrella_colour = RCT1::GetColour(src->umbrella_colour);
        dst->hat_colour = RCT1::GetColour(src->hat_colour);

        // Balloons were always blue in RCT1 without AA/LL
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->balloon_colour = COLOUR_LIGHT_BLUE;
        }
        else
        {
            dst->balloon_colour = RCT1::GetColour(src->balloon_colour);
        }

        dst->destination_x = src->destination_x;
        dst->destination_y = src->destination_y;
        dst->destination_tolerance = src->destination_tolerance;
        dst->direction = src->direction;

        dst->energy = src->energy;
        dst->energy_target = src->energy_target;
        dst->happiness = src->happiness;
        dst->happiness_target = src->happiness_target;
        dst->nausea = src->nausea;
        dst->nausea_target = src->nausea_target;
        dst->hunger = src->hunger;
        dst->thirst = src->thirst;
        dst->toilet = src->toilet;
        dst->mass = src->mass;

        dst->litter_count = src->litter_count;
        dst->disgusting_count = src->disgusting_count;

        dst->intensity = src->intensity;
        dst->nausea_tolerance = src->nausea_tolerance;
        dst->window_invalidate_flags = 0;

        dst->current_ride = src->current_ride;
        dst->current_ride_station = src->current_ride_station;
        dst->current_train = src->current_train;
        dst->current_car = src->current_car;
        dst->current_seat = src->current_seat;
        dst->time_on_ride = src->time_on_ride;
        dst->days_in_queue = src->days_in_queue;

        dst->interaction_ride_index = src->interaction_ride_index;

        dst->id = src->id;
        dst->cash_in_pocket = src->cash_in_pocket;
        dst->cash_spent = src->cash_spent;
        dst->time_in_park = src->time_in_park;

        // This doubles as staff type
        dst->no_of_rides = src->no_of_rides;

        dst->no_of_drinks = src->no_of_drinks;
        dst->no_of_food = src->no_of_food;
        dst->no_of_souvenirs = src->no_of_souvenirs;

        dst->paid_to_enter = src->paid_to_enter;
        dst->paid_on_rides = src->paid_on_rides;
        dst->paid_on_drink = src->paid_on_drink;
        dst->paid_on_food = src->paid_on_food;
        dst->paid_on_souvenirs = src->paid_on_souvenirs;

        dst->voucher_arguments = src->voucher_arguments;
        dst->voucher_type = src->voucher_type;

        dst->surroundings_thought_timeout = src->surroundings_thought_timeout;
        dst->angriness = src->angriness;
        dst->time_lost = src->time_lost;

        for (size_t i = 0; i < 32; i++)
        {
            dst->rides_been_on[i] = src->rides_been_on[i];
        }
        for (size_t i = 0; i < 16; i++)
        {
            dst->ride_types_been_on[i] = src->ride_types_been_on[i];
        }

        dst->photo1_ride_ref = src->photo1_ride_ref;

        for (size_t i = 0; i < PEEP_MAX_THOUGHTS; i++)
        {
            dst->thoughts[i] = src->thoughts[i];
        }

        dst->previous_ride = src->previous_ride;
        dst->previous_ride_time_out = src->previous_ride_time_out;

        dst->path_check_optimisation = 0;
        dst->guest_heading_to_ride_id = src->guest_heading_to_ride_id;
        // Doubles as staff orders
        dst->peep_is_lost_countdown = src->peep_is_lost_countdown;
        // The ID is fixed later
        dst->next_in_queue = src->next_in_queue;

        dst->peep_flags = 0;
        dst->pathfind_goal.x = 0xFF;
        dst->pathfind_goal.y = 0xFF;
        dst->pathfind_goal.z = 0xFF;
        dst->pathfind_goal.direction = 0xFF;

        // Guests' favourite ride was only saved in LL.
        // Set it to N/A if the save comes from the original or AA.
        if (_gameVersion == FILE_VERSION_RCT1_LL)
        {
            dst->favourite_ride = src->favourite_ride;
            dst->favourite_ride_rating = src->favourite_ride_rating;
        }
        else
        {
            dst->favourite_ride = RIDE_ID_NULL;
            dst->favourite_ride_rating = 0;
        }

        dst->item_standard_flags = src->item_standard_flags;

        peep_update_name_sort(dst);

        if (dst->type == PEEP_TYPE_GUEST)
        {
            if (dst->outside_of_park && dst->state != PEEP_STATE_LEAVING_PARK)
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

    void FixPeepNextInQueue(rct_peep* peep, const uint16_t* spriteIndexMap)
    {
        peep->next_in_queue = MapSpriteIndex(peep->next_in_queue, spriteIndexMap);
    }

    void ImportStaffPatrolArea(rct_peep* staffmember)
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

        int32_t peepOffset = staffmember->staff_id * RCT12_PATROL_AREA_SIZE;
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
                staff_set_patrol_area(staffmember->staff_id, x, y, true);
            }
        }
    }

    void ImportLitter()
    {
        for (auto& sprite : _s4.sprites)
        {
            if (sprite.unknown.sprite_identifier == SPRITE_IDENTIFIER_LITTER)
            {
                rct_litter* srcLitter = &sprite.litter;

                rct_litter* litter = (rct_litter*)create_sprite(SPRITE_IDENTIFIER_LITTER);
                move_sprite_to_list((rct_sprite*)litter, SPRITE_LIST_LITTER * 2);

                litter->sprite_identifier = srcLitter->sprite_identifier;
                litter->type = srcLitter->type;

                litter->x = srcLitter->x;
                litter->y = srcLitter->y;
                litter->z = srcLitter->z;
                litter->sprite_direction = srcLitter->sprite_direction;
                litter->sprite_width = srcLitter->sprite_width;
                litter->sprite_height_positive = srcLitter->sprite_height_positive;
                litter->sprite_height_negative = srcLitter->sprite_height_negative;

                sprite_move(srcLitter->x, srcLitter->y, srcLitter->z, (rct_sprite*)litter);
                invalidate_sprite_2((rct_sprite*)litter);
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
                rct_sprite_generic* dst = (rct_sprite_generic*)create_sprite(SPRITE_IDENTIFIER_MISC);
                move_sprite_to_list((rct_sprite*)dst, SPRITE_LIST_MISC * 2);

                dst->sprite_identifier = src->sprite_identifier;
                dst->type = src->misc_identifier;
                dst->flags = src->flags;
                dst->sprite_direction = src->sprite_direction;
                dst->sprite_width = src->sprite_width;
                dst->sprite_height_negative = src->sprite_height_negative;
                dst->sprite_height_positive = src->sprite_height_positive;

                sprite_move(src->x, src->y, src->z, (rct_sprite*)dst);

                switch (src->misc_identifier)
                {
                    case SPRITE_MISC_STEAM_PARTICLE:
                        ImportSteamParticle((rct_steam_particle*)dst, (rct_steam_particle*)src);
                        break;
                    case SPRITE_MISC_MONEY_EFFECT:
                        ImportMoneyEffect((rct_money_effect*)dst, (rct_money_effect*)src);
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
                        ImportJumpingFountainWater((rct_jumping_fountain*)dst, (rct_jumping_fountain*)src);
                        break;
                    case SPRITE_MISC_BALLOON:
                        ImportBalloon((rct_balloon*)dst, (rct_balloon*)src);
                        break;
                    case SPRITE_MISC_DUCK:
                        ImportDuck((rct_duck*)dst, (rct_duck*)src);
                        break;
                }

                sprite_move(src->x, src->y, src->z, (rct_sprite*)dst);
                invalidate_sprite_2((rct_sprite*)dst);
            }
        }
    }

    void ImportMoneyEffect(rct_money_effect* dst, rct_money_effect* src)
    {
        dst->move_delay = src->move_delay;
        dst->num_movements = src->num_movements;
        dst->value = src->value;
        dst->offset_x = src->offset_x;
        dst->wiggle = src->wiggle;
    }

    void ImportSteamParticle(rct_steam_particle* dst, rct_steam_particle* src)
    {
        dst->frame = src->frame;
    }

    void ImportJumpingFountainWater(rct_jumping_fountain* dst, rct_jumping_fountain* src)
    {
        dst->fountain_flags = src->fountain_flags;
        dst->iteration = src->iteration;
        dst->num_ticks_alive = src->num_ticks_alive;
        dst->frame = src->frame;
    }

    void ImportBalloon(rct_balloon* dst, rct_balloon* src)
    {
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

    void ImportDuck(rct_duck* dst, rct_duck* src)
    {
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
            if (_s4.peep_spawn[i].x != PEEP_SPAWN_UNDEFINED)
            {
                PeepSpawn spawn = { _s4.peep_spawn[i].x, _s4.peep_spawn[i].y, _s4.peep_spawn[i].z * 16,
                                    _s4.peep_spawn[i].direction };
                gPeepSpawns.push_back(spawn);
            }
        }
    }

    void ImportMapAnimations()
    {
        // This is sketchy, ideally we should try to re-create them
        rct_map_animation* s4Animations = _s4.map_animations;
        for (size_t i = 0; i < RCT1_MAX_ANIMATED_OBJECTS; i++)
        {
            gAnimatedObjects[i] = s4Animations[i];
            gAnimatedObjects[i].baseZ /= 2;
        }
        gNumMapAnimations = _s4.num_map_animations;
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
        for (size_t i = 0; i < 6; i++)
        {
            gMarketingCampaignDaysLeft[i] = _s4.marketing_status[i];
            gMarketingCampaignRideIndex[i] = _s4.marketing_assoc[i];
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
            if (src->base_height == 0xFF)
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
        // Todo: allow for changing defition of OpenRCT2 tile element types - replace with a map
        uint8_t tileElementType = src->GetType();
        dst->ClearAs(tileElementType);
        dst->SetDirection(src->GetDirection());
        dst->flags = src->flags;
        dst->base_height = src->base_height / 2;
        dst->clearance_height = src->clearance_height / 2;

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
                uint8_t entryIndex = _pathTypeToEntryMap[pathType];

                dst2->SetDirection(0);
                dst2->flags &= ~(TILE_ELEMENT_FLAG_BROKEN | TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE);

                dst2->SetPathEntryIndex(entryIndex);
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
                    uint8_t railingsEntryIndex;
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
                uint8_t additionType = dst2->GetAddition();
                if (additionType != RCT1_PATH_ADDITION_NONE)
                {
                    uint8_t normalisedType = RCT1::NormalisePathAddition(additionType);
                    entryIndex = _pathAdditionTypeToEntryMap[normalisedType];
                    if (additionType != normalisedType)
                    {
                        dst2->flags |= TILE_ELEMENT_FLAG_BROKEN;
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
                dst2->SetStationIndex(src2->GetStationIndex());
                dst2->SetHasChain(src2->HasChain());
                dst2->SetHasCableLift(false);
                dst2->SetInverted(src2->IsInverted());
                dst2->SetBrakeBoosterSpeed(src2->GetBrakeBoosterSpeed());
                dst2->SetHasGreenLight(src2->HasGreenLight());
                dst2->SetSeatRotation(4);
                dst2->SetMazeEntry(src2->GetMazeEntry());
                dst2->SetPhotoTimeout(src2->GetPhotoTimeout());
                // Skipping IsHighlighted()

                // TODO: Import Door A and Door B states.

                break;
            }
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            {
                auto dst2 = dst->AsSmallScenery();
                auto src2 = src->AsSmallScenery();

                uint8_t entryIndex = _smallSceneryTypeToEntryMap[src2->GetEntryIndex()];
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
                    uint8_t pathType = src2->GetPathType();
                    if (pathType == 0)
                    {
                        pathType = RCT1_FOOTPATH_TYPE_TARMAC_GRAY;
                    }
                    uint8_t entryIndex = _pathTypeToEntryMap[pathType];
                    dst2->SetPathType(entryIndex & 0x7F);
                }

                break;
            }
            case TILE_ELEMENT_TYPE_WALL:
            {
                auto dst2 = dst->AsWall();
                auto src2 = src->AsWall();

                dst2->SetEntryIndex(src2->GetEntryIndex());
                dst2->SetSlope(src2->GetSlope());
                dst2->SetPrimaryColour(RCT1::GetColour(src2->GetRCT1WallColour()));
                dst2->SetRawRCT1Data(src2->GetRawRCT1WallTypeData());

                break;
            }
            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            {
                auto dst2 = dst->AsLargeScenery();
                auto src2 = src->AsLargeScenery();

                uint8_t type = src2->GetEntryIndex();
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

                uint8_t index = src2->GetIndex();
                dst2->SetIndex(index);
                dst2->SetPosition(src2->GetPosition());
                dst2->SetAllowedEdges(src2->GetAllowedEdges());

                rct_banner* srcBanner = &_s4.banners[index];
                rct_banner* dstBanner = &gBanners[index];
                ImportBanner(dstBanner, srcBanner);

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
        std::fill(std::begin(_researchRideEntryUsed), std::end(_researchRideEntryUsed), 0);
        std::fill(std::begin(_researchRideTypeUsed), std::end(_researchRideTypeUsed), 0);

        // The first six scenery groups are always available
        for (uint8_t i = 0; i < 6; i++)
        {
            research_insert_scenery_group_entry(i, true);
        }

        bool researched = true;
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
                else if (researchItem->item == RCT1_RESEARCH_END_RESEARCHABLE)
                {
                    continue;
                }
                else if (researchItem->item == RCT1_RESEARCH_END)
                {
                    break;
                }
            }

            switch (researchItem->type)
            {
                case RCT1_RESEARCH_TYPE_THEME:
                {
                    uint8_t rct1SceneryTheme = researchItem->item;
                    uint8_t sceneryGroupEntryIndex = _sceneryThemeTypeToEntryMap[rct1SceneryTheme];
                    if (sceneryGroupEntryIndex != 254 && sceneryGroupEntryIndex != 255)
                    {
                        research_insert_scenery_group_entry(sceneryGroupEntryIndex, researched);
                    }
                    break;
                }
                case RCT1_RESEARCH_TYPE_RIDE:
                {
                    uint8_t rct1RideType = researchItem->item;
                    _researchRideTypeUsed[rct1RideType] = true;

                    // Add all vehicles for this ride type that are researched or before this research item
                    uint32_t numVehicles = 0;
                    for (size_t j = 0; j < researchListCount; j++)
                    {
                        const rct1_research_item* researchItem2 = &researchList[j];
                        if (researchItem2->flags == RCT1_RESEARCH_FLAGS_SEPARATOR
                            && (researchItem2->item == RCT1_RESEARCH_END_RESEARCHABLE
                                || researchItem2->item == RCT1_RESEARCH_END_AVAILABLE))
                        {
                            continue;
                        }

                        if (researchItem2->type == RCT1_RESEARCH_TYPE_VEHICLE && researchItem2->related_ride == rct1RideType)
                        {
                            // Only add the vehicles that were listed before this ride, otherwise we might
                            // change the research order
                            if (j < i)
                            {
                                InsertResearchVehicle(researchItem2, researched);
                            }
                            numVehicles++;
                        }
                    }

                    if (numVehicles == 0)
                    {
                        // No vehicles found so just add the default for this ride
                        uint8_t rideEntryIndex = _rideTypeToRideEntryMap[rct1RideType];
                        Guard::Assert(rideEntryIndex != RIDE_ENTRY_INDEX_NULL, "rideEntryIndex was RIDE_ENTRY_INDEX_NULL");
                        if (!_researchRideEntryUsed[rideEntryIndex])
                        {
                            _researchRideEntryUsed[rideEntryIndex] = true;
                            research_insert_ride_entry(rideEntryIndex, researched);
                        }
                    }

                    break;
                }
                case RCT1_RESEARCH_TYPE_VEHICLE:
                    // Only add vehicle if the related ride has been seen, this to make sure that vehicles
                    // are researched only after the ride has been researched
                    if (_researchRideTypeUsed[researchItem->related_ride])
                    {
                        InsertResearchVehicle(researchItem, researched);
                    }

                    break;
                case RCT1_RESEARCH_TYPE_SPECIAL:
                    // Not supported
                    break;
            }
        }

        // Also import the tables that register the invented status, in case the research list is damaged.
        for (int32_t rideType = 0; rideType < RCT1_RIDE_TYPE_COUNT; rideType++)
        {
            int32_t quadIndex = rideType >> 5;
            int32_t bitIndex = rideType & 0x1F;
            bool invented = (_s4.available_rides[quadIndex] & ((uint32_t)1 << bitIndex));

            if (invented)
            {
                ride_type_set_invented(RCT1::GetRideType(rideType));
            }
        }

        for (int32_t vehicleType = 0; vehicleType < RCT1_VEHICLE_TYPE_COUNT; vehicleType++)
        {
            int32_t quadIndex = vehicleType >> 5;
            int32_t bitIndex = vehicleType & 0x1F;
            bool invented = (_s4.available_vehicles[quadIndex] & ((uint32_t)1 << bitIndex));

            if (invented)
            {
                ride_entry_set_invented(_vehicleTypeToRideEntryMap[vehicleType]);
            }
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

        ConvertResearchEntry(&gResearchNextItem, _s4.next_research_item, _s4.next_research_type);
        if (gResearchNextItem.IsInventedEndMarker())
        {
            gResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
            gResearchProgress = 0;
        }
        ConvertResearchEntry(&gResearchLastItem, _s4.last_research_item, _s4.last_research_type);
    }

    void InsertResearchVehicle(const rct1_research_item* researchItem, bool researched)
    {
        uint8_t vehicle = researchItem->item;
        uint8_t rideEntryIndex = _vehicleTypeToRideEntryMap[vehicle];

        if (!_researchRideEntryUsed[rideEntryIndex])
        {
            _researchRideEntryUsed[rideEntryIndex] = true;
            research_insert_ride_entry(rideEntryIndex, researched);
        }
    }

    void ImportParkName()
    {
        std::string parkName = std::string(_s4.scenario_name);
        if (is_user_string_id((rct_string_id)_s4.park_name_string_index))
        {
            std::string userString = GetUserString(_s4.park_name_string_index);
            if (!userString.empty())
            {
                parkName = userString;
            }
        }

        rct_string_id stringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, parkName.c_str());
        if (stringId != 0)
        {
            gParkName = stringId;
            gParkNameArgs = 0;
        }
    }

    void ImportParkFlags()
    {
        // Date and srand
        gScenarioTicks = _s4.ticks;
        scenario_rand_seed(_s4.random_a, _s4.random_b);
        gDateMonthsElapsed = _s4.month;
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
            NewsItem* dst = &gNewsItems[i];

            dst->Type = src->Type;
            dst->Flags = src->Flags;
            dst->Ticks = src->Ticks;
            dst->MonthYear = src->MonthYear;
            dst->Day = src->Day;
            std::copy(std::begin(src->Text), std::end(src->Text), dst->Text);

            if (dst->Type == NEWS_ITEM_RESEARCH)
            {
                uint8_t researchItem = src->Assoc & 0x000000FF;
                uint8_t researchType = (src->Assoc & 0x00FF0000) >> 16;

                rct_research_item tmpResearchItem = {};
                ConvertResearchEntry(&tmpResearchItem, researchItem, researchType);
                dst->Assoc = (uint32_t)tmpResearchItem.rawValue;
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

    void ConvertResearchEntry(rct_research_item* dst, uint8_t srcItem, uint8_t srcType)
    {
        dst->rawValue = RESEARCHED_ITEMS_SEPARATOR;
        if (srcType == RCT1_RESEARCH_TYPE_RIDE)
        {
            uint8_t entryIndex = _rideTypeToRideEntryMap[srcItem];

            if (entryIndex != RIDE_ENTRY_INDEX_NULL)
            {
                rct_ride_entry* rideEntry = get_ride_entry(entryIndex);

                if (rideEntry != nullptr)
                {
                    dst->entryIndex = entryIndex;
                    dst->baseRideType = ride_entry_get_first_non_null_ride_type(rideEntry);
                    dst->type = RESEARCH_ENTRY_TYPE_RIDE;
                    dst->flags = 0;
                    dst->category = rideEntry->category[0];
                }
            }
        }
        else if (srcType == RCT1_RESEARCH_TYPE_VEHICLE)
        {
            uint8_t entryIndex = _vehicleTypeToRideEntryMap[srcItem];

            if (entryIndex != RIDE_ENTRY_INDEX_NULL)
            {
                rct_ride_entry* rideEntry = get_ride_entry(entryIndex);

                if (rideEntry != nullptr)
                {
                    dst->entryIndex = entryIndex;
                    dst->baseRideType = ride_entry_get_first_non_null_ride_type(rideEntry);
                    dst->type = RESEARCH_ENTRY_TYPE_RIDE;
                    dst->flags = 0;
                    dst->category = rideEntry->category[0];
                }
            }
        }
        else if (srcType == RCT1_RESEARCH_TYPE_THEME)
        {
            uint8_t entryIndex = _sceneryThemeTypeToEntryMap[srcItem];

            if (entryIndex != 254 && entryIndex != 255)
            {
                dst->entryIndex = entryIndex;
                dst->type = RESEARCH_ENTRY_TYPE_SCENERY;
                dst->category = RESEARCH_CATEGORY_SCENERY_GROUP;
                dst->flags = 0;
            }
        }
    }

    void ImportClimate()
    {
        gClimate = _s4.climate;
        gClimateUpdateTimer = _s4.climate_timer;
        gClimateCurrent.Temperature = _s4.temperature;
        gClimateCurrent.Weather = _s4.weather;
        gClimateCurrent.WeatherEffect = WEATHER_EFFECT_NONE;
        gClimateCurrent.WeatherGloom = _s4.weather_gloom;
        gClimateCurrent.RainLevel = _s4.rain;
        gClimateNext.Temperature = _s4.target_temperature;
        gClimateNext.Weather = _s4.target_weather;
        gClimateNext.WeatherEffect = WEATHER_EFFECT_NONE;
        gClimateNext.WeatherGloom = _s4.target_weather_gloom;
        gClimateNext.RainLevel = _s4.target_rain;
    }

    void ImportScenarioNameDetails()
    {
        std::string name = String::ToStd(_s4.scenario_name);
        std::string details;

        int32_t scNumber = _s4.scenario_slot_index;
        if (scNumber != -1)
        {
            source_desc sourceDesc;
            if (scenario_get_source_desc_by_id(scNumber, &sourceDesc))
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
        gScenarioObjectiveType = _s4.scenario_objective_type;
        gScenarioObjectiveYear = _s4.scenario_objective_years;
        gScenarioObjectiveNumGuests = _s4.scenario_objective_num_guests;
        // RCT1 used a different way of calculating the park value.
        // This is corrected here, but since scenario_objective_currency doubles as minimum excitement rating,
        // we need to check the goal to avoid affecting scenarios like Volcania.
        if (_s4.scenario_objective_type == OBJECTIVE_PARK_VALUE_BY)
            gScenarioObjectiveCurrency = CorrectRCT1ParkValue(_s4.scenario_objective_currency);
        else
            gScenarioObjectiveCurrency = _s4.scenario_objective_currency;
    }

    void ImportSavedView()
    {
        gSavedViewX = _s4.view_x;
        gSavedViewY = _s4.view_y;
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
                nextFreeTileElement->flags = TILE_ELEMENT_FLAG_LAST_TILE;
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
            nextFreeTileElement->flags = TILE_ELEMENT_FLAG_LAST_TILE;
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
        // The user might attempt to load a save while in pause mode.
        // Since we cannot place walls in pause mode without a cheat, temporarily turn it on.
        bool oldCheatValue = gCheatsBuildInPauseMode;
        gCheatsBuildInPauseMode = true;

        for (int32_t x = 0; x < RCT1_MAX_MAP_SIZE; x++)
        {
            for (int32_t y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                TileElement* tileElement = map_get_first_element_at(x, y);
                do
                {
                    if (tileElement->GetType() == TILE_ELEMENT_TYPE_WALL)
                    {
                        TileElement originalTileElement = *tileElement;
                        tile_element_remove(tileElement);

                        for (int32_t edge = 0; edge < 4; edge++)
                        {
                            int32_t type = originalTileElement.AsWall()->GetRCT1WallType(edge);

                            if (type != -1)
                            {
                                int32_t colourA = RCT1::GetColour(originalTileElement.AsWall()->GetRCT1WallColour());
                                int32_t colourB = 0;
                                int32_t colourC = 0;
                                ConvertWall(&type, &colourA, &colourB);

                                type = _wallTypeToEntryMap[type];
                                const uint8_t flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                                    | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_PATH_SCENERY;

                                wall_place(type, x * 32, y * 32, 0, edge, colourA, colourB, colourC, flags);
                            }
                        }
                        break;
                    }
                } while (!(tileElement++)->IsLastForTile());
            }
        }

        gCheatsBuildInPauseMode = oldCheatValue;
    }

    void ConvertWall(int32_t* type, int32_t* colourA, int32_t* colourB)
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
            case RCT1_WALL_TYPE_LARGE_CREY_CASTLE:
            case RCT1_WALL_TYPE_LARGE_CREY_CASTLE_CROSS:
            case RCT1_WALL_TYPE_LARGE_CREY_CASTLE_GATE:
            case RCT1_WALL_TYPE_LARGE_CREY_CASTLE_WINDOW:
            case RCT1_WALL_TYPE_MEDIUM_CREY_CASTLE:
                *colourA = COLOUR_GREY;
                break;
        }
    }

    void ImportBanner(rct_banner* dst, rct_banner* src)
    {
        *dst = *src;
        dst->colour = RCT1::GetColour(src->colour);

        dst->string_idx = STR_DEFAULT_SIGN;
        if (is_user_string_id(src->string_idx))
        {
            std::string bannerText = GetUserString(src->string_idx);
            if (!bannerText.empty())
            {
                rct_string_id bannerTextStringId = user_string_allocate(USER_STRING_DUPLICATION_PERMITTED, bannerText.c_str());
                if (bannerTextStringId != 0)
                {
                    dst->string_idx = bannerTextStringId;
                }
            }
        }
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

            CoordsXYZD entrance;
            entrance.x = it.x * 32;
            entrance.y = it.y * 32;
            entrance.z = element->base_height * 8;
            entrance.direction = element->GetDirection();
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
        const char* originalString = _s4.string_table[(stringId - USER_STRING_START) % 1024];
        return rct2_to_utf8(originalString, RCT2_LANGUAGE_ID_ENGLISH_UK);
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
     * Counts the block sections. The reason this iterates over the map is to avoid getting into infinite loops,
     * which can happen with hacked parks.
     */
    void CountBlockSections()
    {
        for (int32_t x = 0; x < RCT1_MAX_MAP_SIZE; x++)
        {
            for (int32_t y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                TileElement* tileElement = map_get_first_element_at(x, y);
                do
                {
                    if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
                    {
                        // Lift hill tops are the only pieces present in RCT1 that can count as a block brake.
                        if (!tileElement->AsTrack()->HasChain())
                            continue;

                        uint8_t trackType = tileElement->AsTrack()->GetTrackType();
                        switch (trackType)
                        {
                            case TRACK_ELEM_25_DEG_UP_TO_FLAT:
                            case TRACK_ELEM_60_DEG_UP_TO_FLAT:
                            case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
                            case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
                                break;
                            default:
                                continue;
                        }

                        ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
                        Ride* ride = get_ride(rideIndex);
                        ride->num_block_brakes++;
                    }
                } while (!(tileElement++)->IsLastForTile());
            }
        }
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
    return ((type & 0xC0) >> 3) | ((entryIndex & 0xE0) >> 5);
}
