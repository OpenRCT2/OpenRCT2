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
#include "../core/String.hpp"
#include "S6Exporter.h"

extern "C"
{
    #include "../config.h"
    #include "../game.h"
    #include "../interface/viewport.h"
    #include "../interface/window.h"
    #include "../localisation/date.h"
    #include "../localisation/localisation.h"
    #include "../management/finance.h"
    #include "../management/marketing.h"
    #include "../management/news_item.h"
    #include "../management/research.h"
    #include "../object.h"
    #include "../openrct2.h"
    #include "../peep/staff.h"
    #include "../ride/ride.h"
    #include "../ride/ride_ratings.h"
    #include "../scenario.h"
    #include "../util/sawyercoding.h"
    #include "../world/climate.h"
    #include "../world/map_animation.h"
    #include "../world/park.h"
}

S6Exporter::S6Exporter()
{
    ExportObjects = false;
    RemoveTracklessRides = false;
    memset(&_s6, 0, sizeof(_s6));
}

void S6Exporter::SaveGame(const utf8 * path)
{
    SDL_RWops * rw = SDL_RWFromFile(path, "rb");
    if (rw == nullptr)
    {
        throw IOException("Unable to write to destination file.");
    }

    SaveGame(rw);

    SDL_RWclose(rw);
}

void S6Exporter::SaveGame(SDL_RWops *rw)
{
    Save(rw, false);
}

void S6Exporter::SaveScenario(const utf8 * path)
{
    SDL_RWops * rw = SDL_RWFromFile(path, "rb");
    if (rw == nullptr)
    {
        throw IOException("Unable to write to destination file.");
    }

    SaveGame(rw);

    SDL_RWclose(rw);
}

void S6Exporter::SaveScenario(SDL_RWops *rw)
{
    Save(rw, true);
}

void S6Exporter::Save(SDL_RWops * rw, bool isScenario)
{
    _s6.header.type = isScenario ? S6_TYPE_SCENARIO : S6_TYPE_SAVEDGAME;
    _s6.header.num_packed_objects = ExportObjects ? scenario_get_num_packed_objects_to_write() : 0;
    _s6.header.version = S6_RCT2_VERSION;
    _s6.header.magic_number = S6_MAGIC_NUMBER;

    _s6.game_version_number = 201028;

    uint8 * buffer = (uint8 *)malloc(0x600000);
    if (buffer == NULL)
    {
        log_error("Unable to allocate enough space for a write buffer.");
        throw Exception("Unable to allocate memory.");
    }
    
    sawyercoding_chunk_header chunkHeader;
    size_t encodedLength;

    // 0: Write header chunk
    chunkHeader.encoding = CHUNK_ENCODING_ROTATE;
    chunkHeader.length = sizeof(rct_s6_header);
    encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.header, chunkHeader);
    SDL_RWwrite(rw, buffer, encodedLength, 1);

    // 1: Write scenario info chunk
    if (_s6.header.type == S6_TYPE_SCENARIO)
    {
        chunkHeader.encoding = CHUNK_ENCODING_ROTATE;
        chunkHeader.length = sizeof(rct_s6_info);
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.info, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);
    }

    // 2: Write packed objects
    if (_s6.header.num_packed_objects > 0)
    {
        if (!scenario_write_packed_objects(rw))
        {
            free(buffer);
            throw Exception("Unable to pack objects.");
        }
    }

    // 3: Write available objects chunk
    chunkHeader.encoding = CHUNK_ENCODING_ROTATE;
    chunkHeader.length = OBJECT_ENTRY_COUNT * sizeof(rct_object_entry);
    encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)_s6.objects, chunkHeader);
    SDL_RWwrite(rw, buffer, encodedLength, 1);

    // 4: Misc fields (data, rand...) chunk
    chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
    chunkHeader.length = 16;
    encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.elapsed_months, chunkHeader);
    SDL_RWwrite(rw, buffer, encodedLength, 1);

    // 5: Map elements + sprites and other fields chunk
    chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
    chunkHeader.length = 0x180000;
    encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)_s6.map_elements, chunkHeader);
    SDL_RWwrite(rw, buffer, encodedLength, 1);

    if (_s6.header.type == S6_TYPE_SCENARIO)
    {
        // 6:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 0x27104C;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.next_free_map_element_pointer_index, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 7:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 4;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.guests_in_park, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 8:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 8;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.last_guests_in_park, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 9:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 2;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.park_rating, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 10:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 1082;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.active_research_types, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 11:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 16;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.current_expenditure, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 12:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 4;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.park_value, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 13:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 0x761E8;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.completed_company_value, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);
    }
    else
    {
        // 6: Everything else...
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 0x2E8570;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.next_free_map_element_pointer_index, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);
    }

    free(buffer);
    
    // Determine number of bytes written
    size_t fileSize = (size_t)SDL_RWtell(rw);
    SDL_RWseek(rw, 0, RW_SEEK_SET);

    // Read all written bytes back into a single buffer
    buffer = (uint8 *)malloc(fileSize);
    SDL_RWread(rw, buffer, fileSize, 1);
    uint32 checksum = sawyercoding_calculate_checksum(buffer, fileSize);
    free(buffer);

    // Append the checksum
    SDL_RWseek(rw, fileSize, RW_SEEK_SET);
    SDL_RWwrite(rw, &checksum, sizeof(uint32), 1);
}

