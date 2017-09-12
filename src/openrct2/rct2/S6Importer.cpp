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

#include "../core/Console.hpp"
#include "../core/Exception.hpp"
#include "../core/FileStream.hpp"
#include "../core/IStream.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../management/award.h"
#include "../network/network.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../object/ObjectManager.h"
#include "../ParkImporter.h"
#include "../rct12/SawyerChunkReader.h"
#include "../rct12/SawyerEncoding.h"
#include "../ride/station.h"

extern "C"
{
    #include "../config/Config.h"
    #include "../game.h"
    #include "../interface/viewport.h"
    #include "../localisation/date.h"
    #include "../localisation/localisation.h"
    #include "../management/finance.h"
    #include "../management/marketing.h"
    #include "../management/news_item.h"
    #include "../management/research.h"
    #include "../OpenRCT2.h"
    #include "../peep/staff.h"
    #include "../ride/ride.h"
    #include "../ride/ride_ratings.h"
    #include "../scenario/scenario.h"
    #include "../util/sawyercoding.h"
    #include "../world/Climate.h"
    #include "../world/entrance.h"
    #include "../world/map_animation.h"
    #include "../world/park.h"
}

class ObjectLoadException : public Exception
{
public:
    ObjectLoadException() : Exception("Unable to load objects.") { }
    explicit ObjectLoadException(const char * message) : Exception(message) { }
};

/**
 * Class to import RollerCoaster Tycoon 2 scenarios (*.SC6) and saved games (*.SV6).
 */
class S6Importer final : public IParkImporter
{
private:
    IObjectRepository * const   _objectRepository;
    IObjectManager * const      _objectManager;

    const utf8 *    _s6Path = nullptr;
    rct_s6_data     _s6;
    uint8           _gameVersion = 0;

public:
    S6Importer(IObjectRepository * objectRepository, IObjectManager * objectManager)
        : _objectRepository(objectRepository),
          _objectManager(objectManager)
    {
        Memory::Set(&_s6, 0, sizeof(_s6));
    }

    ParkLoadResult Load(const utf8 * path) override
    {
        const utf8 * extension = Path::GetExtension(path);
        if (String::Equals(extension, ".sc6", true))
        {
            return LoadScenario(path);
        }
        else if (String::Equals(extension, ".sv6", true))
        {
            return LoadSavedGame(path);
        }
        else
        {
            throw Exception("Invalid RCT2 park extension.");
        }
    }

