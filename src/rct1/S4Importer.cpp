#include <Windows.h>

#include "S4Importer.h"

#include "../core/Exception.hpp"
#include "../core/List.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "import.h"


extern "C"
{
    #include "../audio/audio.h"
    #include "../game.h"
    #include "../interface/window.h"
    #include "../localisation/date.h"
    #include "../localisation/localisation.h"
    #include "../management/finance.h"
    #include "../object.h"
    #include "../peep/staff.h"
    #include "../rct1.h"
    #include "../util/sawyercoding.h"
    #include "../util/util.h"
    #include "../world/footpath.h"
    #include "../world/map_animation.h"
    #include "../world/park.h"
}

void S4Importer::LoadSavedGame(const utf8 * path)
{
    if (!rct1_read_sv4(path, &_s4)) {
        throw Exception("Unable to load SV4.");
    }
    _s4Path = path;
}

void S4Importer::LoadScenario(const utf8 * path)
{
    if (!rct1_read_sc4(path, &_s4)) {
        throw Exception("Unable to load SC4.");
    }
    _s4Path = path;
}

void S4Importer::Import()
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

    RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) = _s4.park_flags;
    RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) &= ~PARK_FLAGS_ANTI_CHEAT_DEPRECATED;

    ImportScenarioNameDetails();
    ImportScenarioObjective();
    ImportSavedView();
}

void S4Importer::Initialise()
{
    _gameVersion = sawyercoding_detect_rct1_version(_s4.game_version) & FILE_VERSION_MASK;

    Memory::Set(_rideTypeToRideEntryMap,     255, sizeof(_rideTypeToRideEntryMap));
    Memory::Set(_vehicleTypeToRideEntryMap,  255, sizeof(_vehicleTypeToRideEntryMap));
    Memory::Set(_smallSceneryTypeToEntryMap, 255, sizeof(_smallSceneryTypeToEntryMap));
    Memory::Set(_largeSceneryTypeToEntryMap, 255, sizeof(_largeSceneryTypeToEntryMap));
    Memory::Set(_wallTypeToEntryMap,         255, sizeof(_wallTypeToEntryMap));
    Memory::Set(_sceneryThemeTypeToEntryMap, 255, sizeof(_sceneryThemeTypeToEntryMap));

    uint16 mapSize = _s4.map_size == 0 ? 128 : _s4.map_size;

    // Do map initialisation, same kind of stuff done when loading scenario editor
    audio_pause_sounds();
    audio_unpause_sounds();
    object_unload_all();
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
    RCT2_GLOBAL(0x0141F570, uint8) = 0;
    RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
    window_new_ride_init_vars();
    RCT2_GLOBAL(0x0141F571, uint8) = 4;
    news_item_init_queue();
}

void S4Importer::CreateAvailableObjectMappings()
{
    AddAvailableEntriesFromResearchList();
    AddAvailableEntriesFromMap();
    AddAvailableEntriesFromRides();
}

void S4Importer::AddAvailableEntriesFromResearchList()
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

void S4Importer::AddAvailableEntriesFromMap()
{
    size_t maxTiles = 128 * 128;
    size_t tileIndex = 0;
    rct_map_element * mapElement = _s4.map_elements;
    
    while (tileIndex < maxTiles)
    {
        switch (map_element_get_type(mapElement)) {
        case MAP_ELEMENT_TYPE_SCENERY:
            AddEntryForSmallScenery(mapElement->properties.scenery.type);
            break;
        case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
            AddEntryForLargeScenery(mapElement->properties.scenerymultiple.type & MAP_ELEMENT_LARGE_TYPE_MASK);
            break;
        case MAP_ELEMENT_TYPE_FENCE:
            AddEntryForWall(mapElement->properties.fence.type);
            break;
        }

        if (map_element_is_last_for_tile(mapElement++))
        {
            tileIndex++;
        }
    }
}

void S4Importer::AddAvailableEntriesFromRides()
{
    for (size_t i = 0; i < Util::CountOf(_s4.rides); i++)
    {
        rct1_ride * ride = &_s4.rides[i];

        // TODO might need to check if ride type has a vehicle type
        AddEntryForVehicleType(ride->type, ride->vehicle_type);
    }
}

