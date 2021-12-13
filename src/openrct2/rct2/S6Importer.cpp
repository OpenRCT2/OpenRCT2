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
#include "../Editor.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/FileStream.h"
#include "../core/IStream.hpp"
#include "../core/Numerics.hpp"
#include "../core/Path.hpp"
#include "../core/Random.hpp"
#include "../core/String.hpp"
#include "../entity/Balloon.h"
#include "../entity/Duck.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/EntityTweener.h"
#include "../entity/Fountain.h"
#include "../entity/Guest.h"
#include "../entity/Litter.h"
#include "../entity/MoneyEffect.h"
#include "../entity/Particle.h"
#include "../entity/Staff.h"
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
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../peep/RideUseSystem.h"
#include "../rct12/EntryList.h"
#include "../rct12/RCT12.h"
#include "../rct12/SawyerChunkReader.h"
#include "../rct12/SawyerEncoding.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideRatings.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/TrainManager.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "../world/TilePointerIndex.hpp"

#include <algorithm>

namespace RCT2
{
#define DECRYPT_MONEY(money) (static_cast<money32>(Numerics::rol32((money) ^ 0xF4EC9621, 13)))

    /**
     * Class to import RollerCoaster Tycoon 2 scenarios (*.SC6) and saved games (*.SV6).
     */
    class S6Importer final : public IParkImporter
    {
    private:
        IObjectRepository& _objectRepository;

        const utf8* _s6Path = nullptr;
        S6Data _s6{};
        uint8_t _gameVersion = 0;
        bool _isSV7 = false;
        OpenRCT2::BitSet<Limits::MaxRidesInPark> _isFlatRide{};
        ObjectEntryIndex _pathToSurfaceMap[16];
        ObjectEntryIndex _pathToQueueSurfaceMap[16];
        ObjectEntryIndex _pathToRailingMap[16];
        RCT12::EntryList _terrainSurfaceEntries;
        RCT12::EntryList _terrainEdgeEntries;

    public:
        S6Importer(IObjectRepository& objectRepository)
            : _objectRepository(objectRepository)
        {
        }

        ParkLoadResult Load(const utf8* path) override
        {
            const auto extension = Path::GetExtension(path);
            if (String::Equals(extension, ".sc6", true))
            {
                return LoadScenario(path);
            }
            if (String::Equals(extension, ".sv6", true))
            {
                return LoadSavedGame(path);
            }

            throw std::runtime_error("Invalid RCT2 park extension.");
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
            if (isScenario && !gConfigGeneral.allow_loading_with_incorrect_checksum
                && !SawyerEncoding::ValidateChecksum(stream))
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
                auto extension = Path::GetExtension(path);
                _isSV7 = _stricmp(extension.c_str(), ".sv7") == 0;
            }

            chunkReader.ReadChunk(&_s6.Objects, sizeof(_s6.Objects));

            if (isScenario)
            {
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

            gEditorStep = _s6.info.editor_step;
            gScenarioCategory = static_cast<SCENARIO_CATEGORY>(_s6.info.category);

            // Some scenarios have their scenario details in UTF-8, due to earlier bugs in OpenRCT2.
            auto loadMaybeUTF8 = [](std::string_view str) -> std::string {
                return !IsLikelyUTF8(str) ? rct2_to_utf8(str, RCT2LanguageId::EnglishUK) : std::string(str);
            };

            if (_s6.header.type == S6_TYPE_SCENARIO)
            {
                gScenarioName = loadMaybeUTF8(_s6.info.name);
                gScenarioDetails = loadMaybeUTF8(_s6.info.details);
            }
            else
            {
                // Saved games do not have an info chunk
                gScenarioName = loadMaybeUTF8(_s6.scenario_name);
                gScenarioDetails = loadMaybeUTF8(_s6.scenario_description);
            }

            gDateMonthsElapsed = static_cast<int32_t>(_s6.elapsed_months);
            gDateMonthTicks = _s6.current_day;
            gCurrentTicks = _s6.game_ticks_1;

            scenario_rand_seed(_s6.scenario_srand_0, _s6.scenario_srand_1);

            DetermineFlatRideStatus();
            ImportTileElements();
            ImportEntities();

            gInitialCash = ToMoney64(_s6.initial_cash);
            gBankLoan = ToMoney64(_s6.current_loan);
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
            // _s6.researched_track_types_a
            // _s6.researched_track_types_b

            gNumGuestsInPark = _s6.guests_in_park;
            gNumGuestsHeadingForPark = _s6.guests_heading_for_park;

            for (size_t i = 0; i < Limits::ExpenditureTableMonthCount; i++)
            {
                for (size_t j = 0; j < Limits::ExpenditureTypeCount; j++)
                {
                    gExpenditureTable[i][j] = ToMoney64(_s6.expenditure_table[i][j]);
                }
            }

            gNumGuestsInParkLastWeek = _s6.last_guests_in_park;
            // pad_01357BCA
            gStaffHandymanColour = _s6.handyman_colour;
            gStaffMechanicColour = _s6.mechanic_colour;
            gStaffSecurityColour = _s6.security_colour;

            gParkRating = _s6.park_rating;

            auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
            park.ResetHistories();
            std::copy(std::begin(_s6.park_rating_history), std::end(_s6.park_rating_history), gParkRatingHistory);
            for (size_t i = 0; i < std::size(_s6.guests_in_park_history); i++)
            {
                if (_s6.guests_in_park_history[i] != RCT12ParkHistoryUndefined)
                {
                    gGuestsInParkHistory[i] = _s6.guests_in_park_history[i] * RCT12GuestsInParkHistoryFactor;
                }
            }

            gResearchPriorities = _s6.active_research_types;
            gResearchProgressStage = _s6.research_progress_stage;
            if (_s6.last_researched_item_subject != RCT12_RESEARCHED_ITEMS_SEPARATOR)
                gResearchLastItem = RCT12ResearchItem{ _s6.last_researched_item_subject,
                                                       EnumValue(ResearchCategory::Transport) }
                                        .ToResearchItem();
            else
                gResearchLastItem = std::nullopt;
            // pad_01357CF8
            if (_s6.next_research_item != RCT12_RESEARCHED_ITEMS_SEPARATOR)
                gResearchNextItem = RCT12ResearchItem{ _s6.next_research_item, _s6.next_research_category }.ToResearchItem();
            else
                gResearchNextItem = std::nullopt;

            gResearchProgress = _s6.research_progress;
            gResearchExpectedDay = _s6.next_research_expected_day;
            gResearchExpectedMonth = _s6.next_research_expected_month;
            gGuestInitialHappiness = _s6.guest_initial_happiness;
            gParkSize = _s6.park_size;
            _guestGenerationProbability = _s6.guest_generation_probability;
            gTotalRideValueForMoney = _s6.total_ride_value_for_money;
            gMaxBankLoan = ToMoney64(_s6.maximum_loan);
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

            gCurrentExpenditure = ToMoney64(_s6.current_expenditure);
            gCurrentProfit = ToMoney64(_s6.current_profit);
            gWeeklyProfitAverageDividend = ToMoney64(_s6.weekly_profit_average_dividend);
            gWeeklyProfitAverageDivisor = _s6.weekly_profit_average_divisor;
            // pad_0135833A

            gParkValue = ToMoney64(_s6.park_value);

            for (size_t i = 0; i < Limits::FinanceGraphSize; i++)
            {
                gCashHistory[i] = ToMoney64(_s6.balance_history[i]);
                gWeeklyProfitHistory[i] = ToMoney64(_s6.weekly_profit_history[i]);
                gParkValueHistory[i] = ToMoney64(_s6.park_value_history[i]);
            }

            gScenarioCompletedCompanyValue = RCT12CompletedCompanyValueToOpenRCT2(_s6.completed_company_value);
            gTotalAdmissions = _s6.total_admissions;
            gTotalIncomeFromAdmissions = ToMoney64(_s6.income_from_admissions);
            gCompanyValue = ToMoney64(_s6.company_value);
            std::memcpy(gPeepWarningThrottle, _s6.peep_warning_throttle, sizeof(_s6.peep_warning_throttle));

            // Awards
            auto& awards = GetAwards();
            for (auto& src : _s6.awards)
            {
                if (src.time != 0)
                {
                    awards.push_back(Award{ src.time, static_cast<AwardType>(src.type) });
                }
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
            gHistoricalProfit = ToMoney64(_s6.historical_profit);
            // pad_013587D4
            gScenarioCompletedBy = std::string_view(_s6.scenario_completed_name, sizeof(_s6.scenario_completed_name));
            gCash = ToMoney64(DECRYPT_MONEY(_s6.cash));
            // pad_013587FC
            gParkRatingCasualtyPenalty = _s6.park_rating_casualty_penalty;
            gMapSize = { _s6.map_size, _s6.map_size };
            gSamePriceThroughoutPark = _s6.same_price_throughout
                | (static_cast<uint64_t>(_s6.same_price_throughout_extended) << 32);
            _suggestedGuestMaximum = _s6.suggested_max_guests;
            gScenarioParkRatingWarningDays = _s6.park_rating_warning_days;
            gLastEntranceStyle = _s6.last_entrance_style;
            // rct1_water_colour
            // pad_01358842
            ImportResearchList();
            gMapBaseZ = _s6.map_base_z;
            gBankLoanInterestRate = _s6.current_interest_rate;
            // pad_0135934B
            // Preserve compatibility with vanilla RCT2's save format.
            gParkEntrances.clear();
            for (uint8_t i = 0; i < Limits::MaxParkEntrances; i++)
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
                gScenarioFileName = Path::GetFileName(_s6Path);
            }
            else
            {
                // For savegames the filename can be arbitrary, so we have no choice but to rely on the name provided
                gScenarioFileName = std::string(String::ToStringView(_s6.scenario_filename, std::size(_s6.scenario_filename)));
            }
            gCurrentRealTimeTicks = 0;

            ImportRides();

            gSavedAge = _s6.saved_age;
            gSavedView = ScreenCoordsXY{ _s6.saved_view_x, _s6.saved_view_y };
            gSavedViewZoom = ZoomLevel{ static_cast<int8_t>(_s6.saved_view_zoom) };
            gSavedViewRotation = _s6.saved_view_rotation;

            ImportRideRatingsCalcData();
            ImportRideMeasurements();
            gNextGuestNumber = _s6.next_guest_index;
            gGrassSceneryTileLoopPosition = _s6.grass_and_scenery_tilepos;
            // unk_13CA73E
            // pad_13CA73F
            // unk_13CA740
            gClimate = ClimateType{ _s6.climate };
            // pad_13CA741;
            // byte_13CA742
            // pad_013CA747
            gClimateUpdateTimer = _s6.climate_update_timer;
            gClimateCurrent.Weather = WeatherType{ _s6.current_weather };
            gClimateNext.Weather = WeatherType{ _s6.next_weather };
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
            for (size_t i = 0; i < Limits::MaxNewsItems; i++)
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
                    dst->Text = ConvertFormattedStringToOpenRCT2(std::string_view(src->Text, sizeof(src->Text)));
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
            gWidePathTileLoopPosition.x = _s6.wide_path_tile_loop_x;
            gWidePathTileLoopPosition.y = _s6.wide_path_tile_loop_y;
            // pad_13CE778

            // Fix and set dynamic variables
            map_strip_ghost_flag_from_elements();
            ConvertScenarioStringsToUTF8();
            map_count_remaining_land_rights();
            determine_ride_entrance_and_exit_locations();

            park.Name = GetUserString(_s6.park_name);

            FixLandOwnership();

            research_determine_first_of_type();
            staff_update_greyed_patrol_areas();

            CheatsReset();
            ClearRestrictedScenery();
        }

