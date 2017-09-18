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

#include <memory>
#include <vector>
#include "../core/Collections.hpp"
#include "../core/Console.hpp"
#include "../core/Exception.hpp"
#include "../core/FileStream.hpp"
#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "../object/Object.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../ParkImporter.h"
#include "../ride/station.h"
#include "../scenario/ScenarioSources.h"
#include "Tables.h"

extern "C"
{
    #include "../audio/audio.h"
    #include "../cheats.h"
    #include "../Editor.h"
    #include "../game.h"
    #include "../interface/window.h"
    #include "../localisation/date.h"
    #include "../localisation/localisation.h"
    #include "../management/finance.h"
    #include "../management/marketing.h"
    #include "../object.h"
    #include "../object/ObjectManager.h"
    #include "../peep/peep.h"
    #include "../peep/staff.h"
    #include "../rct1.h"
    #include "../ride/ride_data.h"
    #include "../ride/track.h"
    #include "../util/sawyercoding.h"
    #include "../util/util.h"
    #include "../world/Climate.h"
    #include "../world/footpath.h"
    #include "../world/map_animation.h"
    #include "../world/park.h"
    #include "../world/entrance.h"
    #include "../world/scenery.h"
}

class EntryList
{
private:
    std::vector<const char *> _entries;

public:
    size_t GetCount() const
    {
        return _entries.size();
    }

    const std::vector<const char *> & GetEntries() const
    {
        return _entries;
    }

    size_t GetOrAddEntry(const char * entryName)
    {
        size_t entryIndex = Collections::IndexOf(_entries, entryName, true);
        if (entryIndex == SIZE_MAX)
        {
            entryIndex = _entries.size();
            _entries.push_back(entryName);
        }
        return entryIndex;
    }

    void AddRange(std::initializer_list<const char *> initializerList)
    {
        Collections::AddRange(_entries, initializerList);
    }
};

class S4Importer final : public IParkImporter
{
private:
    const utf8 * _s4Path = nullptr;
    rct1_s4      _s4 = { 0 };
    uint8        _gameVersion = 0;
    uint8        _parkValueConversionFactor = 0;

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
    uint8 _rideTypeToRideEntryMap[96];
    uint8 _vehicleTypeToRideEntryMap[96];
    uint8 _smallSceneryTypeToEntryMap[256];
    uint8 _largeSceneryTypeToEntryMap[256];
    uint8 _wallTypeToEntryMap[256];
    uint8 _pathTypeToEntryMap[24];
    uint8 _pathAdditionTypeToEntryMap[16];
    uint8 _sceneryThemeTypeToEntryMap[24];

