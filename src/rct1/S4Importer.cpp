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

#include <vector>
#include "../core/Collections.hpp"
#include "../core/Console.hpp"
#include "../core/Exception.hpp"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "../object/ObjectManager.h"
#include "S4Importer.h"
#include "Tables.h"

extern "C"
{
    #include "../audio/audio.h"
    #include "../cheats.h"
    #include "../editor.h"
    #include "../game.h"
    #include "../interface/window.h"
    #include "../localisation/date.h"
    #include "../localisation/localisation.h"
    #include "../management/finance.h"
    #include "../management/marketing.h"
    #include "../object.h"
    #include "../object/ObjectManager.h"
    #include "../peep/staff.h"
    #include "../rct1.h"
    #include "../util/sawyercoding.h"
    #include "../util/util.h"
    #include "../world/climate.h"
    #include "../world/footpath.h"
    #include "../world/map_animation.h"
    #include "../world/park.h"
    #include "../world/scenery.h"
    #include "../ride/ride_data.h"
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

class S4Importer : public IS4Importer
{
private:
    const utf8 * _s4Path;
    rct1_s4      _s4;
    uint8        _gameVersion;

    // Lists of dynamic object entries
    EntryList _rideEntries;
    EntryList _smallSceneryEntries;
    EntryList _largeSceneryEntries;
    EntryList _wallEntries;
    EntryList _pathEntries;
    EntryList _pathAdditionEntries;
    EntryList _sceneryGroupEntries;

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
    void LoadSavedGame(const utf8 * path)
    {
        if (!rct1_read_sv4(path, &_s4))
        {
            throw Exception("Unable to load SV4.");
        }
        _s4Path = path;
    }

    void LoadScenario(const utf8 * path)
    {
        if (!rct1_read_sc4(path, &_s4))
        {
            throw Exception("Unable to load SC4.");
        }
        _s4Path = path;
    }

    void Import()
    {
        Initialise();

        CreateAvailableObjectMappings();
        LoadObjects();

        ImportRides();
        ImportRideMeasurements();
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

        game_convert_strings_to_utf8();
    }

private:
    void Initialise()
    {
        _gameVersion = sawyercoding_detect_rct1_version(_s4.game_version) & FILE_VERSION_MASK;

        Memory::Set(_rideTypeToRideEntryMap,     255, sizeof(_rideTypeToRideEntryMap));
        Memory::Set(_vehicleTypeToRideEntryMap,  255, sizeof(_vehicleTypeToRideEntryMap));
        Memory::Set(_smallSceneryTypeToEntryMap, 255, sizeof(_smallSceneryTypeToEntryMap));
        Memory::Set(_largeSceneryTypeToEntryMap, 255, sizeof(_largeSceneryTypeToEntryMap));
        Memory::Set(_wallTypeToEntryMap,         255, sizeof(_wallTypeToEntryMap));
        Memory::Set(_pathTypeToEntryMap,         255, sizeof(_pathTypeToEntryMap));
        Memory::Set(_pathAdditionTypeToEntryMap, 255, sizeof(_pathAdditionTypeToEntryMap));
        Memory::Set(_sceneryThemeTypeToEntryMap, 255, sizeof(_sceneryThemeTypeToEntryMap));

        uint16 mapSize = _s4.map_size == 0 ? 128 : _s4.map_size;

        // Do map initialisation, same kind of stuff done when loading scenario editor
        audio_pause_sounds();
        audio_unpause_sounds();
        GetObjectManager()->UnloadAll();
        map_init(mapSize);
        banner_init();
        reset_park_entrances();
        user_string_clear_all();
        reset_sprite_list();
        ride_init_all();
        window_guest_list_init_vars_a();
        staff_reset_modes();
        park_init();
        finance_init();
        date_reset();
        window_guest_list_init_vars_b();
        window_staff_list_init_vars();
        gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
        gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        window_new_ride_init_vars();
        gS6Info.category = SCENARIO_CATEGORY_OTHER;
        news_item_init_queue();
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
            if (researchItem->item == RCT1_RESEARCH_END_RESEARCHABLE ||
                researchItem->item == RCT1_RESEARCH_END)
            {
                break;
            }
            if (researchItem->item == RCT1_RESEARCH_END_AVAILABLE)
            {
                continue;
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
                    if (researchItem2->item == RCT1_RESEARCH_END_RESEARCHABLE ||
                        researchItem2->item == RCT1_RESEARCH_END_AVAILABLE)
                    {
                        break;
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
                uint8 pathColour = mapElement->type & 3;
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
            case MAP_ELEMENT_TYPE_FENCE:
            {
                uint8  var_05 = mapElement->properties.fence.item[0];
                uint16 var_06 = mapElement->properties.fence.item[1] |
                               (mapElement->properties.fence.item[2] << 8);

                for (int edge = 0; edge < 4; edge++)
                {
                    int typeA = (var_05 >> (edge * 2)) & 3;
                    int typeB = (var_06 >> (edge * 4)) & 0x0F;
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
        for (int sceneryTheme = 0; sceneryTheme <= RCT1_SCENERY_THEME_PAGODA; sceneryTheme++)
        {
            if (sceneryTheme != 0 &&
                _sceneryThemeTypeToEntryMap[sceneryTheme] == 255) continue;

            std::vector<const char *> objects = RCT1::GetSceneryObjects(sceneryTheme);
            for (const char * objectName : objects)
            {
                rct_object_entry * foundEntry = object_list_find_by_name(objectName);
                if (foundEntry != nullptr)
                {
                    uint8 objectType = foundEntry->flags & 0x0F;
                    switch (objectType) {
                    case OBJECT_TYPE_SMALL_SCENERY:
                    case OBJECT_TYPE_LARGE_SCENERY:
                    case OBJECT_TYPE_WALLS:
                    case OBJECT_TYPE_PATHS:
                    case OBJECT_TYPE_PATH_BITS:
                    {
                        EntryList * entries = GetEntryList(objectType);

                        // Check if there are spare entries available
                        size_t maxEntries = (size_t)object_entry_group_counts[objectType];
                        if (entries->GetCount() < maxEntries)
                        {
                            entries->GetOrAddEntry(objectName);
                        }
                        break;
                    }
                    }
                }
            }
        }
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
        for (int i = 0; i < MAX_RIDES; i++)
        {
            if (_s4.rides[i].type != RIDE_TYPE_NULL)
            {
                ImportRide(get_ride(i), &_s4.rides[i]);
            }
        }
    }

    void ImportRide(rct_ride * dst, rct1_ride * src)
    {
        memset(dst, 0, sizeof(rct_ride));

        dst->type = RCT1::GetRideType(src->type);
        if (RCT1::RideTypeUsesVehicles(src->type))
        {
            dst->subtype = _vehicleTypeToRideEntryMap[src->vehicle_type];
        }
        else
        {
            dst->subtype = _rideTypeToRideEntryMap[src->type];
        }

        rct_ride_entry * rideEntry = get_ride_entry(dst->subtype);
        Guard::Assert(rideEntry != nullptr && rideEntry != (rct_ride_entry*)-1);

        // Ride name
        dst->name = 0;
        if (is_user_string_id(src->name))
        {
            const char * rideName = GetUserString(src->name);
            if (rideName[0] != 0)
            {
                rct_string_id rideNameStringId = user_string_allocate(4, rideName);
                if (rideNameStringId != 0)
                {
                    dst->name = rideNameStringId;
                }
            }
        }
        if (dst->name == 0)
        {
            dst->name = 1;

            uint16 * args = (uint16*)&dst->name_arguments;
            args[0] = 2 + dst->type;
            args[1] = src->name_argument_number;
        }

        // We can't convert vehicles yet so just close the ride
        dst->status = RIDE_STATUS_CLOSED;

        // Flags
        if (src->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)        dst->lifecycle_flags |= RIDE_LIFECYCLE_ON_RIDE_PHOTO;
        if (src->lifecycle_flags & RIDE_LIFECYCLE_MUSIC)                dst->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
        if (src->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)       dst->lifecycle_flags |= RIDE_LIFECYCLE_INDESTRUCTIBLE;
        if (src->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) dst->lifecycle_flags |= RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;

        // Station
        dst->overall_view = src->overall_view;
        for (int i = 0; i < 4; i++)
        {
            dst->station_starts[i] = src->station_starts[i];
            dst->station_heights[i] = src->station_height[i] / 2;
            dst->station_length[i] = src->station_length[i];
            dst->station_depart[i] = src->station_light[i];

            // Use src->station_depart[i] when we import with guests and vehicles intact
            dst->train_at_station[i] = 0xFF;

            dst->entrances[i] = src->entrance[i];
            dst->exits[i] = src->exit[i];
            dst->queue_time[i] = src->queue_time[i];
            dst->last_peep_in_queue[i] = 0xFFFF;
        }
        dst->num_stations = src->num_stations;

        for (int i = 0; i < 32; i++)
        {
            dst->vehicles[i] = SPRITE_INDEX_NULL;
        }
        dst->num_vehicles = src->num_trains;
        dst->num_cars_per_train = src->num_cars_per_train + rideEntry->zero_cars;
        dst->proposed_num_vehicles = src->num_trains;
        dst->max_trains = 32;
        dst->proposed_num_cars_per_train = src->num_cars_per_train + rideEntry->zero_cars;

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

        // Colours
        dst->colour_scheme_type = src->colour_scheme;
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->track_colour_main[0] = RCT1::GetColour(src->track_primary_colour);
            dst->track_colour_additional[0] = RCT1::GetColour(src->track_secondary_colour);
            dst->track_colour_supports[0] = RCT1::GetColour(src->track_support_colour);
        }
        else
        {
            for (int i = 0; i < 4; i++)
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
            for (int i = 0; i < 12; i++)
            {
                dst->vehicle_colours[i].body_colour = RCT1::GetColour(src->vehicle_colours[i].body);
                dst->vehicle_colours[i].trim_colour = RCT1::GetColour(src->vehicle_colours[i].trim);
            }
        }

        // Fix other Z
        // dst->start_drop_height /= 2;
        // dst->highest_drop_height = 1;
        // if (dst->cur_test_track_z != 255)
        // {
        //     dst->cur_test_track_z /= 2;
        // }
        // dst->chairlift_bullwheel_z[0] /= 2;
        // dst->chairlift_bullwheel_z[1] /= 2;

        // Maintenance
        dst->build_date = src->build_date;
        dst->inspection_interval = src->inspection_interval;
        dst->last_inspection = src->last_inspection;
        dst->reliability = src->reliability;
        dst->unreliability_factor = src->unreliability_factor;
        dst->breakdown_reason = src->breakdown_reason;

        // Finance
        dst->upkeep_cost = src->upkeep_cost;
        dst->price = src->price;
        dst->income_per_hour = src->income_per_hour;

        dst->value = src->value;
        dst->satisfaction = 255;
        dst->satisfaction_time_out = 0;
        dst->satisfaction_next = 0;
        dst->popularity = src->popularity;
        dst->popularity_next = src->popularity_next;
        dst->popularity_time_out = src->popularity_time_out;

        dst->music_tune_id = 255;
        dst->measurement_index = 255;
        dst->excitement = (ride_rating)-1;
    }

    void ImportRideMeasurements()
    {
        for (int i = 0; i < MAX_RIDE_MEASUREMENTS; i++)
        {
            rct_ride_measurement * dst = get_ride_measurement(i);
            rct_ride_measurement * src = &_s4.ride_measurements[i];
            ImportRideMeasurement(dst, src);
        }
    }

    void ImportRideMeasurement(rct_ride_measurement * dst, rct_ride_measurement * src)
    {
        // Not yet supported
        // *dst = *src;
        // for (int i = 0; i < RIDE_MEASUREMENT_MAX_ITEMS; i++)
        // {
        //     dst->altitude[i] /= 2;
        // }
    }

    void ImportPeepSpawns()
    {
        for (int i = 0; i < 2; i++)
        {
            gPeepSpawns[i] = _s4.peep_spawn[i];
        }
    }

    void ImportMapAnimations()
    {
        // This is sketchy, ideally we should try to re-create them
        rct_map_animation * s4Animations = (rct_map_animation*)_s4.map_animations;
        for (int i = 0; i < 1000; i++)
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
        gParkValue = _s4.park_value;
        gCurrentProfit = _s4.profit;

        for (int i = 0; i < 128; i++)
        {
            gCashHistory[i] = _s4.cash_history[i];
            gParkValueHistory[i] = _s4.park_value_history[i];
            gWeeklyProfitHistory[i] = _s4.weekly_profit_history[i];
        }

        for (int i = 0; i < 14 * 16; i++)
        {
            gExpenditureTable[i] = _s4.expenditure[i];
        }
        gCurrentExpenditure = _s4.total_expenditure;

        gTotalAdmissions = _s4.num_admissions;
        gTotalIncomeFromAdmissions = _s4.admission_total_income;

        // TODO marketing campaigns not working
        for (int i = 0; i < 6; i++)
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
        LoadObjects(OBJECT_TYPE_WATER, std::vector<const char *>({ "WTRCYAN " }));
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

    void ImportMapElements()
    {
        memcpy(gMapElements, _s4.map_elements, 0xC000 * sizeof(rct_map_element));
        ClearExtraTileEntries();
        FixColours();
        FixZ();
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
        for (int i = 0; i < 6; i++)
        {
            research_insert_scenery_group_entry(i, true);
        }

        bool researched = true;
        for (size_t i = 0; i < researchListCount; i++)
        {
            const rct1_research_item * researchItem = &researchList[i];
            if (researchItem->item == RCT1_RESEARCH_END_AVAILABLE)
            {
                researched = false;
            }
            else if (researchItem->item == RCT1_RESEARCH_END_RESEARCHABLE ||
                     researchItem->item == RCT1_RESEARCH_END)
            {
                break;
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

                // Add all vehicles for this ride type that are researched or before this research item
                uint32 numVehicles = 0;
                for (size_t j = 0; j < researchListCount; j++)
                {
                    const rct1_research_item *researchItem2 = &researchList[j];
                    if (researchItem2->item == RCT1_RESEARCH_END_RESEARCHABLE ||
                        researchItem2->item == RCT1_RESEARCH_END_AVAILABLE)
                    {
                        break;
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
                    Guard::Assert(rideEntryIndex != 255, "rideEntryIndex was 255");

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
        // Fixes availability of rides
        sub_684AC3();

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
        const char * parkName = _s4.scenario_name;
        if (is_user_string_id((rct_string_id)_s4.park_name_string_index))
        {
            const char * userString = GetUserString(_s4.park_name_string_index);
            if (userString[0] != '\0')
            {
                parkName = userString;
            }
        }

        rct_string_id stringId = user_string_allocate(4, parkName);
        if (stringId != 0)
        {
            gParkName = stringId;
            gParkNameArgs = 0;
        }
    }

    void ImportParkFlags()
    {
        // Date and srand
        gCurrentTicks = _s4.ticks;
        gScenarioSrand0 = _s4.random_a;
        gScenarioSrand1 = _s4.random_b;
        gDateMonthsElapsed = _s4.month;
        gDateMonthTicks = _s4.day;

        // Park rating
        gParkRating = _s4.park_rating;
        for (int i = 0; i < 32; i++)
        {
            gParkRatingHistory[i] = _s4.park_rating_history[i];
        }

        // Awards
        for (int i = 0; i < MAX_AWARDS; i++)
        {
            gCurrentAwards[i] = _s4.awards[i];
        }

        // Number of guests history
        for (int i = 0; i < 32; i++)
        {
            gGuestsInParkHistory[i] = _s4.guests_in_park_history[i];
        }

        // News items
        rct_news_item *newsItems = gNewsItems;
        for (int i = 0; i < MAX_NEWS_ITEMS; i++)
        {
            newsItems[i] = _s4.messages[i];
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
        if (!(_s4.park_flags & RCT1_PARK_FLAGS_PARK_ENTRY_LOCKED_AT_FREE))
        {
            gCheatsUnlockAllPrices = true;
        }
        // RCT2 uses two flags for no money (for cheat detection). RCT1 used only one. 
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
        String::Set(gS6Info.name, sizeof(gS6Info.name), _s4.scenario_name);
        String::Set(gS6Info.details, sizeof(gS6Info.details), "");

        int scNumber = GetSCNumber();
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
                        String::Set(gS6Info.name, sizeof(gS6Info.name), language_get_string(localisedStringIds[0]));
                    }
                    if (localisedStringIds[2] != STR_NONE)
                    {
                        String::Set(gS6Info.details, sizeof(gS6Info.details), language_get_string(localisedStringIds[2]));
                    }
                }
            }
        }
    }

    void ImportScenarioObjective()
    {
        gScenarioObjectiveType = _s4.scenario_objective_type;
        gScenarioObjectiveYear = _s4.scenario_objective_years;
        gScenarioObjectiveCurrency = _s4.scenario_objective_currency;
        gScenarioObjectiveNumGuests = _s4.scenario_objective_num_guests;
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
        for (int i = 0; i < 0x10000; i++)
        {
            gMapElementTilePointers[i] = (rct_map_element *)-1;
        }

        // Get the first free map element
        rct_map_element * nextFreeMapElement = gMapElements;
        for (int i = 0; i < 128 * 128; i++)
        {
            do { } while (!map_element_is_last_for_tile(nextFreeMapElement++));
        }

        rct_map_element * mapElement = gMapElements;
        rct_map_element * * tilePointer = gMapElementTilePointers;

        // 128 rows of map data from RCT1 map
        for (int x = 0; x < 128; x++)
        {
            // Assign the first half of this row
            for (int y = 0; y < 128; y++)
            {
                *tilePointer++ = mapElement;
                do { } while (!map_element_is_last_for_tile(mapElement++));
            }

            // Fill the rest of the row with blank tiles
            for (int y = 0; y < 128; y++)
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
        for (int y = 0; y < 128 * 256; y++)
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

    void FixColours()
    {
        colour_t colour;

        // The following code would be worth doing if we were able to import sprites
        // for (int i = 0; i < MAX_SPRITES; i++)
        // {
        //     rct_unk_sprite * sprite = &(g_sprite_list[i].unknown);
        //     switch (sprite->sprite_identifier) {
        //     case SPRITE_IDENTIFIER_PEEP:
        //     {
        //         rct_peep * peep = (rct_peep*)sprite;
        //         peep->tshirt_colour = RCT1ColourConversionTable[peep->tshirt_colour];
        //         peep->trousers_colour = RCT1ColourConversionTable[peep->trousers_colour];
        //         peep->balloon_colour = RCT1ColourConversionTable[peep->balloon_colour];
        //         peep->umbrella_colour = RCT1ColourConversionTable[peep->umbrella_colour];
        //         peep->hat_colour = RCT1ColourConversionTable[peep->hat_colour];
        //         break;
        //     }
        //     case SPRITE_IDENTIFIER_MISC:
        //     {
        //         rct_balloon * balloon = (rct_balloon*)sprite;
        //         balloon->colour = RCT1ColourConversionTable[balloon->colour];
        //         balloon->var_2D = RCT1ColourConversionTable[balloon->var_2D];
        //         break;
        //     }
        //     }
        // }

        rct_map_element * mapElement = gMapElements;
        while (mapElement < gNextFreeMapElement)
        {
            if (mapElement->base_height != 255)
            {
                switch (map_element_get_type(mapElement)) {
                case MAP_ELEMENT_TYPE_SCENERY:
                    colour = RCT1::GetColour(mapElement->properties.scenery.colour_1 & 0x1F);
                    mapElement->properties.scenery.colour_1 &= 0xE0;
                    mapElement->properties.scenery.colour_1 |= colour;

                    // Copied from [rct2: 0x006A2956]
                    switch (mapElement->properties.scenery.type) {
                    case 157: // TGE1 (Geometric Sculpture)
                    case 162: // TGE2 (Geometric Sculpture)
                    case 168: // TGE3 (Geometric Sculpture)
                    case 170: // TGE4 (Geometric Sculpture)
                    case 171: // TGE5 (Geometric Sculpture)
                        mapElement->properties.scenery.colour_2 = COLOUR_WHITE;
                        break;
                    }
                    break;
                case MAP_ELEMENT_TYPE_FENCE:
                    colour = ((mapElement->type & 0xC0) >> 3) |
                             ((mapElement->properties.fence.type & 0xE0) >> 5);
                    colour = RCT1::GetColour(colour);

                    mapElement->type &= 0x3F;
                    mapElement->properties.fence.type &= 0x1F;
                    mapElement->type |= (colour & 0x18) << 3;
                    mapElement->properties.fence.type |= (colour & 7) << 5;
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

    void FixZ()
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
                    uint8 entryIndex = _pathAdditionTypeToEntryMap[normalisedType];
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
        for (int x = 0; x < 128; x++)
        {
            for (int y = 0; y < 128; y++)
            {
                rct_map_element * mapElement = map_get_first_element_at(x, y);
                do
                {
                    if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_FENCE)
                    {
                        rct_map_element originalMapElement = *mapElement;
                        map_element_remove(mapElement);

                        uint8 var_05 = originalMapElement.properties.fence.item[0];
                        uint16 var_06 = originalMapElement.properties.fence.item[1] |
                                       (originalMapElement.properties.fence.item[2] << 8);

                        for (int edge = 0; edge < 4; edge++)
                        {
                            int typeA = (var_05 >> (edge * 2)) & 3;
                            int typeB = (var_06 >> (edge * 4)) & 0x0F;
                            if (typeB != 0x0F)
                            {
                                int type = typeA | (typeB << 2);
                                int colourA = ((originalMapElement.type & 0xC0) >> 3) |
                                               (originalMapElement.properties.fence.type >> 5);
                                int colourB = 0;
                                int colourC = 0;
                                ConvertWall(&type, &colourA, &colourB, &colourC);

                                type = _wallTypeToEntryMap[type];
                                map_place_fence(type, x * 32, y * 32, 0, edge, colourA, colourB, colourC, 169);
                            }
                        }
                        break;
                    }
                }
                while (!map_element_is_last_for_tile(mapElement++));
            }
        }
    }

    void ConvertWall(int * type, int * colourA, int * colourB, int * colourC)
    {
        switch (*type) {
        case 12:    // creepy gate
            *colourA = 24;
            break;
        case 26:    // white wooden fence
            *type = 12;
            *colourA = 2;
            break;
        case 27:    // red wooden fence
            *type = 12;
            *colourA = 25;
            break;
        case 50:    // plate glass
            *colourA = 24;
            break;
        case 13:
            *colourB = *colourA;
            *colourA = 24;
            break;
        case 11:    // tall castle wall with grey gate
        case 22:    // brick wall with gate
            *colourB = 2;
            break;
        case 35:    // wood post fence
        case 42:    // tall grey castle wall
        case 43:    // wooden fence with snow
        case 44:
        case 45:
        case 46:
            *colourA = 1;
            break;
        }
    }

    void FixBanners()
    {
        for (int x = 0; x < 128; x++)
        {
            for (int y = 0; y < 128; y++)
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
            const char * bannerText = GetUserString(src->string_idx);
            if (!String::IsNullOrEmpty(bannerText))
            {
                rct_string_id bannerTextStringId = user_string_allocate(128, bannerText);
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
        for (int i = 0; i < 4; i++)
        {
            gParkEntranceX[i] = (sint16)0x8000;
        }

        uint8 entranceIndex = 0;

        map_element_iterator it;
        map_element_iterator_begin(&it);
        while (map_element_iterator_next(&it) && entranceIndex < 4)
        {
            rct_map_element * element = it.element;

            if (map_element_get_type(element) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
            if (element->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE) continue;
            if ((element->properties.entrance.index & 0x0F) != 0) continue;

            gParkEntranceX[entranceIndex] = it.x * 32;
            gParkEntranceY[entranceIndex] = it.y * 32;
            gParkEntranceZ[entranceIndex] = element->base_height * 8;
            gParkEntranceDirection[entranceIndex] = element->type & 3;
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

    int GetSCNumber()
    {
        const utf8 * fileName = Path::GetFileName(_s4Path);
        if (tolower(fileName[0]) == 's' && tolower(fileName[1]) == 'c') {
            constexpr size_t maxDigits = 7;
            utf8 digitBuffer[maxDigits + 1];
            utf8 * dst = digitBuffer;
            const utf8 * src = fileName + 2;
            for (size_t i = 0; i < maxDigits && *src != '.'; i++)
            {
                *dst++ = *src++;
            }
            *dst++ = 0;

            if (digitBuffer[0] == '0' && digitBuffer[1] == '\0')
            {
                return 0;
            }
            else
            {
                int digits = atoi(digitBuffer);
                return digits == 0 ? -1 : digits;
            }
        }
        else
        {
            return -1;
        }
    }

    const char * GetUserString(rct_string_id stringId)
    {
        return _s4.string_table[(stringId - 0x8000) % 1024];
    }
};

IS4Importer * CreateS4Importer()
{
    return new S4Importer();
}

/////////////////////////////////////////
// C -> C++ transfer
/////////////////////////////////////////
extern "C"
{
    bool rct1_load_saved_game(const utf8 * path)
    {
        bool result;

        auto s4Importer = new S4Importer();
        try
        {
            s4Importer->LoadSavedGame(path);
            s4Importer->Import();
            result = true;
        } catch (Exception ex)
        {
            result = false;
        }
        delete s4Importer;
        return result;
    }

    bool rct1_load_scenario(const utf8 * path)
    {
        bool result;

        auto s4Importer = new S4Importer();
        try
        {
            s4Importer->LoadScenario(path);
            s4Importer->Import();
            result = true;
        } catch (Exception ex)
        {
            result = false;
        }
        delete s4Importer;
        return result;
    }

    colour_t rct1_get_colour(colour_t colour)
    {
        return RCT1::GetColour(colour);
    }

    /**
     * This function keeps a list of the preferred vehicle for every generic track
     * type, out of the available vehicle types in the current game. It determines
     * which picture is shown on the new ride tab and which train type is selected
     * by default.
     */
    int vehicle_preference_compare(uint8 rideType, const char * a, const char * b)
    {
        std::vector<const char *> rideEntryOrder = RCT1::GetPreferedRideEntryOrder(rideType);
        for (const char * object : rideEntryOrder)
        {
            if (String::Equals(object, a, true))
            {
                return -1;
            }
            if (String::Equals(object, b, true))
            {
                return  1;
            }
        }
        return 0;
    }
}
