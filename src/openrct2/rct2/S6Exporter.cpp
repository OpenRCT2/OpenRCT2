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

#include <algorithm>
#include <cstring>
#include "../Context.h"
#include "../core/FileStream.hpp"
#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "../management/Award.h"
#include "../object/Object.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../rct12/SawyerChunkWriter.h"
#include "S6Exporter.h"
#include <functional>

#include "../config/Config.h"
#include "../Game.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../management/Research.h"
#include "../object/Object.h"
#include "../object/ObjectLimits.h"
#include "../OpenRCT2.h"
#include "../peep/Staff.h"
#include "../ride/Ride.h"
#include "../ride/RideRatings.h"
#include "../ride/TrackData.h"
#include "../scenario/Scenario.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "../ride/Station.h"

S6Exporter::S6Exporter()
{
    RemoveTracklessRides = false;
    memset(&_s6, 0, sizeof(_s6));
}

void S6Exporter::SaveGame(const utf8 * path)
{
    auto fs = FileStream(path, FILE_MODE_WRITE);
    SaveGame(&fs);
}

void S6Exporter::SaveGame(IStream * stream)
{
    Save(stream, false);
}

void S6Exporter::SaveScenario(const utf8 * path)
{
    auto fs = FileStream(path, FILE_MODE_WRITE);
    SaveScenario(&fs);
}

void S6Exporter::SaveScenario(IStream * stream)
{
    Save(stream, true);
}

void S6Exporter::Save(IStream * stream, bool isScenario)
{
    _s6.header.type               = isScenario ? S6_TYPE_SCENARIO : S6_TYPE_SAVEDGAME;
    _s6.header.classic_flag       = 0;
    _s6.header.num_packed_objects = uint16(ExportObjectsList.size());
    _s6.header.version            = S6_RCT2_VERSION;
    _s6.header.magic_number       = S6_MAGIC_NUMBER;
    _s6.game_version_number       = 201028;

    auto chunkWriter = SawyerChunkWriter(stream);

    // 0: Write header chunk
    chunkWriter.WriteChunk(&_s6.header, SAWYER_ENCODING::ROTATE);

    // 1: Write scenario info chunk
    if (_s6.header.type == S6_TYPE_SCENARIO)
    {
        chunkWriter.WriteChunk(&_s6.info, SAWYER_ENCODING::ROTATE);
    }

    // 2: Write packed objects
    if (_s6.header.num_packed_objects > 0)
    {
        IObjectRepository * objRepo = OpenRCT2::GetContext()->GetObjectRepository();
        objRepo->WritePackedObjects(stream, ExportObjectsList);
    }

    // 3: Write available objects chunk
    chunkWriter.WriteChunk(_s6.objects, sizeof(_s6.objects), SAWYER_ENCODING::ROTATE);

    // 4: Misc fields (data, rand...) chunk
    chunkWriter.WriteChunk(&_s6.elapsed_months, 16, SAWYER_ENCODING::RLECOMPRESSED);

    // 5: Map elements + sprites and other fields chunk
    chunkWriter.WriteChunk(&_s6.tile_elements, 0x180000, SAWYER_ENCODING::RLECOMPRESSED);

    if (_s6.header.type == S6_TYPE_SCENARIO)
    {
        // 6 to 13:
        chunkWriter.WriteChunk(&_s6.next_free_tile_element_pointer_index, 0x27104C, SAWYER_ENCODING::RLECOMPRESSED);
        chunkWriter.WriteChunk(&_s6.guests_in_park, 4, SAWYER_ENCODING::RLECOMPRESSED);
        chunkWriter.WriteChunk(&_s6.last_guests_in_park, 8, SAWYER_ENCODING::RLECOMPRESSED);
        chunkWriter.WriteChunk(&_s6.park_rating, 2, SAWYER_ENCODING::RLECOMPRESSED);
        chunkWriter.WriteChunk(&_s6.active_research_types, 1082, SAWYER_ENCODING::RLECOMPRESSED);
        chunkWriter.WriteChunk(&_s6.current_expenditure, 16, SAWYER_ENCODING::RLECOMPRESSED);
        chunkWriter.WriteChunk(&_s6.park_value, 4, SAWYER_ENCODING::RLECOMPRESSED);
        chunkWriter.WriteChunk(&_s6.completed_company_value, 0x761E8, SAWYER_ENCODING::RLECOMPRESSED);
    }
    else
    {
        // 6: Everything else...
        chunkWriter.WriteChunk(&_s6.next_free_tile_element_pointer_index, 0x2E8570, SAWYER_ENCODING::RLECOMPRESSED);
    }

    // Determine number of bytes written
    size_t fileSize = stream->GetLength();

    // Read all written bytes back into a single buffer
    stream->SetPosition(0);
    auto   data = std::unique_ptr<uint8, std::function<void(uint8 *)>>(stream->ReadArray<uint8>(fileSize), Memory::Free<uint8>);
    uint32 checksum = sawyercoding_calculate_checksum(data.get(), fileSize);

    // Write the checksum on the end
    stream->SetPosition(fileSize);
    stream->WriteValue(checksum);
}