void S4Importer::AddEntryForRideType(uint8 rideType)
{
    if (_rideTypeToRideEntryMap[rideType] == 255)
    {
        const char * entryName = RCT1::GetRideTypeObject(rideType);
        _rideEntries.Add(entryName);
        _rideTypeToRideEntryMap[rideType] = (uint8)_rideEntries.GetCount();
    }
}

void S4Importer::AddEntryForVehicleType(uint8 rideType, uint8 vehicleType)
{
    if (_vehicleTypeToRideEntryMap[rideType] == 255)
    {
        const char * entryName = RCT1::GetVehicleObject(vehicleType);

        uint8 rideEntryIndex = (uint8)_rideEntries.GetCount();
        _vehicleTypeToRideEntryMap[vehicleType] = rideEntryIndex;
        _rideEntries.Add(entryName);

        // Just overwrite this with the vehicle entry for now...
        _rideTypeToRideEntryMap[rideType] = rideEntryIndex;
    }
}

void S4Importer::AddEntryForSmallScenery(uint8 smallSceneryType)
{
    if (_smallSceneryTypeToEntryMap[smallSceneryType] == 255)
    {
        const char * entryName = RCT1::GetSmallSceneryObject(smallSceneryType);
        _smallSceneryEntries.Add(entryName);
        _smallSceneryTypeToEntryMap[smallSceneryType] = (uint8)_smallSceneryEntries.GetCount();
    }
}

void S4Importer::AddEntryForLargeScenery(uint8 largeSceneryType)
{
    if (_largeSceneryTypeToEntryMap[largeSceneryType] == 255)
    {
        const char * entryName = RCT1::GetLargeSceneryObject(largeSceneryType);
        _largeSceneryEntries.Add(entryName);
        _largeSceneryTypeToEntryMap[largeSceneryType] = (uint8)_largeSceneryEntries.GetCount();
    }
}

void S4Importer::AddEntryForWall(uint8 wallType)
{
    if (_wallTypeToEntryMap[wallType] == 255)
    {
        const char * entryName = RCT1::GetWallObject(wallType);
        _wallEntries.Add(entryName);
        _wallTypeToEntryMap[wallType] = (uint8)_wallEntries.GetCount();
    }
}

void S4Importer::AddEntriesForSceneryTheme(uint8 sceneryThemeType)
{
    const char * entryName = RCT1::GetSceneryGroupObject(sceneryThemeType);

    _sceneryThemeTypeToEntryMap[sceneryThemeType] = (uint8)_sceneryGroupEntries.GetCount();
    _sceneryGroupEntries.Add(entryName);

    // TODO add entries for all scenery items belonging to scenery group
}

void S4Importer::ImportRides()
{
    for (int i = 0; i < MAX_RIDES; i++)
    {
        if (_s4.rides[i].type != RIDE_TYPE_NULL)
        {
            ImportRide(get_ride(i), &_s4.rides[i]);
        }
    }
}

void S4Importer::ImportRide(rct_ride * dst, rct1_ride * src)
{
    memset(dst, 0, sizeof(rct_ride));

    dst->type = RCT1::GetRideType(src->type);
    dst->subtype = _vehicleTypeToRideEntryMap[src->vehicle_type];

    rct_ride_entry * rideEntry = get_ride_entry(dst->subtype);

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
        dst->music = RCT2_ADDRESS(0x0097D4F4, uint8)[dst->type * 8];
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

void S4Importer::ImportRideMeasurements()
{
    for (int i = 0; i < MAX_RIDE_MEASUREMENTS; i++)
    {
        rct_ride_measurement * dst = get_ride_measurement(i);
        rct_ride_measurement * src = &_s4.ride_measurements[i];
        ImportRideMeasurement(dst, src);
    }
}

void S4Importer::ImportRideMeasurement(rct_ride_measurement * dst, rct_ride_measurement * src)
{
    // Not yet supported
    // *dst = *src;
    // for (int i = 0; i < RIDE_MEASUREMENT_MAX_ITEMS; i++)
    // {
    //     dst->altitude[i] /= 2;
    // }
}

void S4Importer::ImportPeepSpawns()
{
    for (int i = 0; i < 2; i++)
    {
        gPeepSpawns[i] = _s4.peep_spawn[i];
    }
}

void S4Importer::ImportMapAnimations()
{
    // This is sketchy, ideally we should try to re-create them
    rct_map_animation * s4Animations = (rct_map_animation*)_s4.map_animations;
    for (int i = 0; i < 1000; i++)
    {
        gAnimatedObjects[i] = s4Animations[i];
        gAnimatedObjects[i].baseZ /= 2;
    }
    RCT2_GLOBAL(0x0138B580, uint16) = _s4.num_map_animations;
}

void S4Importer::ImportFinance()
{
    RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, uint32) = ENCRYPT_MONEY(_s4.cash);
    RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16) = _s4.park_entrance_fee;
}