void S6Exporter::Export()
{
    _s6.info = gS6Info;

    for (int i = 0; i < OBJECT_ENTRY_COUNT; i++)
    {
        const rct_object_entry * entry = get_loaded_object_entry(i);
        void * entryData = get_loaded_object_chunk(i);
        if (entryData == (void *)-1)
        {
            Memory::Set(&_s6.objects[i], 0xFF, sizeof(rct_object_entry));
        }
        else
        {
            _s6.objects[i] = *((rct_object_entry*)entry);
        }
    }

    _s6.elapsed_months = gDateMonthsElapsed;
    _s6.current_day = gDateMonthTicks;
    _s6.scenario_ticks = gScenarioTicks;
    _s6.scenario_srand_0 = gScenarioSrand0;
    _s6.scenario_srand_1 = gScenarioSrand1;

    memcpy(_s6.map_elements, gMapElements, sizeof(_s6.map_elements));

    _s6.next_free_map_element_pointer_index = gNextFreeMapElementPointerIndex;
    for (int i = 0; i < MAX_SPRITES; i++)
    {
        memcpy(&_s6.sprites[i], get_sprite(i), sizeof(rct_sprite));
    }

    for (int i = 0; i < NUM_SPRITE_LISTS; i++)
    {
        _s6.sprite_lists_head[i] = gSpriteListHead[i];
        _s6.sprite_lists_count[i] = gSpriteListCount[i];
    }
    _s6.park_name = gParkName;
    // pad_013573D6
    _s6.park_name_args = gParkNameArgs;
    _s6.initial_cash = gInitialCash;
    _s6.current_loan = gBankLoan;
    _s6.park_flags = gParkFlags;
    _s6.park_entrance_fee = gParkEntranceFee;
    // rct1_park_entrance_x
    // rct1_park_entrance_y
    // pad_013573EE
    // rct1_park_entrance_z
    memcpy(_s6.peep_spawns, gPeepSpawns, sizeof(_s6.peep_spawns));
    _s6.guest_count_change_modifier = gGuestChangeModifier;
    _s6.current_research_level = gResearchFundingLevel;
    // pad_01357400
    memcpy(_s6.researched_ride_types, gResearchedRideTypes, sizeof(_s6.researched_ride_types));
    memcpy(_s6.researched_ride_entries, gResearchedRideEntries, sizeof(_s6.researched_ride_entries));
    memcpy(_s6.researched_track_types_a, gResearchedTrackTypesA, sizeof(_s6.researched_track_types_a));
    memcpy(_s6.researched_track_types_b, gResearchedTrackTypesB, sizeof(_s6.researched_track_types_b));

    _s6.guests_in_park = gNumGuestsInPark;
    _s6.guests_heading_for_park = gNumGuestsHeadingForPark;

    memcpy(_s6.expenditure_table, gExpenditureTable, sizeof(_s6.expenditure_table));

    _s6.last_guests_in_park = gNumGuestsInParkLastWeek;
    // pad_01357BCA
    _s6.handyman_colour = gStaffHandymanColour;
    _s6.mechanic_colour = gStaffMechanicColour;
    _s6.security_colour = gStaffSecurityColour;

    memcpy(_s6.researched_scenery_items, gResearchedSceneryItems, sizeof(_s6.researched_scenery_items));

    _s6.park_rating = gParkRating;

    memcpy(_s6.park_rating_history, gParkRatingHistory, sizeof(_s6.park_rating_history));
    memcpy(_s6.guests_in_park_history, gGuestsInParkHistory, sizeof(_s6.guests_in_park_history));

    _s6.active_research_types = gResearchPriorities;
    _s6.research_progress_stage = gResearchProgressStage;
    _s6.last_researched_item_subject = gResearchLastItemSubject;
    // pad_01357CF8
    _s6.next_research_item = gResearchNextItem;
    _s6.research_progress = gResearchProgress;
    _s6.next_research_category = gResearchNextCategory;
    _s6.next_research_expected_day = gResearchExpectedDay;
    _s6.next_research_expected_month = gResearchExpectedMonth;
    _s6.guest_initial_happiness = gGuestInitialHappiness;
    _s6.park_size = gParkSize;
    _s6.guest_generation_probability = _guestGenerationProbability;
    _s6.total_ride_value = gTotalRideValue;
    _s6.maximum_loan = gMaxBankLoan;
    _s6.guest_initial_cash = gGuestInitialCash;
    _s6.guest_initial_hunger = gGuestInitialHunger;
    _s6.guest_initial_thirst = gGuestInitialThirst;
    _s6.objective_type = gScenarioObjectiveType;
    _s6.objective_year = gScenarioObjectiveYear;
    // pad_013580FA
    _s6.objective_currency = gScenarioObjectiveCurrency;
    _s6.objective_guests = gScenarioObjectiveNumGuests;
    memcpy(_s6.campaign_weeks_left, gMarketingCampaignDaysLeft, sizeof(_s6.campaign_weeks_left));
    memcpy(_s6.campaign_ride_index, gMarketingCampaignRideIndex, sizeof(_s6.campaign_ride_index));

    memcpy(_s6.balance_history, gCashHistory, sizeof(_s6.balance_history));

    _s6.current_expenditure = gCurrentExpenditure;
    _s6.current_profit = gCurrentProfit;
    _s6.weekly_profit_average_dividend = gWeeklyProfitAverageDividend;
	_s6.weekly_profit_average_divisor = gWeeklyProfitAverageDivisor;
    // pad_0135833A

    memcpy(_s6.weekly_profit_history, gWeeklyProfitHistory, sizeof(_s6.weekly_profit_history));

    _s6.park_value = gParkValue;

    memcpy(_s6.park_value_history, gParkValueHistory, sizeof(_s6.park_value_history));

    _s6.completed_company_value = gScenarioCompletedCompanyValue;
    _s6.total_admissions = gTotalAdmissions;
    _s6.income_from_admissions = gTotalIncomeFromAdmissions;
    _s6.company_value = gCompanyValue;
    memcpy(_s6.peep_warning_throttle, gPeepWarningThrottle, sizeof(_s6.peep_warning_throttle));
    memcpy(_s6.awards, gCurrentAwards, sizeof(_s6.awards));
    _s6.land_price = gLandPrice;
    _s6.construction_rights_price = gConstructionRightsPrice;
    // unk_01358774
    // pad_01358776
    // _s6.cd_key
    // _s6.game_version_number
    _s6.completed_company_value_record = gScenarioCompanyValueRecord;
    _s6.loan_hash = GetLoanHash(gInitialCash, gBankLoan, gMaxBankLoan);
    _s6.ride_count = gRideCount;
    // pad_013587CA
    _s6.historical_profit = gHistoricalProfit;
    // pad_013587D4
    memcpy(_s6.scenario_completed_name, gScenarioCompletedBy, sizeof(_s6.scenario_completed_name));
    _s6.cash = gCashEncrypted;
    // pad_013587FC
    _s6.park_rating_casualty_penalty = gParkRatingCasualtyPenalty;
    _s6.map_size_units = gMapSizeUnits;
    _s6.map_size_minus_2 = gMapSizeMinus2;
    _s6.map_size = gMapSize;
    _s6.map_max_xy = gMapSizeMaxXY;
    _s6.same_price_throughout = gSamePriceThroughoutParkA;
    _s6.suggested_max_guests = _suggestedGuestMaximum;
    _s6.park_rating_warning_days = gScenarioParkRatingWarningDays;
    _s6.last_entrance_style = gLastEntranceStyle;
    // rct1_water_colour
    // pad_01358842
    memcpy(_s6.research_items, gResearchItems, sizeof(_s6.research_items));
    _s6.map_base_z = gMapBaseZ;
    memcpy(_s6.scenario_name, gScenarioName, sizeof(_s6.scenario_name));
    memcpy(_s6.scenario_description, gScenarioDetails, sizeof(_s6.scenario_description));
    _s6.current_interest_rate = gBankLoanInterestRate;
    // pad_0135934B
    _s6.same_price_throughout_extended = gSamePriceThroughoutParkB;
    memcpy(_s6.park_entrance_x, gParkEntranceX, sizeof(_s6.park_entrance_x));
    memcpy(_s6.park_entrance_y, gParkEntranceY, sizeof(_s6.park_entrance_y));
    memcpy(_s6.park_entrance_z, gParkEntranceZ, sizeof(_s6.park_entrance_z));
    memcpy(_s6.park_entrance_direction, gParkEntranceDirection, sizeof(_s6.park_entrance_direction));
    strncpy(_s6.scenario_filename, _scenarioFileName, sizeof(_s6.scenario_filename) - 1);
    memcpy(_s6.saved_expansion_pack_names, gScenarioExpansionPacks, sizeof(_s6.saved_expansion_pack_names));
    memcpy(_s6.banners, gBanners, sizeof(_s6.banners));
    memcpy(_s6.custom_strings, gUserStrings, sizeof(_s6.custom_strings));
    _s6.game_ticks_1 = gCurrentTicks;
    memcpy(_s6.rides, gRideList, sizeof(_s6.rides));
    _s6.saved_age = gSavedAge;
    _s6.saved_view_x = gSavedViewX;
    _s6.saved_view_y = gSavedViewY;
    _s6.saved_view_zoom = gSavedViewZoom;
    _s6.saved_view_rotation = gSavedViewRotation;
    memcpy(_s6.map_animations, gAnimatedObjects, sizeof(_s6.map_animations));
    _s6.num_map_animations = gNumMapAnimations;
    // pad_0138B582

    _s6.ride_ratings_calc_data = gRideRatingsCalcData;
    memcpy(_s6.ride_measurements, gRideMeasurements, sizeof(_s6.ride_measurements));
    _s6.next_guest_index = gNextGuestNumber;
    _s6.grass_and_scenery_tilepos = gGrassSceneryTileLoopPosition;
    memcpy(_s6.patrol_areas, gStaffPatrolAreas, sizeof(_s6.patrol_areas));
    memcpy(_s6.staff_modes, gStaffModes, sizeof(_s6.staff_modes));
    // unk_13CA73E
    // pad_13CA73F
    _s6.byte_13CA740 = gUnk13CA740;
    _s6.climate = gClimate;
    // pad_13CA741;
    // byte_13CA742
    // pad_013CA747
    _s6.climate_update_timer = gClimateUpdateTimer;
    _s6.current_weather = gClimateCurrentWeather;
    _s6.next_weather = gClimateNextWeather;
    _s6.temperature = gClimateCurrentTemperature;
    _s6.next_temperature = gClimateNextTemperature;
    _s6.current_weather_effect = gClimateCurrentWeatherEffect;
    _s6.next_weather_effect = gClimateNextWeatherEffect;
    _s6.current_weather_gloom = gClimateCurrentWeatherGloom;
    _s6.next_weather_gloom = gClimateNextWeatherGloom;
    _s6.current_rain_level = gClimateCurrentRainLevel;
    _s6.next_rain_level = gClimateNextRainLevel;
    memcpy(_s6.news_items, gNewsItems, sizeof(_s6.news_items));
    // pad_13CE730
    // rct1_scenario_flags
    _s6.wide_path_tile_loop_x = gWidePathTileLoopX;
    _s6.wide_path_tile_loop_y = gWidePathTileLoopY;
    // pad_13CE778

    String::Set(_s6.scenario_filename, sizeof(_s6.scenario_filename), _scenarioFileName);

    if (RemoveTracklessRides)
    {
        scenario_remove_trackless_rides(&_s6);
    }

    scenario_fix_ghosts(&_s6);
    game_convert_strings_to_rct2(&_s6);
}

