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

#include "../core/Exception.hpp"
#include "../core/IStream.hpp"
#include "../network/network.h"
#include "S6Importer.h"

extern "C"
{
    #include "../config.h"
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
    #include "../rct2.h"
    #include "../ride/ride.h"
    #include "../ride/ride_ratings.h"
    #include "../scenario/scenario.h"
    #include "../util/sawyercoding.h"
    #include "../world/climate.h"
    #include "../world/map_animation.h"
    #include "../world/park.h"
}

class ObjectLoadException : public Exception
{
public:
    ObjectLoadException() : Exception("Unable to load objects.") { }
    explicit ObjectLoadException(const char * message) : Exception(message) { }
};

S6Importer::S6Importer()
{
    FixIssues = false;
    memset(&_s6, 0, sizeof(_s6));
}

void S6Importer::LoadSavedGame(const utf8 * path)
{
    SDL_RWops * rw = SDL_RWFromFile(path, "rb");
    if (rw == nullptr)
    {
        throw IOException("Unable to open SV6.");
    }

    if (!sawyercoding_validate_checksum(rw))
    {
        gErrorType = ERROR_TYPE_FILE_LOAD;
        gGameCommandErrorTitle = STR_FILE_CONTAINS_INVALID_DATA;

        log_error("failed to load saved game, invalid checksum");
        throw IOException("Invalid SV6 checksum.");
    }

    LoadSavedGame(rw);

    SDL_RWclose(rw);

    _s6Path = path;
}

void S6Importer::LoadScenario(const utf8 * path)
{
    SDL_RWops * rw = SDL_RWFromFile(path, "rb");
    if (rw == nullptr)
    {
        throw IOException("Unable to open SV6.");
    }

    if (!gConfigGeneral.allow_loading_with_incorrect_checksum && !sawyercoding_validate_checksum(rw))
    {
        SDL_RWclose(rw);

        gErrorType = ERROR_TYPE_FILE_LOAD;
        gErrorStringId = STR_FILE_CONTAINS_INVALID_DATA;

        log_error("failed to load scenario, invalid checksum");
        throw IOException("Invalid SC6 checksum.");
    }

    LoadScenario(rw);

    SDL_RWclose(rw);

    _s6Path = path;
}

void S6Importer::LoadSavedGame(SDL_RWops *rw)
{
    sawyercoding_read_chunk_safe(rw, &_s6.header, sizeof(_s6.header));
    if (_s6.header.type != S6_TYPE_SAVEDGAME)
    {
        throw Exception("Data is not a saved game.");
    }
    log_verbose("saved game classic_flag = 0x%02x\n", _s6.header.classic_flag);

    // Read packed objects
    // TODO try to contain this more and not store objects until later
    for (uint16 i = 0; i < _s6.header.num_packed_objects; i++)
    {
        object_load_packed(rw);
    }

    sawyercoding_read_chunk_safe(rw, &_s6.objects, sizeof(_s6.objects));
    sawyercoding_read_chunk_safe(rw, &_s6.elapsed_months, 16);
    sawyercoding_read_chunk_safe(rw, &_s6.map_elements, sizeof(_s6.map_elements));
    sawyercoding_read_chunk_safe(rw, &_s6.next_free_map_element_pointer_index, 3048816);
}

void S6Importer::LoadScenario(SDL_RWops *rw)
{
    sawyercoding_read_chunk_safe(rw, &_s6.header, sizeof(_s6.header));
    if (_s6.header.type != S6_TYPE_SCENARIO)
    {
        throw Exception("Data is not a scenario.");
    }
    log_verbose("scenario classic_flag = 0x%02x\n", _s6.header.classic_flag);

    sawyercoding_read_chunk_safe(rw, &_s6.info, sizeof(_s6.info));

    // Read packed objects
    // TODO try to contain this more and not store objects until later
    for (uint16 i = 0; i < _s6.header.num_packed_objects; i++)
    {
        object_load_packed(rw);
    }

    sawyercoding_read_chunk_safe(rw, &_s6.objects, sizeof(_s6.objects));
    sawyercoding_read_chunk_safe(rw, &_s6.elapsed_months, 16);
    sawyercoding_read_chunk_safe(rw, &_s6.map_elements, sizeof(_s6.map_elements));
    sawyercoding_read_chunk_safe(rw, &_s6.next_free_map_element_pointer_index, 2560076);
    sawyercoding_read_chunk_safe(rw, &_s6.guests_in_park, 4);
    sawyercoding_read_chunk_safe(rw, &_s6.last_guests_in_park, 8);
    sawyercoding_read_chunk_safe(rw, &_s6.park_rating, 2);
    sawyercoding_read_chunk_safe(rw, &_s6.active_research_types, 1082);
    sawyercoding_read_chunk_safe(rw, &_s6.current_expenditure, 16);
    sawyercoding_read_chunk_safe(rw, &_s6.park_value, 4);
    sawyercoding_read_chunk_safe(rw, &_s6.completed_company_value, 483816);
}