void S4Importer::LoadObjects()
{
    LoadObjects(OBJECT_TYPE_RIDE, _rideEntries);
    LoadObjects(OBJECT_TYPE_SMALL_SCENERY, _smallSceneryEntries);
    LoadObjects(OBJECT_TYPE_LARGE_SCENERY, _largeSceneryEntries);
    LoadObjects(OBJECT_TYPE_WALLS, _wallEntries);
    LoadObjects(OBJECT_TYPE_SCENERY_SETS, _sceneryGroupEntries);
    LoadObjects(OBJECT_TYPE_PATHS, List<const char *>({
        "TARMAC  ",
        "TARMACB ",
        "PATHSPCE",
        "PATHDIRT",
        "ROAD    ",
        "PATHCRZY",
        "PATHASH "
    }));
    LoadObjects(OBJECT_TYPE_PATH_BITS, List<const char *>({
        "LAMP1   ",
        "LAMP2   ",
        "LITTER1 ",
        "BENCH1  ",
        "JUMPFNT1",
        "LAMP3   ",
        "LAMP4   ",
        "JUMPSNW1"
    }));
    LoadObjects(OBJECT_TYPE_BANNERS, List<const char *>({
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
    LoadObjects(OBJECT_TYPE_PARK_ENTRANCE, List<const char *>({ "PKENT1  " }));
    LoadObjects(OBJECT_TYPE_WATER, List<const char *>({ "WTRCYAN " }));
}

void S4Importer::LoadObjects(uint8 objectType, List<const char *> entries)
{
    for (const char * objectName : entries)
    {
        rct_object_entry entry;
        entry.flags = 0x00008000 + objectType;
        Memory::Copy(entry.name, objectName, 8);
        entry.checksum = 0;
        
        if (!object_load_chunk(objectType, &entry, NULL))
        {
            log_error("Failed to load %s.", objectName);
            throw Exception("Failed to load object.");
        }
    }
}

void S4Importer::ImportMapElements()
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

void S4Importer::ImportResearch()
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
            research_insert_scenery_group_entry(sceneryGroupEntryIndex, researched);
            break;
        }
        case RCT1_RESEARCH_CATEGORY_RIDE:
        {
            uint8 rct1RideType = researchItem->item;
            uint8 rideEntryIndex = rct1RideType;
            rct_ride_entry *rideEntry = get_ride_entry(rideEntryIndex);

            // Add all vehicles for this ride type that are researched or before this research item
            uint32 numVehicles = 0;
            for (size_t j = 0; j < researchListCount; j++)
            {
                const rct1_research_item *researchItem2 = &researchList[j];
                if (researchItem2->item == RCT1_RESEARCH_END_AVAILABLE)
                {
                    break;
                }

                if (researchItem->category == RCT1_RESEARCH_CATEGORY_VEHICLE &&
                    researchItem->related_ride == rct1RideType)
                {
                    // Only add the vehicles that were listed before this ride, otherwise we might
                    // change the research order
                    if (j < i)
                    {
                        InsertResearchVehicle(researchItem, researched);
                    }
                    numVehicles++;
                }
            }

            if (numVehicles == 0)
            {
                // No vehicles found so just add the default for this ride
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
}

void S4Importer::InsertResearchVehicle(const rct1_research_item * researchItem, bool researched)
{
    uint8 rct1RideType = researchItem->related_ride;
    uint8 vehicle = researchItem->item;

    uint8 rideEntryIndex = _vehicleTypeToRideEntryMap[vehicle];
    if (!_researchRideEntryUsed[rideEntryIndex])
    {
        _researchRideEntryUsed[rideEntryIndex] = true;
        research_insert_ride_entry(rideEntryIndex, researched);
    }
}

void S4Importer::ImportParkName()
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
        RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME, rct_string_id) = stringId;
        RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME_ARGS, uint32) = 0;
    }
}