    // Research
    uint8 _researchRideEntryUsed[128];
    uint8 _researchRideTypeUsed[128];

public:
    ParkLoadResult Load(const utf8 * path) override
    {
        const utf8 * extension = Path::GetExtension(path);
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
            throw Exception("Invalid RCT1 park extension.");
        }
    }

    ParkLoadResult LoadSavedGame(const utf8 * path, bool skipObjectCheck = false) override
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        auto result = LoadFromStream(&fs, false, skipObjectCheck, path);
        return result;
    }

    ParkLoadResult LoadScenario(const utf8 * path, bool skipObjectCheck = false) override
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        auto result = LoadFromStream(&fs, true, skipObjectCheck, path);
        return result;
    }

    ParkLoadResult LoadFromStream(IStream * stream,
                                  bool isScenario,
                                  bool skipObjectCheck = false,
                                  const utf8 * path = String::Empty) override
    {
        size_t dataSize = stream->GetLength() - stream->GetPosition();
        std::unique_ptr<uint8> data = std::unique_ptr<uint8>(stream->ReadArray<uint8>(dataSize));
        std::unique_ptr<uint8> decodedData = std::unique_ptr<uint8>(Memory::Allocate<uint8>(sizeof(rct1_s4)));

        size_t decodedSize;
        sint32 fileType = sawyercoding_detect_file_type(data.get(), dataSize);
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
            Memory::Copy<void>(&_s4, decodedData.get(), sizeof(rct1_s4));
            if (_s4Path)
            {
                Memory::Free(_s4Path);
            }
            _s4Path = String::Duplicate(path);

            if (!skipObjectCheck)
            {
                InitialiseEntryMaps();
                CreateAvailableObjectMappings();

                auto missingObjects = GetInvalidObjects();
                if (missingObjects.size() > 0)
                {
                    return ParkLoadResult::CreateMissingObjects(missingObjects);
                }
            }
        }
        else
        {
            throw Exception("Unable to decode park.");
        }
        return ParkLoadResult::CreateOK();
    }

    void Import() override
    {
        Initialise();

        CreateAvailableObjectMappings();
        LoadObjects();

        ImportRides();
        ImportRideMeasurements();
        ImportSprites();
        ImportMapElements();
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

        // Importing the strings is done later on, although that approach needs looking at.
        //game_convert_strings_to_utf8();
        game_convert_news_items_to_utf8();
        map_count_remaining_land_rights();
    }

    bool GetDetails(scenario_index_entry * dst) override
    {
        bool result = false;
        Memory::Set(dst, 0, sizeof(scenario_index_entry));

        source_desc desc;
        if (ScenarioSources::TryGetById(_s4.scenario_slot_index, &desc))
        {
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

            std::string name = std::string(_s4.scenario_name, sizeof(_s4.scenario_name));
            std::string details;
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

            result = true;
        }

        return result;
    }

    sint32 CorrectRCT1ParkValue(money32 oldParkValue)
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
                _parkValueConversionFactor = (calculate_park_value() * 10) / _s4.park_value;
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
    void Initialise()
    {
        _gameVersion = sawyercoding_detect_rct1_version(_s4.game_version) & FILE_VERSION_MASK;
        // Avoid reusing the value used for last import
        _parkValueConversionFactor = 0;

        InitialiseEntryMaps();
        uint16 mapSize = _s4.map_size == 0 ? 128 : _s4.map_size;

        String::Set(gScenarioFileName, sizeof(gScenarioFileName), Path::GetFileName(_s4Path));

        // Do map initialisation, same kind of stuff done when loading scenario editor
        GetObjectManager()->UnloadAll();
        game_init_all(mapSize);
        gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
        gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        gS6Info.category = SCENARIO_CATEGORY_OTHER;
    }

    void InitialiseEntryMaps()
    {
        Memory::Set(_rideTypeToRideEntryMap, 255, sizeof(_rideTypeToRideEntryMap));
        Memory::Set(_vehicleTypeToRideEntryMap, 255, sizeof(_vehicleTypeToRideEntryMap));
        Memory::Set(_smallSceneryTypeToEntryMap, 255, sizeof(_smallSceneryTypeToEntryMap));
        Memory::Set(_largeSceneryTypeToEntryMap, 255, sizeof(_largeSceneryTypeToEntryMap));
        Memory::Set(_wallTypeToEntryMap, 255, sizeof(_wallTypeToEntryMap));
        Memory::Set(_pathTypeToEntryMap, 255, sizeof(_pathTypeToEntryMap));
        Memory::Set(_pathAdditionTypeToEntryMap, 255, sizeof(_pathAdditionTypeToEntryMap));
        Memory::Set(_sceneryThemeTypeToEntryMap, 255, sizeof(_sceneryThemeTypeToEntryMap));
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
        const rct1_research_item * researchList = GetResearchList(&researchListCount);
        for (size_t i = 0; i < researchListCount; i++)
        {
            const rct1_research_item * researchItem = &researchList[i];

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

            switch (researchItem->category) {
            case RCT1_RESEARCH_CATEGORY_THEME:
                AddEntriesForSceneryTheme(researchItem->item);
                break;
            case RCT1_RESEARCH_CATEGORY_RIDE:
            {
                uint8 rideType = researchItem->item;

                // Add all vehicles for this ride type
                uint32 numVehicles = 0;
                for (size_t j = 0; j < researchListCount; j++)
                {
                    const rct1_research_item *researchItem2 = &researchList[j];
                    if (researchItem2->flags == RCT1_RESEARCH_FLAGS_SEPARATOR)
                    {
                        if (researchItem2->item == RCT1_RESEARCH_END_RESEARCHABLE ||
                            researchItem2->item == RCT1_RESEARCH_END_AVAILABLE)
                        {
                            continue;
                        }
                        else if (researchItem2->item == RCT1_RESEARCH_END)
                        {
                            break;
                        }
                    }

                    if (researchItem2->category == RCT1_RESEARCH_CATEGORY_VEHICLE &&
                        researchItem2->related_ride == rideType)
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
    }

    void AddAvailableEntriesFromMap()
    {
        size_t maxTiles = 128 * 128;
        size_t tileIndex = 0;
        rct_map_element * mapElement = _s4.map_elements;

        while (tileIndex < maxTiles)
        {
            switch (map_element_get_type(mapElement)) {
            case MAP_ELEMENT_TYPE_PATH:
            {
                uint8 pathColour = map_element_get_direction(mapElement);
                uint8 pathType = (mapElement->properties.path.type & 0xF0) >> 4;

                pathType = (pathType << 2) | pathColour;
                uint8 pathAdditionsType = mapElement->properties.path.additions & 0x0F;

                AddEntryForPath(pathType);
                AddEntryForPathAddition(pathAdditionsType);
                break;
            }
            case MAP_ELEMENT_TYPE_SCENERY:
                AddEntryForSmallScenery(mapElement->properties.scenery.type);
                break;
            case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
                AddEntryForLargeScenery(mapElement->properties.scenerymultiple.type & MAP_ELEMENT_LARGE_TYPE_MASK);
                break;
            case MAP_ELEMENT_TYPE_WALL:
            {
                uint8  var_05 = mapElement->properties.wall.colour_3;
                uint16 var_06 = mapElement->properties.wall.colour_1 |
                               (mapElement->properties.wall.animation << 8);

                for (sint32 edge = 0; edge < 4; edge++)
                {
                    sint32 typeA = (var_05 >> (edge * 2)) & 3;
                    sint32 typeB = (var_06 >> (edge * 4)) & 0x0F;
                    if (typeB != 0x0F)
                    {
                        uint8 type = typeA | (typeB << 2);
                        AddEntryForWall(type);
                    }
                }
                break;
            }
            }

            if (map_element_is_last_for_tile(mapElement++))
            {
                tileIndex++;
            }
        }
    }

    void AddAvailableEntriesFromRides()
    {
        for (size_t i = 0; i < Util::CountOf(_s4.rides); i++)
        {
            rct1_ride * ride = &_s4.rides[i];
            if (ride->type != RCT1_RIDE_TYPE_NULL && RCT1::RideTypeUsesVehicles(ride->type))
            {
                AddEntryForVehicleType(ride->type, ride->vehicle_type);
            }
        }
    }

    void AddAvailableEntriesFromSceneryGroups()
    {
        for (sint32 sceneryTheme = 0; sceneryTheme <= RCT1_SCENERY_THEME_PAGODA; sceneryTheme++)
        {
            if (sceneryTheme != 0 &&
                _sceneryThemeTypeToEntryMap[sceneryTheme] == 255) continue;

            std::vector<const char *> objects = RCT1::GetSceneryObjects(sceneryTheme);
            for (const char * objectName : objects)
            {
                const rct_object_entry * foundEntry = object_list_find_by_name(objectName);
                if (foundEntry != nullptr)
                {
                    uint8 objectType = foundEntry->flags & 0x0F;
                    switch (objectType) {
                    case OBJECT_TYPE_SMALL_SCENERY:
                    case OBJECT_TYPE_LARGE_SCENERY:
                    case OBJECT_TYPE_WALLS:
                    case OBJECT_TYPE_PATHS:
                    case OBJECT_TYPE_PATH_BITS:
                        EntryList * entries = GetEntryList(objectType);

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
        const char * entryName;

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

    void AddEntryForRideType(uint8 rideType)
    {
        assert(rideType < Util::CountOf(_rideTypeToRideEntryMap));
        if (_rideTypeToRideEntryMap[rideType] == 255)
        {
            const char * entryName = RCT1::GetRideTypeObject(rideType);
            size_t entryIndex = _rideEntries.GetOrAddEntry(entryName);

            _rideTypeToRideEntryMap[rideType] = (uint8)entryIndex;
        }
    }

    void AddEntryForVehicleType(uint8 rideType, uint8 vehicleType)
    {
        assert(vehicleType < Util::CountOf(_vehicleTypeToRideEntryMap));
        if (_vehicleTypeToRideEntryMap[vehicleType] == 255)
        {
            const char * entryName = RCT1::GetVehicleObject(vehicleType);
            size_t entryIndex = _rideEntries.GetOrAddEntry(entryName);

            _vehicleTypeToRideEntryMap[vehicleType] = (uint8)entryIndex;
            _rideTypeToRideEntryMap[rideType] = (uint8)entryIndex;
        }
    }

    void AddEntryForSmallScenery(uint8 smallSceneryType)
    {
        assert(smallSceneryType < Util::CountOf(_smallSceneryTypeToEntryMap));
        if (_smallSceneryTypeToEntryMap[smallSceneryType] == 255)
        {
            const char * entryName = RCT1::GetSmallSceneryObject(smallSceneryType);
            size_t entryIndex = _smallSceneryEntries.GetOrAddEntry(entryName);

            _smallSceneryTypeToEntryMap[smallSceneryType] = (uint8)entryIndex;
        }
    }

    void AddEntryForLargeScenery(uint8 largeSceneryType)
    {
        assert(largeSceneryType < Util::CountOf(_largeSceneryTypeToEntryMap));
        if (_largeSceneryTypeToEntryMap[largeSceneryType] == 255)
        {
            const char * entryName = RCT1::GetLargeSceneryObject(largeSceneryType);
            size_t entryIndex = _largeSceneryEntries.GetOrAddEntry(entryName);

            _largeSceneryTypeToEntryMap[largeSceneryType] = (uint8)entryIndex;
        }
    }

    void AddEntryForWall(uint8 wallType)
    {
        assert(wallType < Util::CountOf(_wallTypeToEntryMap));
        if (_wallTypeToEntryMap[wallType] == 255)
        {
            const char * entryName = RCT1::GetWallObject(wallType);
            size_t entryIndex = _wallEntries.GetOrAddEntry(entryName);

            _wallTypeToEntryMap[wallType] = (uint8)entryIndex;
        }
    }

    void AddEntryForPath(uint8 pathType)
    {
        assert(pathType < Util::CountOf(_pathTypeToEntryMap));
        if (_pathTypeToEntryMap[pathType] == 255)
        {
            const char * entryName = RCT1::GetPathObject(pathType);
            size_t entryIndex = _pathEntries.GetOrAddEntry(entryName);

            _pathTypeToEntryMap[pathType] = (uint8)entryIndex;
        }
    }

    void AddEntryForPathAddition(uint8 pathAdditionType)
    {
        if (pathAdditionType == RCT1_PATH_ADDITION_NONE) return;

        if (_pathAdditionTypeToEntryMap[pathAdditionType] == 255)
        {
            uint8 normalisedPathAdditionType = RCT1::NormalisePathAddition(pathAdditionType);
            if (_pathAdditionTypeToEntryMap[normalisedPathAdditionType] == 255)
            {
                const char * entryName = RCT1::GetPathAddtionObject(normalisedPathAdditionType);
                size_t entryIndex = _pathAdditionEntries.GetOrAddEntry(entryName);

                _pathAdditionTypeToEntryMap[normalisedPathAdditionType] = (uint8)entryIndex;
            }

            _pathAdditionTypeToEntryMap[pathAdditionType] = _pathAdditionTypeToEntryMap[normalisedPathAdditionType];
        }
    }

    void AddEntriesForSceneryTheme(uint8 sceneryThemeType)
    {
        if (sceneryThemeType == RCT1_SCENERY_THEME_GENERAL ||
            sceneryThemeType == RCT1_SCENERY_THEME_JUMPING_FOUNTAINS ||
            sceneryThemeType == RCT1_SCENERY_THEME_GARDEN_CLOCK)
        {
            _sceneryThemeTypeToEntryMap[sceneryThemeType] = 254;
        }
        else
        {
            const char * entryName = RCT1::GetSceneryGroupObject(sceneryThemeType);
            if (_sceneryGroupEntries.GetCount() >= 19)
            {
                Console::WriteLine("Warning: More than 19 (max scenery groups) in RCT1 park.");
                Console::WriteLine("         [%s] scenery group not added.", entryName);
            }
            else
            {
                size_t entryIndex = _sceneryGroupEntries.GetOrAddEntry(entryName);
                _sceneryThemeTypeToEntryMap[sceneryThemeType] = (uint8)entryIndex;
            }
        }
    }

    void ImportRides()
    {
        for (sint32 i = 0; i < MAX_RIDES; i++)
        {
            if (_s4.rides[i].type != RIDE_TYPE_NULL)
            {
                ImportRide(get_ride(i), &_s4.rides[i]);
            }
        }
    }

    void ImportRide(Ride * dst, rct1_ride * src)
    {
        memset(dst, 0, sizeof(Ride));

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

        rct_ride_entry * rideEntry = get_ride_entry(dst->subtype);
        // This can happen with hacked parks
        if (rideEntry == nullptr || rideEntry == (rct_ride_entry*)-1)
        {
            dst = nullptr;
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
        for (sint32 i = 0; i < RCT12_MAX_STATIONS_PER_RIDE; i++)
        {
            dst->station_starts[i] = src->station_starts[i];
            dst->station_heights[i] = src->station_height[i] / 2;
            dst->station_length[i] = src->station_length[i];
            dst->station_depart[i] = src->station_light[i];

            dst->train_at_station[i] = src->station_depart[i];

            dst->entrances[i] = src->entrance[i];
            dst->exits[i] = src->exit[i];
            dst->queue_time[i] = src->queue_time[i];
            dst->last_peep_in_queue[i] = src->last_peep_in_queue[i];
            dst->queue_length[i] = src->num_peeps_in_queue[i];

            dst->time[i] = src->time[i];
            dst->length[i] = src->length[i];
        }
        // All other values take 0 as their default. Since they're already memset to that, no need to do it again.
        for (sint32 i = RCT12_MAX_STATIONS_PER_RIDE; i < MAX_STATIONS; i++)
        {
            dst->station_starts[i].xy = RCT_XY8_UNDEFINED;
            dst->train_at_station[i] = 255;
            dst->entrances[i].xy = RCT_XY8_UNDEFINED;
            dst->exits[i].xy = RCT_XY8_UNDEFINED;
            dst->last_peep_in_queue[i] = SPRITE_INDEX_NULL;
        }

        dst->num_stations = src->num_stations;

        // Vehicle links (indexes converted later)
        for (sint32 i = 0; i < RCT1_MAX_TRAINS_PER_RIDE; i++)
        {
            dst->vehicles[i] = src->vehicles[i];
        }
        for (sint32 i = RCT1_MAX_TRAINS_PER_RIDE; i < MAX_VEHICLES_PER_RIDE; i++)
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
            if (src->type == RCT1_RIDE_TYPE_MERRY_GO_ROUND ||
                src->type == RCT1_RIDE_TYPE_DODGEMS)
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
        dst->breakdown_reason = src->breakdown_reason;
        dst->breakdown_reason_pending = src->breakdown_reason_pending;
        dst->inspection_station = src->inspection_station;
        //dst->broken_car?
        //dst->broken_vehicle?

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
        dst->turn_count_default  = src->turn_count_default;
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

    void SetRideColourScheme(Ride * dst, rct1_ride * src)
    {
        // Colours
        dst->colour_scheme_type = src->colour_scheme;
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->track_colour_main[0] = RCT1::GetColour(src->track_primary_colour);
            dst->track_colour_additional[0] = RCT1::GetColour(src->track_secondary_colour);
            dst->track_colour_supports[0] = RCT1::GetColour(src->track_support_colour);

            // Balloons were always blue in the original RCT.
            if (src->type == RCT1_RIDE_TYPE_BALLOON_STALL)
            {
                dst->track_colour_main[0] = COLOUR_LIGHT_BLUE;
            }
            else if (src->type == RCT1_RIDE_TYPE_RIVER_RAPIDS)
            {
                dst->track_colour_main[0] = COLOUR_WHITE;
            }
        }
        else
        {
            for (int i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++)
            {
                dst->track_colour_main[i] = RCT1::GetColour(src->track_colour_main[i]);
                dst->track_colour_additional[i] = RCT1::GetColour(src->track_colour_additional[i]);
                dst->track_colour_supports[i] = RCT1::GetColour(src->track_colour_supports[i]);
            }
            // Entrance styles were introduced with AA. They correspond directly with those in RCT2.
            dst->entrance_style = src->entrance_style;
        }

        if (_gameVersion < FILE_VERSION_RCT1_LL && dst->type == RIDE_TYPE_MERRY_GO_ROUND)
        {
            // The merry-go-round in pre-LL versions was always yellow with red
            dst->vehicle_colours[0].body_colour = COLOUR_YELLOW;
            dst->vehicle_colours[0].trim_colour = COLOUR_BRIGHT_RED;
        }
        else
        {
            for (int i = 0; i < RCT1_MAX_TRAINS_PER_RIDE; i++)
            {
                // RCT1 had no third colour
                RCT1::RCT1VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = RCT1::GetColourSchemeCopyDescriptor(src->vehicle_type);
                if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_1)
                {
                    dst->vehicle_colours[i].body_colour = RCT1::GetColour(src->vehicle_colours[i].body);
                }
                else if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_2)
                {
                    dst->vehicle_colours[i].body_colour = RCT1::GetColour(src->vehicle_colours[i].trim);
                }
                else
                {
                    dst->vehicle_colours[i].body_colour = colourSchemeCopyDescriptor.colour1;
                }

                if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_1)
                {
                    dst->vehicle_colours[i].trim_colour = RCT1::GetColour(src->vehicle_colours[i].body);
                }
                else if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_2)
                {
                    dst->vehicle_colours[i].trim_colour = RCT1::GetColour(src->vehicle_colours[i].trim);
                }
                else
                {
                    dst->vehicle_colours[i].trim_colour = colourSchemeCopyDescriptor.colour2;
                }

                if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_1)
                {
                    dst->vehicle_colours_extended[i] = RCT1::GetColour(src->vehicle_colours[i].body);
                }
                else if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_2)
                {
                    dst->vehicle_colours_extended[i] = RCT1::GetColour(src->vehicle_colours[i].trim);
                }
                else
                {
                    dst->vehicle_colours_extended[i] = colourSchemeCopyDescriptor.colour3;
                }
            }
        }
    }

    void FixRideVehicleLinks(const uint16 * spriteIndexMap)
    {
        uint8 i;
        Ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            for (uint8 j = 0; j < Util::CountOf(ride->vehicles); j++)
            {
                uint16 originalIndex = ride->vehicles[j];
                if (originalIndex != SPRITE_INDEX_NULL)
                {
                    ride->vehicles[j] = spriteIndexMap[originalIndex];
                }
            }
        }
    }

    void ImportRideMeasurements()
    {
        for (sint32 i = 0; i < MAX_RIDE_MEASUREMENTS; i++)
        {
            rct_ride_measurement * dst = get_ride_measurement(i);
            rct_ride_measurement * src = &_s4.ride_measurements[i];
            ImportRideMeasurement(dst, src);
        }
    }

    void ImportRideMeasurement(rct_ride_measurement * dst, rct_ride_measurement * src)
    {
        *dst = *src;
        for (sint32 i = 0; i < RIDE_MEASUREMENT_MAX_ITEMS; i++)
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
        std::vector<rct_vehicle *> vehicles;
        uint16 spriteIndexMap[RCT1_MAX_SPRITES];
        for (int i = 0; i < RCT1_MAX_SPRITES; i++)
        {
            spriteIndexMap[i] = SPRITE_INDEX_NULL;
            if (_s4.sprites[i].unknown.sprite_identifier == SPRITE_IDENTIFIER_VEHICLE)
            {
                rct1_vehicle * srcVehicle = &_s4.sprites[i].vehicle;
                if (srcVehicle->x != SPRITE_LOCATION_NULL)
                {
                    rct_vehicle * vehicle = (rct_vehicle *)create_sprite(SPRITE_IDENTIFIER_VEHICLE);
                    spriteIndexMap[i] = vehicle->sprite_index;
                    vehicles.push_back(vehicle);

                    ImportVehicle(vehicle, srcVehicle);

                    // If vehicle is the first car on a train add to train list
                    if (!vehicle->is_child)
                    {
                        move_sprite_to_list((rct_sprite *)vehicle, SPRITE_LIST_TRAIN * 2);
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

    void ImportVehicle(rct_vehicle * dst, rct1_vehicle * src)
    {
        Ride * ride = get_ride(src->ride);
        uint8 vehicleEntryIndex = RCT1::GetVehicleSubEntryIndex(src->vehicle_type);

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

        dst->friction = src->friction;
        dst->num_seats = src->num_seats;
        dst->speed = src->speed;
        dst->powered_acceleration = src->powered_acceleration;
        dst->brake_speed = src->brake_speed;

        dst->velocity = src->velocity;
        dst->acceleration = src->acceleration;
        dst->var_4A = src->var_4A;
        dst->swinging_car_var_0 = src->swinging_car_var_0;
        dst->var_4E = src->var_4E;
        dst->restraints_position = src->restraints_position;
        dst->var_BA = src->var_BA;
        dst->var_BF = src->var_BF;
        dst->var_B6 = src->var_B6;
        dst->var_B8 = src->var_B8;
        dst->sound1_id = RCT12_SOUND_ID_NULL;
        dst->sound2_id = RCT12_SOUND_ID_NULL;
        dst->var_C0 = src->var_C0;
        dst->var_C4 = src->var_C4;
        dst->var_C5 = src->var_C5;
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
            uint16 spriteIndex = src->peep[i];
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

        sprite_move(src->x, src->y, src->z, (rct_sprite *)dst);
        invalidate_sprite_2((rct_sprite *)dst);

        dst->num_peeps = src->num_peeps;
        dst->next_free_seat = src->next_free_seat;
    }

    void SetVehicleColours(rct_vehicle * dst, rct1_vehicle * src)
    {
        rct1_ride * srcRide = &_s4.rides[src->ride];
        uint8 vehicleTypeIndex = srcRide->vehicle_type;
        RCT1::RCT1VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = RCT1::GetColourSchemeCopyDescriptor(vehicleTypeIndex);

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

    void FixVehicleLinks(rct_vehicle * vehicle, const uint16 * spriteIndexMap)
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

    void FixVehiclePeepLinks(rct_vehicle * vehicle, const uint16 * spriteIndexMap)
    {
        for (int i = 0; i < 32; i++)
        {
            vehicle->peep[i] = MapSpriteIndex(vehicle->peep[i], spriteIndexMap);
        }
    }

    void ImportPeeps()
    {
        uint16 spriteIndexMap[RCT1_MAX_SPRITES];
        for (size_t i = 0; i < RCT1_MAX_SPRITES; i++)
        {
            spriteIndexMap[i] = SPRITE_INDEX_NULL;
            if (_s4.sprites[i].unknown.sprite_identifier == SPRITE_IDENTIFIER_PEEP)
            {
                rct1_peep * srcPeep = &_s4.sprites[i].peep;
                rct_peep * peep = (rct_peep*)create_sprite(SPRITE_IDENTIFIER_PEEP);
                move_sprite_to_list((rct_sprite*)peep, SPRITE_LIST_PEEP * 2);
                spriteIndexMap[i] = peep->sprite_index;

                ImportPeep(peep, srcPeep);
            }
        }
        for (size_t i = 0; i < MAX_SPRITES; i++)
        {
            rct_sprite * sprite = get_sprite(i);
            if (sprite->unknown.sprite_identifier == SPRITE_IDENTIFIER_VEHICLE)
            {
                rct_vehicle * vehicle = (rct_vehicle *)sprite;
                FixVehiclePeepLinks(vehicle, spriteIndexMap);
            }
        }

        int i;
        Ride *ride;
        rct_peep *peep;

        FOR_ALL_RIDES(i, ride)
        {
            FixRidePeepLinks(ride, spriteIndexMap);
        }

        FOR_ALL_GUESTS(i, peep)
        {
            FixPeepNextInQueue(peep, spriteIndexMap);
        }

        // The RCT2/OpenRCT2 structures are bigger than in RCT1, so set them to zero
        Memory::Set(gStaffModes, 0, sizeof(gStaffModes));
        Memory::Set(gStaffPatrolAreas, 0, sizeof(gStaffPatrolAreas));

        Memory::Copy(gStaffModes, _s4.staff_modes, sizeof(_s4.staff_modes));

        FOR_ALL_STAFF(i, peep)
        {
            ImportStaffPatrolArea(peep);
        }
        // Only the individual patrol areas have been converted, so generate the combined patrol areas of each staff type
        staff_update_greyed_patrol_areas();
    }

    void ImportPeep(rct_peep * dst, rct1_peep * src)
    {
        dst->sprite_identifier = SPRITE_IDENTIFIER_PEEP;
        // Peep vs. staff (including which kind)
        dst->sprite_type = RCT1::GetPeepSpriteType(src->sprite_type);
        dst->action = src->action;
        dst->special_sprite = src->special_sprite;
        dst->next_action_sprite_type = src->next_action_sprite_type;
        dst->action_sprite_image_offset = src->action_sprite_image_offset;
        dst->no_action_frame_no = src->no_action_frame_no;
        dst->action_sprite_type = src->action_sprite_type;
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

        dst->state = src->state;
        dst->sub_state = src->sub_state;
        dst->next_x = src->next_x;
        dst->next_y = src->next_y;
        dst->next_z = src->next_z / 2;
        dst->next_var_29 = src->next_var_29;
        dst->var_37 = src->var_37;
        dst->var_42 = src->var_42;
        dst->var_73 = src->var_73;
        dst->var_EF = src->var_EF;

        dst->type = src->type;

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
        dst->destination_tolerence = src->destination_tolerence;
        dst->direction = src->direction;

        dst->energy = src->energy;
        dst->energy_target = src->energy_target;
        dst->happiness = src->happiness;
        dst->happiness_target = src->happiness_target;
        dst->nausea = src->nausea;
        dst->nausea_target = src->nausea_target;
        dst->hunger = src->hunger;
        dst->thirst = src->thirst;
        dst->bathroom = src->bathroom;
        dst->var_41 = src->var_41;

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

        dst->var_C4 = 0;
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

        dst->favourite_ride = src->favourite_ride;
        dst->favourite_ride_rating = src->favourite_ride_rating;

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

    void FixRidePeepLinks(Ride * ride, const uint16 * spriteIndexMap)
    {
        for (sint32 i = 0; i < RCT12_MAX_STATIONS_PER_RIDE; i++)
        {
            ride->last_peep_in_queue[i] = MapSpriteIndex(ride->last_peep_in_queue[i], spriteIndexMap);
        }
        ride->mechanic = MapSpriteIndex(ride->mechanic, spriteIndexMap);
        if (ride->type == RIDE_TYPE_SPIRAL_SLIDE)
        {
            ride->slide_peep = MapSpriteIndex(ride->slide_peep, spriteIndexMap);
        }
    }

    void FixPeepNextInQueue(rct_peep * peep, const uint16 * spriteIndexMap)
    {
        peep->next_in_queue = MapSpriteIndex(peep->next_in_queue, spriteIndexMap);
    }

    void ImportStaffPatrolArea(rct_peep * staffmember)
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

        sint32 peepOffset = staffmember->staff_id * RCT12_PATROL_AREA_SIZE;
        for (sint32 i = 0; i < RCT12_PATROL_AREA_SIZE; i++)
        {
            if (_s4.patrol_areas[peepOffset + i] == 0)
            {
                // No patrol for this area
                continue;
            }

            // Loop over the bits of the uint8
            for (sint32 j = 0; j < 8; j++)
            {
                sint8 bit = (_s4.patrol_areas[peepOffset + i] >> j) & 1;
                if (bit == 0)
                {
                    // No patrol for this area
                    continue;
                }
                // val contains the 5 highest bits of both the x and y coordinates
                sint32 val = j | (i << 3);
                sint32 x = val & 0x1F;
                x <<= 7;
                sint32 y = val & 0x3E0;
                y <<= 2;
                staff_set_patrol_area(staffmember->staff_id, x, y, true);
            }
        }
    }

    void ImportLitter()
    {

        for (size_t i = 0; i < RCT1_MAX_SPRITES; i++)
        {
            if (_s4.sprites[i].unknown.sprite_identifier == SPRITE_IDENTIFIER_LITTER)
            {
                rct_litter * srcLitter = &_s4.sprites[i].litter;

                rct_litter * litter = (rct_litter *) create_sprite(SPRITE_IDENTIFIER_LITTER);
                move_sprite_to_list((rct_sprite *) litter, SPRITE_LIST_LITTER * 2);

                litter->sprite_identifier = srcLitter->sprite_identifier;
                litter->type = srcLitter->type;

                litter->x = srcLitter->x;
                litter->y = srcLitter->y;
                litter->z = srcLitter->z;
                litter->sprite_direction = srcLitter->sprite_direction;
                litter->sprite_width = srcLitter->sprite_width;
                litter->sprite_height_positive = srcLitter->sprite_height_positive;
                litter->sprite_height_negative = srcLitter->sprite_height_negative;

                sprite_move(srcLitter->x, srcLitter->y, srcLitter->z, (rct_sprite *) litter);
                invalidate_sprite_2((rct_sprite *) litter);
            }
        }
    }

    void ImportMiscSprites()
    {
        for (size_t i = 0; i < RCT1_MAX_SPRITES; i++)
        {
            if (_s4.sprites[i].unknown.sprite_identifier == SPRITE_IDENTIFIER_MISC)
            {
                rct1_unk_sprite * src = &_s4.sprites[i].unknown;
                rct_unk_sprite * dst = (rct_unk_sprite *) create_sprite(SPRITE_IDENTIFIER_MISC);
                move_sprite_to_list((rct_sprite *) dst, SPRITE_LIST_MISC * 2);

                dst->sprite_identifier = src->sprite_identifier;
                dst->misc_identifier = src->misc_identifier;
                dst->flags = src->flags;
                dst->sprite_direction = src->sprite_direction;
                dst->sprite_width = src->sprite_width;
                dst->sprite_height_negative = src->sprite_height_negative;
                dst->sprite_height_positive = src->sprite_height_positive;

                sprite_move(src->x, src->y, src->z, (rct_sprite*)dst);

                switch (src->misc_identifier) {
                case SPRITE_MISC_STEAM_PARTICLE:
                    ImportSteamParticle((rct_steam_particle *) dst, (rct_steam_particle *) src);
                    break;
                case SPRITE_MISC_MONEY_EFFECT:
                    ImportMoneyEffect((rct_money_effect *) dst, (rct_money_effect *) src);
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
                    ImportJumpingFountainWater((rct_jumping_fountain *) dst, (rct_jumping_fountain *) src);
                    break;
                case SPRITE_MISC_BALLOON:
                    ImportBalloon((rct_balloon*)dst, (rct_balloon*)src);
                    break;
                case SPRITE_MISC_DUCK:
                    ImportDuck((rct_duck*)dst, (rct_duck*)src);
                    break;
                }

                sprite_move(src->x, src->y, src->z, (rct_sprite *) dst);
                invalidate_sprite_2((rct_sprite *) dst);
            }
        }
    }

    void ImportMoneyEffect(rct_money_effect * dst, rct_money_effect * src)
    {
        dst->move_delay = src->move_delay;
        dst->num_movements = src->num_movements;
        dst->value = src->value;
        dst->offset_x = src->offset_x;
        dst->wiggle = src->wiggle;
    }

    void ImportSteamParticle(rct_steam_particle * dst, rct_steam_particle * src)
    {
        dst->frame = src->frame;
    }

    void ImportJumpingFountainWater(rct_jumping_fountain * dst, rct_jumping_fountain * src)
    {
        dst->fountain_flags = src->fountain_flags;
        dst->iteration = src->iteration;
        dst->var_26a = src->var_26a;
        dst->var_26b = src->var_26b;
    }

    void ImportBalloon(rct_balloon * dst, rct_balloon * src)
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

    void ImportDuck(rct_duck * dst, rct_duck * src)
    {
        dst->frame = src->frame;
        dst->state = src->state;
    }

    uint16 MapSpriteIndex(uint16 originalSpriteIndex, const uint16 * spriteIndexMap)
    {
        uint16 newSpriteIndex = SPRITE_INDEX_NULL;
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
        for (size_t i = 0; i < RCT12_MAX_PEEP_SPAWNS; i++)
        {
            gPeepSpawns[i] = _s4.peep_spawn[i];
        }
    }

    void ImportMapAnimations()
    {
        // This is sketchy, ideally we should try to re-create them
        rct_map_animation * s4Animations = (rct_map_animation*)_s4.map_animations;
        for (size_t i = 0; i < 1000; i++)
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

        gCashEncrypted = ENCRYPT_MONEY(_s4.cash);
        gBankLoan = _s4.loan;
        gMaxBankLoan = _s4.max_loan;
        gInitialCash = _s4.cash;

        gCompanyValue = _s4.company_value;
        gParkValue = CorrectRCT1ParkValue(_s4.park_value);
        gCurrentProfit = _s4.profit;

        for (size_t i = 0; i < 128; i++)
        {
            gCashHistory[i] = _s4.cash_history[i];
            gParkValueHistory[i] = CorrectRCT1ParkValue(_s4.park_value_history[i]);
            gWeeklyProfitHistory[i] = _s4.weekly_profit_history[i];
        }

        for (size_t i = 0; i < EXPENDITURE_TABLE_TOTAL_COUNT; i++)
        {
            gExpenditureTable[i] = _s4.expenditure[i];
        }
        gCurrentExpenditure = _s4.total_expenditure;

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
        LoadObjects(OBJECT_TYPE_RIDE, _rideEntries);
        LoadObjects(OBJECT_TYPE_SMALL_SCENERY, _smallSceneryEntries);
        LoadObjects(OBJECT_TYPE_LARGE_SCENERY, _largeSceneryEntries);
        LoadObjects(OBJECT_TYPE_WALLS, _wallEntries);
        LoadObjects(OBJECT_TYPE_PATHS, _pathEntries);
        LoadObjects(OBJECT_TYPE_PATH_BITS, _pathAdditionEntries);
        LoadObjects(OBJECT_TYPE_SCENERY_SETS, _sceneryGroupEntries);
        LoadObjects(OBJECT_TYPE_BANNERS, std::vector<const char *>({
            "BN1     ",
            "BN2     ",
            "BN3     ",
            "BN4     ",
            "BN5     ",
            "BN6     ",
            "BN7     ",
            "BN8     ",
            "BN9     "
        }));
        LoadObjects(OBJECT_TYPE_PARK_ENTRANCE, std::vector<const char *>({ "PKENT1  " }));
        LoadObjects(OBJECT_TYPE_WATER, _waterEntry);
    }

    void LoadObjects(uint8 objectType, const EntryList &entries)
    {
        LoadObjects(objectType, entries.GetEntries());
    }

    void LoadObjects(uint8 objectType, const std::vector<const char *> &entries)
    {
        IObjectManager * objectManager = GetObjectManager();

        uint32 entryIndex = 0;
        for (const char * objectName : entries)
        {
            rct_object_entry entry;
            entry.flags = 0x00008000 + objectType;
            Memory::Copy(entry.name, objectName, 8);
            entry.checksum = 0;

            Object * object = objectManager->LoadObject(&entry);
            if (object == nullptr && objectType != OBJECT_TYPE_SCENERY_SETS)
            {
                log_error("Failed to load %s.", objectName);
                throw Exception("Failed to load object.");
            }

            entryIndex++;
        }
    }

    std::vector<rct_object_entry> GetInvalidObjects()
    {
        std::vector<rct_object_entry> missingObjects;
        GetInvalidObjects(OBJECT_TYPE_RIDE, _rideEntries.GetEntries(), missingObjects);
        GetInvalidObjects(OBJECT_TYPE_SMALL_SCENERY, _smallSceneryEntries.GetEntries(), missingObjects);
        GetInvalidObjects(OBJECT_TYPE_LARGE_SCENERY, _largeSceneryEntries.GetEntries(), missingObjects);
        GetInvalidObjects(OBJECT_TYPE_WALLS, _wallEntries.GetEntries(), missingObjects);
        GetInvalidObjects(OBJECT_TYPE_PATHS, _pathEntries.GetEntries(), missingObjects);
        GetInvalidObjects(OBJECT_TYPE_PATH_BITS, _pathAdditionEntries.GetEntries(), missingObjects);
        GetInvalidObjects(OBJECT_TYPE_SCENERY_SETS, _sceneryGroupEntries.GetEntries(), missingObjects);
        GetInvalidObjects(OBJECT_TYPE_BANNERS, std::vector<const char *>({
            "BN1     ",
            "BN2     ",
            "BN3     ",
            "BN4     ",
            "BN5     ",
            "BN6     ",
            "BN7     ",
            "BN8     ",
            "BN9     "
        }), missingObjects);
        GetInvalidObjects(OBJECT_TYPE_PARK_ENTRANCE, std::vector<const char *>({ "PKENT1  " }), missingObjects);
        GetInvalidObjects(OBJECT_TYPE_WATER, _waterEntry.GetEntries(), missingObjects);
        return missingObjects;
    }

    void GetInvalidObjects(uint8 objectType, const std::vector<const char *> &entries, std::vector<rct_object_entry> &missingObjects)
    {
        IObjectRepository * objectRepository = GetObjectRepository();
        for (const char * objectName : entries)
        {
            rct_object_entry entry;
            entry.flags = 0x00008000 + objectType;
            Memory::Copy(entry.name, objectName, 8);
            entry.checksum = 0;
            
            const ObjectRepositoryItem * ori = objectRepository->FindObject(&entry);
            if (ori == nullptr)
            {
                missingObjects.push_back(entry);
                Console::Error::WriteLine("[%s] Object not found.", objectName);
            }
            else
            {
                Object * object = objectRepository->LoadObject(ori);
                if (object == nullptr && objectType != OBJECT_TYPE_SCENERY_SETS)
                {
                    missingObjects.push_back(entry);
                    Console::Error::WriteLine("[%s] Object could not be loaded.", objectName);
                }
                delete object;
            }
        }
    }

    void ImportMapElements()
    {
        Memory::Copy(gMapElements, _s4.map_elements, RCT1_MAX_MAP_ELEMENTS * sizeof(rct_map_element));
        ClearExtraTileEntries();
        FixSceneryColours();
        FixMapElementZ();
        FixPaths();
        FixWalls();
        FixBanners();
        FixTerrain();
        FixEntrancePositions();
        FixMapElementEntryTypes();
    }

    void ImportResearch()
    {
        // All available objects must be loaded before this method is called as it
        // requires them to correctly insert objects into the research list

        research_reset_items();

        size_t researchListCount;
        const rct1_research_item * researchList = GetResearchList(&researchListCount);

        // Initialise the "seen" tables
        Memory::Set(_researchRideEntryUsed, 0, sizeof(_researchRideEntryUsed));
        Memory::Set(_researchRideTypeUsed, 0, sizeof(_researchRideTypeUsed));

        // The first six scenery groups are always available
        for (uint8 i = 0; i < 6; i++)
        {
            research_insert_scenery_group_entry(i, true);
        }

        bool researched = true;
        for (size_t i = 0; i < researchListCount; i++)
        {
            const rct1_research_item * researchItem = &researchList[i];
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

            switch (researchItem->category) {
            case RCT1_RESEARCH_CATEGORY_THEME:
            {
                uint8 rct1SceneryTheme = researchItem->item;
                uint8 sceneryGroupEntryIndex = _sceneryThemeTypeToEntryMap[rct1SceneryTheme];
                if (sceneryGroupEntryIndex != 254 &&
                    sceneryGroupEntryIndex != 255)
                {
                    research_insert_scenery_group_entry(sceneryGroupEntryIndex, researched);
                }
                break;
            }
            case RCT1_RESEARCH_CATEGORY_RIDE:
            {
                uint8 rct1RideType = researchItem->item;
                _researchRideTypeUsed[rct1RideType] = true;

                // Add all vehicles for this ride type that are researched or before this research item
                uint32 numVehicles = 0;
                for (size_t j = 0; j < researchListCount; j++)
                {
                    const rct1_research_item *researchItem2 = &researchList[j];
                    if (researchItem2->flags == RCT1_RESEARCH_FLAGS_SEPARATOR &&
                        (researchItem2->item == RCT1_RESEARCH_END_RESEARCHABLE ||
                        researchItem2->item == RCT1_RESEARCH_END_AVAILABLE))
                    {
                        continue;
                    }

                    if (researchItem2->category == RCT1_RESEARCH_CATEGORY_VEHICLE &&
                        researchItem2->related_ride == rct1RideType)
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
                    uint8 rideEntryIndex = _rideTypeToRideEntryMap[rct1RideType];
                    Guard::Assert(rideEntryIndex != RIDE_ENTRY_INDEX_NULL, "rideEntryIndex was RIDE_ENTRY_INDEX_NULL");
                    if (!_researchRideEntryUsed[rideEntryIndex])
                    {
                        _researchRideEntryUsed[rideEntryIndex] = true;
                        research_insert_ride_entry(rideEntryIndex, researched);
                    }

                }

                break;
            }
            case RCT1_RESEARCH_CATEGORY_VEHICLE:
                // Only add vehicle if the related ride has been seen, this to make sure that vehicles
                // are researched only after the ride has been researched
                if (_researchRideTypeUsed[researchItem->related_ride])
                {
                    InsertResearchVehicle(researchItem, researched);
                }

                break;
            case RCT1_RESEARCH_CATEGORY_SPECIAL:
                // Not supported
                break;
            }
        }

        research_remove_non_separate_vehicle_types();

        // Research funding / priority
        uint8 activeResearchTypes = 0;
        if (_s4.research_priority & RCT1_RESEARCH_EXPENDITURE_ROLLERCOASTERS)
        {
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_ROLLERCOASTER);
        }
        if (_s4.research_priority & RCT1_RESEARCH_EXPENDITURE_THRILL_RIDES)
        {
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_THRILL);
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_WATER);
        }
        if (_s4.research_priority & RCT1_RESEARCH_EXPENDITURE_GENTLE_TRANSPORT_RIDES)
        {
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_GENTLE);
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_TRANSPORT);
        }
        if (_s4.research_priority & RCT1_RESEARCH_EXPENDITURE_SHOPS)
        {
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_SHOP);
        }
        if (_s4.research_priority & RCT1_RESEARCH_EXPENDITURE_SCENERY_THEMEING)
        {
            activeResearchTypes |= (1 << RESEARCH_CATEGORY_SCENERYSET);
        }
        gResearchPriorities = activeResearchTypes;
        gResearchFundingLevel = _s4.research_level;

        // Research history
        gResearchProgress = _s4.research_progress;
        // gResearchProgressStage =
        gResearchNextItem = _s4.next_research_item;
        gResearchNextCategory = _s4.next_research_category;
        // gResearchExpectedDay =
        // gResearchExpectedMonth =
    }

    void InsertResearchVehicle(const rct1_research_item * researchItem, bool researched)
    {
        uint8 vehicle = researchItem->item;
        uint8 rideEntryIndex = _vehicleTypeToRideEntryMap[vehicle];

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
        gScenarioSrand0 = _s4.random_a;
        gScenarioSrand1 = _s4.random_b;
        gDateMonthsElapsed = _s4.month;
        gDateMonthTicks = _s4.day;

        // Park rating
        gParkRating = _s4.park_rating;
        for (size_t i = 0; i < 32; i++)
        {
            gParkRatingHistory[i] = _s4.park_rating_history[i];
        }

        // Awards
        for (sint32 i = 0; i < RCT12_MAX_AWARDS; i++)
        {
            rct12_award * src = &_s4.awards[i];
            Award * dst = &gCurrentAwards[i];
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
            const rct12_news_item * src = &_s4.messages[i];
            NewsItem * dst = &gNewsItems[i];

            dst->Type = src->Type;
            dst->Flags = src->Flags;
            dst->Assoc = src->Assoc;
            dst->Ticks = src->Ticks;
            dst->MonthYear = src->MonthYear;
            dst->Day = src->Day;
            Memory::Copy(dst->Text, src->Text, sizeof(src->Text));
        }

        // Initial guest status
        gGuestInitialCash = _s4.guest_initial_cash;
        gGuestInitialHunger = _s4.guest_initial_hunger;
        gGuestInitialThirst = _s4.guest_initial_thirst;

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
            gCheatsUnlockAllPrices = true;
        }
        else
        {
            gCheatsUnlockAllPrices = false;
        }

        // RCT2 uses two flags for no money (due to the scenario editor). RCT1 used only one.
        // Copy its value to make no money scenarios such as Arid Heights work properly.
        if (_s4.park_flags & RCT1_PARK_FLAGS_NO_MONEY)
        {
            gParkFlags |= PARK_FLAGS_NO_MONEY_SCENARIO;
        }
    }

    void ImportClimate()
    {
        gClimate = _s4.climate;
        gClimateUpdateTimer = _s4.climate_timer;
        gClimateCurrentTemperature = _s4.temperature;
        gClimateCurrentWeather = _s4.weather;
        gClimateCurrentWeatherEffect = 0;
        gClimateCurrentWeatherGloom = _s4.weather_gloom;
        gClimateCurrentRainLevel = _s4.rain;
        gClimateNextTemperature = _s4.target_temperature;
        gClimateNextWeather = _s4.target_weather;
        gClimateNextWeatherEffect = 0;
        gClimateNextWeatherGloom = _s4.target_weather_gloom;
        gClimateNextRainLevel = _s4.target_rain;
    }

    void ImportScenarioNameDetails()
    {
        std::string name = String::ToStd(_s4.scenario_name);
        std::string details;

        sint32 scNumber = _s4.scenario_slot_index;
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
        String::Set(gScenarioName, sizeof(gScenarioName), name.c_str());
        String::Set(gScenarioDetails, sizeof(gScenarioDetails), details.c_str());
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
        for (size_t i = 0; i < 0x10000; i++)
        {
            gMapElementTilePointers[i] = (rct_map_element *)-1;
        }

        // Get the first free map element
        rct_map_element * nextFreeMapElement = gMapElements;
        for (size_t i = 0; i < 128 * 128; i++)
        {
            do { } while (!map_element_is_last_for_tile(nextFreeMapElement++));
        }

        rct_map_element * mapElement = gMapElements;
        rct_map_element * * tilePointer = gMapElementTilePointers;

        // 128 rows of map data from RCT1 map
        for (sint32 x = 0; x < RCT1_MAX_MAP_SIZE; x++)
        {
            // Assign the first half of this row
            for (sint32 y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                *tilePointer++ = mapElement;
                do { } while (!map_element_is_last_for_tile(mapElement++));
            }

            // Fill the rest of the row with blank tiles
            for (sint32 y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                nextFreeMapElement->type = MAP_ELEMENT_TYPE_SURFACE;
                nextFreeMapElement->flags = MAP_ELEMENT_FLAG_LAST_TILE;
                nextFreeMapElement->base_height = 2;
                nextFreeMapElement->clearance_height = 0;
                nextFreeMapElement->properties.surface.slope = 0;
                nextFreeMapElement->properties.surface.terrain = 0;
                nextFreeMapElement->properties.surface.grass_length = GRASS_LENGTH_CLEAR_0;
                nextFreeMapElement->properties.surface.ownership = 0;
                *tilePointer++ = nextFreeMapElement++;
            }
        }

        // 128 extra rows left to fill with blank tiles
        for (sint32 y = 0; y < 128 * 256; y++)
        {
            nextFreeMapElement->type = MAP_ELEMENT_TYPE_SURFACE;
            nextFreeMapElement->flags = MAP_ELEMENT_FLAG_LAST_TILE;
            nextFreeMapElement->base_height = 2;
            nextFreeMapElement->clearance_height = 0;
            nextFreeMapElement->properties.surface.slope = 0;
            nextFreeMapElement->properties.surface.terrain = 0;
            nextFreeMapElement->properties.surface.grass_length = GRASS_LENGTH_CLEAR_0;
            nextFreeMapElement->properties.surface.ownership = 0;
            *tilePointer++ = nextFreeMapElement++;
        }

        gNextFreeMapElement = nextFreeMapElement;
    }

    void FixSceneryColours()
    {
        colour_t colour;
        rct_map_element * mapElement = gMapElements;
        while (mapElement < gNextFreeMapElement)
        {
            if (mapElement->base_height != 255)
            {
                switch (map_element_get_type(mapElement)) {
                case MAP_ELEMENT_TYPE_SCENERY:
                    colour = RCT1::GetColour(scenery_small_get_primary_colour(mapElement));
                        scenery_small_set_primary_colour(mapElement, colour);

                    // Copied from [rct2: 0x006A2956]
                    switch (mapElement->properties.scenery.type) {
                    case 157: // TGE1 (Geometric Sculpture)
                    case 162: // TGE2 (Geometric Sculpture)
                    case 168: // TGE3 (Geometric Sculpture)
                    case 170: // TGE4 (Geometric Sculpture)
                    case 171: // TGE5 (Geometric Sculpture)
                        scenery_small_set_secondary_colour(mapElement, COLOUR_WHITE);
                        break;
                    }
                    break;
                case MAP_ELEMENT_TYPE_WALL:
                    colour = ((mapElement->type & 0xC0) >> 3) |
                             ((mapElement->properties.wall.type & 0xE0) >> 5);
                    colour = RCT1::GetColour(colour);

                    mapElement->type &= 0x3F;
                    mapElement->properties.wall.type &= 0x1F;
                    mapElement->type |= (colour & 0x18) << 3;
                    mapElement->properties.wall.type |= (colour & 7) << 5;
                    break;
                case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
                    colour = RCT1::GetColour(mapElement->properties.scenerymultiple.colour[0] & 0x1F);
                    mapElement->properties.scenerymultiple.colour[0] &= 0xE0;
                    mapElement->properties.scenerymultiple.colour[0] |= colour;

                    colour = RCT1::GetColour(mapElement->properties.scenerymultiple.colour[1] & 0x1F);
                    mapElement->properties.scenerymultiple.colour[1] &= 0xE0;
                    mapElement->properties.scenerymultiple.colour[1] |= colour;
                    break;
                }
            }
            mapElement++;
        }
    }

    void FixMapElementZ()
    {
        rct_map_element * mapElement = gMapElements;
        while (mapElement < gNextFreeMapElement)
        {
            if (mapElement->base_height != 255)
            {
                mapElement->base_height /= 2;
                mapElement->clearance_height /= 2;
            }
            mapElement++;
        }
        gMapBaseZ = 7;
    }

    void FixPaths()
    {
        rct_map_element * mapElement = gMapElements;
        while (mapElement < gNextFreeMapElement)
        {
            switch (map_element_get_type(mapElement)) {
            case MAP_ELEMENT_TYPE_PATH:
            {
                // Type
                uint8 pathColour = mapElement->type & 3;
                uint8 pathType = (mapElement->properties.path.type & 0xF0) >> 4;

                pathType = (pathType << 2) | pathColour;
                uint8 entryIndex = _pathTypeToEntryMap[pathType];

                mapElement->type &= 0xFC;
                mapElement->flags &= ~0x60;
                mapElement->flags &= ~MAP_ELEMENT_FLAG_BROKEN;
                mapElement->properties.path.type &= 0x0F;
                footpath_scenery_set_is_ghost(mapElement, false);
                if (RCT1::PathIsQueue(pathType))
                {
                    mapElement->type |= 1;
                }
                mapElement->properties.path.type |= entryIndex << 4;

                // Additions
                uint8 additionType = footpath_element_get_path_scenery(mapElement);
                if (additionType != RCT1_PATH_ADDITION_NONE)
                {
                    uint8 normalisedType = RCT1::NormalisePathAddition(additionType);
                    entryIndex = _pathAdditionTypeToEntryMap[normalisedType];
                    if (additionType != normalisedType)
                    {
                        mapElement->flags |= MAP_ELEMENT_FLAG_BROKEN;
                    }
                    footpath_element_set_path_scenery(mapElement, entryIndex + 1);
                }
                break;
            }
            case MAP_ELEMENT_TYPE_ENTRANCE:
                if (mapElement->properties.entrance.type == ENTRANCE_TYPE_PARK_ENTRANCE)
                {
                    uint8 pathType = mapElement->properties.entrance.path_type;
                    if (pathType == 0)
                    {
                        pathType = RCT1_FOOTPATH_TYPE_TARMAC_GRAY;
                    }
                    uint8 entryIndex = _pathTypeToEntryMap[pathType];
                    mapElement->properties.entrance.path_type = entryIndex & 0x7F;
                }
                break;
            }
            mapElement++;
        }
    }

    void FixWalls()
    {
        // The user might attempt to load a save while in pause mode.
        // Since we cannot place walls in pause mode without a cheat, temporarily turn it on.
        bool oldCheatValue = gCheatsBuildInPauseMode;
        gCheatsBuildInPauseMode = true;

        for (sint32 x = 0; x < RCT1_MAX_MAP_SIZE; x++)
        {
            for (sint32 y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                rct_map_element * mapElement = map_get_first_element_at(x, y);
                do
                {
                    if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_WALL)
                    {
                        rct_map_element originalMapElement = *mapElement;
                        map_element_remove(mapElement);

                        uint8 var_05 = originalMapElement.properties.wall.colour_3;
                        uint16 var_06 = originalMapElement.properties.wall.colour_1 |
                                       (originalMapElement.properties.wall.animation << 8);

                        for (sint32 edge = 0; edge < 4; edge++)
                        {
                            sint32 typeA = (var_05 >> (edge * 2)) & 3;
                            sint32 typeB = (var_06 >> (edge * 4)) & 0x0F;
                            if (typeB != 0x0F)
                            {
                                sint32 type = typeA | (typeB << 2);
                                sint32 colourA = ((originalMapElement.type & 0xC0) >> 3) |
                                               (originalMapElement.properties.wall.type >> 5);
                                sint32 colourB = 0;
                                sint32 colourC = 0;
                                ConvertWall(&type, &colourA, &colourB, &colourC);

                                type = _wallTypeToEntryMap[type];
                                wall_place(type, x * 32, y * 32, 0, edge, colourA, colourB, colourC, 169);
                            }
                        }
                        break;
                    }
                }
                while (!map_element_is_last_for_tile(mapElement++));
            }
        }

        gCheatsBuildInPauseMode = oldCheatValue;
    }

    void ConvertWall(sint32 * type, sint32 * colourA, sint32 * colourB, sint32 * colourC)
    {
        switch (*type) {
        case RCT1_WALL_TYPE_WOODEN_PANEL_FENCE:
            *colourA = COLOUR_DARK_BROWN;
            break;
        case RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE:
            *type = RCT1_WALL_TYPE_WOODEN_PANEL_FENCE;
            *colourA = COLOUR_WHITE;
            break;
        case RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE:
            *type = RCT1_WALL_TYPE_WOODEN_PANEL_FENCE;
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

    void FixBanners()
    {
        for (sint32 x = 0; x < RCT1_MAX_MAP_SIZE; x++)
        {
            for (sint32 y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                rct_map_element * mapElement = map_get_first_element_at(x, y);
                do
                {
                    if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_BANNER)
                    {
                        uint8 index = mapElement->properties.banner.index;
                        rct_banner * src = &_s4.banners[index];
                        rct_banner * dst = &gBanners[index];
                        ImportBanner(dst, src);
                    }
                }
                while (!map_element_is_last_for_tile(mapElement++));
            }
        }
    }

    void ImportBanner(rct_banner * dst, rct_banner * src)
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

    void FixTerrain()
    {
        map_element_iterator it;
        map_element_iterator_begin(&it);
        while (map_element_iterator_next(&it))
        {
            rct_map_element * element = it.element;
            if (map_element_get_type(element) == MAP_ELEMENT_TYPE_SURFACE)
            {
                map_element_set_terrain(element, RCT1::GetTerrain(map_element_get_terrain(element)));
                map_element_set_terrain_edge(element, RCT1::GetTerrainEdge(map_element_get_terrain_edge(element)));
            }
        }
    }

    void FixEntrancePositions()
    {
        for (size_t i = 0; i < Util::CountOf(gParkEntrances); i++)
        {
            gParkEntrances[i].x = MAP_LOCATION_NULL;
        }

        uint8 entranceIndex = 0;

        map_element_iterator it;
        map_element_iterator_begin(&it);
        while (map_element_iterator_next(&it) && entranceIndex < RCT12_MAX_PARK_ENTRANCES)
        {
            rct_map_element * element = it.element;

            if (map_element_get_type(element) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
            if (element->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE) continue;
            if ((element->properties.entrance.index & 0x0F) != 0) continue;

            gParkEntrances[entranceIndex].x = it.x * 32;
            gParkEntrances[entranceIndex].y = it.y * 32;
            gParkEntrances[entranceIndex].z = element->base_height * 8;
            gParkEntrances[entranceIndex].direction = map_element_get_direction(element);
            entranceIndex++;
        }
    }

    void FixMapElementEntryTypes()
    {
        map_element_iterator it;
        map_element_iterator_begin(&it);
        while (map_element_iterator_next(&it))
        {
            rct_map_element * mapElement = it.element;
            switch (map_element_get_type(mapElement)) {
            case MAP_ELEMENT_TYPE_SCENERY:
                mapElement->properties.scenery.type = _smallSceneryTypeToEntryMap[mapElement->properties.scenery.type];
                break;
            case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
            {
                uint8 type = mapElement->properties.scenerymultiple.type & MAP_ELEMENT_LARGE_TYPE_MASK;
                mapElement->properties.scenerymultiple.type &= ~MAP_ELEMENT_LARGE_TYPE_MASK;
                mapElement->properties.scenerymultiple.type |= _largeSceneryTypeToEntryMap[type];
                break;
            }
            }
        }
    }

    EntryList * GetEntryList(uint8 objectType)
    {
        switch (objectType) {
        case OBJECT_TYPE_RIDE:          return &_rideEntries;
        case OBJECT_TYPE_SMALL_SCENERY: return &_smallSceneryEntries;
        case OBJECT_TYPE_LARGE_SCENERY: return &_largeSceneryEntries;
        case OBJECT_TYPE_WALLS:         return &_wallEntries;
        case OBJECT_TYPE_PATHS:         return &_pathEntries;
        case OBJECT_TYPE_PATH_BITS:     return &_pathAdditionEntries;
        case OBJECT_TYPE_SCENERY_SETS:  return &_sceneryGroupEntries;
        case OBJECT_TYPE_WATER:         return &_waterEntry;
        }
        return nullptr;
    }

    const rct1_research_item * GetResearchList(size_t * count)
    {
        // Loopy Landscapes stores research items in a different place
        if (_gameVersion == FILE_VERSION_RCT1_LL)
        {
            *count = Util::CountOf(_s4.research_items_LL);
            return _s4.research_items_LL;
        }
        else
        {
            *count = Util::CountOf(_s4.research_items);
            return _s4.research_items;
        }
    }

    std::string GetUserString(rct_string_id stringId)
    {
        utf8 buffer[128] = { 0 };
        const char * originalString = _s4.string_table[(stringId - 0x8000) % 1024];
        rct2_to_utf8(buffer, originalString);
        return std::string(buffer);
    }

    void FixLandOwnership()
    {
        switch (_s4.scenario_slot_index) {
        case SC_DYNAMITE_DUNES:
            FixLandOwnershipTiles({ {97, 18}, {99, 19}, {83, 34} });
            break;
        case SC_LEAFY_LAKE:
            FixLandOwnershipTiles({ {49, 66} });
            break;
        case SC_KATIES_DREAMLAND:
            FixLandOwnershipTiles({ {74, 70}, {75, 70}, {76, 70}, {77, 73}, {80, 77} });
            break;
        case SC_POKEY_PARK:
            FixLandOwnershipTiles({ {64, 102} });
            break;
        case SC_MYSTIC_MOUNTAIN:
            FixLandOwnershipTiles({ {98, 69}, {98, 70}, {103, 64}, {53, 79}, {86, 93}, {87, 93} });
            break;
        case SC_PACIFIC_PYRAMIDS:
            FixLandOwnershipTiles({ {93, 105}, {63, 34}, {76, 25}, {85, 31}, {96, 47}, {96, 48} });
            break;
        case SC_UTOPIA:
            FixLandOwnershipTiles({ {85, 73} });
            break;
        case SC_URBAN_PARK:
            FixLandOwnershipTiles({ {64, 77}, {61, 66}, {61, 67}, {39, 20} });
            break;
        }
    }

    void FixLandOwnershipTiles(std::initializer_list<rct_xy8> tiles)
    {

        rct_map_element * currentElement;
        for (const rct_xy8 * tile = tiles.begin(); tile != tiles.end(); ++tile)
        {
            currentElement = map_get_surface_element_at((*tile).x, (*tile).y);
            currentElement->properties.surface.ownership |= OWNERSHIP_AVAILABLE;
        }
    }

    /**
     * Counts the block sections. The reason this iterates over the map is to avoid getting into infinite loops,
     * which can happen with hacked parks.
     */
    void CountBlockSections()
    {
        for (sint32 x = 0; x < RCT1_MAX_MAP_SIZE; x++)
        {
            for (sint32 y = 0; y < RCT1_MAX_MAP_SIZE; y++)
            {
                rct_map_element * mapElement = map_get_first_element_at(x, y);
                do
                {
                    if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK)
                    {
                        // Lift hill tops are the only pieces present in RCT1 that can count as a block brake.
                        if (!track_element_is_lift_hill(mapElement))
                            continue;

                        uint8 trackType = mapElement->properties.track.type;
                        switch (trackType) {
                        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
                        case TRACK_ELEM_60_DEG_UP_TO_FLAT:
                        case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
                        case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
                            break;
                        default:
                            continue;
                        }

                        uint8 rideIndex = mapElement->properties.track.ride_index;
                        Ride * ride = get_ride(rideIndex);
                        ride->num_block_brakes++;
                    }
                }
                while (!map_element_is_last_for_tile(mapElement++));
            }
        }
    }
};

IParkImporter * ParkImporter::CreateS4()
{
    return new S4Importer();
}

/////////////////////////////////////////
// C -> C++ transfer
/////////////////////////////////////////
extern "C"
{
    ParkLoadResult * load_from_sv4(const utf8 * path)
    {
        ParkLoadResult * result = nullptr;
        auto s4Importer = std::make_unique<S4Importer>();
        try
        {
            result = new ParkLoadResult(s4Importer->LoadSavedGame(path));
            if (result->Error == PARK_LOAD_ERROR_OK)
            {
                s4Importer->Import();
            }
        }
        catch (const Exception &)
        {
            delete result;
            result = new ParkLoadResult(ParkLoadResult::CreateUnknown());
        }
        return result;
    }

    ParkLoadResult * load_from_sc4(const utf8 * path)
    {
        ParkLoadResult * result = nullptr;
        auto s4Importer = std::make_unique<S4Importer>();
        try
        {
            result = new ParkLoadResult(s4Importer->LoadScenario(path));
            if (result->Error == PARK_LOAD_ERROR_OK)
            {
                s4Importer->Import();
            }
        }
        catch (const Exception &)
        {
            delete result;
            result = new ParkLoadResult(ParkLoadResult::CreateUnknown());
        }
        return result;
    }
}