        void AddDefaultEntries()
        {
            // Add default surfaces
            _terrainSurfaceEntries.AddRange(DefaultTerrainSurfaces);

            // Add default edges
            _terrainEdgeEntries.AddRange(DefaultTerrainEdges);
        }

        void ConvertScenarioStringsToUTF8()
        {
            // Scenario details
            gScenarioCompletedBy = rct2_to_utf8(gScenarioCompletedBy, RCT2LanguageId::EnglishUK);
            gScenarioName = rct2_to_utf8(gScenarioName, RCT2LanguageId::EnglishUK);
            gScenarioDetails = rct2_to_utf8(gScenarioDetails, RCT2LanguageId::EnglishUK);
        }

        void FixLandOwnership() const
        {
            if (String::Equals(_s6.scenario_filename, "Europe - European Cultural Festival.SC6"))
            {
                // This scenario breaks pathfinding. Create passages between the worlds. (List is grouped by neighbouring
                // tiles.)
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
            else if (String::Equals(gScenarioFileName, "N America - Extreme Hawaiian Island.SC6"))
            {
                FixLandOwnershipTilesWithOwnership(
                    {
                        { 132, 124 },
                        { 133, 124 },
                        { 133, 125 },
                        { 133, 126 },
                        { 119, 35 },
                        { 132, 62 },
                        { 133, 67 },
                        { 136, 71 },
                        { 87, 33 },
                        { 87, 34 },
                        { 90, 36 },
                        { 91, 36 },
                    },
                    OWNERSHIP_OWNED);
                // We set the doNotDowngrade flag for cases where the player has used a cheat to own all land.
                FixLandOwnershipTilesWithOwnership(
                    {
                        { 49, 99 },
                        { 50, 99 },
                        { 88, 110 },
                    },
                    OWNERSHIP_AVAILABLE, true);
            }
        }

        void ImportRides()
        {
            for (uint8_t index = 0; index < Limits::MaxRidesInPark; index++)
            {
                auto src = &_s6.rides[index];
                if (src->type != RIDE_TYPE_NULL)
                {
                    const auto rideId = RideId::FromUnderlying(index);
                    auto dst = GetOrAllocateRide(rideId);
                    ImportRide(dst, src, rideId);
                }
            }
        }

        /**
         * This code is needed to detect hacks where a tracked ride has been made invisible
         * by setting its ride type to a flat ride.
         *
         * The function should classify rides as follows:
         * 1. If the ride type is tracked and its vehicles also belong on tracks, it should be classified as tracked.
         * 2. If the ride type is a flat ride, but its vehicles belong on tracks,
         *     it should be classified as tracked (Crooked House mod).
         * 3. If the ride type is tracked and its vehicles belong to a flat ride, it should be classified as tracked.
         * 4. If the ride type is a flat ride and its vehicles also belong to a flat ride, it should be classified as a flat
         * ride.
         */
        void DetermineFlatRideStatus()
        {
            for (uint8_t index = 0; index < Limits::MaxRidesInPark; index++)
            {
                auto src = &_s6.rides[index];
                if (src->type == RIDE_TYPE_NULL)
                    continue;

                auto subtype = RCTEntryIndexToOpenRCT2EntryIndex(src->subtype);
                auto* rideEntry = get_ride_entry(subtype);
                // If the ride is tracked, we don’t need to check the vehicle any more.
                if (!GetRideTypeDescriptor(src->type).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
                {
                    _isFlatRide[index] = false;
                    continue;
                }

                // We have established the ride type is a flat ride, which means the vehicle now determines whether it is a
                // true flat ride (scenario 4) or a tracked ride with an invisibility hack (scenario 2).
                ObjectEntryIndex originalRideType = src->type;
                if (rideEntry != nullptr)
                {
                    originalRideType = ride_entry_get_first_non_null_ride_type(rideEntry);
                }
                const auto isFlatRide = GetRideTypeDescriptor(originalRideType).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE);
                _isFlatRide.set(static_cast<size_t>(index), isFlatRide);
            }
        }

        bool IsFlatRide(const uint8_t rct12RideIndex)
        {
            if (rct12RideIndex == RCT12_RIDE_ID_NULL)
                return false;
            return _isFlatRide[rct12RideIndex];
        }

        void ImportRide(::Ride* dst, const RCT2::Ride* src, const RideId rideIndex)
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

            if (rideType >= RIDE_TYPE_COUNT)
            {
                log_error("Invalid ride type for a ride in this save.");
                throw UnsupportedRideTypeException(rideType);
            }
            dst->type = rideType;
            dst->subtype = subtype;
            // pad_002;
            dst->mode = static_cast<RideMode>(src->mode);
            dst->colour_scheme_type = src->colour_scheme_type;

            for (uint8_t i = 0; i < Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicle_colours[i].Body = src->vehicle_colours[i].body_colour;
                dst->vehicle_colours[i].Trim = src->vehicle_colours[i].trim_colour;
            }

            // pad_046;
            dst->status = static_cast<RideStatus>(src->status);

            dst->default_name_number = src->name_arguments_number;
            if (is_user_string_id(src->name))
            {
                dst->custom_name = GetUserString(src->name);
            }
            else
            {
                dst->default_name_number = src->name_arguments_number;
            }

            if (src->overall_view.IsNull())
            {
                dst->overall_view.SetNull();
            }
            else
            {
                auto tileLoc = TileCoordsXY(src->overall_view.x, src->overall_view.y);
                dst->overall_view = tileLoc.ToCoordsXY();
            }

            for (int32_t i = 0; i < Limits::MaxStationsPerRide; i++)
            {
                if (src->station_starts[i].IsNull())
                {
                    dst->stations[i].Start.SetNull();
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

                if (src->entrances[i].IsNull())
                    ride_clear_entrance_location(dst, i);
                else
                    ride_set_entrance_location(
                        dst, i, { src->entrances[i].x, src->entrances[i].y, src->station_heights[i], 0 });

                if (src->exits[i].IsNull())
                    ride_clear_exit_location(dst, i);
                else
                    ride_set_exit_location(dst, i, { src->exits[i].x, src->exits[i].y, src->station_heights[i], 0 });

                dst->stations[i].LastPeepInQueue = EntityId::FromUnderlying(src->last_peep_in_queue[i]);

                dst->stations[i].SegmentLength = src->length[i];
                dst->stations[i].SegmentTime = src->time[i];

                dst->stations[i].QueueTime = src->queue_time[i];

                dst->stations[i].QueueLength = src->queue_length[i];
            }
            // All other values take 0 as their default. Since they're already memset to that, no need to do it again.
            for (int32_t i = Limits::MaxStationsPerRide; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
            {
                dst->stations[i].Start.SetNull();
                dst->stations[i].TrainAtStation = RideStation::NO_TRAIN;
                ride_clear_entrance_location(dst, i);
                ride_clear_exit_location(dst, i);
                dst->stations[i].LastPeepInQueue = EntityId::GetNull();
            }

            for (int32_t i = 0; i < Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::FromUnderlying(src->vehicles[i]);
            }
            for (int32_t i = Limits::MaxTrainsPerRide - 1; i <= OpenRCT2::Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::GetNull();
            }

            dst->depart_flags = src->depart_flags;

            dst->num_stations = src->num_stations;
            dst->num_vehicles = src->num_vehicles;
            dst->num_cars_per_train = src->num_cars_per_train;
            dst->proposed_num_vehicles = src->proposed_num_vehicles;
            dst->proposed_num_cars_per_train = src->proposed_num_cars_per_train;
            dst->max_trains = src->max_trains;
            dst->MinCarsPerTrain = src->GetMinCarsPerTrain();
            dst->MaxCarsPerTrain = src->GetMaxCarsPerTrain();
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

            if (src->cur_test_track_location.IsNull())
            {
                dst->CurTestTrackLocation.SetNull();
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

            for (uint8_t i = 0; i < Limits::CustomerHistorySize; i++)
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
            dst->total_profit = ToMoney64(src->total_profit);
            dst->popularity = src->popularity;
            dst->popularity_time_out = src->popularity_time_out;
            dst->popularity_next = src->popularity_next;

            ImportNumRiders(dst, rideIndex);

            dst->music_tune_id = src->music_tune_id;
            dst->slide_in_use = src->slide_in_use;
            // Includes maze_tiles
            dst->slide_peep = EntityId::FromUnderlying(src->slide_peep);
            // pad_160[0xE];
            dst->slide_peep_t_shirt_colour = src->slide_peep_t_shirt_colour;
            // pad_16F[0x7];
            dst->spiral_slide_progress = src->spiral_slide_progress;
            // pad_177[0x9];
            dst->build_date = static_cast<int32_t>(src->build_date);
            dst->upkeep_cost = src->upkeep_cost;
            dst->race_winner = EntityId::FromUnderlying(src->race_winner);
            // pad_186[0x02];
            dst->music_position = src->music_position;

            dst->breakdown_reason_pending = src->breakdown_reason_pending;
            dst->mechanic_status = src->mechanic_status;
            dst->mechanic = EntityId::FromUnderlying(src->mechanic);
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

            for (uint8_t i = 0; i < Limits::DowntimeHistorySize; i++)
            {
                dst->downtime_history[i] = src->downtime_history[i];
            }

            dst->no_primary_items_sold = src->no_primary_items_sold;
            dst->no_secondary_items_sold = src->no_secondary_items_sold;

            dst->breakdown_sound_modifier = src->breakdown_sound_modifier;
            dst->not_fixed_timeout = src->not_fixed_timeout;
            dst->last_crash_type = src->last_crash_type;
            dst->connected_message_throttle = src->connected_message_throttle;

            dst->income_per_hour = ToMoney64(src->income_per_hour);
            dst->profit = ToMoney64(src->profit);

            for (uint8_t i = 0; i < Limits::NumColourSchemes; i++)
            {
                dst->track_colour[i].main = src->track_colour_main[i];
                dst->track_colour[i].additional = src->track_colour_additional[i];
                dst->track_colour[i].supports = src->track_colour_supports[i];
            }
            // This stall was not colourable in RCT2.
            if (dst->type == RIDE_TYPE_FOOD_STALL)
            {
                auto object = object_entry_get_object(ObjectType::Ride, dst->subtype);
                if (object != nullptr && object->GetIdentifier() == "rct2.ride.icecr1")
                {
                    dst->track_colour[0].main = COLOUR_LIGHT_BLUE;
                }
            }

            auto musicStyle = OBJECT_ENTRY_INDEX_NULL;
            if (GetRideTypeDescriptor(dst->type).HasFlag(RIDE_TYPE_FLAG_ALLOW_MUSIC))
            {
                musicStyle = src->music;
            }
            dst->music = musicStyle;

            // In SV7, "plain" entrances are invisible.
            auto entranceStyle = OBJECT_ENTRY_INDEX_NULL;
            if (!_isSV7 && GetRideTypeDescriptor(dst->type).HasFlag(RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT))
            {
                entranceStyle = src->entrance_style;
            }
            dst->entrance_style = entranceStyle;

            dst->vehicle_change_timeout = src->vehicle_change_timeout;
            dst->num_block_brakes = src->num_block_brakes;
            dst->lift_hill_speed = src->lift_hill_speed;
            dst->guests_favourite = src->guests_favourite;
            dst->lifecycle_flags = src->lifecycle_flags;

            for (uint8_t i = 0; i < Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicle_colours[i].Ternary = src->vehicle_colours_extended[i];
            }

            dst->total_air_time = src->total_air_time;
            dst->current_test_station = src->current_test_station;
            dst->num_circuits = src->num_circuits;
            dst->CableLiftLoc = { src->cable_lift_x, src->cable_lift_y, src->cable_lift_z * COORDS_Z_STEP };
            // pad_1FD;
            dst->cable_lift = EntityId::FromUnderlying(src->cable_lift);

            // pad_208[0x58];
        }

        void ImportRideRatingsCalcData()
        {
            const auto& src = _s6.ride_ratings_calc_data;
            auto& dst = gRideRatingUpdateState;
            dst = {};
            dst.Proximity = { src.proximity_x, src.proximity_y, src.proximity_z };
            dst.ProximityStart = { src.proximity_start_x, src.proximity_start_y, src.proximity_start_z };
            dst.CurrentRide = RCT12RideIdToOpenRCT2RideId(src.current_ride);
            dst.State = src.state;
            if (src.current_ride < Limits::MaxRidesInPark && _s6.rides[src.current_ride].type < std::size(RideTypeDescriptors))
                dst.ProximityTrackType = RCT2TrackTypeToOpenRCT2(
                    src.proximity_track_type, _s6.rides[src.current_ride].type, IsFlatRide(src.current_ride));
            else
                dst.ProximityTrackType = 0xFF;
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
                    const auto rideId = RideId::FromUnderlying(src.ride_index);
                    auto ride = get_ride(rideId);
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
                if (researchItem.IsUninventedEndMarker() || researchItem.IsRandomEndMarker())
                {
                    break;
                }

                if (invented)
                    gResearchItemsInvented.emplace_back(researchItem.ToResearchItem());
                else
                    gResearchItemsUninvented.emplace_back(researchItem.ToResearchItem());
            }
        }

        void ImportBanner(Banner* dst, const RCT12Banner* src)
        {
            auto id = dst->id;

            *dst = {};
            dst->id = id;
            dst->type = RCTEntryIndexToOpenRCT2EntryIndex(src->type);
            dst->flags = src->flags;

            if (!(src->flags & BANNER_FLAG_LINKED_TO_RIDE) && is_user_string_id(src->string_idx))
            {
                dst->text = GetUserString(src->string_idx);
            }

            if (src->flags & BANNER_FLAG_LINKED_TO_RIDE)
            {
                dst->ride_index = RCT12RideIdToOpenRCT2RideId(src->ride_index);
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
            OpenRCT2::GetContext()->GetGameState()->InitAll({ _s6.map_size, _s6.map_size });
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
            for (size_t i = 0; i < Limits::MaxPeepSpawns; i++)
            {
                if (_s6.peep_spawns[i].x != RCT12_PEEP_SPAWN_UNDEFINED)
                {
                    PeepSpawn spawn = { _s6.peep_spawns[i].x, _s6.peep_spawns[i].y, _s6.peep_spawns[i].z * 16,
                                        _s6.peep_spawns[i].direction };
                    gPeepSpawns.push_back(spawn);
                }
            }
        }

        void ImportNumRiders(::Ride* dst, const RideId rideIndex)
        {
            // The number of riders might have overflown or underflown. Re-calculate the value.
            uint16_t numRiders = 0;
            for (const auto& sprite : _s6.sprites)
            {
                if (sprite.unknown.sprite_identifier == RCT12SpriteIdentifier::Peep)
                {
                    if (sprite.peep.current_ride == static_cast<RCT12RideId>(rideIndex.ToUnderlying())
                        && (static_cast<PeepState>(sprite.peep.state) == PeepState::OnRide
                            || static_cast<PeepState>(sprite.peep.state) == PeepState::EnteringRide))
                    {
                        numRiders++;
                    }
                }
            }
            dst->num_riders = numRiders;
        }

        void ImportTileElements()
        {
            // Build tile pointer cache (needed to get the first element at a certain location)
            auto tilePointerIndex = TilePointerIndex<RCT12TileElement>(
                Limits::MaxMapSize, _s6.tile_elements, std::size(_s6.tile_elements));

            std::vector<TileElement> tileElements;
            bool nextElementInvisible = false;
            bool restOfTileInvisible = false;
            const auto maxSize = std::min(Limits::MaxMapSize, _s6.map_size);
            for (TileCoordsXY coords = { 0, 0 }; coords.y < MAXIMUM_MAP_SIZE_TECHNICAL; coords.y++)
            {
                for (coords.x = 0; coords.x < MAXIMUM_MAP_SIZE_TECHNICAL; coords.x++)
                {
                    nextElementInvisible = false;
                    restOfTileInvisible = false;

                    auto tileAdded = false;
                    if (coords.x < maxSize && coords.y < maxSize)
                    {
                        const auto* srcElement = tilePointerIndex.GetFirstElementAt(coords);
                        if (srcElement != nullptr)
                        {
                            do
                            {
                                if (srcElement->base_height == RCT12::Limits::MaxElementHeight)
                                {
                                    continue;
                                }

                                auto tileElementType = srcElement->GetType();
                                if (tileElementType == RCT12TileElementType::Corrupt)
                                {
                                    // One property of corrupt elements was to hide tops of tower tracks, and to avoid the next
                                    // element from being hidden, multiple consecutive corrupt elements were sometimes used.
                                    // This would essentially toggle the flag, so we inverse nextElementInvisible here instead
                                    // of always setting it to true.
                                    nextElementInvisible = !nextElementInvisible;
                                    continue;
                                }
                                if (tileElementType == RCT12TileElementType::EightCarsCorrupt14
                                    || tileElementType == RCT12TileElementType::EightCarsCorrupt15)
                                {
                                    restOfTileInvisible = true;
                                    continue;
                                }

                                auto& dstElement = tileElements.emplace_back();
                                ImportTileElement(&dstElement, srcElement, nextElementInvisible || restOfTileInvisible);
                                nextElementInvisible = false;
                                tileAdded = true;
                            } while (!(srcElement++)->IsLastForTile());
                        }
                    }

                    if (!tileAdded)
                    {
                        // Add a default surface element, we always need at least one element per tile
                        auto& dstElement = tileElements.emplace_back();
                        dstElement.ClearAs(TileElementType::Surface);
                        dstElement.SetLastForTile(true);
                    }

                    // Set last element flag in case the original last element was never added
                    if (tileElements.size() > 0)
                    {
                        tileElements.back().SetLastForTile(true);
                    }
                }
            }
            SetTileElements(std::move(tileElements));
        }

        void ImportTileElement(TileElement* dst, const RCT12TileElement* src, bool invisible)
        {
            const auto rct12Type = src->GetType();
            dst->ClearAs(ToOpenRCT2TileElementType(rct12Type));
            dst->SetDirection(src->GetDirection());
            dst->SetBaseZ(src->base_height * COORDS_Z_STEP);
            dst->SetClearanceZ(src->clearance_height * COORDS_Z_STEP);

            // All saved in "flags"
            dst->SetOccupiedQuadrants(src->GetOccupiedQuadrants());
            dst->SetGhost(src->IsGhost());
            dst->SetLastForTile(src->IsLastForTile());
            dst->SetInvisible(invisible);

            switch (rct12Type)
            {
                case RCT12TileElementType::Surface:
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
                case RCT12TileElementType::Path:
                {
                    auto dst2 = dst->AsPath();
                    auto src2 = src->AsPath();

                    auto pathEntryIndex = src2->GetEntryIndex();
                    auto surfaceEntry = src2->IsQueue() ? _pathToQueueSurfaceMap[pathEntryIndex]
                                                        : _pathToSurfaceMap[pathEntryIndex];
                    if (surfaceEntry == OBJECT_ENTRY_INDEX_NULL)
                    {
                        // Legacy footpath object
                        dst2->SetLegacyPathEntryIndex(pathEntryIndex);
                    }
                    else
                    {
                        // Surface / railing
                        dst2->SetSurfaceEntryIndex(surfaceEntry);
                        dst2->SetRailingsEntryIndex(_pathToRailingMap[pathEntryIndex]);
                    }

                    dst2->SetQueueBannerDirection(src2->GetQueueBannerDirection());
                    dst2->SetSloped(src2->IsSloped());
                    dst2->SetSlopeDirection(src2->GetSlopeDirection());
                    dst2->SetRideIndex(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
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
                case RCT12TileElementType::Track:
                {
                    auto dst2 = dst->AsTrack();
                    auto src2 = src->AsTrack();

                    auto rideType = _s6.rides[src2->GetRideIndex()].type;
                    track_type_t trackType = static_cast<track_type_t>(src2->GetTrackType());

                    dst2->SetTrackType(RCT2TrackTypeToOpenRCT2(trackType, rideType, IsFlatRide(src2->GetRideIndex())));
                    dst2->SetRideType(rideType);
                    dst2->SetSequenceIndex(src2->GetSequenceIndex());
                    dst2->SetRideIndex(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    dst2->SetColourScheme(src2->GetColourScheme());
                    dst2->SetHasChain(src2->HasChain());
                    dst2->SetHasCableLift(src2->HasCableLift());
                    dst2->SetInverted(src2->IsInverted());
                    dst2->SetStationIndex(src2->GetStationIndex());
                    dst2->SetHasGreenLight(src2->HasGreenLight());
                    dst2->SetBlockBrakeClosed(src2->BlockBrakeClosed());
                    dst2->SetIsIndestructible(src2->IsIndestructible());
                    // Skipping IsHighlighted()

                    if (TrackTypeHasSpeedSetting(trackType))
                    {
                        dst2->SetBrakeBoosterSpeed(src2->GetBrakeBoosterSpeed());
                    }
                    else if (trackType == TrackElemType::OnRidePhoto)
                    {
                        dst2->SetPhotoTimeout(src2->GetPhotoTimeout());
                    }

                    // This has to be done last, since the maze entry shares fields with the colour and sequence fields.
                    if (rideType == RIDE_TYPE_MAZE)
                    {
                        dst2->SetMazeEntry(src2->GetMazeEntry());
                    }
                    else if (rideType == RIDE_TYPE_GHOST_TRAIN)
                    {
                        dst2->SetDoorAState(src2->GetDoorAState());
                        dst2->SetDoorBState(src2->GetDoorBState());
                    }
                    else
                    {
                        dst2->SetSeatRotation(src2->GetSeatRotation());
                    }

                    break;
                }
                case RCT12TileElementType::SmallScenery:
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
                case RCT12TileElementType::Entrance:
                {
                    auto dst2 = dst->AsEntrance();
                    auto src2 = src->AsEntrance();

                    dst2->SetEntranceType(src2->GetEntranceType());
                    dst2->SetRideIndex(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    dst2->SetStationIndex(src2->GetStationIndex());
                    dst2->SetSequenceIndex(src2->GetSequenceIndex());

                    if (src2->GetSequenceIndex() == 0)
                    {
                        auto pathEntryIndex = src2->GetPathType();
                        auto surfaceEntry = _pathToSurfaceMap[pathEntryIndex];
                        if (surfaceEntry == OBJECT_ENTRY_INDEX_NULL)
                        {
                            // Legacy footpath object
                            dst2->SetLegacyPathEntryIndex(pathEntryIndex);
                        }
                        else
                        {
                            // Surface
                            dst2->SetSurfaceEntryIndex(surfaceEntry);
                        }
                    }
                    else
                    {
                        dst2->SetSurfaceEntryIndex(OBJECT_ENTRY_INDEX_NULL);
                    }
                    break;
                }
                case RCT12TileElementType::Wall:
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
                    dst2->SetBannerIndex(BannerIndex::GetNull());
                    auto entry = dst2->GetEntry();
                    if (entry != nullptr && entry->scrolling_mode != SCROLLING_MODE_NONE)
                    {
                        auto bannerIndex = src2->GetBannerIndex();
                        if (bannerIndex < std::size(_s6.banners))
                        {
                            auto srcBanner = &_s6.banners[bannerIndex];
                            auto dstBanner = GetOrCreateBanner(BannerIndex::FromUnderlying(bannerIndex));
                            if (dstBanner == nullptr)
                            {
                                dst2->SetBannerIndex(BannerIndex::GetNull());
                            }
                            else
                            {
                                ImportBanner(dstBanner, srcBanner);
                                dst2->SetBannerIndex(BannerIndex::FromUnderlying(src2->GetBannerIndex()));
                            }
                        }
                    }
                    break;
                }
                case RCT12TileElementType::LargeScenery:
                {
                    auto dst2 = dst->AsLargeScenery();
                    auto src2 = src->AsLargeScenery();

                    dst2->SetEntryIndex(src2->GetEntryIndex());
                    dst2->SetSequenceIndex(src2->GetSequenceIndex());
                    dst2->SetPrimaryColour(src2->GetPrimaryColour());
                    dst2->SetSecondaryColour(src2->GetSecondaryColour());

                    // Import banner information
                    dst2->SetBannerIndex(BannerIndex::GetNull());
                    auto entry = dst2->GetEntry();
                    if (entry != nullptr && entry->scrolling_mode != SCROLLING_MODE_NONE)
                    {
                        auto bannerIndex = src2->GetBannerIndex();
                        if (bannerIndex < std::size(_s6.banners))
                        {
                            auto srcBanner = &_s6.banners[bannerIndex];
                            auto dstBanner = GetOrCreateBanner(BannerIndex::FromUnderlying(bannerIndex));
                            if (dstBanner == nullptr)
                            {
                                dst2->SetBannerIndex(BannerIndex::GetNull());
                            }
                            else
                            {
                                ImportBanner(dstBanner, srcBanner);
                                dst2->SetBannerIndex(BannerIndex::FromUnderlying(src2->GetBannerIndex()));
                            }
                        }
                    }
                    break;
                }
                case RCT12TileElementType::Banner:
                {
                    auto dst2 = dst->AsBanner();
                    auto src2 = src->AsBanner();

                    dst2->SetPosition(src2->GetPosition());
                    dst2->SetAllowedEdges(src2->GetAllowedEdges());

                    auto bannerIndex = src2->GetIndex();
                    if (bannerIndex < std::size(_s6.banners))
                    {
                        auto srcBanner = &_s6.banners[bannerIndex];
                        auto dstBanner = GetOrCreateBanner(BannerIndex::FromUnderlying(bannerIndex));
                        if (dstBanner == nullptr)
                        {
                            dst2->SetIndex(BannerIndex::GetNull());
                        }
                        else
                        {
                            ImportBanner(dstBanner, srcBanner);
                            dst2->SetIndex(BannerIndex::FromUnderlying(bannerIndex));
                        }
                    }
                    else
                    {
                        dst2->SetIndex(BannerIndex::GetNull());
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
                        campaign.RideId = RCT12RideIdToOpenRCT2RideId(_s6.campaign_ride_index[i]);
                    }
                    else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
                    {
                        campaign.ShopItemType = ShopItem(_s6.campaign_ride_index[i]);
                    }
                    gMarketingCampaigns.push_back(campaign);
                }
            }
        }

        void ImportStaffPatrolArea(Staff* staffmember, uint8_t staffId)
        {
            // First check staff mode as vanilla did not clean up patrol areas when switching from patrol to walk
            // without doing this we could accidentally add a patrol when it didn't exist.
            if (_s6.staff_modes[staffId] != StaffMode::Patrol)
            {
                return;
            }
            int32_t peepOffset = staffId * Limits::PatrolAreaSize;
            for (int32_t i = 0; i < Limits::PatrolAreaSize; i++)
            {
                if (_s6.patrol_areas[peepOffset + i] == 0)
                {
                    // No patrol for this area
                    continue;
                }

                // Loop over the bits of the uint32_t
                for (int32_t j = 0; j < 32; j++)
                {
                    int8_t bit = (_s6.patrol_areas[peepOffset + i] >> j) & 1;
                    if (bit == 0)
                    {
                        // No patrol for this area
                        continue;
                    }
                    // val contains the 6 highest bits of both the x and y coordinates
                    int32_t val = j | (i << 5);
                    int32_t x = val & 0x03F;
                    x <<= 7;
                    int32_t y = val & 0xFC0;
                    y <<= 1;
                    staffmember->SetPatrolArea({ x, y }, true);
                }
            }
        }

        void ImportEntities()
        {
            for (int32_t i = 0; i < Limits::MaxEntities; i++)
            {
                ImportEntity(_s6.sprites[i].unknown);
            }
        }

        template<typename OpenRCT2_T> void ImportEntity(const RCT12SpriteBase& src);

        void ImportEntityPeep(::Peep* dst, const Peep* src)
        {
            const auto isNullLocation = [](const rct12_xyzd8& pos) {
                return pos.x == 0xFF && pos.y == 0xFF && pos.z == 0xFF && pos.direction == INVALID_DIRECTION;
            };

            ImportEntityCommonProperties(static_cast<EntityBase*>(dst), src);
            if (is_user_string_id(src->name_string_idx))
            {
                dst->SetName(GetUserString(src->name_string_idx));
            }
            dst->NextLoc = { src->next_x, src->next_y, src->next_z * COORDS_Z_STEP };
            dst->NextFlags = src->next_flags;
            dst->State = static_cast<PeepState>(src->state);
            dst->SubState = src->sub_state;
            dst->SpriteType = static_cast<PeepSpriteType>(src->sprite_type);
            dst->TshirtColour = src->tshirt_colour;
            dst->TrousersColour = src->trousers_colour;
            dst->DestinationX = src->destination_x;
            dst->DestinationY = src->destination_y;
            dst->DestinationTolerance = src->destination_tolerance;
            dst->Var37 = src->var_37;
            dst->Energy = src->energy;
            dst->EnergyTarget = src->energy_target;
            dst->Mass = src->mass;
            dst->WindowInvalidateFlags = src->window_invalidate_flags;
            dst->CurrentRide = RCT12RideIdToOpenRCT2RideId(src->current_ride);
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
            dst->PeepDirection = src->direction;
            dst->InteractionRideIndex = RCT12RideIdToOpenRCT2RideId(src->interaction_ride_index);
            dst->Id = src->id;
            dst->PathCheckOptimisation = src->path_check_optimisation;
            dst->PeepFlags = src->peep_flags;
            if (isNullLocation(src->pathfind_goal))
            {
                dst->PathfindGoal.SetNull();
                dst->PathfindGoal.direction = INVALID_DIRECTION;
            }
            else
            {
                dst->PathfindGoal = { src->pathfind_goal.x, src->pathfind_goal.y, src->pathfind_goal.z,
                                      src->pathfind_goal.direction };
            }
            for (size_t i = 0; i < std::size(src->pathfind_history); i++)
            {
                if (isNullLocation(src->pathfind_history[i]))
                {
                    dst->PathfindHistory[i].SetNull();
                    dst->PathfindHistory[i].direction = INVALID_DIRECTION;
                }
                else
                {
                    dst->PathfindHistory[i] = { src->pathfind_history[i].x, src->pathfind_history[i].y,
                                                src->pathfind_history[i].z, src->pathfind_history[i].direction };
                }
            }
            dst->WalkingFrameNum = src->no_action_frame_num;
        }

        constexpr EntityType GetEntityTypeFromRCT2Sprite(const RCT12SpriteBase* src)
        {
            EntityType output = EntityType::Null;
            switch (src->sprite_identifier)
            {
                case RCT12SpriteIdentifier::Vehicle:
                    output = EntityType::Vehicle;
                    break;
                case RCT12SpriteIdentifier::Peep:
                    if (RCT12PeepType(static_cast<const Peep*>(src)->peep_type) == RCT12PeepType::Guest)
                    {
                        output = EntityType::Guest;
                    }
                    else
                    {
                        output = EntityType::Staff;
                    }
                    break;
                case RCT12SpriteIdentifier::Misc:

                    switch (RCT12MiscEntityType(src->type))
                    {
                        case RCT12MiscEntityType::SteamParticle:
                            output = EntityType::SteamParticle;
                            break;
                        case RCT12MiscEntityType::MoneyEffect:
                            output = EntityType::MoneyEffect;
                            break;
                        case RCT12MiscEntityType::CrashedVehicleParticle:
                            output = EntityType::CrashedVehicleParticle;
                            break;
                        case RCT12MiscEntityType::ExplosionCloud:
                            output = EntityType::ExplosionCloud;
                            break;
                        case RCT12MiscEntityType::CrashSplash:
                            output = EntityType::CrashSplash;
                            break;
                        case RCT12MiscEntityType::ExplosionFlare:
                            output = EntityType::ExplosionFlare;
                            break;
                        case RCT12MiscEntityType::JumpingFountainWater:
                        case RCT12MiscEntityType::JumpingFountainSnow:
                            output = EntityType::JumpingFountain;
                            break;
                        case RCT12MiscEntityType::Balloon:
                            output = EntityType::Balloon;
                            break;
                        case RCT12MiscEntityType::Duck:
                            output = EntityType::Duck;
                            break;
                        default:
                            break;
                    }
                    break;
                case RCT12SpriteIdentifier::Litter:
                    output = EntityType::Litter;
                    break;
                default:
                    break;
            }
            return output;
        }

        void ImportEntityCommonProperties(EntityBase* dst, const RCT12SpriteBase* src)
        {
            dst->Type = GetEntityTypeFromRCT2Sprite(src);
            dst->sprite_height_negative = src->sprite_height_negative;
            dst->sprite_index = EntityId::FromUnderlying(src->sprite_index);
            dst->x = src->x;
            dst->y = src->y;
            dst->z = src->z;
            dst->sprite_width = src->sprite_width;
            dst->sprite_height_positive = src->sprite_height_positive;
            dst->SpriteRect = ScreenRect(src->sprite_left, src->sprite_top, src->sprite_right, src->sprite_bottom);
            dst->sprite_direction = src->sprite_direction;
        }

        void ImportEntity(const RCT12SpriteBase& src);

        std::string GetUserString(rct_string_id stringId)
        {
            const auto originalString = _s6.custom_strings[(stringId - USER_STRING_START) % 1024];
            auto originalStringView = std::string_view(
                originalString, GetRCT2StringBufferLen(originalString, USER_STRING_MAX_LENGTH));
            auto asUtf8 = rct2_to_utf8(originalStringView, RCT2LanguageId::EnglishUK);
            auto justText = RCT12RemoveFormattingUTF8(asUtf8);
            return justText.data();
        }

        ObjectList GetRequiredObjects()
        {
            std::fill(std::begin(_pathToSurfaceMap), std::end(_pathToSurfaceMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_pathToQueueSurfaceMap), std::end(_pathToQueueSurfaceMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_pathToRailingMap), std::end(_pathToRailingMap), OBJECT_ENTRY_INDEX_NULL);

            ObjectList objectList;
            int objectIt = 0;
            ObjectEntryIndex surfaceCount = 0;
            ObjectEntryIndex railingCount = 0;
            for (int16_t objectType = EnumValue(ObjectType::Ride); objectType <= EnumValue(ObjectType::Water); objectType++)
            {
                for (int16_t i = 0; i < rct2_object_entry_group_counts[objectType]; i++, objectIt++)
                {
                    auto entry = ObjectEntryDescriptor(_s6.Objects[objectIt]);
                    if (entry.HasValue())
                    {
                        if (objectType == EnumValue(ObjectType::Paths))
                        {
                            auto footpathMapping = GetFootpathSurfaceId(entry);
                            if (footpathMapping == nullptr)
                            {
                                // Unsupported footpath
                                objectList.SetObject(i, entry);
                            }
                            else
                            {
                                // We have surface objects for this footpath
                                auto surfaceIndex = objectList.Find(
                                    ObjectType::FootpathSurface, footpathMapping->NormalSurface);
                                if (surfaceIndex == OBJECT_ENTRY_INDEX_NULL)
                                {
                                    objectList.SetObject(
                                        ObjectType::FootpathSurface, surfaceCount, footpathMapping->NormalSurface);
                                    surfaceIndex = surfaceCount++;
                                }
                                _pathToSurfaceMap[i] = surfaceIndex;

                                surfaceIndex = objectList.Find(ObjectType::FootpathSurface, footpathMapping->QueueSurface);
                                if (surfaceIndex == OBJECT_ENTRY_INDEX_NULL)
                                {
                                    objectList.SetObject(
                                        ObjectType::FootpathSurface, surfaceCount, footpathMapping->QueueSurface);
                                    surfaceIndex = surfaceCount++;
                                }
                                _pathToQueueSurfaceMap[i] = surfaceIndex;

                                auto railingIndex = objectList.Find(ObjectType::FootpathRailings, footpathMapping->Railing);
                                if (railingIndex == OBJECT_ENTRY_INDEX_NULL)
                                {
                                    objectList.SetObject(ObjectType::FootpathRailings, railingCount, footpathMapping->Railing);
                                    railingIndex = railingCount++;
                                }
                                _pathToRailingMap[i] = railingIndex;
                            }
                        }
                        else
                        {
                            objectList.SetObject(i, entry);
                        }
                    }
                }
            }

            // Add default rct2 terrain surfaces and edges
            AddDefaultEntries();

            // Find if any rct1 terrain surfaces or edges have been used
            const bool hasRCT1Terrain = std::any_of(
                std::begin(_s6.tile_elements), std::end(_s6.tile_elements), [](RCT12TileElement& tile) {
                    auto* surface = tile.AsSurface();
                    if (surface == nullptr)
                    {
                        return false;
                    }
                    if (surface->GetSurfaceStyle() >= std::size(RCT2::DefaultTerrainSurfaces))
                    {
                        return true;
                    }
                    if (surface->GetEdgeStyle() >= std::size(RCT2::DefaultTerrainEdges))
                    {
                        return true;
                    }
                    return false;
                });

            // If an rct1 surface or edge then load all the Hybrid surfaces and edges
            if (hasRCT1Terrain)
            {
                _terrainSurfaceEntries.AddRange(OpenRCT2HybridTerrainSurfaces);
                _terrainEdgeEntries.AddRange(OpenRCT2HybridTerrainEdges);
            }

            AppendRequiredObjects(objectList, ObjectType::TerrainSurface, _terrainSurfaceEntries);
            AppendRequiredObjects(objectList, ObjectType::TerrainEdge, _terrainEdgeEntries);
            RCT12AddDefaultObjects(objectList);
            return objectList;
        }
    };

    template<> void S6Importer::ImportEntity<::Vehicle>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Vehicle>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT2::Vehicle*>(&baseSrc);
        const auto& ride = _s6.rides[src->ride];

        ImportEntityCommonProperties(dst, src);
        dst->SubType = ::Vehicle::Type(src->type);
        dst->Pitch = src->Pitch;
        dst->bank_rotation = src->bank_rotation;
        dst->remaining_distance = src->remaining_distance;
        dst->velocity = src->velocity;
        dst->acceleration = src->acceleration;
        dst->ride = RideId::FromUnderlying(src->ride);
        dst->vehicle_type = src->vehicle_type;
        dst->colours = src->colours;
        dst->track_progress = src->track_progress;
        dst->TrackLocation = { src->track_x, src->track_y, src->track_z };
        if (src->boat_location.IsNull() || static_cast<RideMode>(ride.mode) != RideMode::BoatHire
            || src->status != static_cast<uint8_t>(::Vehicle::Status::TravellingBoat))
        {
            dst->BoatLocation.SetNull();
            dst->SetTrackDirection(src->GetTrackDirection());
            dst->SetTrackType(src->GetTrackType());
            // RotationControlToggle and Booster are saved as the same track piece ID
            // Which one the vehicle is using must be determined
            if (IsFlatRide(src->ride))
            {
                dst->SetTrackType(RCT12FlatTrackTypeToOpenRCT2(src->GetTrackType()));
            }
            else if (src->GetTrackType() == TrackElemType::RotationControlToggleAlias)
            {
                // Merging hacks mean the track type that's appropriate for the ride type is not necessarily the track type the
                // ride is on. It's possible to create unwanted behavior if a user layers spinning control track on top of
                // booster track but this is unlikely since only two rides have spinning control track - by default they load as
                // booster
                TileElement* tileElement2 = map_get_track_element_at_of_type_seq(
                    dst->TrackLocation, TrackElemType::RotationControlToggle, 0);

                if (tileElement2 != nullptr)
                    dst->SetTrackType(TrackElemType::RotationControlToggle);
            }
        }
        else
        {
            dst->BoatLocation = TileCoordsXY{ src->boat_location.x, src->boat_location.y }.ToCoordsXY();
            dst->SetTrackDirection(0);
            dst->SetTrackType(0);
        }

        dst->next_vehicle_on_train = EntityId::FromUnderlying(src->next_vehicle_on_train);
        dst->prev_vehicle_on_ride = EntityId::FromUnderlying(src->prev_vehicle_on_ride);
        dst->next_vehicle_on_ride = EntityId::FromUnderlying(src->next_vehicle_on_ride);
        dst->var_44 = src->var_44;
        dst->mass = src->mass;
        dst->update_flags = src->update_flags;
        dst->SwingSprite = src->SwingSprite;
        dst->current_station = src->current_station;
        dst->current_time = src->current_time;
        dst->crash_z = src->crash_z;

        ::Vehicle::Status statusSrc = ::Vehicle::Status::MovingToEndOfStation;
        if (src->status <= static_cast<uint8_t>(::Vehicle::Status::StoppedByBlockBrakes))
        {
            statusSrc = static_cast<::Vehicle::Status>(src->status);
        }

        dst->status = statusSrc;
        dst->sub_state = src->sub_state;
        for (size_t i = 0; i < std::size(src->peep); i++)
        {
            dst->peep[i] = EntityId::FromUnderlying(src->peep[i]);
            dst->peep_tshirt_colours[i] = src->peep_tshirt_colours[i];
        }
        dst->num_seats = src->num_seats;
        dst->num_peeps = src->num_peeps;
        dst->next_free_seat = src->next_free_seat;
        dst->restraints_position = src->restraints_position;
        dst->crash_x = src->crash_x;
        dst->sound2_flags = src->sound2_flags;
        dst->spin_sprite = src->spin_sprite;
        dst->sound1_id = static_cast<OpenRCT2::Audio::SoundId>(src->sound1_id);
        dst->sound1_volume = src->sound1_volume;
        dst->sound2_id = static_cast<OpenRCT2::Audio::SoundId>(src->sound2_id);
        dst->sound2_volume = src->sound2_volume;
        dst->sound_vector_factor = src->sound_vector_factor;
        dst->time_waiting = src->time_waiting;
        dst->speed = src->speed;
        dst->powered_acceleration = src->powered_acceleration;
        dst->CollisionDetectionDirection = src->CollisionDetectionDirection;
        dst->animation_frame = src->animation_frame;
        dst->animationState = src->animationState;
        dst->scream_sound_id = static_cast<OpenRCT2::Audio::SoundId>(src->scream_sound_id);
        dst->TrackSubposition = VehicleTrackSubposition{ src->TrackSubposition };
        dst->var_CE = src->var_CE;
        dst->brake_speed = src->brake_speed;
        dst->lost_time_out = src->lost_time_out;
        dst->vertical_drop_countdown = src->vertical_drop_countdown;
        dst->var_D3 = src->var_D3;
        dst->mini_golf_current_animation = MiniGolfAnimation(src->mini_golf_current_animation);
        dst->mini_golf_flags = src->mini_golf_flags;
        dst->ride_subtype = RCTEntryIndexToOpenRCT2EntryIndex(src->ride_subtype);
        dst->colours_extended = src->colours_extended;
        dst->seat_rotation = src->seat_rotation;
        dst->target_seat_rotation = src->target_seat_rotation;
        dst->IsCrashedVehicle = src->flags & RCT12_SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE;
    }

    static uint32_t AdjustScenarioToCurrentTicks(const S6Data& s6, uint32_t tick)
    {
        // Previously gScenarioTicks was used as a time point, now it's gCurrentTicks.
        // gCurrentTicks and gScenarioTicks are now exported as the same, older saves that have a different
        // scenario tick must account for the difference between the two.
        uint32_t ticksElapsed = s6.scenario_ticks - tick;
        return s6.game_ticks_1 - ticksElapsed;
    }

    template<> void S6Importer::ImportEntity<::Guest>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Guest>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const Peep*>(&baseSrc);
        ImportEntityPeep(dst, src);

        dst->OutsideOfPark = static_cast<bool>(src->outside_of_park);
        dst->GuestNumRides = src->no_of_rides;
        dst->Happiness = src->happiness;
        dst->HappinessTarget = src->happiness_target;
        dst->Nausea = src->nausea;
        dst->NauseaTarget = src->nausea_target;
        dst->Hunger = src->hunger;
        dst->Thirst = src->thirst;
        dst->Toilet = src->toilet;
        dst->TimeToConsume = src->time_to_consume;
        dst->Intensity = static_cast<IntensityRange>(src->intensity);
        dst->NauseaTolerance = static_cast<PeepNauseaTolerance>(src->nausea_tolerance);
        dst->PaidOnDrink = src->paid_on_drink;

        OpenRCT2::RideUse::GetHistory().Set(dst->sprite_index, RCT12GetRidesBeenOn(src));
        OpenRCT2::RideUse::GetTypeHistory().Set(dst->sprite_index, RCT12GetRideTypesBeenOn(src));

        dst->SetItemFlags(src->GetItemFlags());
        dst->Photo1RideRef = RCT12RideIdToOpenRCT2RideId(src->photo1_ride_ref);
        dst->Photo2RideRef = RCT12RideIdToOpenRCT2RideId(src->photo2_ride_ref);
        dst->Photo3RideRef = RCT12RideIdToOpenRCT2RideId(src->photo3_ride_ref);
        dst->Photo4RideRef = RCT12RideIdToOpenRCT2RideId(src->photo4_ride_ref);
        dst->GuestNextInQueue = EntityId::FromUnderlying(src->next_in_queue);
        dst->TimeInQueue = src->time_in_queue;
        dst->CashInPocket = src->cash_in_pocket;
        dst->CashSpent = src->cash_spent;
        dst->ParkEntryTime = AdjustScenarioToCurrentTicks(_s6, src->park_entry_time);
        dst->RejoinQueueTimeout = src->rejoin_queue_timeout;
        dst->PreviousRide = RCT12RideIdToOpenRCT2RideId(src->previous_ride);
        dst->PreviousRideTimeOut = src->previous_ride_time_out;
        for (size_t i = 0; i < std::size(src->thoughts); i++)
        {
            auto srcThought = &src->thoughts[i];
            auto dstThought = &dst->Thoughts[i];
            dstThought->type = static_cast<PeepThoughtType>(srcThought->type);
            if (srcThought->item == RCT12PeepThoughtItemNone)
                dstThought->item = PeepThoughtItemNone;
            else
                dstThought->item = srcThought->item;
            dstThought->freshness = srcThought->freshness;
            dstThought->fresh_timeout = srcThought->fresh_timeout;
        }
        dst->GuestHeadingToRideId = RCT12RideIdToOpenRCT2RideId(src->guest_heading_to_ride_id);
        dst->GuestIsLostCountdown = src->peep_is_lost_countdown;
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
        dst->VoucherRideId = RCT12RideIdToOpenRCT2RideId(src->voucher_arguments);
        dst->SurroundingsThoughtTimeout = src->surroundings_thought_timeout;
        dst->Angriness = src->angriness;
        dst->TimeLost = src->time_lost;
        dst->DaysInQueue = src->days_in_queue;
        dst->BalloonColour = src->balloon_colour;
        dst->UmbrellaColour = src->umbrella_colour;
        dst->HatColour = src->hat_colour;
        dst->FavouriteRide = RCT12RideIdToOpenRCT2RideId(src->favourite_ride);
        dst->FavouriteRideRating = src->favourite_ride_rating;
    }

    template<> void S6Importer::ImportEntity<::Staff>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Staff>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const Peep*>(&baseSrc);
        ImportEntityPeep(dst, src);

        dst->AssignedStaffType = StaffType(src->staff_type);
        dst->MechanicTimeSinceCall = src->mechanic_time_since_call;

        dst->HireDate = src->park_entry_time;
        dst->StaffOrders = src->staff_orders;
        dst->StaffMowingTimeout = src->staff_mowing_timeout;
        dst->StaffLawnsMown = src->paid_to_enter;
        dst->StaffGardensWatered = src->paid_on_rides;
        dst->StaffLitterSwept = src->paid_on_food;
        dst->StaffBinsEmptied = src->paid_on_souvenirs;

        ImportStaffPatrolArea(dst, src->staff_id);
    }

    template<> void S6Importer::ImportEntity<::SteamParticle>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::SteamParticle>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT12SpriteSteamParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->time_to_move = src->time_to_move;
        dst->frame = src->frame;
    }

    template<> void S6Importer::ImportEntity<::MoneyEffect>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::MoneyEffect>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT12SpriteMoneyEffect*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->MoveDelay = src->move_delay;
        dst->NumMovements = src->num_movements;
        dst->Vertical = src->vertical;
        dst->Value = src->value;
        dst->OffsetX = src->offset_x;
        dst->Wiggle = src->wiggle;
    }