void S4Importer::ImportScenarioNameDetails()
{
    rct_s6_info * s6Info = (rct_s6_info*)0x0141F570;

    String::Set(s6Info->name, sizeof(s6Info->name), _s4.scenario_name);
    String::Set(s6Info->details, sizeof(s6Info->details), "");

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
                    String::Set(s6Info->name, sizeof(s6Info->name), language_get_string(localisedStringIds[0]));
                }
                if (localisedStringIds[2] != STR_NONE)
                {
                    String::Set(s6Info->details, sizeof(s6Info->details), language_get_string(localisedStringIds[2]));
                }
            }
        }
    }
}

void S4Importer::ImportScenarioObjective()
{
    RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) = _s4.scenario_objective_type;
    RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8) = _s4.scenario_objective_years;
    RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, uint32) = _s4.scenario_objective_currency;
    RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) = _s4.scenario_objective_num_guests;
}

void S4Importer::ImportSavedView()
{
    RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, uint16) = _s4.view_x;
    RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, uint16) = _s4.view_y;
    RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint8) = _s4.view_zoom;
    RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION + 1, uint8) = _s4.view_rotation;
}

void S4Importer::ClearExtraTileEntries()
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

    RCT2_GLOBAL(RCT2_ADDRESS_NEXT_FREE_MAP_ELEMENT, rct_map_element*) = nextFreeMapElement;
}

void S4Importer::FixColours()
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
    while (mapElement < RCT2_GLOBAL(RCT2_ADDRESS_NEXT_FREE_MAP_ELEMENT, rct_map_element*))
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
                case 157:	// TGE1	(Geometric Sculpture)
                case 162:	// TGE2	(Geometric Sculpture)
                case 168:	// TGE3	(Geometric Sculpture)
                case 170:	// TGE4	(Geometric Sculpture)
                case 171:	// TGE5	(Geometric Sculpture)
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

void S4Importer::FixZ()
{
    // The following code would be useful if we imported sprites
    // for (int i = 0; i < MAX_SPRITES; i++)
    // {
    //     rct_unk_sprite * sprite = &(g_sprite_list[i].unknown);
    //     if (sprite->sprite_identifier == SPRITE_IDENTIFIER_PEEP) {
    //         rct_peep * peep = (rct_peep*)sprite;
    //         peep->next_z /= 2;
    //         RCT2_GLOBAL((int)peep + 0xCE, uint8) /= 2;
    //     }
    // }

    rct_map_element * mapElement = gMapElements;
    while (mapElement < RCT2_GLOBAL(RCT2_ADDRESS_NEXT_FREE_MAP_ELEMENT, rct_map_element*))
    {
        if (mapElement->base_height != 255)
        {
            mapElement->base_height /= 2;
            mapElement->clearance_height /= 2;
        }
        mapElement++;
    }
    RCT2_GLOBAL(0x01359208, uint16) = 7;
}

// rct2: 0x0098BC9F
static const uint8 RCT1PathTypeConversionTable[96] =
{
    0x80 | 0, 0x80 | 1, 0x80 | 2, 0x80 | 3,
    0x80 | 0, 0x80 | 1, 0x80 | 2, 0x80 | 3,
    0x80 | 0, 0x80 | 1, 0x80 | 2, 0x80 | 3,
    0x80 | 0, 0x80 | 1, 0x80 | 2, 0x80 | 3,
    0, 0, 0, 0,
    2, 2, 2, 2,
    1, 1, 1, 1,
    0, 0, 0, 0,
    3, 3, 3, 3,
    6, 6, 6, 6,
    0, 0, 0, 0,
    0, 0, 0, 0,
    5, 5, 5, 5,
    5, 5, 5, 5,
    5, 5, 5, 5,
    5, 5, 5, 5,
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
};

// rct2: 0x0098BCFF
static const uint8 RCT1PathAdditionConversionTable[15] =
{
    0,
    1, 2, 3, 4, 5, 6, 7,
    0x80 | 1, 0x80 | 2, 0x80 | 3, 0x80 | 4, 0x80 | 6, 0x80 | 7,
    8,
};