void S6Exporter::Export()
{
    sint32 spatial_cycle = check_for_spatial_index_cycles(false);
    sint32 regular_cycle = check_for_sprite_list_cycles(false);
    sint32 disjoint_sprites_count = fix_disjoint_sprites();
    openrct2_assert(spatial_cycle == -1, "Sprite cycle exists in spatial list %d", spatial_cycle);
    openrct2_assert(regular_cycle == -1, "Sprite cycle exists in regular list %d", regular_cycle);
    // This one is less harmful, no need to assert for it ~janisozaur
    if (disjoint_sprites_count > 0)
    {
        log_error("Found %d disjoint null sprites", disjoint_sprites_count);
    }
    _s6.info = gS6Info;
    {
        auto temp = utf8_to_rct2(gS6Info.name);
        safe_strcpy(_s6.info.name, temp.data(), sizeof(_s6.info.name));
    }
    {
        auto temp = utf8_to_rct2(gS6Info.details);
        safe_strcpy(_s6.info.details, temp.data(), sizeof(_s6.info.details));
    }
    uint32 researchedTrackPiecesA[128];
    uint32 researchedTrackPiecesB[128];

    for (sint32 i = 0; i < OBJECT_ENTRY_COUNT; i++)
    {
        const rct_object_entry * entry     = get_loaded_object_entry(i);
        void *                   entryData = get_loaded_object_chunk(i);
        // RCT2 uses (void *)-1 to mark NULL. Make sure it's written in a vanilla-compatible way.
        if (entryData == nullptr || entryData == (void *)-1)
        {
            std::memset(&_s6.objects[i], 0xFF, sizeof(rct_object_entry));
        }
        else
        {
            _s6.objects[i] = *entry;
        }
    }

    _s6.elapsed_months   = gDateMonthsElapsed;
    _s6.current_day      = gDateMonthTicks;
    _s6.scenario_ticks   = gScenarioTicks;
    _s6.scenario_srand_0 = gScenarioSrand0;
    _s6.scenario_srand_1 = gScenarioSrand1;

    memcpy(_s6.tile_elements, gTileElements, sizeof(_s6.tile_elements));

    _s6.next_free_tile_element_pointer_index = gNextFreeTileElementPointerIndex;
    // Sprites needs to be reset before they get used.
    // Might as well reset them in here to zero out the space and improve
    // compression ratios. Especially useful for multiplayer servers that
    // use zlib on the sent stream.
    sprite_clear_all_unused();
    for (sint32 i = 0; i < RCT2_MAX_SPRITES; i++)
    {
        memcpy(&_s6.sprites[i], get_sprite(i), sizeof(rct_sprite));
    }

    for (sint32 i = 0; i < NUM_SPRITE_LISTS; i++)
    {
        _s6.sprite_lists_head[i]  = gSpriteListHead[i];
        _s6.sprite_lists_count[i] = gSpriteListCount[i];
    }
    _s6.park_name = gParkName;
    // pad_013573D6
    _s6.park_name_args    = gParkNameArgs;
    _s6.initial_cash      = gInitialCash;
    _s6.current_loan      = gBankLoan;
    _s6.park_flags        = gParkFlags;
    _s6.park_entrance_fee = gParkEntranceFee;
    // rct1_park_entrance_x
    // rct1_park_entrance_y
    // pad_013573EE
    // rct1_park_entrance_z
    ExportPeepSpawns();
    _s6.guest_count_change_modifier = gGuestChangeModifier;
    _s6.current_research_level      = gResearchFundingLevel;
    // pad_01357400
    ExportResearchedRideTypes();
    ExportResearchedRideEntries();
    // Not used by OpenRCT2 any more, but left in to keep RCT2 export working.
    for (uint8 i = 0; i < Util::CountOf(RideTypePossibleTrackConfigurations); i++)
    {
        researchedTrackPiecesA[i] = (RideTypePossibleTrackConfigurations[i]         ) & 0xFFFFFFFFULL;
        researchedTrackPiecesB[i] = (RideTypePossibleTrackConfigurations[i] >> 32ULL) & 0xFFFFFFFFULL;
    }
    memcpy(_s6.researched_track_types_a, researchedTrackPiecesA, sizeof(_s6.researched_track_types_a));
    memcpy(_s6.researched_track_types_b, researchedTrackPiecesB, sizeof(_s6.researched_track_types_b));

    _s6.guests_in_park          = gNumGuestsInPark;
    _s6.guests_heading_for_park = gNumGuestsHeadingForPark;

    memcpy(_s6.expenditure_table, gExpenditureTable, sizeof(_s6.expenditure_table));

    _s6.last_guests_in_park = gNumGuestsInParkLastWeek;
    // pad_01357BCA
    _s6.handyman_colour = gStaffHandymanColour;
    _s6.mechanic_colour = gStaffMechanicColour;
    _s6.security_colour = gStaffSecurityColour;

    ExportResearchedSceneryItems();

    _s6.park_rating = gParkRating;

    memcpy(_s6.park_rating_history, gParkRatingHistory, sizeof(_s6.park_rating_history));
    memcpy(_s6.guests_in_park_history, gGuestsInParkHistory, sizeof(_s6.guests_in_park_history));

    _s6.active_research_types        = gResearchPriorities;
    _s6.research_progress_stage      = gResearchProgressStage;
    _s6.last_researched_item_subject = gResearchLastItem.rawValue;
    // pad_01357CF8
    _s6.next_research_item           = gResearchNextItem.rawValue;
    _s6.research_progress            = gResearchProgress;
    _s6.next_research_category       = gResearchNextItem.category;
    _s6.next_research_expected_day   = gResearchExpectedDay;
    _s6.next_research_expected_month = gResearchExpectedMonth;
    _s6.guest_initial_happiness      = gGuestInitialHappiness;
    _s6.park_size                    = gParkSize;
    _s6.guest_generation_probability = _guestGenerationProbability;
    _s6.total_ride_value_for_money   = gTotalRideValueForMoney;
    _s6.maximum_loan                 = gMaxBankLoan;
    _s6.guest_initial_cash           = gGuestInitialCash;
    _s6.guest_initial_hunger         = gGuestInitialHunger;
    _s6.guest_initial_thirst         = gGuestInitialThirst;
    _s6.objective_type               = gScenarioObjectiveType;
    _s6.objective_year               = gScenarioObjectiveYear;
    // pad_013580FA
    _s6.objective_currency = gScenarioObjectiveCurrency;
    _s6.objective_guests   = gScenarioObjectiveNumGuests;
    memcpy(_s6.campaign_weeks_left, gMarketingCampaignDaysLeft, sizeof(_s6.campaign_weeks_left));
    memcpy(_s6.campaign_ride_index, gMarketingCampaignRideIndex, sizeof(_s6.campaign_ride_index));

    memcpy(_s6.balance_history, gCashHistory, sizeof(_s6.balance_history));

    _s6.current_expenditure            = gCurrentExpenditure;
    _s6.current_profit                 = gCurrentProfit;
    _s6.weekly_profit_average_dividend = gWeeklyProfitAverageDividend;
    _s6.weekly_profit_average_divisor  = gWeeklyProfitAverageDivisor;
    // pad_0135833A

    memcpy(_s6.weekly_profit_history, gWeeklyProfitHistory, sizeof(_s6.weekly_profit_history));

    _s6.park_value = gParkValue;

    memcpy(_s6.park_value_history, gParkValueHistory, sizeof(_s6.park_value_history));

    _s6.completed_company_value = gScenarioCompletedCompanyValue;
    _s6.total_admissions        = gTotalAdmissions;
    _s6.income_from_admissions  = gTotalIncomeFromAdmissions;
    _s6.company_value           = gCompanyValue;
    memcpy(_s6.peep_warning_throttle, gPeepWarningThrottle, sizeof(_s6.peep_warning_throttle));

    // Awards
    for (sint32 i = 0; i < RCT12_MAX_AWARDS; i++)
    {
        Award *       src = &gCurrentAwards[i];
        rct12_award * dst = &_s6.awards[i];
        dst->time         = src->Time;
        dst->type         = src->Type;
    }

    _s6.land_price                = gLandPrice;
    _s6.construction_rights_price = gConstructionRightsPrice;
    // unk_01358774
    // pad_01358776
    // _s6.cd_key
    // _s6.game_version_number
    _s6.completed_company_value_record = gScenarioCompanyValueRecord;
    _s6.loan_hash                      = GetLoanHash(gInitialCash, gBankLoan, gMaxBankLoan);
    _s6.ride_count                     = gRideCount;
    // pad_013587CA
    _s6.historical_profit = gHistoricalProfit;
    // pad_013587D4
    memcpy(_s6.scenario_completed_name, gScenarioCompletedBy, sizeof(_s6.scenario_completed_name));
    _s6.cash = ENCRYPT_MONEY(gCash);
    // pad_013587FC
    _s6.park_rating_casualty_penalty = gParkRatingCasualtyPenalty;
    _s6.map_size_units               = gMapSizeUnits;
    _s6.map_size_minus_2             = gMapSizeMinus2;
    _s6.map_size                     = gMapSize;
    _s6.map_max_xy                   = gMapSizeMaxXY;
    _s6.same_price_throughout        = gSamePriceThroughoutParkA;
    _s6.suggested_max_guests         = _suggestedGuestMaximum;
    _s6.park_rating_warning_days     = gScenarioParkRatingWarningDays;
    _s6.last_entrance_style          = gLastEntranceStyle;
    // rct1_water_colour
    // pad_01358842
    ExportResearchList();
    _s6.map_base_z = gMapBaseZ;
    memcpy(_s6.scenario_name, gScenarioName, sizeof(_s6.scenario_name));
    memcpy(_s6.scenario_description, gScenarioDetails, sizeof(_s6.scenario_description));
    _s6.current_interest_rate = gBankLoanInterestRate;
    // pad_0135934B
    _s6.same_price_throughout_extended = gSamePriceThroughoutParkB;
    // Preserve compatibility with vanilla RCT2's save format.
    for (uint8 i = 0; i < RCT12_MAX_PARK_ENTRANCES; i++)
    {
        _s6.park_entrance_x[i]         = gParkEntrances[i].x;
        _s6.park_entrance_y[i]         = gParkEntrances[i].y;
        _s6.park_entrance_z[i]         = gParkEntrances[i].z;
        _s6.park_entrance_direction[i] = gParkEntrances[i].direction;
    }
    safe_strcpy(_s6.scenario_filename, gScenarioFileName, sizeof(_s6.scenario_filename));
    memcpy(_s6.saved_expansion_pack_names, gScenarioExpansionPacks, sizeof(_s6.saved_expansion_pack_names));
    memcpy(_s6.banners, gBanners, sizeof(_s6.banners));
    memcpy(_s6.custom_strings, gUserStrings, sizeof(_s6.custom_strings));
    _s6.game_ticks_1 = gCurrentTicks;

    this->ExportRides();

    _s6.saved_age           = gSavedAge;
    _s6.saved_view_x        = gSavedViewX;
    _s6.saved_view_y        = gSavedViewY;
    _s6.saved_view_zoom     = gSavedViewZoom;
    _s6.saved_view_rotation = gSavedViewRotation;
    memcpy(_s6.map_animations, gAnimatedObjects, sizeof(_s6.map_animations));
    _s6.num_map_animations = gNumMapAnimations;
    // pad_0138B582

    _s6.ride_ratings_calc_data = gRideRatingsCalcData;
    memcpy(_s6.ride_measurements, gRideMeasurements, sizeof(_s6.ride_measurements));
    _s6.next_guest_index          = gNextGuestNumber;
    _s6.grass_and_scenery_tilepos = gGrassSceneryTileLoopPosition;
    memcpy(_s6.patrol_areas, gStaffPatrolAreas, sizeof(_s6.patrol_areas));
    memcpy(_s6.staff_modes, gStaffModes, sizeof(_s6.staff_modes));
    // unk_13CA73E
    // pad_13CA73F
    _s6.byte_13CA740 = gUnk13CA740;
    _s6.climate      = gClimate;
    // pad_13CA741;
    // byte_13CA742
    // pad_013CA747
    _s6.climate_update_timer   = gClimateUpdateTimer;
    _s6.current_weather        = gClimateCurrent.Weather;
    _s6.next_weather           = gClimateNext.Weather;
    _s6.temperature            = gClimateCurrent.Temperature;
    _s6.next_temperature       = gClimateNext.Temperature;
    _s6.current_weather_effect = gClimateCurrent.WeatherEffect;
    _s6.next_weather_effect    = gClimateNext.WeatherEffect;
    _s6.current_weather_gloom  = gClimateCurrent.WeatherGloom;
    _s6.next_weather_gloom     = gClimateNext.WeatherGloom;
    _s6.current_rain_level     = gClimateCurrent.RainLevel;
    _s6.next_rain_level        = gClimateNext.RainLevel;

    // News items
    for (size_t i = 0; i < RCT12_MAX_NEWS_ITEMS; i++)
    {
        const NewsItem *  src = &gNewsItems[i];
        rct12_news_item * dst = &_s6.news_items[i];

        dst->Type      = src->Type;
        dst->Flags     = src->Flags;
        dst->Assoc     = src->Assoc;
        dst->Ticks     = src->Ticks;
        dst->MonthYear = src->MonthYear;
        dst->Day       = src->Day;
        memcpy(dst->Text, src->Text, sizeof(dst->Text));
    }

    // pad_13CE730
    // rct1_scenario_flags
    _s6.wide_path_tile_loop_x = gWidePathTileLoopX;
    _s6.wide_path_tile_loop_y = gWidePathTileLoopY;
    // pad_13CE778

    String::Set(_s6.scenario_filename, sizeof(_s6.scenario_filename), gScenarioFileName);

    if (RemoveTracklessRides)
    {
        scenario_remove_trackless_rides(&_s6);
    }

    scenario_fix_ghosts(&_s6);
    game_convert_strings_to_rct2(&_s6);
}