    template<> void S6Importer::ImportEntity<::VehicleCrashParticle>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::VehicleCrashParticle>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT12SpriteCrashedVehicleParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
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
    }

    template<> void S6Importer::ImportEntity<::ExplosionCloud>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::ExplosionCloud>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT12SpriteParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->frame;
    }

    template<> void S6Importer::ImportEntity<::ExplosionFlare>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::ExplosionFlare>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT12SpriteParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->frame;
    }

    template<> void S6Importer::ImportEntity<::CrashSplashParticle>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::CrashSplashParticle>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT12SpriteParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->frame;
    }

    template<> void S6Importer::ImportEntity<::JumpingFountain>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::JumpingFountain>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT12SpriteJumpingFountain*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->NumTicksAlive = src->num_ticks_alive;
        dst->frame = src->frame;
        dst->FountainFlags = src->fountain_flags;
        dst->TargetX = src->target_x;
        dst->TargetY = src->target_y;
        dst->Iteration = src->iteration;
        dst->FountainType = RCT12MiscEntityType(src->type) == RCT12MiscEntityType::JumpingFountainSnow
            ? ::JumpingFountainType::Snow
            : ::JumpingFountainType::Water;
    }

    template<> void S6Importer::ImportEntity<::Balloon>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Balloon>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT12SpriteBalloon*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->popped = src->popped;
        dst->time_to_move = src->time_to_move;
        dst->frame = src->frame;
        dst->colour = src->colour;
    }

    template<> void S6Importer::ImportEntity<::Duck>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Duck>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT12SpriteDuck*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->frame;
        dst->target_x = src->target_x;
        dst->target_y = src->target_y;
        dst->state = static_cast<::Duck::DuckState>(src->state);
    }

    template<> void S6Importer::ImportEntity<::Litter>(const RCT12SpriteBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Litter>(EntityId::FromUnderlying(baseSrc.sprite_index));
        auto src = static_cast<const RCT12SpriteLitter*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->SubType = ::Litter::Type(src->type);
        dst->creationTick = AdjustScenarioToCurrentTicks(_s6, src->creationTick);
    }

    void S6Importer::ImportEntity(const RCT12SpriteBase& src)
    {
        switch (GetEntityTypeFromRCT2Sprite(&src))
        {
            case EntityType::Vehicle:
                ImportEntity<::Vehicle>(src);
                break;
            case EntityType::Guest:
                ImportEntity<::Guest>(src);
                break;
            case EntityType::Staff:
                ImportEntity<::Staff>(src);
                break;
            case EntityType::SteamParticle:
                ImportEntity<::SteamParticle>(src);
                break;
            case EntityType::MoneyEffect:
                ImportEntity<::MoneyEffect>(src);
                break;
            case EntityType::CrashedVehicleParticle:
                ImportEntity<::VehicleCrashParticle>(src);
                break;
            case EntityType::ExplosionCloud:
                ImportEntity<::ExplosionCloud>(src);
                break;
            case EntityType::ExplosionFlare:
                ImportEntity<::ExplosionFlare>(src);
                break;
            case EntityType::CrashSplash:
                ImportEntity<::CrashSplashParticle>(src);
                break;
            case EntityType::JumpingFountain:
                ImportEntity<::JumpingFountain>(src);
                break;
            case EntityType::Balloon:
                ImportEntity<::Balloon>(src);
                break;
            case EntityType::Duck:
                ImportEntity<::Duck>(src);
                break;
            case EntityType::Litter:
                ImportEntity<::Litter>(src);
                break;
            default:
                // Null elements do not need imported
                break;
        }
    }
} // namespace RCT2

std::unique_ptr<IParkImporter> ParkImporter::CreateS6(IObjectRepository& objectRepository)
{
    return std::make_unique<RCT2::S6Importer>(objectRepository);
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
    auto s6Importer = std::make_unique<RCT2::S6Importer>(context->GetObjectRepository());
    try
    {
        auto& objectMgr = context->GetObjectManager();
        auto result = s6Importer->LoadSavedGame(path);
        objectMgr.LoadObjects(result.RequiredObjects);
        s6Importer->Import();
        game_fix_save_vars();
        AutoCreateMapAnimations();
        EntityTweener::Get().Reset();
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
    catch (const UnsupportedRideTypeException&)
    {
        show_error(ERROR_TYPE_FILE_LOAD, STR_FILE_CONTAINS_UNSUPPORTED_RIDE_TYPES);
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
    auto s6Importer = std::make_unique<RCT2::S6Importer>(context->GetObjectRepository());
    try
    {
        auto result = s6Importer->LoadScenario(path);
        objManager.LoadObjects(result.RequiredObjects);
        s6Importer->Import();
        game_fix_save_vars();
        AutoCreateMapAnimations();
        EntityTweener::Get().Reset();
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