void S4Importer::FixPaths()
{
    rct_map_element * mapElement = gMapElements;
    while (mapElement < RCT2_GLOBAL(RCT2_ADDRESS_NEXT_FREE_MAP_ELEMENT, rct_map_element*))
    {
        switch (map_element_get_type(mapElement)) {
        case MAP_ELEMENT_TYPE_PATH:
        {
            // Type
            int pathType = ((mapElement->properties.path.type & 0xF0) >> 2) | (mapElement->type & 3);
            int secondaryType = (mapElement->flags & 0x60) >> 5;
            pathType = RCT1PathTypeConversionTable[pathType * 4 + secondaryType];

            mapElement->type &= 0xFC;
            mapElement->flags &= ~0x60;
            mapElement->properties.path.type &= 0x0F;
            footpath_scenery_set_is_ghost(mapElement, false);
            if (pathType & 0x80)
            {
                mapElement->type |= 1;
            }
            mapElement->properties.path.type |= pathType << 4;

            // Additions
            int additions = RCT1PathAdditionConversionTable[footpath_element_get_path_scenery(mapElement)];
            if (footpath_element_path_scenery_is_ghost(mapElement))
            {
                footpath_scenery_set_is_ghost(mapElement, false);
                mapElement->flags |= MAP_ELEMENT_FLAG_BROKEN;
            }
            else
            {
                mapElement->flags &= ~MAP_ELEMENT_FLAG_BROKEN;
            }

            footpath_element_set_path_scenery(mapElement, additions);
            break;
        }
        case MAP_ELEMENT_TYPE_ENTRANCE:
            if (mapElement->properties.entrance.type == ENTRANCE_TYPE_PARK_ENTRANCE)
            {
                int pathType = mapElement->properties.entrance.path_type;
                mapElement->properties.entrance.path_type = RCT1PathTypeConversionTable[pathType * 4] & 0x7F;
            }
            break;
        }
        mapElement++;
    }
}

void S4Importer::FixWalls()
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

void S4Importer::ConvertWall(int * type, int * colourA, int * colourB, int * colourC)
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

void S4Importer::FixBanners()
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

void S4Importer::ImportBanner(rct_banner * dst, rct_banner * src)
{
    *dst = *src;
    dst->colour = RCT1::GetColour(src->colour);

    dst->string_idx = 778;
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

void S4Importer::FixTerrain()
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

void S4Importer::FixEntrancePositions()
{
    for (int i = 0; i < 4; i++)
    {
        RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] = 0x8000;
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

        RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[entranceIndex] = it.x * 32;
        RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, uint16)[entranceIndex] = it.y * 32;
        RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Z, uint16)[entranceIndex] = element->base_height * 8;
        RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_DIRECTION, uint8)[entranceIndex] = element->type & 3;
        entranceIndex++;
    }
}

void S4Importer::FixMapElementEntryTypes()
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
        case MAP_ELEMENT_TYPE_FENCE:
            mapElement->properties.fence.type = _wallTypeToEntryMap[mapElement->properties.fence.type];
            break;
        }
    }
}

const rct1_research_item * S4Importer::GetResearchList(size_t * count)
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

int S4Importer::GetSCNumber()
{
    const utf8 * fileName = Path::GetFileName(_s4Path);
    if (tolower(fileName[0]) == 's' && tolower(fileName[1]) == 'c') {
        constexpr size_t maxDigits = 7;
        utf8 digitBuffer[maxDigits + 1];
        utf8 * dst = digitBuffer;
        const utf8 * src = fileName + 2;
        for (int i = 0; i < maxDigits && *src != '.'; i++)
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

const char * S4Importer::GetUserString(rct_string_id stringId)
{
    return _s4.string_table[(stringId - 0x8000) % 1024];
}

/////////////////////////////////////////
// C -> C++ transfer
/////////////////////////////////////////
extern "C"
bool rct1_load_saved_game(const utf8 * path)
{
    bool result;

    auto s4Importer = new S4Importer();
    try
    {
        s4Importer->LoadSavedGame(path);
        s4Importer->Import();
        result = true;
    }
    catch (Exception ex)
    {
        result = false;
    }
    delete s4Importer;
    
    if (result)
    {
        game_load_init();
    }
    return result;
}

extern "C"
bool rct1_load_scenario(const utf8 * path)
{
    bool result;

    auto s4Importer = new S4Importer();
    try
    {
        s4Importer->LoadSavedGame(path);
        s4Importer->Import();
        result = true;
    }
    catch (Exception ex)
    {
        result = false;
    }
    delete s4Importer;

    if (result)
    {
        scenario_begin();
    }
	return result;
}