    ParkLoadResult LoadSavedGame(const utf8 * path, bool skipObjectCheck = false) override
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        auto result = LoadFromStream(&fs, false, skipObjectCheck);
        _s6Path = path;
        return result;
    }

    ParkLoadResult LoadScenario(const utf8 * path, bool skipObjectCheck = false) override
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        auto result = LoadFromStream(&fs, true, skipObjectCheck);
        _s6Path = path;
        return result;
    }

    ParkLoadResult LoadFromStream(IStream * stream, bool isScenario, bool skipObjectCheck = false) override
    {
        if (isScenario && !gConfigGeneral.allow_loading_with_incorrect_checksum && !SawyerEncoding::ValidateChecksum(stream))
        {
            throw IOException("Invalid checksum.");
        }

        auto chunkReader = SawyerChunkReader(stream);
        chunkReader.ReadChunk(&_s6.header, sizeof(_s6.header));

        log_verbose("saved game classic_flag = 0x%02x\n", _s6.header.classic_flag);
        if (isScenario)
        {
            if (_s6.header.type != S6_TYPE_SCENARIO)
            {
                throw Exception("Park is not a scenario.");
            }
            chunkReader.ReadChunk(&_s6.info, sizeof(_s6.info));
        }
        else
        {
            if (_s6.header.type != S6_TYPE_SAVEDGAME)
            {
                throw Exception("Park is not a saved game.");
            }
        }

        // Read packed objects
        // TODO try to contain this more and not store objects until later
        for (uint16 i = 0; i < _s6.header.num_packed_objects; i++)
        {
            _objectRepository->ExportPackedObject(stream);
        }

        if (isScenario)
        {
            chunkReader.ReadChunk(&_s6.objects, sizeof(_s6.objects));
            chunkReader.ReadChunk(&_s6.elapsed_months, 16);
            chunkReader.ReadChunk(&_s6.map_elements, sizeof(_s6.map_elements));
            chunkReader.ReadChunk(&_s6.next_free_map_element_pointer_index, 2560076);
            chunkReader.ReadChunk(&_s6.guests_in_park, 4);
            chunkReader.ReadChunk(&_s6.last_guests_in_park, 8);
            chunkReader.ReadChunk(&_s6.park_rating, 2);
            chunkReader.ReadChunk(&_s6.active_research_types, 1082);
            chunkReader.ReadChunk(&_s6.current_expenditure, 16);
            chunkReader.ReadChunk(&_s6.park_value, 4);
            chunkReader.ReadChunk(&_s6.completed_company_value, 483816);
        }
        else
        {
            chunkReader.ReadChunk(&_s6.objects, sizeof(_s6.objects));
            chunkReader.ReadChunk(&_s6.elapsed_months, 16);
            chunkReader.ReadChunk(&_s6.map_elements, sizeof(_s6.map_elements));
            chunkReader.ReadChunk(&_s6.next_free_map_element_pointer_index, 3048816);
        }

        auto missingObjects = _objectManager->GetInvalidObjects(_s6.objects);
        if (missingObjects.size() > 0)
        {
            return ParkLoadResult::CreateMissingObjects(missingObjects);
        }
        return ParkLoadResult::CreateOK();
    }

    bool GetDetails(scenario_index_entry * dst) override
    {
        Memory::Set(dst, 0, sizeof(scenario_index_entry));
        return false;
    }

    void Import() override
    {
        Initialise();

        // _s6.header
        gS6Info = _s6.info;

        gDateMonthsElapsed = _s6.elapsed_months;
        gDateMonthTicks    = _s6.current_day;
        gScenarioTicks     = _s6.scenario_ticks;
        gScenarioSrand0    = _s6.scenario_srand_0;
        gScenarioSrand1    = _s6.scenario_srand_1;

        memcpy(gMapElements, _s6.map_elements, sizeof(_s6.map_elements));

        gNextFreeMapElementPointerIndex = _s6.next_free_map_element_pointer_index;
        for (sint32 i = 0; i < MAX_SPRITES; i++)
        {
            memcpy(get_sprite(i), &_s6.sprites[i], sizeof(rct_sprite));
        }

        for (sint32 i = 0; i < NUM_SPRITE_LISTS; i++)
        {
            gSpriteListHead[i]  = _s6.sprite_lists_head[i];
            gSpriteListCount[i] = _s6.sprite_lists_count[i];
        }
        gParkName = _s6.park_name;
        // pad_013573D6
        gParkNameArgs    = _s6.park_name_args;
        gInitialCash     = _s6.initial_cash;
        gBankLoan        = _s6.current_loan;
        gParkFlags       = _s6.park_flags;
        gParkEntranceFee = _s6.park_entrance_fee;
        // Force RCT2 scenarios to Unlock All Prices being false
        gCheatsUnlockAllPrices = false;
        // rct1_park_entrance_x
        // rct1_park_entrance_y
        // pad_013573EE
        // rct1_park_entrance_z
        memcpy(gPeepSpawns, _s6.peep_spawns, sizeof(_s6.peep_spawns));
        gGuestChangeModifier  = _s6.guest_count_change_modifier;
        gResearchFundingLevel = _s6.current_research_level;
        // pad_01357400
        memcpy(gResearchedRideTypes, _s6.researched_ride_types, sizeof(_s6.researched_ride_types));
        memcpy(gResearchedRideEntries, _s6.researched_ride_entries, sizeof(_s6.researched_ride_entries));
        // _s6.researched_track_types_a
        // _s6.researched_track_types_b

        gNumGuestsInPark         = _s6.guests_in_park;
        gNumGuestsHeadingForPark = _s6.guests_heading_for_park;

        memcpy(gExpenditureTable, _s6.expenditure_table, sizeof(_s6.expenditure_table));

        gNumGuestsInParkLastWeek = _s6.last_guests_in_park;
        // pad_01357BCA
        gStaffHandymanColour = _s6.handyman_colour;
        gStaffMechanicColour = _s6.mechanic_colour;
        gStaffSecurityColour = _s6.security_colour;

        memcpy(gResearchedSceneryItems, _s6.researched_scenery_items, sizeof(_s6.researched_scenery_items));

        gParkRating = _s6.park_rating;

        memcpy(gParkRatingHistory, _s6.park_rating_history, sizeof(_s6.park_rating_history));
        memcpy(gGuestsInParkHistory, _s6.guests_in_park_history, sizeof(_s6.guests_in_park_history));

        gResearchPriorities      = _s6.active_research_types;
        gResearchProgressStage   = _s6.research_progress_stage;
        gResearchLastItemSubject = _s6.last_researched_item_subject;
        // pad_01357CF8
        gResearchNextItem           = _s6.next_research_item;
        gResearchProgress           = _s6.research_progress;
        gResearchNextCategory       = _s6.next_research_category;
        gResearchExpectedDay        = _s6.next_research_expected_day;
        gResearchExpectedMonth      = _s6.next_research_expected_month;
        gGuestInitialHappiness      = _s6.guest_initial_happiness;
        gParkSize                   = _s6.park_size;
        _guestGenerationProbability = _s6.guest_generation_probability;
        gTotalRideValue             = _s6.total_ride_value;
        gMaxBankLoan                = _s6.maximum_loan;
        gGuestInitialCash           = _s6.guest_initial_cash;
        gGuestInitialHunger         = _s6.guest_initial_hunger;
        gGuestInitialThirst         = _s6.guest_initial_thirst;
        gScenarioObjectiveType      = _s6.objective_type;
        gScenarioObjectiveYear      = _s6.objective_year;
        // pad_013580FA
        gScenarioObjectiveCurrency  = _s6.objective_currency;
        gScenarioObjectiveNumGuests = _s6.objective_guests;
        memcpy(gMarketingCampaignDaysLeft, _s6.campaign_weeks_left, sizeof(_s6.campaign_weeks_left));
        memcpy(gMarketingCampaignRideIndex, _s6.campaign_ride_index, sizeof(_s6.campaign_ride_index));

        memcpy(gCashHistory, _s6.balance_history, sizeof(_s6.balance_history));

        gCurrentExpenditure          = _s6.current_expenditure;
        gCurrentProfit               = _s6.current_profit;
        gWeeklyProfitAverageDividend = _s6.weekly_profit_average_dividend;
        gWeeklyProfitAverageDivisor  = _s6.weekly_profit_average_divisor;
        // pad_0135833A

        memcpy(gWeeklyProfitHistory, _s6.weekly_profit_history, sizeof(_s6.weekly_profit_history));

        gParkValue = _s6.park_value;

        memcpy(gParkValueHistory, _s6.park_value_history, sizeof(_s6.park_value_history));

        gScenarioCompletedCompanyValue = _s6.completed_company_value;
        gTotalAdmissions               = _s6.total_admissions;
        gTotalIncomeFromAdmissions     = _s6.income_from_admissions;
        gCompanyValue                  = _s6.company_value;
        memcpy(gPeepWarningThrottle, _s6.peep_warning_throttle, sizeof(_s6.peep_warning_throttle));

        // Awards
        for (sint32 i = 0; i < RCT12_MAX_AWARDS; i++)
        {
            rct12_award * src = &_s6.awards[i];
            Award *       dst = &gCurrentAwards[i];
            dst->Time         = src->time;
            dst->Type         = src->type;
        }

        gLandPrice               = _s6.land_price;
        gConstructionRightsPrice = _s6.construction_rights_price;
        // unk_01358774
        // pad_01358776
        // _s6.cd_key
        _gameVersion                = _s6.game_version_number;
        gScenarioCompanyValueRecord = _s6.completed_company_value_record;
        // _s6.loan_hash;
        gRideCount = _s6.ride_count;
        // pad_013587CA
        gHistoricalProfit = _s6.historical_profit;
        // pad_013587D4
        memcpy(gScenarioCompletedBy, _s6.scenario_completed_name, sizeof(_s6.scenario_completed_name));
        gCashEncrypted = _s6.cash;
        // pad_013587FC
        gParkRatingCasualtyPenalty     = _s6.park_rating_casualty_penalty;
        gMapSizeUnits                  = _s6.map_size_units;
        gMapSizeMinus2                 = _s6.map_size_minus_2;
        gMapSize                       = _s6.map_size;
        gMapSizeMaxXY                  = _s6.map_max_xy;
        gSamePriceThroughoutParkA      = _s6.same_price_throughout;
        _suggestedGuestMaximum         = _s6.suggested_max_guests;
        gScenarioParkRatingWarningDays = _s6.park_rating_warning_days;
        gLastEntranceStyle             = _s6.last_entrance_style;
        // rct1_water_colour
        // pad_01358842
        memcpy(gResearchItems, _s6.research_items, sizeof(_s6.research_items));
        gMapBaseZ = _s6.map_base_z;
        memcpy(gScenarioName, _s6.scenario_name, sizeof(_s6.scenario_name));
        memcpy(gScenarioDetails, _s6.scenario_description, sizeof(_s6.scenario_description));
        gBankLoanInterestRate = _s6.current_interest_rate;
        // pad_0135934B
        gSamePriceThroughoutParkB = _s6.same_price_throughout_extended;
        // Preserve compatibility with vanilla RCT2's save format.
        for (uint8 i = 0; i < RCT12_MAX_PARK_ENTRANCES; i++)
        {
            gParkEntrances[i].x         = _s6.park_entrance_x[i];
            gParkEntrances[i].y         = _s6.park_entrance_y[i];
            gParkEntrances[i].z         = _s6.park_entrance_z[i];
            gParkEntrances[i].direction = _s6.park_entrance_direction[i];
        }
        String::Set(gScenarioFileName, sizeof(gScenarioFileName), _s6.scenario_filename);
        memcpy(gScenarioExpansionPacks, _s6.saved_expansion_pack_names, sizeof(_s6.saved_expansion_pack_names));
        memcpy(gBanners, _s6.banners, sizeof(_s6.banners));
        memcpy(gUserStrings, _s6.custom_strings, sizeof(_s6.custom_strings));
        gCurrentTicks = _s6.game_ticks_1;

        ImportRides();

        gSavedAge          = _s6.saved_age;
        gSavedViewX        = _s6.saved_view_x;
        gSavedViewY        = _s6.saved_view_y;
        gSavedViewZoom     = _s6.saved_view_zoom;
        gSavedViewRotation = _s6.saved_view_rotation;
        memcpy(gAnimatedObjects, _s6.map_animations, sizeof(_s6.map_animations));
        gNumMapAnimations = _s6.num_map_animations;
        // pad_0138B582

        gRideRatingsCalcData = _s6.ride_ratings_calc_data;
        memcpy(gRideMeasurements, _s6.ride_measurements, sizeof(_s6.ride_measurements));
        gNextGuestNumber              = _s6.next_guest_index;
        gGrassSceneryTileLoopPosition = _s6.grass_and_scenery_tilepos;
        memcpy(gStaffPatrolAreas, _s6.patrol_areas, sizeof(_s6.patrol_areas));
        memcpy(gStaffModes, _s6.staff_modes, sizeof(_s6.staff_modes));
        // unk_13CA73E
        // pad_13CA73F
        gUnk13CA740 = _s6.byte_13CA740;
        gClimate    = _s6.climate;
        // pad_13CA741;
        // byte_13CA742
        // pad_013CA747
        gClimateUpdateTimer          = _s6.climate_update_timer;
        gClimateCurrentWeather       = _s6.current_weather;
        gClimateNextWeather          = _s6.next_weather;
        gClimateCurrentTemperature   = _s6.temperature;
        gClimateNextTemperature      = _s6.next_temperature;
        gClimateCurrentWeatherEffect = _s6.current_weather_effect;
        gClimateNextWeatherEffect    = _s6.next_weather_effect;
        gClimateCurrentWeatherGloom  = _s6.current_weather_gloom;
        gClimateNextWeatherGloom     = _s6.next_weather_gloom;
        gClimateCurrentRainLevel     = _s6.current_rain_level;
        gClimateNextRainLevel        = _s6.next_rain_level;

        // News items
        for (size_t i = 0; i < RCT12_MAX_NEWS_ITEMS; i++)
        {
            const rct12_news_item * src = &_s6.news_items[i];
            NewsItem *              dst = &gNewsItems[i];

            dst->Type      = src->Type;
            dst->Flags     = src->Flags;
            dst->Assoc     = src->Assoc;
            dst->Ticks     = src->Ticks;
            dst->MonthYear = src->MonthYear;
            dst->Day       = src->Day;
            memcpy(dst->Text, src->Text, sizeof(src->Text));
        }

        // pad_13CE730
        // rct1_scenario_flags
        gWidePathTileLoopX = _s6.wide_path_tile_loop_x;
        gWidePathTileLoopY = _s6.wide_path_tile_loop_y;
        // pad_13CE778

        // Fix and set dynamic variables
        if (!_objectManager->LoadObjects(_s6.objects, OBJECT_ENTRY_COUNT))
        {
            throw ObjectLoadException();
        }
        map_strip_ghost_flag_from_elements();
        map_update_tile_pointers();
        game_convert_strings_to_utf8();
        map_count_remaining_land_rights();

        // We try to fix the cycles on import, hence the 'true' parameter
        check_for_sprite_list_cycles(true);
        check_for_spatial_index_cycles(true);
        sint32 disjoint_sprites_count = fix_disjoint_sprites();
        // This one is less harmful, no need to assert for it ~janisozaur
        if (disjoint_sprites_count > 0)
        {
            log_error("Found %d disjoint null sprites", disjoint_sprites_count);
        }
    }

    void ImportRides()
    {
        for (uint16 index = 0; index < RCT2_MAX_RIDES_IN_PARK; index++)
        {
            if (_s6.rides[index].type != RIDE_TYPE_NULL)
            {
                ImportRide(get_ride(index), &_s6.rides[index]);
            }
        }
    }

    void ImportRide(Ride * dst, const rct2_ride * src)
    {
        memset(dst, 0, sizeof(Ride));

        dst->type = src->type;
        dst->subtype = src->subtype;
        // pad_002;
        dst->mode = src->mode;
        dst->colour_scheme_type = src->colour_scheme_type;

        for (uint8 i = 0; i < RCT2_MAX_CARS_PER_TRAIN; i ++)
        {
            dst->vehicle_colours[i] = src->vehicle_colours[i];
        }

        // pad_046;
        dst->status = src->status;
        dst->name = src->name;
        dst->name_arguments = src->name_arguments;

        dst->overall_view = src->overall_view;

        for (sint32 i = 0; i < RCT12_MAX_STATIONS_PER_RIDE; i++)
        {
            dst->station_starts[i] = src->station_starts[i];
            dst->station_heights[i] = src->station_heights[i];
            dst->station_length[i] = src->station_length[i];
            dst->station_depart[i] = src->station_depart[i];
            dst->train_at_station[i] = src->train_at_station[i];
            dst->entrances[i] = src->entrances[i];
            dst->exits[i] = src->exits[i];
            dst->last_peep_in_queue[i] = src->last_peep_in_queue[i];

            dst->length[i] = src->length[i];
            dst->time[i] = src->time[i];

            dst->queue_time[i] = src->queue_time[i];

            dst->queue_length[i] = src->queue_length[i];
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

        for (uint8 i = 0; i < MAX_VEHICLES_PER_RIDE; i++)
        {
            dst->vehicles[i] = src->vehicles[i];
        }

        dst->depart_flags = src->depart_flags;

        dst->num_stations = src->num_stations;
        dst->num_vehicles = src->num_vehicles;
        dst->num_cars_per_train = src->num_cars_per_train;
        dst->proposed_num_vehicles = src->proposed_num_vehicles;
        dst->proposed_num_cars_per_train = src->proposed_num_cars_per_train;
        dst->max_trains = src->max_trains;
        dst->min_max_cars_per_train = src->min_max_cars_per_train;
        dst->min_waiting_time = src->min_waiting_time;
        dst->max_waiting_time = src->max_waiting_time;

        // Includes time_limit, num_laps, launch_speed, speed, rotations
        dst->operation_option = src->operation_option;

        dst->boat_hire_return_direction = src->boat_hire_return_direction;
        dst->boat_hire_return_position = src->boat_hire_return_position;

        dst->measurement_index = src->measurement_index;

        dst->special_track_elements = src->special_track_elements;
        // pad_0D6[2];

        dst->max_speed = src->max_speed;
        dst->average_speed = src->average_speed;
        dst->current_test_segment = src->current_test_segment;
        dst->average_speed_test_timeout = src->average_speed_test_timeout;
        // pad_0E2[0x2];

        dst->max_positive_vertical_g = src->max_positive_vertical_g;
        dst->max_negative_vertical_g = src->max_negative_vertical_g;
        dst->max_lateral_g = src->max_lateral_g;
        dst->previous_vertical_g = src->previous_vertical_g;
        dst->previous_lateral_g = src->previous_lateral_g;
        // pad_106[0x2];
        dst->testing_flags = src->testing_flags;
        dst->cur_test_track_location = src->cur_test_track_location;
        dst->turn_count_default = src->turn_count_default;
        dst->turn_count_banked = src->turn_count_banked;
        dst->turn_count_sloped = src->turn_count_sloped;
        // Includes holes and (for some strange reason?!) sheltered_eights
        dst->inversions = src->inversions;
        dst->drops = src->drops;
        dst->start_drop_height = src->start_drop_height;
        dst->highest_drop_height = src->highest_drop_height;
        dst->sheltered_length = src->sheltered_length;
        dst->var_11C = src->var_11C;
        dst->num_sheltered_sections = src->num_sheltered_sections;
        dst->cur_test_track_z = src->cur_test_track_z;

        dst->cur_num_customers = src->cur_num_customers;
        dst->num_customers_timeout = src->num_customers_timeout;

        for (uint8 i = 0; i < RCT2_CUSTOMER_HISTORY_SIZE; i++)
        {
            dst->num_customers[i] = src->num_customers[i];
        }

        dst->price = src->price;

        for (uint8 i = 0; i < 2; i++)
        {
            dst->chairlift_bullwheel_location[i] = src->chairlift_bullwheel_location[i];
            dst->chairlift_bullwheel_z[i] = src->chairlift_bullwheel_z[i];
        }

        dst->ratings = src->ratings;
        dst->value = src->value;

        dst->chairlift_bullwheel_rotation = src->chairlift_bullwheel_rotation;

        dst->satisfaction = src->satisfaction;
        dst->satisfaction_time_out = src->satisfaction_time_out;
        dst->satisfaction_next = src->satisfaction_next;

        dst->window_invalidate_flags = src->window_invalidate_flags;
        // pad_14E[0x02];

        dst->total_customers = src->total_customers;
        dst->total_profit = src->total_profit;
        dst->popularity = src->popularity;
        dst->popularity_time_out = src->popularity_time_out;
        dst->popularity_next = src->popularity_next;
        dst->num_riders = src->num_riders;
        dst->music_tune_id = src->music_tune_id;
        dst->slide_in_use = src->slide_in_use;
        // Includes maze_tiles
        dst->slide_peep = src->slide_peep;
        // pad_160[0xE];
        dst->slide_peep_t_shirt_colour = src->slide_peep_t_shirt_colour;
        // pad_16F[0x7];
        dst->spiral_slide_progress = src->spiral_slide_progress;
        // pad_177[0x9];
        dst->build_date = src->build_date;
        dst->upkeep_cost = src->upkeep_cost;
        dst->race_winner = src->race_winner;
        // pad_186[0x02];
        dst->music_position = src->music_position;

        dst->breakdown_reason_pending = src->breakdown_reason_pending;
        dst->mechanic_status = src->mechanic_status;
        dst->mechanic = src->mechanic;
        dst->inspection_station = src->inspection_station;
        dst->broken_vehicle = src->broken_vehicle;
        dst->broken_car = src->broken_car;
        dst->breakdown_reason = src->breakdown_reason;

        dst->price_secondary = src->price_secondary;

        dst->reliability = src->reliability;
        dst->unreliability_factor = src->unreliability_factor;
        dst->downtime = src->downtime;
        dst->inspection_interval = src->inspection_interval;
        dst->last_inspection = src->last_inspection;

        for (uint8 i = 0; i < RCT2_DOWNTIME_HISTORY_SIZE; i++)
        {
            dst->downtime_history[i] = src->downtime_history[i];
        }

        dst->no_primary_items_sold = src->no_primary_items_sold;
        dst->no_secondary_items_sold = src->no_secondary_items_sold;

        dst->breakdown_sound_modifier = src->breakdown_sound_modifier;
        dst->not_fixed_timeout = src->not_fixed_timeout;
        dst->last_crash_type = src->last_crash_type;
        dst->connected_message_throttle = src->connected_message_throttle;

        dst->income_per_hour = src->income_per_hour;
        dst->profit = src->profit;

        for (uint8 i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++)
        {
            dst->track_colour_main[i] = src->track_colour_main[i];
            dst->track_colour_additional[i] = src->track_colour_additional[i];
            dst->track_colour_supports[i] = src->track_colour_supports[i];
        }

        dst->music = src->music;
        dst->entrance_style = src->entrance_style;
        dst->vehicle_change_timeout = src->vehicle_change_timeout;
        dst->num_block_brakes = src->num_block_brakes;
        dst->lift_hill_speed = src->lift_hill_speed;
        dst->guests_favourite = src->guests_favourite;
        dst->lifecycle_flags = src->lifecycle_flags;

        for (uint8 i = 0; i < RCT2_MAX_CARS_PER_TRAIN; i++)
        {
            dst->vehicle_colours_extended[i] = src->vehicle_colours_extended[i];
        }

        dst->total_air_time = src->total_air_time;
        dst->current_test_station = src->current_test_station;
        dst->num_circuits = src->num_circuits;
        dst->cable_lift_x = src->cable_lift_x;
        dst->cable_lift_y = src->cable_lift_y;
        dst->cable_lift_z = src->cable_lift_z;
        // pad_1FD;
        dst->cable_lift = src->cable_lift;

        // pad_208[0x58];
    }

    void Initialise()
    {
        game_init_all(_s6.map_size);
    }
};

