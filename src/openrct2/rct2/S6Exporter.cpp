/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "S6Exporter.h"

#include "../Context.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../common.h"
#include "../config/Config.h"
#include "../core/FileStream.h"
#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../management/Research.h"
#include "../object/Object.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../peep/Staff.h"
#include "../rct12/SawyerChunkWriter.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideRatings.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ride/TrackData.h"
#include "../scenario/Scenario.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Sprite.h"

#include <algorithm>
#include <cstring>
#include <functional>
#include <iterator>
#include <optional>

S6Exporter::S6Exporter()
{
    RemoveTracklessRides = false;
    std::memset(&_s6, 0x00, sizeof(_s6));
}

void S6Exporter::SaveGame(const utf8* path)
{
    auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_WRITE);
    SaveGame(&fs);
}

void S6Exporter::SaveGame(OpenRCT2::IStream* stream)
{
    Save(stream, false);
}

void S6Exporter::SaveScenario(const utf8* path)
{
    auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_WRITE);
    SaveScenario(&fs);
}

void S6Exporter::SaveScenario(OpenRCT2::IStream* stream)
{
    Save(stream, true);
}

void S6Exporter::Save(OpenRCT2::IStream* stream, bool isScenario)
{
    _s6.header.type = isScenario ? S6_TYPE_SCENARIO : S6_TYPE_SAVEDGAME;
    _s6.header.classic_flag = 0;
    _s6.header.num_packed_objects = uint16_t(ExportObjectsList.size());
    _s6.header.version = S6_RCT2_VERSION;
    _s6.header.magic_number = S6_MAGIC_NUMBER;
    _s6.game_version_number = 201028;

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
        auto& objRepo = OpenRCT2::GetContext()->GetObjectRepository();
        objRepo.WritePackedObjects(stream, ExportObjectsList);
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
    auto data = std::unique_ptr<uint8_t, std::function<void(uint8_t*)>>(
        stream->ReadArray<uint8_t>(fileSize), Memory::Free<uint8_t>);
    uint32_t checksum = sawyercoding_calculate_checksum(data.get(), fileSize);

    // Write the checksum on the end
    stream->SetPosition(fileSize);
    stream->WriteValue(checksum);
}