void S6Exporter::ExportPeepSpawns()
{
    for (size_t i = 0; i < RCT12_MAX_PEEP_SPAWNS; i++)
    {
        _s6.peep_spawns[i] = {
            (uint16)gPeepSpawns[i].x, (uint16)gPeepSpawns[i].y, (uint8)(gPeepSpawns[i].z / 16), gPeepSpawns[i].direction
        };
    }
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

void S6Exporter::ExportRides()
{
    for (sint32 index = 0; index < RCT12_MAX_RIDES_IN_PARK; index++)
    {
        auto src = get_ride(index);
        auto dst = &_s6.rides[index];
        *dst = { 0 };
        if (src->type == RIDE_TYPE_NULL)
        {
            dst->type = RIDE_TYPE_NULL;
        }
        else
        {
            ExportRide(dst, src);
        }
    }
}

void S6Exporter::ExportRide(rct2_ride * dst, const Ride * src)
{
    memset(dst, 0, sizeof(rct2_ride));

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

        TileCoordsXYZD entrance = ride_get_entrance_location(src, i);
        if (entrance.isNull())
            dst->entrances[i].xy = RCT_XY8_UNDEFINED;
        else
            dst->entrances[i] = { (uint8)entrance.x, (uint8)entrance.y };

        TileCoordsXYZD exit = ride_get_exit_location(src, i);
        if (exit.isNull())
            dst->exits[i].xy = RCT_XY8_UNDEFINED;
        else
            dst->exits[i] = { (uint8)exit.x, (uint8)exit.y };

        dst->last_peep_in_queue[i] = src->last_peep_in_queue[i];

        dst->length[i] = src->length[i];
        dst->time[i] = src->time[i];

        dst->queue_time[i] = src->queue_time[i];

        dst->queue_length[i] = src->queue_length[i];
    }

    for (uint8 i = 0; i < RCT2_MAX_VEHICLES_PER_RIDE; i++)
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

void S6Exporter::ExportResearchedRideTypes()
{
    std::fill(
        std::begin(_s6.researched_ride_types),
        std::end(_s6.researched_ride_types),
        false);

    for (sint32 rideType = 0; rideType < RIDE_TYPE_COUNT; rideType++)
    {
        if (ride_type_is_invented(rideType))
        {
            sint32 quadIndex = rideType >> 5;
            sint32 bitIndex  = rideType & 0x1F;
            _s6.researched_ride_types[quadIndex] |= (uint32) 1 << bitIndex;
        }
    }
}

void S6Exporter::ExportResearchedRideEntries()
{
    std::fill(
        std::begin(_s6.researched_ride_entries),
        std::end(_s6.researched_ride_entries),
        false);

    for (sint32 rideEntryIndex = 0; rideEntryIndex < MAX_RIDE_OBJECTS; rideEntryIndex++)
    {
        if (ride_entry_is_invented(rideEntryIndex))
        {
            sint32 quadIndex = rideEntryIndex >> 5;
            sint32 bitIndex  = rideEntryIndex & 0x1F;
            _s6.researched_ride_entries[quadIndex] |= (uint32) 1 << bitIndex;
        }
    }
}

void S6Exporter::ExportResearchedSceneryItems()
{
    std::fill(
        std::begin(_s6.researched_scenery_items),
        std::end(_s6.researched_scenery_items),
        false);

    for (uint16 sceneryEntryIndex = 0; sceneryEntryIndex < RCT2_MAX_RESEARCHED_SCENERY_ITEMS; sceneryEntryIndex++)
    {
        if (scenery_is_invented(sceneryEntryIndex))
        {
            sint32 quadIndex = sceneryEntryIndex >> 5;
            sint32 bitIndex  = sceneryEntryIndex & 0x1F;
            _s6.researched_scenery_items[quadIndex] |= (uint32) 1 << bitIndex;
        }
    }
}

void S6Exporter::ExportResearchList()
{
    memcpy(_s6.research_items, gResearchItems, sizeof(_s6.research_items));
}

enum {
    S6_SAVE_FLAG_EXPORT    = 1 << 0,
    S6_SAVE_FLAG_SCENARIO  = 1 << 1,
    S6_SAVE_FLAG_AUTOMATIC = 1u << 31,
};

/**
 *
 *  rct2: 0x006754F5
 * @param flags bit 0: pack objects, 1: save as scenario
 */
sint32 scenario_save(const utf8 * path, sint32 flags)
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
    viewport_set_saved_view();

    bool result     = false;
    auto s6exporter = new S6Exporter();
    try
    {
        if (flags & S6_SAVE_FLAG_EXPORT)
        {
            IObjectManager * objManager   = OpenRCT2::GetContext()->GetObjectManager();
            s6exporter->ExportObjectsList = objManager->GetPackableObjects();
        }
        s6exporter->RemoveTracklessRides = true;
        s6exporter->Export();
        if (flags & S6_SAVE_FLAG_SCENARIO)
        {
            s6exporter->SaveScenario(path);
        }
        else
        {
            s6exporter->SaveGame(path);
        }
        result = true;
    }
    catch (const std::exception &)
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