uint32 S6Exporter::GetLoanHash(money32 initialCash, money32 bankLoan, uint32 maxBankLoan)
{
    sint32 value = 0x70093A;
    value -= initialCash;
    value = ror32(value, 5);
    value -= bankLoan;
    value = ror32(value, 7);
    value += maxBankLoan;
    value = ror32(value, 3);
    return value;
}

extern "C"
{
    enum {
        S6_SAVE_FLAG_EXPORT    = 1 << 0,
        S6_SAVE_FLAG_SCENARIO  = 1 << 1,
        S6_SAVE_FLAG_AUTOMATIC = 1 << 31,
    };

    /**
     *
     *  rct2: 0x006754F5
     * @param flags bit 0: pack objects, 1: save as scenario
     */
    int scenario_save(SDL_RWops* rw, int flags)
    {
        if (flags & S6_SAVE_FLAG_SCENARIO)
        {
            log_verbose("saving scenario");
        }
        else
        {
            log_verbose("saving game");
        }

        if (!(flags & S6_SAVE_FLAG_AUTOMATIC))
        {
            window_close_construction_windows();
        }

        map_reorganise_elements();
        sprite_clear_all_unused();

        viewport_set_saved_view();

        bool result = false;
        auto s6exporter = new S6Exporter();
        try
        {
            s6exporter->ExportObjects = (flags & S6_SAVE_FLAG_EXPORT);
            s6exporter->RemoveTracklessRides = true;
            s6exporter->Export();
            if (flags & S6_SAVE_FLAG_SCENARIO)
            {
                s6exporter->SaveScenario(rw);
            }
            else
            {
                s6exporter->SaveGame(rw);
            }
            result = true;
        }
        catch (Exception)
        {
        }
        delete s6exporter;

        gfx_invalidate_screen();

        if (result && !(flags & S6_SAVE_FLAG_AUTOMATIC))
        {
            gScreenAge = 0;
        }
        return result;
    }

    // Save game state without modifying any of the state for multiplayer
    int scenario_save_network(SDL_RWops * rw)
    {
        viewport_set_saved_view();

        bool result = false;
        auto s6exporter = new S6Exporter();
        try
        {
            s6exporter->ExportObjects = true;
            s6exporter->Export();
            s6exporter->SaveGame(rw);
            result = true;
        }
        catch (Exception)
        {
        }
        delete s6exporter;

        if (!result)
        {
            return 0;
        }

        // Write other data not in normal save files
        SDL_RWwrite(rw, gSpriteSpatialIndex, 0x10001 * sizeof(uint16), 1);
        SDL_WriteLE32(rw, gGamePaused);
        SDL_WriteLE32(rw, _guestGenerationProbability);
        SDL_WriteLE32(rw, _suggestedGuestMaximum);
        SDL_WriteU8(rw, gCheatsSandboxMode);
        SDL_WriteU8(rw, gCheatsDisableClearanceChecks);
        SDL_WriteU8(rw, gCheatsDisableSupportLimits);
        SDL_WriteU8(rw, gCheatsDisableTrainLengthLimit);
        SDL_WriteU8(rw, gCheatsEnableChainLiftOnAllTrack);
        SDL_WriteU8(rw, gCheatsShowAllOperatingModes);
        SDL_WriteU8(rw, gCheatsShowVehiclesFromOtherTrackTypes);
        SDL_WriteU8(rw, gCheatsFastLiftHill);
        SDL_WriteU8(rw, gCheatsDisableBrakesFailure);
        SDL_WriteU8(rw, gCheatsDisableAllBreakdowns);
        SDL_WriteU8(rw, gCheatsUnlockAllPrices);
        SDL_WriteU8(rw, gCheatsBuildInPauseMode);
        SDL_WriteU8(rw, gCheatsIgnoreRideIntensity);
        SDL_WriteU8(rw, gCheatsDisableVandalism);
        SDL_WriteU8(rw, gCheatsDisableLittering);
        SDL_WriteU8(rw, gCheatsNeverendingMarketing);
        SDL_WriteU8(rw, gCheatsFreezeClimate);
        SDL_WriteU8(rw, gCheatsDisablePlantAging);
        SDL_WriteU8(rw, gCheatsAllowArbitraryRideTypeChanges);

        gfx_invalidate_screen();
        return 1;
    }
}
