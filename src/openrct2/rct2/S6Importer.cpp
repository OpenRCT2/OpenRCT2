/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/FileStream.hpp"
#include "../core/IStream.hpp"
#include "../core/Path.hpp"
#include "../core/Random.hpp"
#include "../core/String.hpp"
#include "../interface/Viewport.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../management/Research.h"
#include "../network/network.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../peep/Staff.h"
#include "../rct12/SawyerChunkReader.h"
#include "../rct12/SawyerEncoding.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideRatings.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"

#include <algorithm>

/**
 * Class to import RollerCoaster Tycoon 2 scenarios (*.SC6) and saved games (*.SV6).
 */
class S6Importer final : public IParkImporter
{
private:
    IObjectRepository& _objectRepository;

    const utf8* _s6Path = nullptr;
    rct_s6_data _s6{};
    uint8_t _gameVersion = 0;
    bool _isSV7 = false;

public:
    S6Importer(IObjectRepository& objectRepository)
        : _objectRepository(objectRepository)
    {
    }

    ParkLoadResult Load(const utf8* path) override
    {
        const utf8* extension = Path::GetExtension(path);
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
            throw std::runtime_error("Invalid RCT2 park extension.");
        }
    }

    ParkLoadResult LoadSavedGame(const utf8* path, bool skipObjectCheck = false) override
    {
        auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);
        auto result = LoadFromStream(&fs, false, skipObjectCheck);
        _s6Path = path;
        return result;
    }

    ParkLoadResult LoadScenario(const utf8* path, bool skipObjectCheck = false) override
    {
        auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);
        auto result = LoadFromStream(&fs, true, skipObjectCheck);
        _s6Path = path;
        return result;
    }

    ParkLoadResult LoadFromStream(
        OpenRCT2::IStream* stream, bool isScenario, [[maybe_unused]] bool skipObjectCheck = false,
        const utf8* path = String::Empty) override
    {
        if (isScenario && !gConfigGeneral.allow_loading_with_incorrect_checksum && !SawyerEncoding::ValidateChecksum(stream))
        {
            throw IOException("Invalid checksum.");
        }

        auto chunkReader = SawyerChunkReader(stream);
        chunkReader.ReadChunk(&_s6.header, sizeof(_s6.header));

        log_verbose("saved game classic_flag = 0x%02x", _s6.header.classic_flag);
        if (isScenario)
        {
            if (_s6.header.type != S6_TYPE_SCENARIO)
            {
                throw std::runtime_error("Park is not a scenario.");
            }
            chunkReader.ReadChunk(&_s6.info, sizeof(_s6.info));
        }
        else
        {
            if (_s6.header.type != S6_TYPE_SAVEDGAME)
            {
                throw std::runtime_error("Park is not a saved game.");
            }
        }

        if (_s6.header.classic_flag == 0xf)
        {
            throw UnsupportedRCTCFlagException(_s6.header.classic_flag);
        }

        // Read packed objects
        // TODO try to contain this more and not store objects until later
        for (uint16_t i = 0; i < _s6.header.num_packed_objects; i++)
        {
            _objectRepository.ExportPackedObject(stream);
        }

        if (path)
        {
            auto extension = path_get_extension(path);
            _isSV7 = _stricmp(extension, ".sv7") == 0;
        }

        if (isScenario)
        {
            chunkReader.ReadChunk(&_s6.objects, sizeof(_s6.objects));
            chunkReader.ReadChunk(&_s6.elapsed_months, 16);
            chunkReader.ReadChunk(&_s6.tile_elements, sizeof(_s6.tile_elements));
            chunkReader.ReadChunk(&_s6.next_free_tile_element_pointer_index, 2560076);
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
            chunkReader.ReadChunk(&_s6.tile_elements, sizeof(_s6.tile_elements));
            chunkReader.ReadChunk(&_s6.next_free_tile_element_pointer_index, 3048816);
        }

        _s6Path = path;

        return ParkLoadResult(GetRequiredObjects());
    }

    bool GetDetails(scenario_index_entry* dst) override
    {
        *dst = {};
        return false;
    }

    void Import() override
    {
        Initialise();

        // _s6.header
        gS6Info = _s6.info;

        // Some scenarios have their scenario details in UTF-8, due to earlier bugs in OpenRCT2.
        // This is hard to detect. Therefore, consider invalid characters like colour codes as a sign the text is in UTF-8.
        bool alreadyInUTF8 = false;

        if (String::ContainsColourCode(_s6.info.name) || String::ContainsColourCode(_s6.info.details))
        {
            alreadyInUTF8 = true;
        }

        if (!alreadyInUTF8)
        {
            auto temp = rct2_to_utf8(_s6.info.name, RCT2_LANGUAGE_ID_ENGLISH_UK);
            safe_strcpy(gS6Info.name, temp.data(), sizeof(gS6Info.name));
            auto temp2 = rct2_to_utf8(_s6.info.details, RCT2_LANGUAGE_ID_ENGLISH_UK);
            safe_strcpy(gS6Info.details, temp2.data(), sizeof(gS6Info.details));
        }
        else
        {
            safe_strcpy(gS6Info.name, _s6.info.name, sizeof(gS6Info.name));
            safe_strcpy(gS6Info.details, _s6.info.details, sizeof(gS6Info.details));
        }

        gDateMonthsElapsed = static_cast<int32_t>(_s6.elapsed_months);
        gDateMonthTicks = _s6.current_day;
        gScenarioTicks = _s6.scenario_ticks;

        scenario_rand_seed(_s6.scenario_srand_0, _s6.scenario_srand_1);

        ImportTileElements();
        ImportSprites();

        gInitialCash = _s6.initial_cash;
        gBankLoan = _s6.current_loan;
        gParkFlags = _s6.park_flags;
        gParkEntranceFee = _s6.park_entrance_fee;
        // rct1_park_entrance_x
        // rct1_park_entrance_y
        // pad_013573EE
        // rct1_park_entrance_z

        ImportPeepSpawns();

        gGuestChangeModifier = _s6.guest_count_change_modifier;
        gResearchFundingLevel = _s6.current_research_level;
        // pad_01357400
        ImportResearchedRideTypes();
        ImportResearchedRideEntries();
        // _s6.researched_track_types_a
        // _s6.researched_track_types_b

        gNumGuestsInPark = _s6.guests_in_park;
        gNumGuestsHeadingForPark = _s6.guests_heading_for_park;

        for (size_t i = 0; i < RCT12_EXPENDITURE_TABLE_MONTH_COUNT; i++)
        {
            for (size_t j = 0; j < RCT12_EXPENDITURE_TYPE_COUNT; j++)
            {
                gExpenditureTable[i][j] = _s6.expenditure_table[i][j];
            }
        }

        gNumGuestsInParkLastWeek = _s6.last_guests_in_park;
        // pad_01357BCA
        gStaffHandymanColour = _s6.handyman_colour;
        gStaffMechanicColour = _s6.mechanic_colour;
        gStaffSecurityColour = _s6.security_colour;

        ImportResearchedSceneryItems();

        gParkRating = _s6.park_rating;

        std::memcpy(gParkRatingHistory, _s6.park_rating_history, sizeof(_s6.park_rating_history));
        std::memcpy(gGuestsInParkHistory, _s6.guests_in_park_history, sizeof(_s6.guests_in_park_history));

        gResearchPriorities = _s6.active_research_types;
        gResearchProgressStage = _s6.research_progress_stage;
        if (_s6.last_researched_item_subject != RCT12_RESEARCHED_ITEMS_SEPARATOR)
            gResearchLastItem = ResearchItem(
                RCT12ResearchItem{ _s6.last_researched_item_subject, RESEARCH_CATEGORY_TRANSPORT });
        else
            gResearchLastItem = std::nullopt;
        // pad_01357CF8
        if (_s6.next_research_item != RCT12_RESEARCHED_ITEMS_SEPARATOR)
            gResearchNextItem = ResearchItem(RCT12ResearchItem{ _s6.next_research_item, _s6.next_research_category });
        else
            gResearchNextItem = std::nullopt;

        gResearchProgress = _s6.research_progress;
        gResearchExpectedDay = _s6.next_research_expected_day;
        gResearchExpectedMonth = _s6.next_research_expected_month;
        gGuestInitialHappiness = _s6.guest_initial_happiness;
        gParkSize = _s6.park_size;
        _guestGenerationProbability = _s6.guest_generation_probability;
        gTotalRideValueForMoney = _s6.total_ride_value_for_money;
        gMaxBankLoan = _s6.maximum_loan;
        gGuestInitialCash = _s6.guest_initial_cash;
        gGuestInitialHunger = _s6.guest_initial_hunger;
        gGuestInitialThirst = _s6.guest_initial_thirst;
        gScenarioObjective.Type = _s6.objective_type;
        gScenarioObjective.Year = _s6.objective_year;
        // pad_013580FA
        gScenarioObjective.Currency = _s6.objective_currency;
        // In RCT2, the ride string IDs start at index STR_0002 and are directly mappable.
        // This is not always the case in OpenRCT2, so we use the actual ride ID.
        if (gScenarioObjective.Type == OBJECTIVE_BUILD_THE_BEST)
            gScenarioObjective.RideId = _s6.objective_guests - RCT2_RIDE_STRING_START;
        else
            gScenarioObjective.NumGuests = _s6.objective_guests;
        ImportMarketingCampaigns();

        gCurrentExpenditure = _s6.current_expenditure;
        gCurrentProfit = _s6.current_profit;
        gWeeklyProfitAverageDividend = _s6.weekly_profit_average_dividend;
        gWeeklyProfitAverageDivisor = _s6.weekly_profit_average_divisor;
        // pad_0135833A

        gParkValue = _s6.park_value;

        for (size_t i = 0; i < RCT12_FINANCE_GRAPH_SIZE; i++)
        {
            gCashHistory[i] = _s6.balance_history[i];
            gWeeklyProfitHistory[i] = _s6.weekly_profit_history[i];
            gParkValueHistory[i] = _s6.park_value_history[i];
        }

        gScenarioCompletedCompanyValue = _s6.completed_company_value;
        gTotalAdmissions = _s6.total_admissions;
        gTotalIncomeFromAdmissions = _s6.income_from_admissions;
        gCompanyValue = _s6.company_value;
        std::memcpy(gPeepWarningThrottle, _s6.peep_warning_throttle, sizeof(_s6.peep_warning_throttle));

        // Awards
        for (int32_t i = 0; i < RCT12_MAX_AWARDS; i++)
        {
            rct12_award* src = &_s6.awards[i];
            Award* dst = &gCurrentAwards[i];
            dst->Time = src->time;
            dst->Type = src->type;
        }

        gLandPrice = _s6.land_price;
        gConstructionRightsPrice = _s6.construction_rights_price;
        // unk_01358774
        // pad_01358776
        // _s6.cd_key
        _gameVersion = _s6.game_version_number;
        gScenarioCompanyValueRecord = _s6.completed_company_value_record;
        // _s6.loan_hash;
        // pad_013587CA
        gHistoricalProfit = _s6.historical_profit;
        // pad_013587D4
        gScenarioCompletedBy = std::string_view(_s6.scenario_completed_name, sizeof(_s6.scenario_completed_name));
        gCash = DECRYPT_MONEY(_s6.cash);
        // pad_013587FC
        gParkRatingCasualtyPenalty = _s6.park_rating_casualty_penalty;
        gMapSizeUnits = _s6.map_size_units;
        gMapSizeMinus2 = _s6.map_size_minus_2;
        gMapSize = _s6.map_size;
        gMapSizeMaxXY = _s6.map_max_xy;
        gSamePriceThroughoutPark = _s6.same_price_throughout
            | (static_cast<uint64_t>(_s6.same_price_throughout_extended) << 32);
        _suggestedGuestMaximum = _s6.suggested_max_guests;
        gScenarioParkRatingWarningDays = _s6.park_rating_warning_days;
        gLastEntranceStyle = _s6.last_entrance_style;
        // rct1_water_colour
        // pad_01358842
        ImportResearchList();
        gMapBaseZ = _s6.map_base_z;
        gScenarioName = std::string_view(_s6.scenario_name, sizeof(_s6.scenario_name));
        gScenarioDetails = std::string_view(_s6.scenario_description, sizeof(_s6.scenario_description));
        gBankLoanInterestRate = _s6.current_interest_rate;
        // pad_0135934B
        // Preserve compatibility with vanilla RCT2's save format.
        gParkEntrances.clear();
        for (uint8_t i = 0; i < RCT12_MAX_PARK_ENTRANCES; i++)
        {
            if (_s6.park_entrance_x[i] != LOCATION_NULL)
            {
                CoordsXYZD entrance;
                entrance.x = _s6.park_entrance_x[i];
                entrance.y = _s6.park_entrance_y[i];
                entrance.z = _s6.park_entrance_z[i];
                entrance.direction = _s6.park_entrance_direction[i];
                gParkEntrances.push_back(entrance);
            }
        }
        if (_s6.header.type == S6_TYPE_SCENARIO)
        {
            // _s6.scenario_filename is wrong for some RCT2 expansion scenarios, so we use the real filename
            String::Set(gScenarioFileName, sizeof(gScenarioFileName), Path::GetFileName(_s6Path));
        }
        else
        {
            // For savegames the filename can be arbitrary, so we have no choice but to rely on the name provided
            String::Set(gScenarioFileName, sizeof(gScenarioFileName), _s6.scenario_filename);
        }
        std::memcpy(gScenarioExpansionPacks, _s6.saved_expansion_pack_names, sizeof(_s6.saved_expansion_pack_names));
        gCurrentTicks = _s6.game_ticks_1;
        gCurrentRealTimeTicks = 0;

        ImportRides();

        gSavedAge = _s6.saved_age;
        gSavedView = ScreenCoordsXY{ _s6.saved_view_x, _s6.saved_view_y };
        gSavedViewZoom = _s6.saved_view_zoom;
        gSavedViewRotation = _s6.saved_view_rotation;

        ImportRideRatingsCalcData();
        ImportRideMeasurements();
        gNextGuestNumber = _s6.next_guest_index;
        gGrassSceneryTileLoopPosition = _s6.grass_and_scenery_tilepos;
        std::memcpy(gStaffPatrolAreas, _s6.patrol_areas, sizeof(_s6.patrol_areas));
        std::memcpy(gStaffModes, _s6.staff_modes, sizeof(_s6.staff_modes));
        // unk_13CA73E
        // pad_13CA73F
        // unk_13CA740
        gClimate = ClimateType{ _s6.climate };
        // pad_13CA741;
        // byte_13CA742
        // pad_013CA747
        gClimateUpdateTimer = _s6.climate_update_timer;
        gClimateCurrent.Weather = _s6.current_weather;
        gClimateNext.Weather = _s6.next_weather;
        gClimateCurrent.Temperature = _s6.temperature;
        gClimateNext.Temperature = _s6.next_temperature;
        gClimateCurrent.WeatherEffect = WeatherEffectType{ _s6.current_weather_effect };
        gClimateNext.WeatherEffect = WeatherEffectType{ _s6.next_weather_effect };
        gClimateCurrent.WeatherGloom = _s6.current_weather_gloom;
        gClimateNext.WeatherGloom = _s6.next_weather_gloom;
        gClimateCurrent.Level = static_cast<WeatherLevel>(_s6.current_weather_level);
        gClimateNext.Level = static_cast<WeatherLevel>(_s6.next_weather_level);

        // News items
        News::InitQueue();
        for (size_t i = 0; i < RCT12_MAX_NEWS_ITEMS; i++)
        {
            const rct12_news_item* src = &_s6.news_items[i];
            News::Item* dst = &gNewsItems[i];
            if (src->Type < News::ItemTypeCount)
            {
                dst->Type = static_cast<News::ItemType>(src->Type);
                dst->Flags = src->Flags;
                dst->Assoc = src->Assoc;
                dst->Ticks = src->Ticks;
                dst->MonthYear = src->MonthYear;
                dst->Day = src->Day;
                std::memcpy(dst->Text, src->Text, sizeof(src->Text));
            }
            else
            {
                // In case where news item type is broken, consider all remaining news items invalid.
                log_error("Invalid news type 0x%x for news item %d, ignoring remaining news items", src->Type, i);
                // Still need to set the correct type to properly terminate the queue
                dst->Type = News::ItemType::Null;
                break;
            }
        }

        // pad_13CE730
        // rct1_scenario_flags
        gWidePathTileLoopX = _s6.wide_path_tile_loop_x;
        gWidePathTileLoopY = _s6.wide_path_tile_loop_y;
        // pad_13CE778

        // Fix and set dynamic variables
        map_strip_ghost_flag_from_elements();
        map_update_tile_pointers();
        game_convert_strings_to_utf8();
        map_count_remaining_land_rights();
        determine_ride_entrance_and_exit_locations();

        auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
        park.Name = GetUserString(_s6.park_name);

        // We try to fix the cycles on import, hence the 'true' parameter
        check_for_sprite_list_cycles(true);
        int32_t disjoint_sprites_count = fix_disjoint_sprites();
        // This one is less harmful, no need to assert for it ~janisozaur
        if (disjoint_sprites_count > 0)
        {
            log_error("Found %d disjoint null sprites", disjoint_sprites_count);
        }

        if (String::Equals(_s6.scenario_filename, "Europe - European Cultural Festival.SC6"))
        {
            // This scenario breaks pathfinding. Create passages between the worlds. (List is grouped by neighbouring tiles.)
            // clang-format off
            FixLandOwnershipTilesWithOwnership(
                {
                    { 67, 94 }, { 68, 94 }, { 69, 94 },
                    { 58, 24 }, { 58, 25 }, { 58, 26 }, { 58, 27 }, { 58, 28 }, { 58, 29 }, { 58, 30 }, { 58, 31 }, { 58, 32 },
                    { 26, 44 }, { 26, 45 },
                    { 32, 79 }, { 32, 80 }, { 32, 81 },
                },
                OWNERSHIP_OWNED);
            // clang-format on
        }

        research_determine_first_of_type();
    }

    void ImportRides()
    {
        for (uint8_t index = 0; index < RCT12_MAX_RIDES_IN_PARK; index++)
        {
            auto src = &_s6.rides[index];
            if (src->type != RIDE_TYPE_NULL)
            {
                auto dst = GetOrAllocateRide(index);
                ImportRide(dst, src, index);
            }
        }
    }

    void ImportRide(Ride* dst, const rct2_ride* src, const ride_id_t rideIndex)
    {
        *dst = {};
        dst->id = rideIndex;

        ObjectEntryIndex rideType = src->type;
        auto subtype = RCTEntryIndexToOpenRCT2EntryIndex(src->subtype);
        if (RCT2RideTypeNeedsConversion(src->type))
        {
            auto* rideEntry = get_ride_entry(subtype);
            if (rideEntry != nullptr)
            {
                rideType = RCT2RideTypeToOpenRCT2RideType(src->type, rideEntry);
            }
        }
        dst->type = rideType;
        dst->subtype = subtype;
        // pad_002;
        dst->mode = src->mode;
        dst->colour_scheme_type = src->colour_scheme_type;

        for (uint8_t i = 0; i < RCT2_MAX_CARS_PER_TRAIN; i++)
        {
            dst->vehicle_colours[i].Body = src->vehicle_colours[i].body_colour;
            dst->vehicle_colours[i].Trim = src->vehicle_colours[i].trim_colour;
        }

        // pad_046;
        dst->status = src->status;

        dst->default_name_number = src->name_arguments_number;
        if (is_user_string_id(src->name))
        {
            dst->custom_name = GetUserString(src->name);
        }
        else
        {
            dst->default_name_number = src->name_arguments_number;
        }

        if (src->overall_view.isNull())
        {
            dst->overall_view.setNull();
        }
        else
        {
            auto tileLoc = TileCoordsXY(src->overall_view.x, src->overall_view.y);
            dst->overall_view = tileLoc.ToCoordsXY();
        }

        for (int32_t i = 0; i < RCT12_MAX_STATIONS_PER_RIDE; i++)
        {
            if (src->station_starts[i].isNull())
            {
                dst->stations[i].Start.setNull();
            }
            else
            {
                auto tileStartLoc = TileCoordsXY(src->station_starts[i].x, src->station_starts[i].y);
                dst->stations[i].Start = tileStartLoc.ToCoordsXY();
            }
            dst->stations[i].Height = src->station_heights[i];
            dst->stations[i].Length = src->station_length[i];
            dst->stations[i].Depart = src->station_depart[i];
            dst->stations[i].TrainAtStation = src->train_at_station[i];
            // Direction is fixed later.

            if (src->entrances[i].isNull())
                ride_clear_entrance_location(dst, i);
            else
                ride_set_entrance_location(dst, i, { src->entrances[i].x, src->entrances[i].y, src->station_heights[i], 0 });

            if (src->exits[i].isNull())
                ride_clear_exit_location(dst, i);
            else
                ride_set_exit_location(dst, i, { src->exits[i].x, src->exits[i].y, src->station_heights[i], 0 });

            dst->stations[i].LastPeepInQueue = src->last_peep_in_queue[i];

            dst->stations[i].SegmentLength = src->length[i];
            dst->stations[i].SegmentTime = src->time[i];

            dst->stations[i].QueueTime = src->queue_time[i];

            dst->stations[i].QueueLength = src->queue_length[i];
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

        for (int32_t i = 0; i <= RCT2_MAX_VEHICLES_PER_RIDE; i++)
        {
            dst->vehicles[i] = src->vehicles[i];
        }
        for (int32_t i = RCT2_MAX_VEHICLES_PER_RIDE; i <= MAX_VEHICLES_PER_RIDE; i++)
        {
            dst->vehicles[i] = SPRITE_INDEX_NULL;
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
        dst->boat_hire_return_position = { src->boat_hire_return_position.x, src->boat_hire_return_position.y };

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

        if (src->cur_test_track_location.isNull())
        {
            dst->CurTestTrackLocation.setNull();
        }
        else
        {
            dst->CurTestTrackLocation = { src->cur_test_track_location.x, src->cur_test_track_location.y,
                                          src->cur_test_track_z };
        }

        dst->turn_count_default = src->turn_count_default;
        dst->turn_count_banked = src->turn_count_banked;
        dst->turn_count_sloped = src->turn_count_sloped;
        if (dst->type == RIDE_TYPE_MINI_GOLF)
            dst->holes = src->inversions & 0x1F;
        else
            dst->inversions = src->inversions & 0x1F;
        dst->sheltered_eighths = src->inversions >> 5;
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

        dst->price[0] = src->price;

        for (uint8_t i = 0; i < 2; i++)
        {
            dst->ChairliftBullwheelLocation[i] = { src->chairlift_bullwheel_location[i].x,
                                                   src->chairlift_bullwheel_location[i].y, src->chairlift_bullwheel_z[i] };
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

        ImportNumRiders(dst, rideIndex);

        dst->music_tune_id = src->music_tune_id;
        dst->slide_in_use = src->slide_in_use;
        // Includes maze_tiles
        dst->slide_peep = src->slide_peep;
        // pad_160[0xE];
        dst->slide_peep_t_shirt_colour = src->slide_peep_t_shirt_colour;
        // pad_16F[0x7];
        dst->spiral_slide_progress = src->spiral_slide_progress;
        // pad_177[0x9];
        dst->build_date = static_cast<int32_t>(src->build_date);
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

        dst->price[1] = src->price_secondary;

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
            dst->track_colour[i].main = src->track_colour_main[i];
            dst->track_colour[i].additional = src->track_colour_additional[i];
            dst->track_colour[i].supports = src->track_colour_supports[i];
        }
        // This stall was not colourable in RCT2.
        if (dst->type == RIDE_TYPE_FOOD_STALL)
        {
            auto entry = object_entry_get_entry(OBJECT_TYPE_RIDE, dst->subtype);
            if (entry != nullptr)
            {
                char name[DAT_NAME_LENGTH + 1];
                object_entry_get_name_fixed(name, sizeof(name), entry);
                if (strncmp(name, "ICECR1  ", DAT_NAME_LENGTH) == 0)
                {
                    dst->track_colour[0].main = COLOUR_LIGHT_BLUE;
                }
            }
        }

        dst->music = src->music;

        auto entranceStyle = src->entrance_style;
        // In SV7, "plain" entrances are invisible.
        if (_isSV7 && entranceStyle == RCT12_STATION_STYLE_PLAIN)
        {
            entranceStyle = RCT12_STATION_STYLE_INVISIBLE;
        }
        dst->entrance_style = entranceStyle;
        dst->vehicle_change_timeout = src->vehicle_change_timeout;
        dst->num_block_brakes = src->num_block_brakes;
        dst->lift_hill_speed = src->lift_hill_speed;
        dst->guests_favourite = src->guests_favourite;
        dst->lifecycle_flags = src->lifecycle_flags;

        for (uint8_t i = 0; i < RCT2_MAX_CARS_PER_TRAIN; i++)
        {
            dst->vehicle_colours[i].Ternary = src->vehicle_colours_extended[i];
        }

        dst->total_air_time = src->total_air_time;
        dst->current_test_station = src->current_test_station;
        dst->num_circuits = src->num_circuits;
        dst->CableLiftLoc = { src->cable_lift_x, src->cable_lift_y, src->cable_lift_z * COORDS_Z_STEP };
        // pad_1FD;
        dst->cable_lift = src->cable_lift;

        // pad_208[0x58];
    }

    void ImportRideRatingsCalcData()
    {
        const auto& src = _s6.ride_ratings_calc_data;
        auto& dst = gRideRatingsCalcData;
        dst = {};
        dst.Proximity = { src.proximity_x, src.proximity_y, src.proximity_z };
        dst.ProximityStart = { src.proximity_start_x, src.proximity_start_y, src.proximity_start_z };
        dst.CurrentRide = src.current_ride;
        dst.State = src.state;
        dst.ProximityTrackType = src.proximity_track_type;
        dst.ProximityBaseHeight = src.proximity_base_height;
        dst.ProximityTotal = src.proximity_total;
        for (size_t i = 0; i < std::size(src.proximity_scores); i++)
        {
            dst.ProximityScores[i] = src.proximity_scores[i];
        }
        dst.AmountOfBrakes = src.num_brakes;
        dst.AmountOfReversers = src.num_reversers;
        dst.StationFlags = src.station_flags;
    }

    void ImportRideMeasurements()
    {
        for (const auto& src : _s6.ride_measurements)
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
            dst.velocity[i] = src.velocity[i];
            dst.altitude[i] = src.altitude[i];
            dst.vertical[i] = src.vertical[i];
            dst.lateral[i] = src.lateral[i];
        }
    }

    void ImportResearchedRideTypes()
    {
        set_every_ride_type_not_invented();

        for (int32_t rideType = 0; rideType < RCT2_RIDE_TYPE_COUNT; rideType++)
        {
            int32_t quadIndex = rideType >> 5;
            int32_t bitIndex = rideType & 0x1F;
            bool invented = (_s6.researched_ride_types[quadIndex] & (1UL << bitIndex));

            if (invented)
                ride_type_set_invented(rideType);
        }
    }

    void ImportResearchedRideEntries()
    {
        set_every_ride_entry_not_invented();

        for (int32_t rideEntryIndex = 0; rideEntryIndex < MAX_RIDE_OBJECTS; rideEntryIndex++)
        {
            int32_t quadIndex = rideEntryIndex >> 5;
            int32_t bitIndex = rideEntryIndex & 0x1F;
            bool invented = (_s6.researched_ride_entries[quadIndex] & (1UL << bitIndex));

            if (invented)
                ride_entry_set_invented(rideEntryIndex);
        }
    }

    void ImportResearchedSceneryItems()
    {
        set_all_scenery_items_not_invented();

        for (uint16_t sceneryEntryIndex = 0; sceneryEntryIndex < RCT2_MAX_RESEARCHED_SCENERY_ITEMS; sceneryEntryIndex++)
        {
            int32_t quadIndex = sceneryEntryIndex >> 5;
            int32_t bitIndex = sceneryEntryIndex & 0x1F;
            bool invented = (_s6.researched_scenery_items[quadIndex] & (1UL << bitIndex));

            if (invented)
            {
                ScenerySelection scenerySelection = { static_cast<uint8_t>((sceneryEntryIndex >> 8) & 0xFF),
                                                      static_cast<uint16_t>(sceneryEntryIndex & 0xFF) };

                // SV6 has room for 8 types of scenery, and sometimes scenery of non-existing types 5 and 6 is marked as
                // "invented".
                if (scenerySelection.SceneryType < SCENERY_TYPE_COUNT)
                {
                    scenery_set_invented(scenerySelection);
                }
            }
        }
    }

    void ImportResearchList()
    {
        bool invented = true;
        for (const auto& researchItem : _s6.research_items)
        {
            if (researchItem.IsInventedEndMarker())
            {
                invented = false;
                continue;
            }
            else if (researchItem.IsUninventedEndMarker() || researchItem.IsRandomEndMarker())
            {
                break;
            }

            if (invented)
                gResearchItemsInvented.push_back(ResearchItem(researchItem));
            else
                gResearchItemsUninvented.push_back(ResearchItem(researchItem));
        }
    }

    void ImportBanner(Banner* dst, const RCT12Banner* src)
    {
        *dst = {};
        dst->type = RCTEntryIndexToOpenRCT2EntryIndex(src->type);
        dst->flags = src->flags;

        if (!(src->flags & BANNER_FLAG_LINKED_TO_RIDE) && is_user_string_id(src->string_idx))
        {
            dst->text = GetUserString(src->string_idx);
        }

        if (src->flags & BANNER_FLAG_LINKED_TO_RIDE)
        {
            dst->ride_index = src->ride_index;
        }
        else
        {
            dst->colour = src->colour;
        }

        dst->text_colour = src->text_colour;
        dst->position.x = src->x;
        dst->position.y = src->y;
    }

    void Initialise()
    {
        OpenRCT2::GetContext()->GetGameState()->InitAll(_s6.map_size);
    }

    /**
     * Imports guest entry points.
     * Includes fixes for incorrectly set guest entry points in some scenarios.
     */
    void ImportPeepSpawns()
    {
        // Many WW and TT have scenario_filename fields containing an incorrect filename. Check for both this filename
        // and the corrected filename.

        // In this park, peep_spawns[0] is incorrect, and peep_spawns[1] is correct.
        if (String::Equals(_s6.scenario_filename, "WW South America - Rio Carnival.SC6")
            || String::Equals(_s6.scenario_filename, "South America - Rio Carnival.SC6"))
        {
            _s6.peep_spawns[0] = { 2160, 3167, 6, 1 };
            _s6.peep_spawns[1].x = RCT12_PEEP_SPAWN_UNDEFINED;
        }
        // In this park, peep_spawns[0] is correct. Just clear the other.
        else if (
            String::Equals(_s6.scenario_filename, "Great Wall of China Tourism Enhancement.SC6")
            || String::Equals(_s6.scenario_filename, "Asia - Great Wall of China Tourism Enhancement.SC6"))
        {
            _s6.peep_spawns[1].x = RCT12_PEEP_SPAWN_UNDEFINED;
        }
        // Amity Airfield has peeps entering from the corner of the tile, instead of the middle.
        else if (String::Equals(_s6.scenario_filename, "Amity Airfield.SC6"))
        {
            _s6.peep_spawns[0].y = 1296;
        }
        // #9926: Africa - Oasis has peeps spawning on the edge underground near the entrance
        else if (String::Equals(_s6.scenario_filename, "Africa - Oasis.SC6"))
        {
            _s6.peep_spawns[0].y = 2128;
            _s6.peep_spawns[0].z = 7;
        }

        gPeepSpawns.clear();
        for (size_t i = 0; i < RCT12_MAX_PEEP_SPAWNS; i++)
        {
            if (_s6.peep_spawns[i].x != RCT12_PEEP_SPAWN_UNDEFINED)
            {
                PeepSpawn spawn = { _s6.peep_spawns[i].x, _s6.peep_spawns[i].y, _s6.peep_spawns[i].z * 16,
                                    _s6.peep_spawns[i].direction };
                gPeepSpawns.push_back(spawn);
            }
        }
    }

    void ImportNumRiders(Ride* dst, const ride_id_t rideIndex)
    {
        // The number of riders might have overflown or underflown. Re-calculate the value.
        uint16_t numRiders = 0;
        for (const auto& sprite : _s6.sprites)
        {
            if (sprite.unknown.sprite_identifier == SPRITE_IDENTIFIER_PEEP)
            {
                if (sprite.peep.current_ride == rideIndex
                    && (sprite.peep.state == PEEP_STATE_ON_RIDE || sprite.peep.state == PEEP_STATE_ENTERING_RIDE))
                {
                    numRiders++;
                }
            }
        }
        dst->num_riders = numRiders;
    }

    void ImportTileElements()
    {
        for (uint32_t index = 0; index < RCT2_MAX_TILE_ELEMENTS; index++)
        {
            auto src = &_s6.tile_elements[index];
            auto dst = &gTileElements[index];
            if (src->base_height == RCT12_MAX_ELEMENT_HEIGHT)
            {
                std::memcpy(dst, src, sizeof(*src));
            }
            else
            {
                auto tileElementType = static_cast<RCT12TileElementType>(src->GetType());
                // Todo: replace with setting invisibility bit
                if (tileElementType == RCT12TileElementType::Corrupt
                    || tileElementType == RCT12TileElementType::EightCarsCorrupt14
                    || tileElementType == RCT12TileElementType::EightCarsCorrupt15)
                    std::memcpy(dst, src, sizeof(*src));
                else
                    ImportTileElement(dst, src);
            }
        }
        gNextFreeTileElementPointerIndex = _s6.next_free_tile_element_pointer_index;
    }

    void ImportTileElement(TileElement* dst, const RCT12TileElement* src)
    {
        // Todo: allow for changing defition of OpenRCT2 tile element types - replace with a map
        uint8_t tileElementType = src->GetType();
        dst->ClearAs(tileElementType);
        dst->SetDirection(src->GetDirection());
        dst->SetBaseZ(src->base_height * COORDS_Z_STEP);
        dst->SetClearanceZ(src->clearance_height * COORDS_Z_STEP);

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

                dst2->SetSurfaceEntryIndex(src2->GetEntryIndex());
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

                dst2->SetTrackType(src2->GetTrackType());
                dst2->SetSequenceIndex(src2->GetSequenceIndex());
                dst2->SetRideIndex(src2->GetRideIndex());
                dst2->SetColourScheme(src2->GetColourScheme());
                dst2->SetHasChain(src2->HasChain());
                dst2->SetHasCableLift(src2->HasCableLift());
                dst2->SetInverted(src2->IsInverted());
                dst2->SetStationIndex(src2->GetStationIndex());
                dst2->SetHasGreenLight(src2->HasGreenLight());
                dst2->SetBlockBrakeClosed(src2->BlockBrakeClosed());
                dst2->SetIsIndestructible(src2->IsIndestructible());
                dst2->SetSeatRotation(src2->GetSeatRotation());
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
                auto rideType = _s6.rides[src2->GetRideIndex()].type;
                if (rideType == RIDE_TYPE_MAZE)
                {
                    dst2->SetMazeEntry(src2->GetMazeEntry());
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

                // Import banner information
                dst2->SetBannerIndex(BANNER_INDEX_NULL);
                auto entry = dst2->GetEntry();
                if (entry != nullptr && entry->wall.scrolling_mode != SCROLLING_MODE_NONE)
                {
                    auto bannerIndex = src2->GetBannerIndex();
                    if (bannerIndex < std::size(_s6.banners))
                    {
                        auto srcBanner = &_s6.banners[bannerIndex];
                        auto dstBanner = GetBanner(bannerIndex);
                        ImportBanner(dstBanner, srcBanner);
                        dst2->SetBannerIndex(src2->GetBannerIndex());
                    }
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

                // Import banner information
                dst2->SetBannerIndex(BANNER_INDEX_NULL);
                auto entry = dst2->GetEntry();
                if (entry != nullptr && entry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
                {
                    auto bannerIndex = src2->GetBannerIndex();
                    if (bannerIndex < std::size(_s6.banners))
                    {
                        auto srcBanner = &_s6.banners[bannerIndex];
                        auto dstBanner = GetBanner(bannerIndex);
                        ImportBanner(dstBanner, srcBanner);
                        dst2->SetBannerIndex(src2->GetBannerIndex());
                    }
                }
                break;
            }
            case TILE_ELEMENT_TYPE_BANNER:
            {
                auto dst2 = dst->AsBanner();
                auto src2 = src->AsBanner();

                dst2->SetIndex(src2->GetIndex());
                dst2->SetPosition(src2->GetPosition());
                dst2->SetAllowedEdges(src2->GetAllowedEdges());

                auto bannerIndex = src2->GetIndex();
                if (bannerIndex < std::size(_s6.banners))
                {
                    auto srcBanner = &_s6.banners[bannerIndex];
                    auto dstBanner = GetBanner(bannerIndex);
                    ImportBanner(dstBanner, srcBanner);
                }
                else
                {
                    dst2->SetIndex(BANNER_INDEX_NULL);
                }
                break;
            }
            default:
                assert(false);
        }
    }

    void ImportMarketingCampaigns()
    {
        for (size_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
        {
            if (_s6.campaign_weeks_left[i] & CAMPAIGN_ACTIVE_FLAG)
            {
                MarketingCampaign campaign{};
                campaign.Type = static_cast<uint8_t>(i);
                campaign.WeeksLeft = _s6.campaign_weeks_left[i] & ~(CAMPAIGN_ACTIVE_FLAG | CAMPAIGN_FIRST_WEEK_FLAG);
                if ((_s6.campaign_weeks_left[i] & CAMPAIGN_FIRST_WEEK_FLAG) != 0)
                {
                    campaign.Flags |= MarketingCampaignFlags::FIRST_WEEK;
                }
                if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
                {
                    campaign.RideId = _s6.campaign_ride_index[i];
                }
                else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
                {
                    campaign.ShopItemType = _s6.campaign_ride_index[i];
                }
                gMarketingCampaigns.push_back(campaign);
            }
        }
    }

    void ImportSprites()
    {
        for (int32_t i = 0; i < RCT2_MAX_SPRITES; i++)
        {
            auto src = &_s6.sprites[i];
            auto dst = GetEntity(i);
            ImportSprite(reinterpret_cast<rct_sprite*>(dst), src);
        }

        for (int32_t i = 0; i < static_cast<uint8_t>(EntityListId::Count); i++)
        {
            gSpriteListHead[i] = _s6.sprite_lists_head[i];
            gSpriteListCount[i] = _s6.sprite_lists_count[i];
        }
        // This list contains the number of free slots. Increase it according to our own sprite limit.
        gSpriteListCount[static_cast<uint8_t>(EntityListId::Free)] += (MAX_SPRITES - RCT2_MAX_SPRITES);
    }

    void ImportSprite(rct_sprite* dst, const RCT2Sprite* src)
    {
        std::memset(&dst->pad_00, 0, sizeof(rct_sprite));
        switch (src->unknown.sprite_identifier)
        {
            case SPRITE_IDENTIFIER_NULL:
                ImportSpriteCommonProperties(reinterpret_cast<SpriteBase*>(dst), &src->unknown);
                break;
            case SPRITE_IDENTIFIER_VEHICLE:
                ImportSpriteVehicle(&dst->vehicle, &src->vehicle);
                break;
            case SPRITE_IDENTIFIER_PEEP:
                ImportSpritePeep(&dst->peep, &src->peep);
                break;
            case SPRITE_IDENTIFIER_MISC:
                ImportSpriteMisc(&dst->generic, &src->unknown);
                break;
            case SPRITE_IDENTIFIER_LITTER:
                ImportSpriteLitter(&dst->litter, &src->litter);
                break;
            default:
                ImportSpriteCommonProperties(reinterpret_cast<SpriteBase*>(dst), reinterpret_cast<const RCT12SpriteBase*>(src));
                log_warning("Sprite identifier %d can not be imported.", src->unknown.sprite_identifier);
                break;
        }
    }

    void ImportSpriteVehicle(Vehicle* dst, const RCT2SpriteVehicle* src)
    {
        const auto& ride = _s6.rides[src->ride];

        ImportSpriteCommonProperties(static_cast<SpriteBase*>(dst), src);
        dst->vehicle_sprite_type = src->vehicle_sprite_type;
        dst->bank_rotation = src->bank_rotation;
        dst->remaining_distance = src->remaining_distance;
        dst->velocity = src->velocity;
        dst->acceleration = src->acceleration;
        dst->ride = src->ride;
        dst->vehicle_type = src->vehicle_type;
        dst->colours = src->colours;
        dst->track_progress = src->track_progress;
        dst->track_direction = src->track_direction;
        if (src->boat_location.isNull() || ride.mode != RIDE_MODE_BOAT_HIRE
            || src->status != static_cast<uint8_t>(Vehicle::Status::TravellingBoat))
        {
            dst->BoatLocation.setNull();
            dst->track_type = src->track_type;
        }
        else
        {
            dst->BoatLocation = TileCoordsXY{ src->boat_location.x, src->boat_location.y }.ToCoordsXY();
            dst->track_type = 0;
        }

        dst->TrackLocation = { src->track_x, src->track_y, src->track_z };
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

        Vehicle::Status statusSrc = Vehicle::Status::MovingToEndOfStation;
        if (src->status <= static_cast<uint8_t>(Vehicle::Status::StoppedByBlockBrakes))
        {
            statusSrc = static_cast<Vehicle::Status>(src->status);
        }

        dst->status = statusSrc;
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
        dst->sound1_id = static_cast<SoundId>(src->sound1_id);
        dst->sound1_volume = src->sound1_volume;
        dst->sound2_id = static_cast<SoundId>(src->sound2_id);
        dst->sound2_volume = src->sound2_volume;
        dst->sound_vector_factor = src->sound_vector_factor;
        dst->time_waiting = src->time_waiting;
        dst->speed = src->speed;
        dst->powered_acceleration = src->powered_acceleration;
        dst->dodgems_collision_direction = src->dodgems_collision_direction;
        dst->animation_frame = src->animation_frame;
        dst->var_C8 = src->var_C8;
        dst->var_CA = src->var_CA;
        dst->scream_sound_id = static_cast<SoundId>(src->scream_sound_id);
        dst->TrackSubposition = VehicleTrackSubposition{ src->TrackSubposition };
        dst->var_CE = src->var_CE;
        dst->var_CF = src->var_CF;
        dst->lost_time_out = src->lost_time_out;
        dst->vertical_drop_countdown = src->vertical_drop_countdown;
        dst->var_D3 = src->var_D3;
        dst->mini_golf_current_animation = src->mini_golf_current_animation;
        dst->mini_golf_flags = src->mini_golf_flags;
        dst->ride_subtype = RCTEntryIndexToOpenRCT2EntryIndex(src->ride_subtype);
        dst->colours_extended = src->colours_extended;
        dst->seat_rotation = src->seat_rotation;
        dst->target_seat_rotation = src->target_seat_rotation;
    }

    void ImportSpritePeep(Peep* dst, const RCT2SpritePeep* src)
    {
        ImportSpriteCommonProperties(static_cast<SpriteBase*>(dst), src);
        if (is_user_string_id(src->name_string_idx))
        {
            dst->SetName(GetUserString(src->name_string_idx));
        }
        dst->NextLoc = { src->next_x, src->next_y, src->next_z * COORDS_Z_STEP };
        dst->NextFlags = src->next_flags;
        dst->OutsideOfPark = static_cast<bool>(src->outside_of_park);
        dst->State = static_cast<PeepState>(src->state);
        dst->SubState = src->sub_state;
        dst->SpriteType = static_cast<PeepSpriteType>(src->sprite_type);
        dst->AssignedPeepType = static_cast<PeepType>(src->peep_type);
        dst->GuestNumRides = src->no_of_rides;
        dst->TshirtColour = src->tshirt_colour;
        dst->TrousersColour = src->trousers_colour;
        dst->DestinationX = src->destination_x;
        dst->DestinationY = src->destination_y;
        dst->DestinationTolerance = src->destination_tolerance;
        dst->Var37 = src->var_37;
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
        dst->TimeToConsume = src->time_to_consume;
        dst->Intensity = static_cast<IntensityRange>(src->intensity);
        dst->NauseaTolerance = src->nausea_tolerance;
        dst->WindowInvalidateFlags = src->window_invalidate_flags;
        dst->PaidOnDrink = src->paid_on_drink;
        for (size_t i = 0; i < std::size(src->ride_types_been_on); i++)
        {
            dst->RideTypesBeenOn[i] = src->ride_types_been_on[i];
        }
        dst->ItemExtraFlags = src->item_extra_flags;
        dst->Photo2RideRef = src->photo2_ride_ref;
        dst->Photo3RideRef = src->photo3_ride_ref;
        dst->Photo4RideRef = src->photo4_ride_ref;
        dst->CurrentRide = src->current_ride;
        dst->CurrentRideStation = src->current_ride_station;
        dst->CurrentTrain = src->current_train;
        dst->TimeToSitdown = src->time_to_sitdown;
        dst->SpecialSprite = src->special_sprite;
        dst->ActionSpriteType = static_cast<PeepActionSpriteType>(src->action_sprite_type);
        dst->NextActionSpriteType = static_cast<PeepActionSpriteType>(src->next_action_sprite_type);
        dst->ActionSpriteImageOffset = src->action_sprite_image_offset;
        dst->Action = static_cast<PeepActionType>(src->action);
        dst->ActionFrame = src->action_frame;
        dst->StepProgress = src->step_progress;
        dst->GuestNextInQueue = src->next_in_queue;
        dst->PeepDirection = src->direction;
        dst->InteractionRideIndex = src->interaction_ride_index;
        dst->TimeInQueue = src->time_in_queue;
        for (size_t i = 0; i < std::size(src->rides_been_on); i++)
        {
            dst->RidesBeenOn[i] = src->rides_been_on[i];
        }
        dst->Id = src->id;
        dst->CashInPocket = src->cash_in_pocket;
        dst->CashSpent = src->cash_spent;
        dst->TimeInPark = src->time_in_park;
        dst->RejoinQueueTimeout = src->rejoin_queue_timeout;
        dst->PreviousRide = src->previous_ride;
        dst->PreviousRideTimeOut = src->previous_ride_time_out;
        for (size_t i = 0; i < std::size(src->thoughts); i++)
        {
            auto srcThought = &src->thoughts[i];
            auto dstThought = &dst->Thoughts[i];
            dstThought->type = static_cast<PeepThoughtType>(srcThought->type);
            dstThought->item = srcThought->item;
            dstThought->freshness = srcThought->freshness;
            dstThought->fresh_timeout = srcThought->fresh_timeout;
        }
        dst->PathCheckOptimisation = src->path_check_optimisation;
        dst->GuestHeadingToRideId = src->guest_heading_to_ride_id;
        dst->GuestIsLostCountdown = src->peep_is_lost_countdown;
        dst->Photo1RideRef = src->photo1_ride_ref;
        dst->PeepFlags = src->peep_flags;
        dst->PathfindGoal = src->pathfind_goal;
        for (size_t i = 0; i < std::size(src->pathfind_history); i++)
        {
            dst->PathfindHistory[i] = src->pathfind_history[i];
        }
        dst->WalkingFrameNum = src->no_action_frame_num;
        dst->LitterCount = src->litter_count;
        dst->GuestTimeOnRide = src->time_on_ride;
        dst->DisgustingCount = src->disgusting_count;
        dst->PaidToEnter = src->paid_to_enter;
        dst->PaidOnRides = src->paid_on_rides;
        dst->PaidOnFood = src->paid_on_food;
        dst->PaidOnSouvenirs = src->paid_on_souvenirs;
        dst->AmountOfFood = src->no_of_food;
        dst->AmountOfDrinks = src->no_of_drinks;
        dst->AmountOfSouvenirs = src->no_of_souvenirs;
        dst->VandalismSeen = src->vandalism_seen;
        dst->VoucherType = src->voucher_type;
        dst->VoucherRideId = src->voucher_arguments;
        dst->SurroundingsThoughtTimeout = src->surroundings_thought_timeout;
        dst->Angriness = src->angriness;
        dst->TimeLost = src->time_lost;
        dst->DaysInQueue = src->days_in_queue;
        dst->BalloonColour = src->balloon_colour;
        dst->UmbrellaColour = src->umbrella_colour;
        dst->HatColour = src->hat_colour;
        dst->FavouriteRide = src->favourite_ride;
        dst->FavouriteRideRating = src->favourite_ride_rating;
        dst->ItemStandardFlags = src->item_standard_flags;
    }

    void ImportSpriteMisc(SpriteBase* cdst, const RCT12SpriteBase* csrc)
    {
        ImportSpriteCommonProperties(cdst, csrc);
        switch (cdst->type)
        {
            case SPRITE_MISC_STEAM_PARTICLE:
            {
                auto src = static_cast<const RCT12SpriteSteamParticle*>(csrc);
                auto dst = static_cast<SteamParticle*>(cdst);
                dst->time_to_move = src->time_to_move;
                dst->frame = src->frame;
                break;
            }
            case SPRITE_MISC_MONEY_EFFECT:
            {
                auto src = static_cast<const RCT12SpriteMoneyEffect*>(csrc);
                auto dst = static_cast<MoneyEffect*>(cdst);
                dst->MoveDelay = src->move_delay;
                dst->NumMovements = src->num_movements;
                dst->Vertical = src->vertical;
                dst->Value = src->value;
                dst->OffsetX = src->offset_x;
                dst->Wiggle = src->wiggle;
                break;
            }
            case SPRITE_MISC_CRASHED_VEHICLE_PARTICLE:
            {
                auto src = static_cast<const RCT12SpriteCrashedVehicleParticle*>(csrc);
                auto dst = static_cast<VehicleCrashParticle*>(cdst);
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
            case SPRITE_MISC_EXPLOSION_CLOUD:
            case SPRITE_MISC_EXPLOSION_FLARE:
            case SPRITE_MISC_CRASH_SPLASH:
            {
                auto src = static_cast<const RCT12SpriteParticle*>(csrc);
                auto dst = static_cast<SpriteGeneric*>(cdst);
                dst->frame = src->frame;
                break;
            }
            case SPRITE_MISC_JUMPING_FOUNTAIN_WATER:
            case SPRITE_MISC_JUMPING_FOUNTAIN_SNOW:
            {
                auto* src = static_cast<const RCT12SpriteJumpingFountain*>(csrc);
                auto* dst = static_cast<JumpingFountain*>(cdst);
                dst->NumTicksAlive = src->num_ticks_alive;
                dst->frame = src->frame;
                dst->FountainFlags = src->fountain_flags;
                dst->TargetX = src->target_x;
                dst->TargetY = src->target_y;
                dst->Iteration = src->iteration;
                break;
            }
            case SPRITE_MISC_BALLOON:
            {
                auto src = static_cast<const RCT12SpriteBalloon*>(csrc);
                auto dst = static_cast<Balloon*>(cdst);
                dst->popped = src->popped;
                dst->time_to_move = src->time_to_move;
                dst->frame = src->frame;
                dst->colour = src->colour;
                break;
            }
            case SPRITE_MISC_DUCK:
            {
                auto src = static_cast<const RCT12SpriteDuck*>(csrc);
                auto dst = static_cast<Duck*>(cdst);
                dst->frame = src->frame;
                dst->target_x = src->target_x;
                dst->target_y = src->target_y;
                dst->state = src->state;
                break;
            }
            default:
                log_warning("Misc. sprite type %d can not be imported.", cdst->type);
                break;
        }
    }

    void ImportSpriteLitter(Litter* dst, const RCT12SpriteLitter* src)
    {
        ImportSpriteCommonProperties(dst, src);
        dst->creationTick = src->creationTick;
    }

    void ImportSpriteCommonProperties(SpriteBase* dst, const RCT12SpriteBase* src)
    {
        dst->sprite_identifier = src->sprite_identifier;
        dst->type = src->type;
        dst->next_in_quadrant = src->next_in_quadrant;
        dst->next = src->next;
        dst->previous = src->previous;
        dst->linked_list_index = static_cast<EntityListId>(src->linked_list_type_offset >> 1);
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

    std::string GetUserString(rct_string_id stringId)
    {
        const auto originalString = _s6.custom_strings[(stringId - USER_STRING_START) % 1024];
        std::string_view originalStringView(originalString, USER_STRING_MAX_LENGTH);
        auto asUtf8 = rct2_to_utf8(originalStringView, RCT2_LANGUAGE_ID_ENGLISH_UK);
        utf8_remove_format_codes(asUtf8.data(), /*allow colour*/ false);
        return asUtf8.data();
    }

    std::vector<rct_object_entry> GetRequiredObjects()
    {
        std::vector<rct_object_entry> result;
        rct_object_entry nullEntry = {};
        std::memset(&nullEntry, 0xFF, sizeof(nullEntry));

        int objectIt = 0;
        for (int16_t objectType = OBJECT_TYPE_RIDE; objectType <= OBJECT_TYPE_WATER; objectType++)
        {
            for (int16_t i = 0; i < rct2_object_entry_group_counts[objectType]; i++, objectIt++)
            {
                result.push_back(_s6.objects[objectIt]);
            }
            for (int16_t i = rct2_object_entry_group_counts[objectType]; i < object_entry_group_counts[objectType]; i++)
            {
                result.push_back(nullEntry);
            }
        }

        return result;
    }
};

std::unique_ptr<IParkImporter> ParkImporter::CreateS6(IObjectRepository& objectRepository)
{
    return std::make_unique<S6Importer>(objectRepository);
}

static void show_error(uint8_t errorType, rct_string_id errorStringId)
{
    if (errorType == ERROR_TYPE_GENERIC)
    {
        context_show_error(errorStringId, STR_NONE, {});
    }
    context_show_error(STR_UNABLE_TO_LOAD_FILE, errorStringId, {});
}

void load_from_sv6(const char* path)
{
    auto context = OpenRCT2::GetContext();
    auto s6Importer = std::make_unique<S6Importer>(context->GetObjectRepository());
    try
    {
        auto& objectMgr = context->GetObjectManager();
        auto result = s6Importer->LoadSavedGame(path);
        objectMgr.LoadObjects(result.RequiredObjects.data(), result.RequiredObjects.size());
        s6Importer->Import();
        game_fix_save_vars();
        AutoCreateMapAnimations();
        sprite_position_tween_reset();
        gScreenAge = 0;
        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
    }
    catch (const ObjectLoadException&)
    {
        show_error(ERROR_TYPE_FILE_LOAD, STR_FILE_CONTAINS_INVALID_DATA);
    }
    catch (const IOException& loadError)
    {
        log_error("Error loading: %s", loadError.what());
        show_error(ERROR_TYPE_FILE_LOAD, STR_GAME_SAVE_FAILED);
    }
    catch (const std::exception&)
    {
        show_error(ERROR_TYPE_FILE_LOAD, STR_FILE_CONTAINS_INVALID_DATA);
    }
}

/**
 *
 *  rct2: 0x00676053
 * scenario (ebx)
 */
void load_from_sc6(const char* path)
{
    auto context = OpenRCT2::GetContext();
    auto& objManager = context->GetObjectManager();
    auto s6Importer = std::make_unique<S6Importer>(context->GetObjectRepository());
    try
    {
        auto result = s6Importer->LoadScenario(path);
        objManager.LoadObjects(result.RequiredObjects.data(), result.RequiredObjects.size());
        s6Importer->Import();
        game_fix_save_vars();
        AutoCreateMapAnimations();
        sprite_position_tween_reset();
        return;
    }
    catch (const ObjectLoadException& loadError)
    {
        log_error("Error loading: %s", loadError.what());
        show_error(ERROR_TYPE_FILE_LOAD, STR_GAME_SAVE_FAILED);
    }
    catch (const IOException& loadError)
    {
        log_error("Error loading: %s", loadError.what());
        show_error(ERROR_TYPE_FILE_LOAD, STR_GAME_SAVE_FAILED);
    }
    catch (const std::exception&)
    {
        show_error(ERROR_TYPE_FILE_LOAD, STR_FILE_CONTAINS_INVALID_DATA);
    }
    gScreenAge = 0;
    gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
}