void S6Importer::Import()
{
    // _s6.header
    gS6Info = _s6.info;

    gDateMonthsElapsed = _s6.elapsed_months;
    gDateMonthTicks = _s6.current_day;
    gScenarioTicks = _s6.scenario_ticks;
    gScenarioSrand0 = _s6.scenario_srand_0;
    gScenarioSrand1 = _s6.scenario_srand_1;

    memcpy(gMapElements, _s6.map_elements, sizeof(_s6.map_elements));

    gNextFreeMapElementPointerIndex = _s6.next_free_map_element_pointer_index;
    for (int i = 0; i < MAX_SPRITES; i++)
    {
        memcpy(get_sprite(i), &_s6.sprites[i], sizeof(rct_sprite));
    }

    for (int i = 0; i < NUM_SPRITE_LISTS; i++)
    {
        gSpriteListHead[i] = _s6.sprite_lists_head[i];
        gSpriteListCount[i] = _s6.sprite_lists_count[i];
    }
    gParkName = _s6.park_name;
    // pad_013573D6
    gParkNameArgs = _s6.park_name_args;
    gInitialCash = _s6.initial_cash;
    gBankLoan = _s6.current_loan;
    gParkFlags = _s6.park_flags;
    gParkEntranceFee = _s6.park_entrance_fee;
    // rct1_park_entrance_x
    // rct1_park_entrance_y
    // pad_013573EE
    // rct1_park_entrance_z
    memcpy(gPeepSpawns, _s6.peep_spawns, sizeof(_s6.peep_spawns));
    gGuestChangeModifier = _s6.guest_count_change_modifier;
    gResearchFundingLevel = _s6.current_research_level;
    // pad_01357400
    memcpy(gResearchedRideTypes, _s6.researched_ride_types, sizeof(_s6.researched_ride_types));
    memcpy(gResearchedRideEntries, _s6.researched_ride_entries, sizeof(_s6.researched_ride_entries));
    memcpy(gResearchedTrackTypesA, _s6.researched_track_types_a, sizeof(_s6.researched_track_types_a));
    memcpy(gResearchedTrackTypesB, _s6.researched_track_types_b, sizeof(_s6.researched_track_types_b));

    gNumGuestsInPark = _s6.guests_in_park;
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

    gResearchPriorities = _s6.active_research_types;
    gResearchProgressStage = _s6.research_progress_stage;
    gResearchLastItemSubject = _s6.last_researched_item_subject;
    // pad_01357CF8
    gResearchNextItem = _s6.next_research_item;
    gResearchProgress = _s6.research_progress;
    gResearchNextCategory = _s6.next_research_category;
    gResearchExpectedDay = _s6.next_research_expected_day;
    gResearchExpectedMonth = _s6.next_research_expected_month;
    gGuestInitialHappiness = _s6.guest_initial_happiness;
    gParkSize = _s6.park_size;
    _guestGenerationProbability = _s6.guest_generation_probability;
    gTotalRideValue = _s6.total_ride_value;
    gMaxBankLoan = _s6.maximum_loan;
    gGuestInitialCash = _s6.guest_initial_cash;
    gGuestInitialHunger = _s6.guest_initial_hunger;
    gGuestInitialThirst = _s6.guest_initial_thirst;
    gScenarioObjectiveType = _s6.objective_type;
    gScenarioObjectiveYear = _s6.objective_year;
    // pad_013580FA
    gScenarioObjectiveCurrency = _s6.objective_currency;
    gScenarioObjectiveNumGuests = _s6.objective_guests;
    memcpy(gMarketingCampaignDaysLeft, _s6.campaign_weeks_left, sizeof(_s6.campaign_weeks_left));
    memcpy(gMarketingCampaignRideIndex, _s6.campaign_ride_index, sizeof(_s6.campaign_ride_index));

    memcpy(gCashHistory, _s6.balance_history, sizeof(_s6.balance_history));

    gCurrentExpenditure = _s6.current_expenditure;
    gCurrentProfit = _s6.current_profit;
    gWeeklyProfitAverageDividend = _s6.weekly_profit_average_dividend;
    gWeeklyProfitAverageDivisor = _s6.weekly_profit_average_divisor;
    // pad_0135833A

    memcpy(gWeeklyProfitHistory, _s6.weekly_profit_history, sizeof(_s6.weekly_profit_history));

    gParkValue = _s6.park_value;

    memcpy(gParkValueHistory, _s6.park_value_history, sizeof(_s6.park_value_history));

    gScenarioCompletedCompanyValue = _s6.completed_company_value;
    gTotalAdmissions = _s6.total_admissions;
    gTotalIncomeFromAdmissions = _s6.income_from_admissions;
    gCompanyValue = _s6.company_value;
    memcpy(gPeepWarningThrottle, _s6.peep_warning_throttle, sizeof(_s6.peep_warning_throttle));
    memcpy(gCurrentAwards, _s6.awards, sizeof(_s6.awards));
    gLandPrice = _s6.land_price;
    gConstructionRightsPrice = _s6.construction_rights_price;
    // unk_01358774
    // pad_01358776
    // _s6.cd_key
    _gameVersion = _s6.game_version_number;
    gScenarioCompanyValueRecord = _s6.completed_company_value_record;
    // _s6.loan_hash;
    gRideCount = _s6.ride_count;
    // pad_013587CA
    gHistoricalProfit = _s6.historical_profit;
    // pad_013587D4
    memcpy(gScenarioCompletedBy, _s6.scenario_completed_name, sizeof(_s6.scenario_completed_name));
    gCashEncrypted = _s6.cash;
    // pad_013587FC
    gParkRatingCasualtyPenalty = _s6.park_rating_casualty_penalty;
    gMapSizeUnits = _s6.map_size_units;
    gMapSizeMinus2 = _s6.map_size_minus_2;
    gMapSize = _s6.map_size;
    gMapSizeMaxXY = _s6.map_max_xy;
    gSamePriceThroughoutParkA = _s6.same_price_throughout;
    _suggestedGuestMaximum = _s6.suggested_max_guests;
    gScenarioParkRatingWarningDays = _s6.park_rating_warning_days;
    gLastEntranceStyle = _s6.last_entrance_style;
    // rct1_water_colour
    // pad_01358842
    memcpy(gResearchItems, _s6.research_items, sizeof(_s6.research_items));
    gMapBaseZ = _s6.map_base_z;
    memcpy(gScenarioName, _s6.scenario_name, sizeof(_s6.scenario_name));
    memcpy(gScenarioDetails, _s6.scenario_description, sizeof(_s6.scenario_description));
    gBankLoanInterestRate = _s6.current_interest_rate;
    // pad_0135934B
    gSamePriceThroughoutParkB = _s6.same_price_throughout_extended;
    memcpy(gParkEntranceX, _s6.park_entrance_x, sizeof(_s6.park_entrance_x));
    memcpy(gParkEntranceY, _s6.park_entrance_y, sizeof(_s6.park_entrance_y));
    memcpy(gParkEntranceZ, _s6.park_entrance_z, sizeof(_s6.park_entrance_z));
    memcpy(gParkEntranceDirection, _s6.park_entrance_direction, sizeof(_s6.park_entrance_direction));
    scenario_set_filename(_s6.scenario_filename);
    memcpy(gScenarioExpansionPacks, _s6.saved_expansion_pack_names, sizeof(_s6.saved_expansion_pack_names));
    memcpy(gBanners, _s6.banners, sizeof(_s6.banners));
    memcpy(gUserStrings, _s6.custom_strings, sizeof(_s6.custom_strings));
    gCurrentTicks = _s6.game_ticks_1;
    memcpy(gRideList, _s6.rides, sizeof(_s6.rides));
    gSavedAge = _s6.saved_age;
    gSavedViewX = _s6.saved_view_x;
    gSavedViewY = _s6.saved_view_y;
    gSavedViewZoom = _s6.saved_view_zoom;
    gSavedViewRotation = _s6.saved_view_rotation;
    memcpy(gAnimatedObjects, _s6.map_animations, sizeof(_s6.map_animations));
    gNumMapAnimations = _s6.num_map_animations;
    // pad_0138B582

    gRideRatingsCalcData = _s6.ride_ratings_calc_data;
    memcpy(gRideMeasurements, _s6.ride_measurements, sizeof(_s6.ride_measurements));
    gNextGuestNumber = _s6.next_guest_index;
    gGrassSceneryTileLoopPosition = _s6.grass_and_scenery_tilepos;
    memcpy(gStaffPatrolAreas, _s6.patrol_areas, sizeof(_s6.patrol_areas));
    memcpy(gStaffModes, _s6.staff_modes, sizeof(_s6.staff_modes));
    // unk_13CA73E
    // pad_13CA73F
    gUnk13CA740 = _s6.byte_13CA740;
    gClimate = _s6.climate;
    // pad_13CA741;
    // byte_13CA742
    // pad_013CA747
    gClimateUpdateTimer = _s6.climate_update_timer;
    gClimateCurrentWeather = _s6.current_weather;
    gClimateNextWeather = _s6.next_weather;
    gClimateCurrentTemperature = _s6.temperature;
    gClimateNextTemperature = _s6.next_temperature;
    gClimateCurrentWeatherEffect = _s6.current_weather_effect;
    gClimateNextWeatherEffect = _s6.next_weather_effect;
    gClimateCurrentWeatherGloom = _s6.current_weather_gloom;
    gClimateNextWeatherGloom = _s6.next_weather_gloom;
    gClimateCurrentRainLevel = _s6.current_rain_level;
    gClimateNextRainLevel = _s6.next_rain_level;
    memcpy(gNewsItems, _s6.news_items, sizeof(_s6.news_items));
    // pad_13CE730
    // rct1_scenario_flags
    gWidePathTileLoopX = _s6.wide_path_tile_loop_x;
    gWidePathTileLoopY = _s6.wide_path_tile_loop_y;
    // pad_13CE778

    // Fix and set dynamic variables
    if (!object_load_entries(_s6.objects))
    {
        throw ObjectLoadException();
    }
    map_update_tile_pointers();
    game_convert_strings_to_utf8();
    map_count_remaining_land_rights();
    if (FixIssues)
    {
        game_fix_save_vars();
    }
}