void S6Exporter::Export()
{
    _s6.info = gS6Info;
    {
        auto temp = utf8_to_rct2(gS6Info.name);
        safe_strcpy(_s6.info.name, temp.data(), sizeof(_s6.info.name));
    }
    {
        auto temp = utf8_to_rct2(gS6Info.details);
        safe_strcpy(_s6.info.details, temp.data(), sizeof(_s6.info.details));
    }
    uint32_t researchedTrackPiecesA[128] = {};
    uint32_t researchedTrackPiecesB[128] = {};

    for (int32_t i = 0; i < RCT2_OBJECT_ENTRY_COUNT; i++)
    {
        const rct_object_entry* entry = get_loaded_object_entry(i);
        void* entryData = get_loaded_object_chunk(i);
        // RCT2 uses (void *)-1 to mark NULL. Make sure it's written in a vanilla-compatible way.
        if (entry == nullptr || entryData == nullptr || entryData == reinterpret_cast<void*>(-1))
        {
            std::memset(&_s6.objects[i], 0xFF, sizeof(rct_object_entry));
        }
        else
        {
            _s6.objects[i] = *entry;
        }
    }

    _s6.elapsed_months = static_cast<uint16_t>(gDateMonthsElapsed);
    _s6.current_day = gDateMonthTicks;
    _s6.scenario_ticks = gScenarioTicks;

    auto state = scenario_rand_state();
    _s6.scenario_srand_0 = state.s0;
    _s6.scenario_srand_1 = state.s1;

    // Map elements must be reorganised prior to saving otherwise save may be invalid
    map_reorganise_elements();
    ExportTileElements();
    ExportSprites();
    ExportParkName();

    _s6.initial_cash = gInitialCash;
    _s6.current_loan = gBankLoan;
    _s6.park_flags = gParkFlags;
    _s6.park_entrance_fee = gParkEntranceFee;
    // rct1_park_entrance_x
    // rct1_park_entrance_y
    // pad_013573EE
    // rct1_park_entrance_z
    ExportPeepSpawns();
    _s6.guest_count_change_modifier = gGuestChangeModifier;
    _s6.current_research_level = gResearchFundingLevel;
    // pad_01357400
    ExportResearchedRideTypes();
    ExportResearchedRideEntries();
    // Not used by OpenRCT2 any more, but left in to keep RCT2 export working.
    for (uint8_t i = 0; i < std::size(RideTypeDescriptors); i++)
    {
        researchedTrackPiecesA[i] = (RideTypeDescriptors[i].EnabledTrackPieces) & 0xFFFFFFFFULL;
        researchedTrackPiecesB[i] = (RideTypeDescriptors[i].EnabledTrackPieces >> 32ULL) & 0xFFFFFFFFULL;
    }
    std::memcpy(_s6.researched_track_types_a, researchedTrackPiecesA, sizeof(_s6.researched_track_types_a));
    std::memcpy(_s6.researched_track_types_b, researchedTrackPiecesB, sizeof(_s6.researched_track_types_b));

    _s6.guests_in_park = gNumGuestsInPark;
    _s6.guests_heading_for_park = gNumGuestsHeadingForPark;

    std::memcpy(_s6.expenditure_table, gExpenditureTable, sizeof(_s6.expenditure_table));

    _s6.last_guests_in_park = gNumGuestsInParkLastWeek;
    // pad_01357BCA
    _s6.handyman_colour = gStaffHandymanColour;
    _s6.mechanic_colour = gStaffMechanicColour;
    _s6.security_colour = gStaffSecurityColour;

    ExportResearchedSceneryItems();

    _s6.park_rating = gParkRating;

    std::memcpy(_s6.park_rating_history, gParkRatingHistory, sizeof(_s6.park_rating_history));
    std::memcpy(_s6.guests_in_park_history, gGuestsInParkHistory, sizeof(_s6.guests_in_park_history));

    _s6.active_research_types = gResearchPriorities;
    _s6.research_progress_stage = gResearchProgressStage;
    if (gResearchLastItem.has_value())
        _s6.last_researched_item_subject = gResearchLastItem->ToRCT12ResearchItem().rawValue;
    else
        _s6.last_researched_item_subject = RCT12_RESEARCHED_ITEMS_SEPARATOR;
    // pad_01357CF8
    _s6.research_progress = gResearchProgress;

    if (gResearchNextItem.has_value())
    {
        auto RCT2ResearchItem = gResearchNextItem->ToRCT12ResearchItem();
        _s6.next_research_item = RCT2ResearchItem.rawValue;
        _s6.next_research_category = RCT2ResearchItem.category;
    }
    else
    {
        _s6.next_research_item = RCT12_RESEARCHED_ITEMS_SEPARATOR;
        _s6.next_research_category = 0;
    }

    _s6.next_research_expected_day = gResearchExpectedDay;
    _s6.next_research_expected_month = gResearchExpectedMonth;
    _s6.guest_initial_happiness = gGuestInitialHappiness;
    _s6.park_size = gParkSize;
    _s6.guest_generation_probability = _guestGenerationProbability;
    _s6.total_ride_value_for_money = gTotalRideValueForMoney;
    _s6.maximum_loan = gMaxBankLoan;
    _s6.guest_initial_cash = gGuestInitialCash;
    _s6.guest_initial_hunger = gGuestInitialHunger;
    _s6.guest_initial_thirst = gGuestInitialThirst;
    _s6.objective_type = gScenarioObjective.Type;
    _s6.objective_year = gScenarioObjective.Year;
    // pad_013580FA
    _s6.objective_currency = gScenarioObjective.Currency;
    // In RCT2, the ride string IDs start at index STR_0002 and are directly mappable.
    // This is not always the case in OpenRCT2, so we use the actual ride ID.
    if (gScenarioObjective.Type == OBJECTIVE_BUILD_THE_BEST)
        _s6.objective_guests = gScenarioObjective.RideId + RCT2_RIDE_STRING_START;
    else
        _s6.objective_guests = gScenarioObjective.NumGuests;

    ExportMarketingCampaigns();

    std::memcpy(_s6.balance_history, gCashHistory, sizeof(_s6.balance_history));

    _s6.current_expenditure = gCurrentExpenditure;
    _s6.current_profit = gCurrentProfit;
    _s6.weekly_profit_average_dividend = gWeeklyProfitAverageDividend;
    _s6.weekly_profit_average_divisor = gWeeklyProfitAverageDivisor;
    // pad_0135833A

    std::memcpy(_s6.weekly_profit_history, gWeeklyProfitHistory, sizeof(_s6.weekly_profit_history));

    _s6.park_value = gParkValue;

    std::memcpy(_s6.park_value_history, gParkValueHistory, sizeof(_s6.park_value_history));

    _s6.completed_company_value = gScenarioCompletedCompanyValue;
    _s6.total_admissions = gTotalAdmissions;
    _s6.income_from_admissions = gTotalIncomeFromAdmissions;
    _s6.company_value = gCompanyValue;
    std::memcpy(_s6.peep_warning_throttle, gPeepWarningThrottle, sizeof(_s6.peep_warning_throttle));

    // Awards
    for (int32_t i = 0; i < RCT12_MAX_AWARDS; i++)
    {
        Award* src = &gCurrentAwards[i];
        rct12_award* dst = &_s6.awards[i];
        dst->time = src->Time;
        dst->type = src->Type;
    }

    _s6.land_price = gLandPrice;
    _s6.construction_rights_price = gConstructionRightsPrice;
    // unk_01358774
    // pad_01358776
    // _s6.cd_key
    // _s6.game_version_number
    _s6.completed_company_value_record = gScenarioCompanyValueRecord;
    _s6.loan_hash = GetLoanHash(gInitialCash, gBankLoan, gMaxBankLoan);
    _s6.ride_count = ride_get_count();
    // pad_013587CA
    _s6.historical_profit = gHistoricalProfit;
    // pad_013587D4
    String::Set(_s6.scenario_completed_name, sizeof(_s6.scenario_completed_name), gScenarioCompletedBy.c_str());
    _s6.cash = ENCRYPT_MONEY(gCash);
    // pad_013587FC
    _s6.park_rating_casualty_penalty = gParkRatingCasualtyPenalty;
    _s6.map_size_units = gMapSizeUnits;
    _s6.map_size_minus_2 = gMapSizeMinus2;
    _s6.map_size = gMapSize;
    _s6.map_max_xy = gMapSizeMaxXY;
    _s6.same_price_throughout = gSamePriceThroughoutPark & 0xFFFFFFFF;
    _s6.suggested_max_guests = _suggestedGuestMaximum;
    _s6.park_rating_warning_days = gScenarioParkRatingWarningDays;
    _s6.last_entrance_style = gLastEntranceStyle;
    // rct1_water_colour
    // pad_01358842
    ExportResearchList();
    _s6.map_base_z = gMapBaseZ;
    String::Set(_s6.scenario_name, sizeof(_s6.scenario_name), gScenarioName.c_str());
    String::Set(_s6.scenario_description, sizeof(_s6.scenario_description), gScenarioDetails.c_str());
    _s6.current_interest_rate = gBankLoanInterestRate;
    // pad_0135934B
    _s6.same_price_throughout_extended = gSamePriceThroughoutPark >> 32;
    // Preserve compatibility with vanilla RCT2's save format.
    for (uint8_t i = 0; i < RCT12_MAX_PARK_ENTRANCES; i++)
    {
        CoordsXYZD entrance = { LOCATION_NULL, LOCATION_NULL, 0, 0 };
        if (gParkEntrances.size() > i)
        {
            entrance = gParkEntrances[i];
        }
        _s6.park_entrance_x[i] = entrance.x;
        _s6.park_entrance_y[i] = entrance.y;
        _s6.park_entrance_z[i] = entrance.z;
        _s6.park_entrance_direction[i] = entrance.direction;
    }
    safe_strcpy(_s6.scenario_filename, gScenarioFileName, sizeof(_s6.scenario_filename));
    std::memcpy(_s6.saved_expansion_pack_names, gScenarioExpansionPacks, sizeof(_s6.saved_expansion_pack_names));
    ExportBanners();
    _s6.game_ticks_1 = gCurrentTicks;

    this->ExportRides();

    _s6.saved_age = gSavedAge;
    _s6.saved_view_x = gSavedView.x;
    _s6.saved_view_y = gSavedView.y;
    _s6.saved_view_zoom = static_cast<int8_t>(std::clamp<ZoomLevel>(gSavedViewZoom, 0, 3));
    _s6.saved_view_rotation = gSavedViewRotation;

    ExportMapAnimations();

    ExportRideRatingsCalcData();
    ExportRideMeasurements();
    _s6.next_guest_index = gNextGuestNumber;
    _s6.grass_and_scenery_tilepos = gGrassSceneryTileLoopPosition;
    std::memcpy(_s6.patrol_areas, gStaffPatrolAreas, sizeof(_s6.patrol_areas));
    std::memcpy(_s6.staff_modes, gStaffModes, sizeof(_s6.staff_modes));
    // unk_13CA73E
    // pad_13CA73F
    // unk_13CA740
    _s6.climate = static_cast<uint8_t>(gClimate);
    // pad_13CA741;
    // byte_13CA742
    // pad_013CA747
    _s6.climate_update_timer = gClimateUpdateTimer;
    _s6.current_weather = EnumValue(gClimateCurrent.Weather);
    _s6.next_weather = EnumValue(gClimateNext.Weather);
    _s6.temperature = gClimateCurrent.Temperature;
    _s6.next_temperature = gClimateNext.Temperature;
    _s6.current_weather_effect = static_cast<uint8_t>(gClimateCurrent.WeatherEffect);
    _s6.next_weather_effect = static_cast<uint8_t>(gClimateNext.WeatherEffect);
    _s6.current_weather_gloom = gClimateCurrent.WeatherGloom;
    _s6.next_weather_gloom = gClimateNext.WeatherGloom;
    _s6.current_weather_level = static_cast<uint8_t>(gClimateCurrent.Level);
    _s6.next_weather_level = static_cast<uint8_t>(gClimateNext.Level);

    // News items
    for (size_t i = 0; i < RCT12_MAX_NEWS_ITEMS; i++)
    {
        const News::Item* src = &gNewsItems[i];
        rct12_news_item* dst = &_s6.news_items[i];

        dst->Type = static_cast<uint8_t>(src->Type);
        dst->Flags = src->Flags;
        dst->Assoc = src->Assoc;
        dst->Ticks = src->Ticks;
        dst->MonthYear = src->MonthYear;
        dst->Day = src->Day;

        auto rct2text = ConvertFormattedStringToRCT2(src->Text, sizeof(dst->Text));
        std::memcpy(dst->Text, rct2text.c_str(), std::min(sizeof(dst->Text), rct2text.size()));
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

    ExportUserStrings();
}

void S6Exporter::ExportPeepSpawns()
{
    for (size_t i = 0; i < RCT12_MAX_PEEP_SPAWNS; i++)
    {
        if (gPeepSpawns.size() > i)
        {
            _s6.peep_spawns[i] = { static_cast<uint16_t>(gPeepSpawns[i].x), static_cast<uint16_t>(gPeepSpawns[i].y),
                                   static_cast<uint8_t>(gPeepSpawns[i].z / 16), gPeepSpawns[i].direction };
        }
        else
        {
            _s6.peep_spawns[i] = { RCT12_PEEP_SPAWN_UNDEFINED, RCT12_PEEP_SPAWN_UNDEFINED, 0, 0 };
        }
    }
}

uint32_t S6Exporter::GetLoanHash(money32 initialCash, money32 bankLoan, uint32_t maxBankLoan)
{
    int32_t value = 0x70093A;
    value -= initialCash;
    value = ror32(value, 5);
    value -= bankLoan;
    value = ror32(value, 7);
    value += maxBankLoan;
    value = ror32(value, 3);
    return value;
}

void S6Exporter::ExportParkName()
{
    auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
    auto stringId = AllocateUserString(park.Name);
    if (stringId != std::nullopt)
    {
        _s6.park_name = *stringId;
        _s6.park_name_args = 0;
    }
    else
    {
        log_warning("Unable to allocate user string for park name during S6 export.");
        _s6.park_name = STR_UNNAMED_PARK;
        _s6.park_name_args = 0;
    }
}

void S6Exporter::ExportRides()
{
    const Ride nullRide{};
    for (int32_t index = 0; index < RCT12_MAX_RIDES_IN_PARK; index++)
    {
        const auto* src = get_ride(index);
        if (src == nullptr)
        {
            src = &nullRide;
        }
        auto dst = &_s6.rides[index];
        *dst = {};
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

void S6Exporter::ExportRide(rct2_ride* dst, const Ride* src)
{
    std::memset(dst, 0, sizeof(rct2_ride));

    dst->type = OpenRCT2RideTypeToRCT2RideType(src->type);
    dst->subtype = OpenRCT2EntryIndexToRCTEntryIndex(src->subtype);
    // pad_002;
    dst->mode = static_cast<uint8_t>(src->mode);
    dst->colour_scheme_type = src->colour_scheme_type;

    for (uint8_t i = 0; i < RCT2_MAX_CARS_PER_TRAIN; i++)
    {
        dst->vehicle_colours[i].body_colour = src->vehicle_colours[i].Body;
        dst->vehicle_colours[i].trim_colour = src->vehicle_colours[i].Trim;
    }

    // pad_046;
    dst->status = src->status;

    bool useDefaultName = true;
    if (!src->custom_name.empty())
    {
        // Custom name, allocate user string for ride
        auto stringId = AllocateUserString(src->custom_name);
        if (stringId != std::nullopt)
        {
            dst->name = *stringId;
            dst->name_arguments = 0;
            useDefaultName = false;
        }
        else
        {
            log_warning(
                "Unable to allocate user string for ride #%d (%s).", static_cast<int>(src->id), src->custom_name.c_str());
        }
    }
    if (useDefaultName)
    {
        // Default name with number
        dst->name = RideTypeDescriptors[src->type].Naming.Name;
        dst->name_arguments_number = src->default_name_number;
    }

    if (src->overall_view.isNull())
    {
        dst->overall_view.setNull();
    }
    else
    {
        auto tileLoc = TileCoordsXY(src->overall_view);
        dst->overall_view = { static_cast<uint8_t>(tileLoc.x), static_cast<uint8_t>(tileLoc.y) };
    }

    for (int32_t i = 0; i < RCT12_MAX_STATIONS_PER_RIDE; i++)
    {
        if (src->stations[i].Start.isNull())
        {
            dst->station_starts[i].setNull();
        }
        else
        {
            auto tileStartLoc = TileCoordsXY(src->stations[i].Start);
            dst->station_starts[i] = { static_cast<uint8_t>(tileStartLoc.x), static_cast<uint8_t>(tileStartLoc.y) };
        }
        dst->station_heights[i] = src->stations[i].Height;
        dst->station_length[i] = src->stations[i].Length;
        dst->station_depart[i] = src->stations[i].Depart;
        dst->train_at_station[i] = src->stations[i].TrainAtStation;

        TileCoordsXYZD entrance = ride_get_entrance_location(src, i);
        if (entrance.isNull())
            dst->entrances[i].setNull();
        else
            dst->entrances[i] = { static_cast<uint8_t>(entrance.x), static_cast<uint8_t>(entrance.y) };

        TileCoordsXYZD exit = ride_get_exit_location(src, i);
        if (exit.isNull())
            dst->exits[i].setNull();
        else
            dst->exits[i] = { static_cast<uint8_t>(exit.x), static_cast<uint8_t>(exit.y) };

        dst->last_peep_in_queue[i] = src->stations[i].LastPeepInQueue;

        dst->length[i] = src->stations[i].SegmentLength;
        dst->time[i] = src->stations[i].SegmentTime;

        dst->queue_time[i] = src->stations[i].QueueTime;

        dst->queue_length[i] = src->stations[i].QueueLength;
    }

    for (uint8_t i = 0; i <= RCT2_MAX_VEHICLES_PER_RIDE; i++)
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
    dst->SetMinCarsPerTrain(src->GetMinCarsPerTrain());
    dst->SetMaxCarsPerTrain(src->GetMaxCarsPerTrain());
    dst->min_waiting_time = src->min_waiting_time;
    dst->max_waiting_time = src->max_waiting_time;

    // Includes time_limit, num_laps, launch_speed, speed, rotations
    dst->operation_option = src->operation_option;

    dst->boat_hire_return_direction = src->boat_hire_return_direction;
    dst->boat_hire_return_position = { static_cast<uint8_t>(src->boat_hire_return_position.x),
                                       static_cast<uint8_t>(src->boat_hire_return_position.y) };

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

    if (src->CurTestTrackLocation.isNull())
    {
        dst->cur_test_track_location.setNull();
    }
    else
    {
        dst->cur_test_track_location = { static_cast<uint8_t>(src->CurTestTrackLocation.x),
                                         static_cast<uint8_t>(src->CurTestTrackLocation.y) };
        dst->cur_test_track_z = static_cast<uint8_t>(src->CurTestTrackLocation.z);
    }

    dst->turn_count_default = src->turn_count_default;
    dst->turn_count_banked = src->turn_count_banked;
    dst->turn_count_sloped = src->turn_count_sloped;
    if (dst->type == RIDE_TYPE_MINI_GOLF)
        dst->inversions = static_cast<uint8_t>(std::min(src->holes, RCT12_MAX_GOLF_HOLES));
    else
        dst->inversions = static_cast<uint8_t>(std::min(src->inversions, RCT12_MAX_INVERSIONS));
    dst->inversions |= (src->sheltered_eighths << 5);
    dst->drops = src->drops;
    dst->start_drop_height = src->start_drop_height;
    dst->highest_drop_height = src->highest_drop_height;
    dst->sheltered_length = src->sheltered_length;
    dst->var_11C = src->var_11C;
    dst->num_sheltered_sections = src->num_sheltered_sections;

    dst->cur_num_customers = src->cur_num_customers;
    dst->num_customers_timeout = src->num_customers_timeout;

    for (uint8_t i = 0; i < RCT2_CUSTOMER_HISTORY_SIZE; i++)
    {
        dst->num_customers[i] = src->num_customers[i];
    }

    dst->price = src->price[0];

    for (uint8_t i = 0; i < 2; i++)
    {
        dst->chairlift_bullwheel_location[i] = { static_cast<uint8_t>(src->ChairliftBullwheelLocation[i].x),
                                                 static_cast<uint8_t>(src->ChairliftBullwheelLocation[i].y) };
        dst->chairlift_bullwheel_z[i] = static_cast<uint8_t>(src->ChairliftBullwheelLocation[i].z);
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
    dst->build_date = static_cast<int16_t>(src->build_date);
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

    dst->price_secondary = src->price[1];

    dst->reliability = src->reliability;
    dst->unreliability_factor = src->unreliability_factor;
    dst->downtime = src->downtime;
    dst->inspection_interval = src->inspection_interval;
    dst->last_inspection = src->last_inspection;

    for (uint8_t i = 0; i < RCT2_DOWNTIME_HISTORY_SIZE; i++)
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

    for (uint8_t i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++)
    {
        dst->track_colour_main[i] = src->track_colour[i].main;
        dst->track_colour_additional[i] = src->track_colour[i].additional;
        dst->track_colour_supports[i] = src->track_colour[i].supports;
    }

    dst->music = src->music;
    dst->entrance_style = src->entrance_style;
    dst->vehicle_change_timeout = src->vehicle_change_timeout;
    dst->num_block_brakes = src->num_block_brakes;
    dst->lift_hill_speed = src->lift_hill_speed;
    dst->guests_favourite = src->guests_favourite;
    dst->lifecycle_flags = src->lifecycle_flags;

    for (uint8_t i = 0; i < RCT2_MAX_CARS_PER_TRAIN; i++)
    {
        dst->vehicle_colours_extended[i] = src->vehicle_colours[i].Ternary;
    }

    dst->total_air_time = src->total_air_time;
    dst->current_test_station = src->current_test_station;
    dst->num_circuits = src->num_circuits;
    dst->cable_lift_x = static_cast<int16_t>(src->CableLiftLoc.x);
    dst->cable_lift_y = static_cast<int16_t>(src->CableLiftLoc.y);
    dst->cable_lift_z = static_cast<int16_t>(src->CableLiftLoc.z / COORDS_Z_STEP);
    // pad_1FD;
    dst->cable_lift = src->cable_lift;

    // pad_208[0x58];
}

void S6Exporter::ExportRideRatingsCalcData()
{
    const auto& src = gRideRatingsCalcData;
    auto& dst = _s6.ride_ratings_calc_data;
    dst.proximity_x = src.Proximity.x;
    dst.proximity_y = src.Proximity.y;
    dst.proximity_z = src.Proximity.z;
    dst.proximity_start_x = src.ProximityStart.x;
    dst.proximity_start_y = src.ProximityStart.y;
    dst.proximity_start_z = src.ProximityStart.z;
    dst.current_ride = src.CurrentRide;
    dst.state = src.State;
    dst.proximity_track_type = static_cast<uint8_t>(src.ProximityTrackType);
    if (src.ProximityTrackType == TrackElemType::RotationControlToggle)
        dst.proximity_track_type = static_cast<uint8_t>(TrackElemType::RotationControlToggleAlias);
    dst.proximity_base_height = src.ProximityBaseHeight;
    dst.proximity_total = src.ProximityTotal;
    for (size_t i = 0; i < std::size(dst.proximity_scores); i++)
    {
        dst.proximity_scores[i] = src.ProximityScores[i];
    }
    dst.num_brakes = src.AmountOfBrakes;
    dst.num_reversers = src.AmountOfReversers;
    dst.station_flags = src.StationFlags;
}

void S6Exporter::ExportRideMeasurements()
{
    // Get all the ride measurements
    std::vector<Ride*> ridesWithMeasurements;
    for (ride_id_t i = 0; i < RCT12_MAX_RIDES_IN_PARK; i++)
    {
        auto ride = get_ride(i);
        if (ride != nullptr && ride->measurement != nullptr)
        {
            ridesWithMeasurements.push_back(ride);
        }
    }

    // If there are more than S6 can hold, trim it by LRU
    if (ridesWithMeasurements.size() > RCT12_RIDE_MEASUREMENT_MAX_ITEMS)
    {
        // Sort in order of last recently used
        std::sort(ridesWithMeasurements.begin(), ridesWithMeasurements.end(), [](const Ride* a, const Ride* b) {
            return a->measurement->last_use_tick > b->measurement->last_use_tick;
        });
        ridesWithMeasurements.resize(RCT12_RIDE_MEASUREMENT_MAX_ITEMS);
    }

    // Convert ride measurements to S6 format
    uint8_t i{};
    for (auto src : ridesWithMeasurements)
    {
        auto& dst = _s6.ride_measurements[i];
        ExportRideMeasurement(_s6.ride_measurements[i], *src->measurement.get());

        auto rideId = src->id;
        dst.ride_index = rideId;
        _s6.rides[rideId].measurement_index = i;
        i++;
    }
}

void S6Exporter::ExportRideMeasurement(RCT12RideMeasurement& dst, const RideMeasurement& src)
{
    dst.flags = src.flags;
    dst.last_use_tick = src.last_use_tick;
    dst.num_items = src.num_items;
    dst.current_item = src.current_item;
    dst.vehicle_index = src.vehicle_index;
    dst.current_station = src.current_station;
    for (size_t i = 0; i < std::size(src.velocity); i++)
    {
        dst.velocity[i] = src.velocity[i];
        dst.altitude[i] = src.altitude[i];
        dst.vertical[i] = src.vertical[i];
        dst.lateral[i] = src.lateral[i];
    }
}

void S6Exporter::ExportResearchedRideTypes()
{
    std::fill(std::begin(_s6.researched_ride_types), std::end(_s6.researched_ride_types), false);

    for (int32_t rideType = 0; rideType < RCT2_RIDE_TYPE_COUNT; rideType++)
    {
        if (ride_type_is_invented(rideType))
        {
            int32_t quadIndex = rideType >> 5;
            int32_t bitIndex = rideType & 0x1F;
            _s6.researched_ride_types[quadIndex] |= 1UL << bitIndex;
        }
    }
}

void S6Exporter::ExportResearchedRideEntries()
{
    std::fill(std::begin(_s6.researched_ride_entries), std::end(_s6.researched_ride_entries), false);

    for (int32_t rideEntryIndex = 0; rideEntryIndex < MAX_RIDE_OBJECTS; rideEntryIndex++)
    {
        if (ride_entry_is_invented(rideEntryIndex))
        {
            int32_t quadIndex = rideEntryIndex >> 5;
            int32_t bitIndex = rideEntryIndex & 0x1F;
            _s6.researched_ride_entries[quadIndex] |= 1UL << bitIndex;
        }
    }
}

void S6Exporter::ExportResearchedSceneryItems()
{
    std::fill(std::begin(_s6.researched_scenery_items), std::end(_s6.researched_scenery_items), false);

    for (uint16_t sceneryEntryIndex = 0; sceneryEntryIndex < RCT2_MAX_RESEARCHED_SCENERY_ITEMS; sceneryEntryIndex++)
    {
        ScenerySelection scenerySelection = { static_cast<uint8_t>((sceneryEntryIndex >> 8) & 0xFF),
                                              static_cast<uint16_t>(sceneryEntryIndex & 0xFF) };

        // SV6 allows for more scenery types than there are. Avoid triggering an assertion in scenery_is_invented().
        if (scenerySelection.SceneryType >= SCENERY_TYPE_COUNT)
            break;

        if (scenery_is_invented(scenerySelection))
        {
            int32_t quadIndex = sceneryEntryIndex >> 5;
            int32_t bitIndex = sceneryEntryIndex & 0x1F;
            _s6.researched_scenery_items[quadIndex] |= 1UL << bitIndex;
        }
    }
}

void S6Exporter::ExportResearchList()
{
    size_t i = 0;
    for (const auto& researchItem : gResearchItemsInvented)
    {
        _s6.research_items[i++] = researchItem.ToRCT12ResearchItem();
    }
    _s6.research_items[i++] = { RCT12_RESEARCHED_ITEMS_SEPARATOR, 0 };
    for (const auto& researchItem : gResearchItemsUninvented)
    {
        _s6.research_items[i++] = researchItem.ToRCT12ResearchItem();
    }
    _s6.research_items[i++] = { RCT12_RESEARCHED_ITEMS_END, 0 };
    _s6.research_items[i] = { RCT12_RESEARCHED_ITEMS_END_2, 0 };
}

void S6Exporter::ExportMarketingCampaigns()
{
    std::memset(_s6.campaign_weeks_left, 0, sizeof(_s6.campaign_weeks_left));
    std::memset(_s6.campaign_ride_index, 0, sizeof(_s6.campaign_ride_index));
    for (const auto& campaign : gMarketingCampaigns)
    {
        _s6.campaign_weeks_left[campaign.Type] = campaign.WeeksLeft | CAMPAIGN_ACTIVE_FLAG;
        if ((campaign.Flags & MarketingCampaignFlags::FIRST_WEEK))
            _s6.campaign_weeks_left[campaign.Type] |= CAMPAIGN_FIRST_WEEK_FLAG;
        if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
        {
            _s6.campaign_ride_index[campaign.Type] = campaign.RideId;
        }
        else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
        {
            _s6.campaign_ride_index[campaign.Type] = EnumValue(campaign.ShopItemType);
        }
    }
}

void S6Exporter::ExportSprites()
{
    // Sprites needs to be reset before they get used.
    // Might as well reset them in here to zero out the space and improve
    // compression ratios. Especially useful for multiplayer servers that
    // use zlib on the sent stream.
    sprite_clear_all_unused();
    for (int32_t i = 0; i < RCT2_MAX_SPRITES; i++)
    {
        ExportSprite(&_s6.sprites[i], reinterpret_cast<const rct_sprite*>(GetEntity(i)));
    }

    RebuildEntityLinks();
}

void S6Exporter::RebuildEntityLinks()
{
    // Rebuild next/previous linked list entity indexs
    for (auto list :
         { RCT12EntityLinkListOffset::Free, RCT12EntityLinkListOffset::Litter, RCT12EntityLinkListOffset::Misc,
           RCT12EntityLinkListOffset::Peep, RCT12EntityLinkListOffset::TrainHead, RCT12EntityLinkListOffset::Vehicle })
    {
        uint16_t previous = SPRITE_INDEX_NULL;
        for (auto& entity : _s6.sprites)
        {
            if (entity.unknown.linked_list_type_offset == list)
            {
                _s6.sprite_lists_count[EnumValue(list) >> 1]++;
                _s6.sprites[entity.unknown.sprite_index].unknown.previous = previous;
                if (previous != SPRITE_INDEX_NULL)
                {
                    _s6.sprites[previous].unknown.next = entity.unknown.sprite_index;
                }
                else
                {
                    _s6.sprite_lists_head[EnumValue(list) >> 1] = entity.unknown.sprite_index;
                }
                _s6.sprites[entity.unknown.sprite_index].unknown.next = SPRITE_INDEX_NULL;
                previous = entity.unknown.sprite_index;
            }
        }
    }

    // Rebuild next_in_quadrant linked list entity indexs
    for (auto x = 0; x < 255; ++x)
    {
        for (auto y = 0; y < 255; ++y)
        {
            uint16_t previous = SPRITE_INDEX_NULL;
            for (auto* entity : EntityTileList(TileCoordsXY{ x, y }.ToCoordsXY()))
            {
                if (previous != SPRITE_INDEX_NULL)
                {
                    _s6.sprites[previous].unknown.next_in_quadrant = entity->sprite_index;
                }
                previous = entity->sprite_index;
            }
            if (previous != SPRITE_INDEX_NULL)
            {
                _s6.sprites[previous].unknown.next_in_quadrant = SPRITE_INDEX_NULL;
            }
        }
    }
}

void S6Exporter::ExportSprite(RCT2Sprite* dst, const rct_sprite* src)
{
    std::memset(dst, 0, sizeof(rct_sprite));
    switch (src->misc.sprite_identifier)
    {
        case SpriteIdentifier::Null:
            ExportSpriteCommonProperties(&dst->unknown, &src->misc);
            break;
        case SpriteIdentifier::Vehicle:
            ExportSpriteVehicle(&dst->vehicle, &src->vehicle);
            break;
        case SpriteIdentifier::Peep:
            ExportSpritePeep(&dst->peep, &src->peep);
            break;
        case SpriteIdentifier::Misc:
            ExportSpriteMisc(&dst->unknown, &src->misc);
            break;
        case SpriteIdentifier::Litter:
            ExportSpriteLitter(&dst->litter, &src->litter);
            break;
        default:
            ExportSpriteCommonProperties(&dst->unknown, &src->misc);
            log_warning("Sprite identifier %d can not be exported.", src->misc.sprite_identifier);
            break;
    }
}

constexpr RCT12EntityLinkListOffset GetRCT2LinkListOffset(const SpriteBase* src)
{
    RCT12EntityLinkListOffset output = RCT12EntityLinkListOffset::Free;
    switch (src->sprite_identifier)
    {
        case SpriteIdentifier::Vehicle:
        {
            auto veh = src->As<Vehicle>();
            if (veh && veh->IsHead())
            {
                output = RCT12EntityLinkListOffset::TrainHead;
            }
            else
            {
                output = RCT12EntityLinkListOffset::Vehicle;
            }
        }
        break;
        case SpriteIdentifier::Peep:
            output = RCT12EntityLinkListOffset::Peep;
            break;
        case SpriteIdentifier::Misc:
            output = RCT12EntityLinkListOffset::Misc;
            break;
        case SpriteIdentifier::Litter:
            output = RCT12EntityLinkListOffset::Litter;
            break;
        default:
            break;
    }
    return output;
}

void S6Exporter::ExportSpriteCommonProperties(RCT12SpriteBase* dst, const SpriteBase* src)
{
    dst->sprite_identifier = src->sprite_identifier;
    dst->linked_list_type_offset = GetRCT2LinkListOffset(src);
    dst->next_in_quadrant = SPRITE_INDEX_NULL;
    dst->sprite_height_negative = src->sprite_height_negative;
    dst->sprite_index = src->sprite_index;
    dst->flags = src->flags;
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
    dst->sprite_width = src->sprite_width;
    dst->sprite_height_positive = src->sprite_height_positive;
    dst->sprite_left = src->sprite_left;
    dst->sprite_top = src->sprite_top;
    dst->sprite_right = src->sprite_right;
    dst->sprite_bottom = src->sprite_bottom;
    dst->sprite_direction = src->sprite_direction;
}

void S6Exporter::ExportSpriteVehicle(RCT2SpriteVehicle* dst, const Vehicle* src)
{
    const auto* ride = src->GetRide();

    ExportSpriteCommonProperties(dst, static_cast<const SpriteBase*>(src));
    dst->type = EnumValue(src->SubType);
    dst->vehicle_sprite_type = src->vehicle_sprite_type;
    dst->bank_rotation = src->bank_rotation;
    dst->remaining_distance = src->remaining_distance;
    dst->velocity = src->velocity;
    dst->acceleration = src->acceleration;
    dst->ride = src->ride;
    dst->vehicle_type = src->vehicle_type;
    dst->colours = src->colours;
    dst->track_progress = src->track_progress;
    if (ride != nullptr && ride->mode == RideMode::BoatHire && src->status == Vehicle::Status::TravellingBoat)
    {
        if (src->BoatLocation.isNull())
        {
            dst->boat_location.setNull();
        }
        else
        {
            dst->boat_location = { static_cast<uint8_t>(src->BoatLocation.x / COORDS_XY_STEP),
                                   static_cast<uint8_t>(src->BoatLocation.y / COORDS_XY_STEP) };
        }
    }
    else
    {
        // export SpinningControlToggle (256) as SpinningControlToggleAlias (100) for backwards compatability with OpenRCT2
        auto trackType = src->GetTrackType();
        if (trackType == TrackElemType::RotationControlToggle)
            trackType = TrackElemType::RotationControlToggleAlias;
        // Track direction and type are in the same field
        dst->SetTrackType(trackType);
        dst->SetTrackDirection(src->GetTrackDirection());
    }
    dst->track_x = src->TrackLocation.x;
    dst->track_y = src->TrackLocation.y;
    dst->track_z = src->TrackLocation.z;
    dst->next_vehicle_on_train = src->next_vehicle_on_train;
    dst->prev_vehicle_on_ride = src->prev_vehicle_on_ride;
    dst->next_vehicle_on_ride = src->next_vehicle_on_ride;
    dst->var_44 = src->var_44;
    dst->mass = src->mass;
    dst->update_flags = src->update_flags;
    dst->SwingSprite = src->SwingSprite;
    dst->current_station = src->current_station;
    dst->current_time = src->current_time;
    dst->crash_z = src->crash_z;
    dst->status = static_cast<uint8_t>(src->status);
    dst->sub_state = src->sub_state;
    for (size_t i = 0; i < std::size(src->peep); i++)
    {
        dst->peep[i] = src->peep[i];
        dst->peep_tshirt_colours[i] = src->peep_tshirt_colours[i];
    }
    dst->num_seats = src->num_seats;
    dst->num_peeps = src->num_peeps;
    dst->next_free_seat = src->next_free_seat;
    dst->restraints_position = src->restraints_position;
    dst->crash_x = src->crash_x;
    dst->sound2_flags = src->sound2_flags;
    dst->spin_sprite = src->spin_sprite;
    dst->sound1_id = static_cast<uint8_t>(src->sound1_id);
    dst->sound1_volume = src->sound1_volume;
    dst->sound2_id = static_cast<uint8_t>(src->sound2_id);
    dst->sound2_volume = src->sound2_volume;
    dst->sound_vector_factor = src->sound_vector_factor;
    dst->time_waiting = src->time_waiting;
    dst->speed = src->speed;
    dst->powered_acceleration = src->powered_acceleration;
    dst->dodgems_collision_direction = src->dodgems_collision_direction;
    dst->animation_frame = src->animation_frame;
    dst->var_C8 = src->var_C8;
    dst->var_CA = src->var_CA;
    dst->scream_sound_id = static_cast<uint8_t>(src->scream_sound_id);
    dst->TrackSubposition = static_cast<uint8_t>(src->TrackSubposition);
    dst->var_CE = src->var_CE;
    dst->var_CF = src->var_CF;
    dst->lost_time_out = src->lost_time_out;
    dst->vertical_drop_countdown = src->vertical_drop_countdown;
    dst->var_D3 = src->var_D3;
    dst->mini_golf_current_animation = src->mini_golf_current_animation;
    dst->mini_golf_flags = src->mini_golf_flags;
    dst->ride_subtype = OpenRCT2EntryIndexToRCTEntryIndex(src->ride_subtype);
    dst->colours_extended = src->colours_extended;
    dst->seat_rotation = src->seat_rotation;
    dst->target_seat_rotation = src->target_seat_rotation;
}

void S6Exporter::ExportSpritePeep(RCT2SpritePeep* dst, const Peep* src)
{
    ExportSpriteCommonProperties(dst, static_cast<const SpriteBase*>(src));

    auto generateName = true;
    if (src->Name != nullptr)
    {
        auto stringId = AllocateUserString(src->Name);
        if (stringId != std::nullopt)
        {
            dst->name_string_idx = *stringId;
            generateName = false;
        }
        else
        {
            log_warning(
                "Unable to allocate user string for peep #%d (%s) during S6 export.", static_cast<int>(src->sprite_index),
                src->Name);
        }
    }
    if (generateName)
    {
        if (src->AssignedPeepType == PeepType::Staff)
        {
            static constexpr const rct_string_id staffNames[] = {
                STR_HANDYMAN_X,
                STR_MECHANIC_X,
                STR_SECURITY_GUARD_X,
                STR_ENTERTAINER_X,
            };
            dst->name_string_idx = staffNames[static_cast<uint8_t>(src->AssignedStaffType) % sizeof(staffNames)];
        }
        else if (gParkFlags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
        {
            dst->name_string_idx = get_real_name_string_id_from_id(src->Id);
        }
        else
        {
            dst->name_string_idx = STR_GUEST_X;
        }
    }

    dst->next_x = src->NextLoc.x;
    dst->next_y = src->NextLoc.y;
    dst->next_z = src->NextLoc.z / COORDS_Z_STEP;
    dst->next_flags = src->NextFlags;
    dst->outside_of_park = static_cast<uint8_t>(src->OutsideOfPark);
    dst->state = static_cast<uint8_t>(src->State);
    dst->sub_state = src->SubState;
    dst->sprite_type = static_cast<uint8_t>(src->SpriteType);
    dst->peep_type = static_cast<uint8_t>(src->AssignedPeepType);
    dst->no_of_rides = src->GuestNumRides;
    dst->tshirt_colour = src->TshirtColour;
    dst->trousers_colour = src->TrousersColour;
    dst->destination_x = src->DestinationX;
    dst->destination_y = src->DestinationY;
    dst->destination_tolerance = src->DestinationTolerance;
    dst->var_37 = src->Var37;
    dst->energy = src->Energy;
    dst->energy_target = src->EnergyTarget;
    dst->happiness = src->Happiness;
    dst->happiness_target = src->HappinessTarget;
    dst->nausea = src->Nausea;
    dst->nausea_target = src->NauseaTarget;
    dst->hunger = src->Hunger;
    dst->thirst = src->Thirst;
    dst->toilet = src->Toilet;
    dst->mass = src->Mass;
    dst->time_to_consume = src->TimeToConsume;
    dst->intensity = static_cast<uint8_t>(src->Intensity);
    dst->nausea_tolerance = EnumValue(src->NauseaTolerance);
    dst->window_invalidate_flags = src->WindowInvalidateFlags;
    dst->paid_on_drink = src->PaidOnDrink;
    for (size_t i = 0; i < std::size(src->RideTypesBeenOn); i++)
    {
        dst->ride_types_been_on[i] = src->RideTypesBeenOn[i];
    }
    dst->item_extra_flags = static_cast<uint32_t>(src->GetItemFlags() >> 32);
    dst->photo2_ride_ref = OpenRCT2RideIdToRCT12RideId(src->Photo2RideRef);
    dst->photo3_ride_ref = OpenRCT2RideIdToRCT12RideId(src->Photo3RideRef);
    dst->photo4_ride_ref = OpenRCT2RideIdToRCT12RideId(src->Photo4RideRef);
    dst->current_ride = OpenRCT2RideIdToRCT12RideId(src->CurrentRide);
    dst->current_ride_station = src->CurrentRideStation;
    dst->current_train = src->CurrentTrain;
    dst->time_to_sitdown = src->TimeToSitdown;
    dst->special_sprite = src->SpecialSprite;
    dst->action_sprite_type = static_cast<uint8_t>(src->ActionSpriteType);
    dst->next_action_sprite_type = static_cast<uint8_t>(src->NextActionSpriteType);
    dst->action_sprite_image_offset = src->ActionSpriteImageOffset;
    dst->action = static_cast<uint8_t>(src->Action);
    dst->action_frame = src->ActionFrame;
    dst->step_progress = src->StepProgress;
    dst->next_in_queue = src->GuestNextInQueue;
    dst->direction = src->PeepDirection;
    dst->interaction_ride_index = OpenRCT2RideIdToRCT12RideId(src->InteractionRideIndex);
    dst->time_in_queue = src->TimeInQueue;
    for (size_t i = 0; i < std::size(src->RidesBeenOn); i++)
    {
        dst->rides_been_on[i] = src->RidesBeenOn[i];
    }
    dst->id = src->Id;
    dst->cash_in_pocket = src->CashInPocket;
    dst->cash_spent = src->CashSpent;
    dst->park_entry_time = src->ParkEntryTime;
    dst->rejoin_queue_timeout = src->RejoinQueueTimeout;
    dst->previous_ride = OpenRCT2RideIdToRCT12RideId(src->PreviousRide);
    dst->previous_ride_time_out = src->PreviousRideTimeOut;
    for (size_t i = 0; i < std::size(src->Thoughts); i++)
    {
        auto srcThought = &src->Thoughts[i];
        auto dstThought = &dst->thoughts[i];
        dstThought->type = static_cast<uint8_t>(srcThought->type);
        dstThought->item = srcThought->item;
        dstThought->freshness = srcThought->freshness;
        dstThought->fresh_timeout = srcThought->fresh_timeout;
    }
    dst->path_check_optimisation = src->PathCheckOptimisation;
    dst->guest_heading_to_ride_id = OpenRCT2RideIdToRCT12RideId(src->GuestHeadingToRideId);
    dst->peep_is_lost_countdown = src->GuestIsLostCountdown;
    dst->photo1_ride_ref = OpenRCT2RideIdToRCT12RideId(src->Photo1RideRef);
    dst->peep_flags = src->PeepFlags;
    dst->pathfind_goal = src->PathfindGoal;
    for (size_t i = 0; i < std::size(src->PathfindHistory); i++)
    {
        dst->pathfind_history[i] = src->PathfindHistory[i];
    }
    dst->no_action_frame_num = src->WalkingFrameNum;
    dst->litter_count = src->LitterCount;
    dst->time_on_ride = src->GuestTimeOnRide;
    dst->disgusting_count = src->DisgustingCount;
    dst->paid_to_enter = src->PaidToEnter;
    dst->paid_on_rides = src->PaidOnRides;
    dst->paid_on_food = src->PaidOnFood;
    dst->paid_on_souvenirs = src->PaidOnSouvenirs;
    dst->no_of_food = src->AmountOfFood;
    dst->no_of_drinks = src->AmountOfDrinks;
    dst->no_of_souvenirs = src->AmountOfSouvenirs;
    dst->vandalism_seen = src->VandalismSeen;
    dst->voucher_type = src->VoucherType;
    dst->voucher_arguments = OpenRCT2RideIdToRCT12RideId(src->VoucherRideId);
    dst->surroundings_thought_timeout = src->SurroundingsThoughtTimeout;
    dst->angriness = src->Angriness;
    dst->time_lost = src->TimeLost;
    dst->days_in_queue = src->DaysInQueue;
    dst->balloon_colour = src->BalloonColour;
    dst->umbrella_colour = src->UmbrellaColour;
    dst->hat_colour = src->HatColour;
    dst->favourite_ride = OpenRCT2RideIdToRCT12RideId(src->FavouriteRide);
    dst->favourite_ride_rating = src->FavouriteRideRating;
    dst->item_standard_flags = static_cast<uint32_t>(src->GetItemFlags());
}

void S6Exporter::ExportSpriteMisc(RCT12SpriteBase* cdst, const MiscEntity* csrc)
{
    ExportSpriteCommonProperties(cdst, csrc);
    cdst->type = EnumValue(csrc->SubType);
    switch (csrc->SubType)
    {
        case MiscEntityType::SteamParticle:
        {
            auto src = static_cast<const SteamParticle*>(csrc);
            auto dst = static_cast<RCT12SpriteSteamParticle*>(cdst);
            dst->time_to_move = src->time_to_move;
            dst->frame = src->frame;
            break;
        }
        case MiscEntityType::MoneyEffect:
        {
            auto src = static_cast<const MoneyEffect*>(csrc);
            auto dst = static_cast<RCT12SpriteMoneyEffect*>(cdst);
            dst->move_delay = src->MoveDelay;
            dst->num_movements = src->NumMovements;
            dst->vertical = src->Vertical;
            dst->value = src->Value;
            dst->offset_x = src->OffsetX;
            dst->wiggle = src->Wiggle;
            break;
        }
        case MiscEntityType::CrashedVehicleParticle:
        {
            auto src = static_cast<const VehicleCrashParticle*>(csrc);
            auto dst = static_cast<RCT12SpriteCrashedVehicleParticle*>(cdst);
            dst->frame = src->frame;
            dst->time_to_live = src->time_to_live;
            dst->frame = src->frame;
            dst->colour[0] = src->colour[0];
            dst->colour[1] = src->colour[1];
            dst->crashed_sprite_base = src->crashed_sprite_base;
            dst->velocity_x = src->velocity_x;
            dst->velocity_y = src->velocity_y;
            dst->velocity_z = src->velocity_z;
            dst->acceleration_x = src->acceleration_x;
            dst->acceleration_y = src->acceleration_y;
            dst->acceleration_z = src->acceleration_z;
            break;
        }
        case MiscEntityType::ExplosionCloud:
        case MiscEntityType::ExplosionFlare:
        case MiscEntityType::CrashSplash:
        {
            auto src = static_cast<const MiscEntity*>(csrc);
            auto dst = static_cast<RCT12SpriteParticle*>(cdst);
            dst->frame = src->frame;
            break;
        }
        case MiscEntityType::JumpingFountainWater:
        case MiscEntityType::JumpingFountainSnow:
        {
            auto* src = static_cast<const JumpingFountain*>(csrc);
            auto* dst = static_cast<RCT12SpriteJumpingFountain*>(cdst);
            dst->num_ticks_alive = src->NumTicksAlive;
            dst->frame = src->frame;
            dst->fountain_flags = src->FountainFlags;
            dst->target_x = src->TargetX;
            dst->target_y = src->TargetY;
            dst->target_y = src->TargetY;
            dst->iteration = src->Iteration;
            break;
        }
        case MiscEntityType::Balloon:
        {
            auto src = static_cast<const Balloon*>(csrc);
            auto dst = static_cast<RCT12SpriteBalloon*>(cdst);
            dst->popped = src->popped;
            dst->time_to_move = src->time_to_move;
            dst->frame = src->frame;
            dst->colour = src->colour;
            break;
        }
        case MiscEntityType::Duck:
        {
            auto src = static_cast<const Duck*>(csrc);
            auto dst = static_cast<RCT12SpriteDuck*>(cdst);
            dst->frame = src->frame;
            dst->target_x = src->target_x;
            dst->target_y = src->target_y;
            dst->state = EnumValue(src->state);
            break;
        }
        default:
            log_warning("Misc. sprite type %d can not be exported.", cdst->type);
            break;
    }
}

void S6Exporter::ExportSpriteLitter(RCT12SpriteLitter* dst, const Litter* src)
{
    ExportSpriteCommonProperties(dst, src);
    dst->type = EnumValue(src->SubType);
    dst->creationTick = src->creationTick;
}

void S6Exporter::ExportBanners()
{
    for (BannerIndex i = 0; i < RCT2_MAX_BANNERS_IN_PARK; i++)
    {
        auto src = GetBanner(i);
        auto dst = &_s6.banners[i];
        ExportBanner(*dst, *src);
    }
}

void S6Exporter::ExportBanner(RCT12Banner& dst, const Banner& src)
{
    dst = {};
    dst.type = OpenRCT2EntryIndexToRCTEntryIndex(src.type);

    if (!src.IsNull())
    {
        dst.flags = src.flags;

        dst.string_idx = STR_DEFAULT_SIGN;

        std::string bannerText;
        if (!(src.flags & BANNER_FLAG_IS_WALL) && !(src.flags & BANNER_FLAG_IS_LARGE_SCENERY))
        {
            auto formatCode = static_cast<codepoint_t>(RCT2_STRING_FORMAT_COLOUR_START + src.text_colour);
            String::AppendCodepoint(bannerText, formatCode);
        }
        bannerText.append(src.text);

        auto stringId = AllocateUserString(bannerText);
        if (stringId != std::nullopt)
        {
            dst.string_idx = *stringId;
        }

        if (src.flags & BANNER_FLAG_LINKED_TO_RIDE)
        {
            dst.ride_index = src.ride_index;
        }
        else
        {
            dst.colour = src.colour;
        }
        dst.text_colour = src.text_colour;
        dst.x = src.position.x;
        dst.y = src.position.y;
    }
}

void S6Exporter::ExportMapAnimations()
{
    const auto& mapAnimations = GetMapAnimations();
    auto numAnimations = std::min(mapAnimations.size(), std::size(_s6.map_animations));
    _s6.num_map_animations = static_cast<uint16_t>(numAnimations);
    for (size_t i = 0; i < numAnimations; i++)
    {
        const auto& src = mapAnimations[i];
        auto& dst = _s6.map_animations[i];

        dst.type = src.type;
        dst.x = src.location.x;
        dst.y = src.location.y;
        dst.baseZ = src.location.z;
    }
}

void S6Exporter::ExportTileElements()
{
    for (uint32_t index = 0; index < RCT2_MAX_TILE_ELEMENTS; index++)
    {
        auto src = &gTileElements[index];
        auto dst = &_s6.tile_elements[index];
        if (src->base_height == MAX_ELEMENT_HEIGHT)
        {
            std::memcpy(dst, src, sizeof(*dst));
        }
        else
        {
            auto tileElementType = static_cast<RCT12TileElementType>(src->GetType());
            if (tileElementType == RCT12TileElementType::Corrupt || tileElementType == RCT12TileElementType::EightCarsCorrupt14
                || tileElementType == RCT12TileElementType::EightCarsCorrupt15)
                std::memcpy(dst, src, sizeof(*dst));
            else
                ExportTileElement(dst, src);
        }
    }
    _s6.next_free_tile_element_pointer_index = gNextFreeTileElementPointerIndex;
}

void S6Exporter::ExportTileElement(RCT12TileElement* dst, TileElement* src)
{
    // Todo: allow for changing definition of OpenRCT2 tile element types - replace with a map
    uint8_t tileElementType = src->GetType();
    dst->ClearAs(tileElementType);
    dst->SetDirection(src->GetDirection());
    dst->base_height = src->base_height;
    dst->clearance_height = src->clearance_height;

    // All saved in "flags"
    dst->SetOccupiedQuadrants(src->GetOccupiedQuadrants());
    dst->SetGhost(src->IsGhost());
    dst->SetLastForTile(src->IsLastForTile());

    switch (tileElementType)
    {
        case TILE_ELEMENT_TYPE_SURFACE:
        {
            auto dst2 = dst->AsSurface();
            auto src2 = src->AsSurface();

            dst2->SetSlope(src2->GetSlope());
            dst2->SetSurfaceStyle(src2->GetSurfaceStyle());
            dst2->SetEdgeStyle(src2->GetEdgeStyle());
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

            dst2->SetPathEntryIndex(src2->GetSurfaceEntryIndex());
            dst2->SetQueueBannerDirection(src2->GetQueueBannerDirection());
            dst2->SetSloped(src2->IsSloped());
            dst2->SetSlopeDirection(src2->GetSlopeDirection());
            dst2->SetRideIndex(src2->GetRideIndex());
            dst2->SetStationIndex(src2->GetStationIndex());
            dst2->SetWide(src2->IsWide());
            dst2->SetIsQueue(src2->IsQueue());
            dst2->SetHasQueueBanner(src2->HasQueueBanner());
            dst2->SetEdges(src2->GetEdges());
            dst2->SetCorners(src2->GetCorners());
            dst2->SetAddition(src2->GetAddition());
            dst2->SetAdditionIsGhost(src2->AdditionIsGhost());
            dst2->SetAdditionStatus(src2->GetAdditionStatus());
            dst2->SetIsBroken(src2->IsBroken());
            dst2->SetIsBlockedByVehicle(src2->IsBlockedByVehicle());

            break;
        }
        case TILE_ELEMENT_TYPE_TRACK:
        {
            auto dst2 = dst->AsTrack();
            auto src2 = src->AsTrack();

            auto trackType = src2->GetTrackType();
            if (trackType == TrackElemType::RotationControlToggle)
                trackType = TrackElemType::RotationControlToggleAlias;
            // SV6 track type is uint8_t; we have aliased Rotation Control (256) to Booster (100) to allow this
            dst2->SetTrackType(static_cast<uint8_t>(trackType));
            dst2->SetSequenceIndex(src2->GetSequenceIndex());
            dst2->SetRideIndex(src2->GetRideIndex());
            dst2->SetColourScheme(src2->GetColourScheme());
            dst2->SetStationIndex(src2->GetStationIndex());
            dst2->SetHasGreenLight(src2->HasGreenLight());
            dst2->SetHasChain(src2->HasChain());
            dst2->SetHasCableLift(src2->HasCableLift());
            dst2->SetInverted(src2->IsInverted());
            dst2->SetBrakeBoosterSpeed(src2->GetBrakeBoosterSpeed());
            dst2->SetPhotoTimeout(src2->GetPhotoTimeout());
            dst2->SetBlockBrakeClosed(src2->BlockBrakeClosed());
            dst2->SetIsIndestructible(src2->IsIndestructible());

            // Skipping IsHighlighted()

            // This has to be done last, since the maze entry shares fields with the colour and sequence fields.
            auto ride = get_ride(dst2->GetRideIndex());
            if (ride)
            {
                if (ride->type == RIDE_TYPE_MAZE)
                {
                    dst2->SetMazeEntry(src2->GetMazeEntry());
                }
                else if (ride->type == RIDE_TYPE_GHOST_TRAIN)
                {
                    dst2->SetDoorAState(src2->GetDoorAState());
                    dst2->SetDoorBState(src2->GetDoorBState());
                }
                else
                {
                    dst2->SetSeatRotation(src2->GetSeatRotation());
                }
            }
            // _Should_ not happen, but if it does, pick the most likely option.
            else
            {
                dst2->SetSeatRotation(src2->GetSeatRotation());
            }

            break;
        }
        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
        {
            auto dst2 = dst->AsSmallScenery();
            auto src2 = src->AsSmallScenery();

            dst2->SetEntryIndex(src2->GetEntryIndex());
            dst2->SetAge(src2->GetAge());
            dst2->SetSceneryQuadrant(src2->GetSceneryQuadrant());
            dst2->SetPrimaryColour(src2->GetPrimaryColour());
            dst2->SetSecondaryColour(src2->GetSecondaryColour());
            if (src2->NeedsSupports())
                dst2->SetNeedsSupports();

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
            dst2->SetPathType(src2->GetPathType());

            break;
        }
        case TILE_ELEMENT_TYPE_WALL:
        {
            auto dst2 = dst->AsWall();
            auto src2 = src->AsWall();

            dst2->SetEntryIndex(src2->GetEntryIndex());
            dst2->SetSlope(src2->GetSlope());
            dst2->SetPrimaryColour(src2->GetPrimaryColour());
            dst2->SetSecondaryColour(src2->GetSecondaryColour());
            dst2->SetTertiaryColour(src2->GetTertiaryColour());
            dst2->SetAnimationFrame(src2->GetAnimationFrame());
            dst2->SetAcrossTrack(src2->IsAcrossTrack());
            dst2->SetAnimationIsBackwards(src2->AnimationIsBackwards());

            auto entry = src2->GetEntry();
            if (entry != nullptr && entry->wall.scrolling_mode != SCROLLING_MODE_NONE)
            {
                auto bannerIndex = src2->GetBannerIndex();
                if (bannerIndex != BANNER_INDEX_NULL)
                    dst2->SetBannerIndex(bannerIndex);
                else
                    dst2->SetBannerIndex(RCT12_BANNER_INDEX_NULL);
            }

            break;
        }
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        {
            auto dst2 = dst->AsLargeScenery();
            auto src2 = src->AsLargeScenery();

            dst2->SetEntryIndex(src2->GetEntryIndex());
            dst2->SetSequenceIndex(src2->GetSequenceIndex());
            dst2->SetPrimaryColour(src2->GetPrimaryColour());
            dst2->SetSecondaryColour(src2->GetSecondaryColour());

            auto entry = src2->GetEntry();
            if (entry != nullptr && entry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
            {
                auto bannerIndex = src2->GetBannerIndex();
                if (bannerIndex != BANNER_INDEX_NULL)
                    dst2->SetBannerIndex(bannerIndex);
                else
                    dst2->SetBannerIndex(RCT12_BANNER_INDEX_NULL);
            }

            break;
        }
        case TILE_ELEMENT_TYPE_BANNER:
        {
            auto dst2 = dst->AsBanner();
            auto src2 = src->AsBanner();

            dst2->SetPosition(src2->GetPosition());
            dst2->SetAllowedEdges(src2->GetAllowedEdges());
            auto bannerIndex = src2->GetIndex();
            if (bannerIndex != BANNER_INDEX_NULL)
                dst2->SetIndex(bannerIndex);
            else
                dst2->SetIndex(RCT12_BANNER_INDEX_NULL);
            break;
        }
        default:
            assert(false);
    }
}

std::optional<uint16_t> S6Exporter::AllocateUserString(std::string_view value)
{
    auto nextId = _userStrings.size();
    if (nextId < RCT12_MAX_USER_STRINGS)
    {
        _userStrings.emplace_back(value);
        return static_cast<uint16_t>(USER_STRING_START + nextId);
    }
    return std::nullopt;
}

void S6Exporter::ExportUserStrings()
{
    auto numUserStrings = std::min<size_t>(_userStrings.size(), RCT12_MAX_USER_STRINGS);
    for (size_t i = 0; i < numUserStrings; i++)
    {
        auto dst = _s6.custom_strings[i];
        const auto& src = _userStrings[i];
        auto encodedSrc = GetTruncatedRCT2String(src, RCT12_USER_STRING_MAX_LENGTH);
        auto stringLen = std::min<size_t>(encodedSrc.size(), RCT12_USER_STRING_MAX_LENGTH - 1);
        std::memcpy(dst, encodedSrc.data(), stringLen);
    }
}

enum : uint32_t
{
    S6_SAVE_FLAG_EXPORT = 1 << 0,
    S6_SAVE_FLAG_SCENARIO = 1 << 1,
    S6_SAVE_FLAG_AUTOMATIC = 1u << 31,
};

/**
 *
 *  rct2: 0x006754F5
 * @param flags bit 0: pack objects, 1: save as scenario
 */
int32_t scenario_save(const utf8* path, int32_t flags)
{
    if (flags & S6_SAVE_FLAG_SCENARIO)
    {
        log_verbose("scenario_save(%s, SCENARIO)", path);
    }
    else
    {
        log_verbose("scenario_save(%s, SAVED GAME)", path);
    }

    if (!(flags & S6_SAVE_FLAG_AUTOMATIC))
    {
        window_close_construction_windows();
    }

    map_reorganise_elements();
    viewport_set_saved_view();

    bool result = false;
    auto s6exporter = new S6Exporter();
    try
    {
        if (flags & S6_SAVE_FLAG_EXPORT)
        {
            auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
            s6exporter->ExportObjectsList = objManager.GetPackableObjects();
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
    catch (const std::exception& e)
    {
        log_error("Unable to save park: '%s'", e.what());
    }
    delete s6exporter;

    gfx_invalidate_screen();

    if (result && !(flags & S6_SAVE_FLAG_AUTOMATIC))
    {
        gScreenAge = 0;
    }
    return result;
}