IParkImporter * ParkImporter::CreateS6(IObjectRepository * objectRepository, IObjectManager * objectManager)
{
    return new S6Importer(objectRepository, objectManager);
}

extern "C"
{
    ParkLoadResult * load_from_sv6(const char * path)
    {
        ParkLoadResult * result = nullptr;
        auto s6Importer = new S6Importer(GetObjectRepository(), GetObjectManager());
        try
        {
            result = new ParkLoadResult(s6Importer->LoadSavedGame(path));

            // We mustn't import if there's something
            // wrong with the park data
            if (result->Error == PARK_LOAD_ERROR_OK)
            {
                s6Importer->Import();

                game_fix_save_vars();
                sprite_position_tween_reset();
            }
        }
        catch (const ObjectLoadException &)
        {
            gErrorType     = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_FILE_CONTAINS_INVALID_DATA;
        }
        catch (const IOException &)
        {
            gErrorType     = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_GAME_SAVE_FAILED;
        }
        catch (const Exception &)
        {
            gErrorType     = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_FILE_CONTAINS_INVALID_DATA;
        }
        delete s6Importer;

        if (result == nullptr)
        {
            result = new ParkLoadResult(ParkLoadResult::CreateUnknown());
        }
        if (result->Error == PARK_LOAD_ERROR_OK)
        {
            gScreenAge          = 0;
            gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        }
        return result;
    }

    /**
     *
     *  rct2: 0x00676053
     * scenario (ebx)
     */
    ParkLoadResult * load_from_sc6(const char * path)
    {
        ParkLoadResult * result = nullptr;
        auto s6Importer = new S6Importer(GetObjectRepository(), GetObjectManager());
        try
        {
            result = new ParkLoadResult(s6Importer->LoadScenario(path));
            if (result->Error == PARK_LOAD_ERROR_OK)
            {
                s6Importer->Import();

                game_fix_save_vars();
                sprite_position_tween_reset();
            }
        }
        catch (const ObjectLoadException &)
        {
            gErrorType     = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_GAME_SAVE_FAILED;
        }
        catch (const IOException &)
        {
            gErrorType     = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_GAME_SAVE_FAILED;
        }
        catch (const Exception &)
        {
            gErrorType     = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_FILE_CONTAINS_INVALID_DATA;
        }
        delete s6Importer;

        if (result == nullptr)
        {
            result = new ParkLoadResult(ParkLoadResult::CreateUnknown());
        }
        if (result->Error != PARK_LOAD_ERROR_OK)
        {
            gScreenAge = 0;
            gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        }
        return result;
    }
}