extern "C"
{
    /**
     *
     *  rct2: 0x00675E1B
     */
    bool game_load_sv6(SDL_RWops * rw)
    {
        if (!sawyercoding_validate_checksum(rw) && !gConfigGeneral.allow_loading_with_incorrect_checksum)
        {
            log_error("invalid checksum");

            gErrorType = ERROR_TYPE_FILE_LOAD;
            gGameCommandErrorTitle = STR_FILE_CONTAINS_INVALID_DATA;
            return 0;
        }

        bool result = false;
        auto s6Importer = new S6Importer();
        try
        {
            s6Importer->FixIssues = true;
            s6Importer->LoadSavedGame(rw);
            s6Importer->Import();

            sprite_position_tween_reset();
            result = true;
        }
        catch (const ObjectLoadException &)
        {
        }
        catch (const Exception &)
        {
        }
        delete s6Importer;

        // #2407: Resetting screen time to not open a save prompt shortly after loading a park.
        gScreenAge = 0;
        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        return result;
    }

    bool game_load_sv6_path(const char * path)
    {
        bool result = false;
        auto s6Importer = new S6Importer();
        try
        {
            s6Importer->FixIssues = true;
            s6Importer->LoadSavedGame(path);
            s6Importer->Import();

            sprite_position_tween_reset();
            result = true;
        }
        catch (const ObjectLoadException &)
        {
            gErrorType = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_GAME_SAVE_FAILED;
        }
        catch (const IOException &)
        {
            gErrorType = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_GAME_SAVE_FAILED;
        }
        catch (const Exception &)
        {
            gErrorType = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_FILE_CONTAINS_INVALID_DATA;
        }
        delete s6Importer;

        gScreenAge = 0;
        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        return result;
    }

    bool scenario_load_rw(SDL_RWops * rw)
    {
        bool result = false;
        auto s6Importer = new S6Importer();
        try
        {
            s6Importer->FixIssues = true;
            s6Importer->LoadScenario(rw);
            s6Importer->Import();

            sprite_position_tween_reset();
            result = true;
        }
        catch (ObjectLoadException)
        {
            gErrorType = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_GAME_SAVE_FAILED;
        }
        catch (IOException)
        {
            gErrorType = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_GAME_SAVE_FAILED;
        }
        catch (Exception)
        {
            gErrorType = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_FILE_CONTAINS_INVALID_DATA;
        }
        delete s6Importer;

        gScreenAge = 0;
        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        return result;
    }

    /**
     *
     *  rct2: 0x00676053
     * scenario (ebx)
     */
    int scenario_load(const char * path)
    {
        bool result = false;
        auto s6Importer = new S6Importer();
        try
        {
            s6Importer->FixIssues = true;
            s6Importer->LoadScenario(path);
            s6Importer->Import();

            sprite_position_tween_reset();
            result = true;
        }
        catch (const ObjectLoadException &)
        {
            gErrorType = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_GAME_SAVE_FAILED;
        }
        catch (const IOException &)
        {
            gErrorType = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_GAME_SAVE_FAILED;
        }
        catch (const Exception &)
        {
            gErrorType = ERROR_TYPE_FILE_LOAD;
            gErrorStringId = STR_FILE_CONTAINS_INVALID_DATA;
        }
        delete s6Importer;

        gScreenAge = 0;
        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        return result;
    }

    int game_load_network(SDL_RWops* rw)
    {
        bool result = false;
        auto s6Importer = new S6Importer();
        try
        {
            s6Importer->LoadSavedGame(rw);
            s6Importer->Import();

            sprite_position_tween_reset();
            result = true;
        }
        catch (const ObjectLoadException &)
        {
        }
        catch (const Exception &)
        {
        }
        delete s6Importer;

        if (!result)
        {
            return 0;
        }

        // Read checksum
        uint32 checksum;
        SDL_RWread(rw, &checksum, sizeof(uint32), 1);

        // Read other data not in normal save files
        SDL_RWread(rw, gSpriteSpatialIndex, 0x10001 * sizeof(uint16), 1);
        gGamePaused = SDL_ReadLE32(rw);
        _guestGenerationProbability = SDL_ReadLE32(rw);
        _suggestedGuestMaximum = SDL_ReadLE32(rw);
        gCheatsSandboxMode = SDL_ReadU8(rw) != 0;
        gCheatsDisableClearanceChecks = SDL_ReadU8(rw) != 0;
        gCheatsDisableSupportLimits = SDL_ReadU8(rw) != 0;
        gCheatsDisableTrainLengthLimit = SDL_ReadU8(rw) != 0;
        gCheatsEnableChainLiftOnAllTrack = SDL_ReadU8(rw) != 0;
        gCheatsShowAllOperatingModes = SDL_ReadU8(rw) != 0;
        gCheatsShowVehiclesFromOtherTrackTypes = SDL_ReadU8(rw) != 0;
        gCheatsFastLiftHill = SDL_ReadU8(rw) != 0;
        gCheatsDisableBrakesFailure = SDL_ReadU8(rw) != 0;
        gCheatsDisableAllBreakdowns = SDL_ReadU8(rw) != 0;
        gCheatsUnlockAllPrices = SDL_ReadU8(rw) != 0;
        gCheatsBuildInPauseMode = SDL_ReadU8(rw) != 0;
        gCheatsIgnoreRideIntensity = SDL_ReadU8(rw) != 0;
        gCheatsDisableVandalism = SDL_ReadU8(rw) != 0;
        gCheatsDisableLittering = SDL_ReadU8(rw) != 0;
        gCheatsNeverendingMarketing = SDL_ReadU8(rw) != 0;
        gCheatsFreezeClimate = SDL_ReadU8(rw) != 0;
        gCheatsDisablePlantAging = SDL_ReadU8(rw) != 0;
        gCheatsAllowArbitraryRideTypeChanges = SDL_ReadU8(rw) != 0;

        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        return 1;
    }
}
