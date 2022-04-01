/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Scenario.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../FileClassifier.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../PlatformEnvironment.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/Path.hpp"
#include "../core/Random.hpp"
#include "../entity/Duck.h"
#include "../entity/Guest.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/network.h"
#include "../object/Object.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../platform/Platform.h"
#include "../profiling/Profiling.h"
#include "../rct1/RCT1.h"
#include "../rct12/RCT12.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Water.h"
#include "ScenarioRepository.h"
#include "ScenarioSources.h"

#include <algorithm>

const rct_string_id ScenarioCategoryStringIds[SCENARIO_CATEGORY_COUNT] = {
    STR_BEGINNER_PARKS, STR_CHALLENGING_PARKS,    STR_EXPERT_PARKS, STR_REAL_PARKS, STR_OTHER_PARKS,

    STR_DLC_PARKS,      STR_BUILD_YOUR_OWN_PARKS,
};

SCENARIO_CATEGORY gScenarioCategory;
std::string gScenarioName;
std::string gScenarioDetails;
std::string gScenarioObjectiveDescription;
std::string gScenarioCompletedBy;
std::string gScenarioSavePath;
bool gFirstTimeSaving = true;
uint16_t gSavedAge;
uint32_t gLastAutoSaveUpdate = 0;

random_engine_t gScenarioRand;

Objective gScenarioObjective;

bool gAllowEarlyCompletionInNetworkPlay;
uint16_t gScenarioParkRatingWarningDays;
std::vector<uint16_t> gScenarioObjectiveWarningDays;
money64 gScenarioCompletedCompanyValue;
money64 gScenarioCompanyValueRecord;

std::string gScenarioFileName;

static void scenario_objective_check();

using namespace OpenRCT2;

void scenario_begin()
{
    game_load_init();

    // Set the scenario pseudo-random seeds
    Random::Rct2::Seed s{ 0x1234567F ^ Platform::GetTicks(), 0x789FABCD ^ Platform::GetTicks() };
    gScenarioRand.seed(s);

    gParkFlags &= ~PARK_FLAGS_NO_MONEY;
    if (gParkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)
        gParkFlags |= PARK_FLAGS_NO_MONEY;
    research_reset_current_item();
    scenery_set_default_placement_configuration();
    News::InitQueue();
    if (gScenarioObjective.Type != OBJECTIVE_NONE && !gLoadKeepWindowsOpen)
        context_open_window_view(WV_PARK_OBJECTIVE);

    auto& park = GetContext()->GetGameState()->GetPark();
    gParkRating = park.CalculateParkRating();
    gParkValue = park.CalculateParkValue();
    gCompanyValue = park.CalculateCompanyValue();
    gHistoricalProfit = gInitialCash - gBankLoan;
    gCash = gInitialCash;

    {
        utf8 normalisedName[64];
        ScenarioSources::NormaliseName(normalisedName, sizeof(normalisedName), gScenarioName.c_str());

        rct_string_id localisedStringIds[3];
        if (language_get_localised_scenario_strings(normalisedName, localisedStringIds))
        {
            if (localisedStringIds[0] != STR_NONE)
            {
                gScenarioName = language_get_string(localisedStringIds[0]);
            }
            if (localisedStringIds[1] != STR_NONE)
            {
                park.Name = language_get_string(localisedStringIds[1]);
            }
            if (localisedStringIds[2] != STR_NONE)
            {
                gScenarioDetails = language_get_string(localisedStringIds[2]);
            }
        }
    }

    // Set the last saved game path
    auto env = GetContext()->GetPlatformEnvironment();
    auto savePath = env->GetDirectoryPath(DIRBASE::USER, DIRID::SAVE);
    gScenarioSavePath = Path::Combine(savePath, park.Name + u8".park");

    gCurrentExpenditure = 0;
    gCurrentProfit = 0;
    gWeeklyProfitAverageDividend = 0;
    gWeeklyProfitAverageDivisor = 0;
    gScenarioCompletedCompanyValue = MONEY64_UNDEFINED;
    gTotalAdmissions = 0;
    gTotalIncomeFromAdmissions = 0;
    gScenarioCompletedBy = "?";
    park.ResetHistories();
    finance_reset_history();
    award_reset();
    reset_all_ride_build_dates();
    date_reset();
    Duck::RemoveAll();
    park_calculate_size();
    map_count_remaining_land_rights();
    Staff::ResetStats();

    gScenarioObjective.SetPhasedGoalIndex(0);

    auto& objManager = GetContext()->GetObjectManager();
    gLastEntranceStyle = objManager.GetLoadedObjectEntryIndex("rct2.station.plain");
    if (gLastEntranceStyle == OBJECT_ENTRY_INDEX_NULL)
    {
        // Fall back to first entrance object
        gLastEntranceStyle = 0;
    }

    gMarketingCampaigns.clear();
    gParkRatingCasualtyPenalty = 0;

    // Open park with free entry when there is no money
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        gParkFlags |= PARK_FLAGS_PARK_OPEN;
        gParkEntranceFee = 0;
    }

    gParkFlags |= PARK_FLAGS_SPRITES_INITIALISED;

    gScreenAge = 0;
}

static void scenario_end_phase()
{
    game_reset_speed();
    window_close_by_class(WC_DROPDOWN);
    window_close_all_except_flags(WF_STICK_TO_BACK | WF_STICK_TO_FRONT);
    context_open_window_view(WV_PARK_OBJECTIVE);
}

/**
 *
 *  rct2: 0x0066A752
 */
void scenario_failure()
{
    gScenarioCompletedCompanyValue = COMPANY_VALUE_ON_FAILED_OBJECTIVE;
    scenario_end_phase();
}

/**
 *
 *  rct2: 0x0066A75E
 */
void scenario_success()
{
    auto companyValue = gCompanyValue;

    gScenarioCompletedCompanyValue = companyValue;
    peep_applause();

    if (scenario_repository_try_record_highscore(gScenarioFileName.c_str(), companyValue, nullptr))
    {
        // Allow name entry
        gParkFlags |= PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;
        gScenarioCompanyValueRecord = companyValue;
    }
    scenario_end_phase();
}

/**
 *
 *  rct2: 0x006695E8
 */
void scenario_success_submit_name(const char* name)
{
    if (scenario_repository_try_record_highscore(gScenarioFileName.c_str(), gScenarioCompanyValueRecord, name))
    {
        gScenarioCompletedBy = name;
    }
    gParkFlags &= ~PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;
}

/**
 * Send a warning when entrance price is too high.
 *  rct2: 0x0066A80E
 */
static void scenario_entrance_fee_too_high_check()
{
    const auto max_fee = add_clamp_money16(gTotalRideValueForMoney, gTotalRideValueForMoney / 2);

    if ((gParkFlags & PARK_FLAGS_PARK_OPEN) && park_get_entrance_fee() > max_fee)
    {
        if (!gParkEntrances.empty())
        {
            const auto& entrance = gParkEntrances[0];
            auto x = entrance.x + 16;
            auto y = entrance.y + 16;

            uint32_t packed_xy = (y << 16) | x;
            if (gConfigNotifications.park_warnings)
            {
                News::AddItemToQueue(News::ItemType::Blank, STR_ENTRANCE_FEE_TOO_HI, packed_xy, {});
            }
        }
    }
}

void scenario_autosave_check()
{
    if (gLastAutoSaveUpdate == AUTOSAVE_PAUSE)
        return;

    // Milliseconds since last save
    uint32_t timeSinceSave = Platform::GetTicks() - gLastAutoSaveUpdate;

    bool shouldSave = false;
    switch (gConfigGeneral.autosave_frequency)
    {
        case AUTOSAVE_EVERY_MINUTE:
            shouldSave = timeSinceSave >= 1 * 60 * 1000;
            break;
        case AUTOSAVE_EVERY_5MINUTES:
            shouldSave = timeSinceSave >= 5 * 60 * 1000;
            break;
        case AUTOSAVE_EVERY_15MINUTES:
            shouldSave = timeSinceSave >= 15 * 60 * 1000;
            break;
        case AUTOSAVE_EVERY_30MINUTES:
            shouldSave = timeSinceSave >= 30 * 60 * 1000;
            break;
        case AUTOSAVE_EVERY_HOUR:
            shouldSave = timeSinceSave >= 60 * 60 * 1000;
            break;
    }

    if (shouldSave)
    {
        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        game_autosave();
    }
}

static void scenario_day_update()
{
    finance_update_daily_profit();
    peep_update_days_in_queue();
    scenario_objective_check();

    // Lower the casualty penalty
    uint16_t casualtyPenaltyModifier = (gParkFlags & PARK_FLAGS_NO_MONEY) ? 40 : 7;
    gParkRatingCasualtyPenalty = std::max(0, gParkRatingCasualtyPenalty - casualtyPenaltyModifier);

    auto intent = Intent(INTENT_ACTION_UPDATE_DATE);
    context_broadcast_intent(&intent);
}

static void scenario_week_update()
{
    int32_t month = date_get_month(gDateMonthsElapsed);

    finance_pay_wages();
    finance_pay_research();
    finance_pay_interest();
    marketing_update();
    peep_problem_warnings_update();
    ride_check_all_reachable();
    ride_update_favourited_stat();

    auto water_type = static_cast<rct_water_type*>(object_entry_get_chunk(ObjectType::Water, 0));

    if (month <= MONTH_APRIL && water_type != nullptr && water_type->flags & WATER_FLAGS_ALLOW_DUCKS)
    {
        // 100 attempts at finding some water to create a few ducks at
        for (int32_t i = 0; i < 100; i++)
        {
            if (scenario_create_ducks())
                break;
        }
    }
}

static void scenario_fortnight_update()
{
    finance_pay_ride_upkeep();
}

static void scenario_month_update()
{
    finance_shift_expenditure_table();
    scenario_objective_check();
    scenario_entrance_fee_too_high_check();
    award_update_all();
}

static void scenario_update_daynight_cycle()
{
    float currentDayNightCycle = gDayNightCycle;
    gDayNightCycle = 0;

    if (gScreenFlags == SCREEN_FLAGS_PLAYING && gConfigGeneral.day_night_cycle)
    {
        float monthFraction = gDateMonthTicks / static_cast<float>(TICKS_PER_MONTH);
        if (monthFraction < (1 / 8.0f))
        {
            gDayNightCycle = 0.0f;
        }
        else if (monthFraction < (3 / 8.0f))
        {
            gDayNightCycle = (monthFraction - (1 / 8.0f)) / (2 / 8.0f);
        }
        else if (monthFraction < (5 / 8.0f))
        {
            gDayNightCycle = 1.0f;
        }
        else if (monthFraction < (7 / 8.0f))
        {
            gDayNightCycle = 1.0f - ((monthFraction - (5 / 8.0f)) / (2 / 8.0f));
        }
        else
        {
            gDayNightCycle = 0.0f;
        }
    }

    // Only update palette if day / night cycle has changed
    if (gDayNightCycle != currentDayNightCycle)
    {
        UpdatePalette(gGamePalette, 10, 236);
    }
}

/**
 * Scenario and finance related update iteration.
 *  rct2: 0x006C44B1
 */
void scenario_update()
{
    PROFILED_FUNCTION();

    if (gScreenFlags == SCREEN_FLAGS_PLAYING)
    {
        if (date_is_day_start(gDateMonthTicks))
        {
            scenario_day_update();
        }
        if (date_is_week_start(gDateMonthTicks))
        {
            scenario_week_update();
        }
        if (date_is_fortnight_start(gDateMonthTicks))
        {
            scenario_fortnight_update();
        }
        if (date_is_month_start(gDateMonthTicks))
        {
            scenario_month_update();
        }
    }
    scenario_update_daynight_cycle();
}
/**
 *
 *  rct2: 0x006744A9
 */
bool scenario_create_ducks()
{
    // Check NxN area around centre tile defined by SquareSize
    constexpr int32_t SquareSize = 7;
    constexpr int32_t SquareCentre = SquareSize / 2;
    constexpr int32_t SquareRadiusSize = SquareCentre * 32;

    CoordsXY centrePos;
    centrePos.x = SquareRadiusSize + (scenario_rand_max(MAXIMUM_MAP_SIZE_TECHNICAL - SquareCentre) * 32);
    centrePos.y = SquareRadiusSize + (scenario_rand_max(MAXIMUM_MAP_SIZE_TECHNICAL - SquareCentre) * 32);

    Guard::Assert(map_is_location_valid(centrePos));

    if (!map_is_location_in_park(centrePos))
        return false;

    int32_t centreWaterZ = (tile_element_water_height(centrePos));
    if (centreWaterZ == 0)
        return false;

    CoordsXY innerPos{ centrePos.x - (32 * SquareCentre), centrePos.y - (32 * SquareCentre) };
    int32_t waterTiles = 0;
    for (int32_t y = 0; y < SquareSize; y++)
    {
        for (int32_t x = 0; x < SquareSize; x++)
        {
            if (!map_is_location_valid(innerPos))
                continue;

            if (!map_is_location_in_park(innerPos))
                continue;

            int32_t waterZ = (tile_element_water_height(innerPos));
            if (waterZ == centreWaterZ)
                waterTiles++;

            innerPos.x += 32;
        }
        innerPos.x -= SquareSize * 32;
        innerPos.y += 32;
    }

    // Must be at least 25 water tiles of the same height in 7x7 area
    if (waterTiles < 25)
        return false;

    // Set x, y to the centre of the tile
    centrePos.x += 16;
    centrePos.y += 16;

    uint32_t duckCount = (scenario_rand() % 4) + 2;
    for (uint32_t i = 0; i < duckCount; i++)
    {
        uint32_t r = scenario_rand();
        innerPos.x = (r >> 16) % SquareRadiusSize;
        innerPos.y = (r & 0xFFFF) % SquareRadiusSize;

        CoordsXY targetPos{ centrePos.x + innerPos.x - SquareRadiusSize, centrePos.y + innerPos.y - SquareRadiusSize };

        Guard::Assert(map_is_location_valid(targetPos));
        Duck::Create(targetPos);
    }

    return true;
}

const random_engine_t::state_type& scenario_rand_state()
{
    return gScenarioRand.state();
};

void scenario_rand_seed(random_engine_t::result_type s0, random_engine_t::result_type s1)
{
    Random::Rct2::Seed s{ s0, s1 };
    gScenarioRand.seed(s);
}

/**
 *
 *  rct2: 0x006E37D2
 *
 * @return eax
 */
random_engine_t::result_type scenario_rand()
{
    return gScenarioRand();
}

uint32_t scenario_rand_max(uint32_t max)
{
    if (max < 2)
        return 0;
    if ((max & (max - 1)) == 0)
        return scenario_rand() & (max - 1);
    uint32_t rand, cap = ~(static_cast<uint32_t>(0)) - (~(static_cast<uint32_t>(0)) % max) - 1;
    do
    {
        rand = scenario_rand();
    } while (rand > cap);
    return rand % max;
}

/**
 * Prepare rides, for the finish five rollercoasters objective.
 *  rct2: 0x006788F7
 */
static bool scenario_prepare_rides_for_save()
{
    // In case of finish-rides objectives the player must select which rides are to be finished themselves

    bool markTrackAsIndestructible;
    tile_element_iterator it;
    tile_element_iterator_begin(&it);
    do
    {
        if (it.element->GetType() == TileElementType::Track)
        {
            markTrackAsIndestructible = false;

            auto ride = get_ride(it.element->AsTrack()->GetRideIndex());

            if (ride != nullptr && ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
            {
                markTrackAsIndestructible = true;
            }

            it.element->AsTrack()->SetIsIndestructible(markTrackAsIndestructible);
        }
    } while (tile_element_iterator_next(&it));

    return true;
}

/**
 *
 *  rct2: 0x006726C7
 */
bool scenario_prepare_for_save()
{
    if (!gScenarioObjective.IsValid())
        return false;

    // This can return false if there are any objectives involving finishing partially built rides, and there and there are too
    // few.
    if (!scenario_prepare_rides_for_save())
    {
        return false;
    }

    if (gScenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
        gParkFlags |= PARK_FLAGS_PARK_OPEN;

    // Fix #2385: saved scenarios did not initialise temperatures to selected climate
    climate_reset(gClimate); // TODO make optional

    return true;
}

/*
 * Returns the AllowEarlyCompletion-Option to be used
 * depending on the Current Network-Mode.
 */
bool AllowEarlyCompletion()
{
    switch (network_get_mode())
    {
        case NETWORK_MODE_CLIENT:
            return gAllowEarlyCompletionInNetworkPlay;
        case NETWORK_MODE_NONE:
        case NETWORK_MODE_SERVER:
        default:
            return gConfigGeneral.allow_early_completion;
    }
}

bool ObjectiveNeedsMoney(const uint8_t objective)
{
    switch (objective)
    {
        case OBJECTIVE_PARK_VALUE_BY:
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
        case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            return true;
    }
    return false;
}

static void scenario_objective_check()
{
    auto status = gScenarioObjective.Check();
    if (status == ObjectiveStatus::Success)
    {
        scenario_success();
    }
    else if (status == ObjectiveStatus::Failure)
    {
        scenario_failure();
    }
}

void Objective::ConvertObjective(uint8_t _type, std::string _details)
{
    uint8_t year = 3;
    union
    {
        uint16_t numGuests;
        rct_string_id rideId;
        uint16_t minimumLength; // For the "Build 10 coasters of minimum length" objective.
    };
    union
    {
        money64 currency;
        uint16_t minimumExcitement; // For the "Finish 5 coaster with a minimum excitement rating" objective.
    };
    switch (_type)
    {
        case OBJECTIVE_NONE:
        case OBJECTIVE_HAVE_FUN:
        case OBJECTIVE_BUILD_THE_BEST:
        case OBJECTIVE_10_ROLLERCOASTERS:
            break;
        case OBJECTIVE_GUESTS_BY:
            year = 3;
            numGuests = 1500;
            break;
        case OBJECTIVE_PARK_VALUE_BY:
            year = 3;
            currency = MONEY(50000, 00);
            break;
        case OBJECTIVE_GUESTS_AND_RATING:
            numGuests = 2000;
            break;
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
            currency = MONEY(10000, 00);
            break;
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            minimumLength = 1200;
            break;
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
            currency = FIXED_2DP(6, 70);
            break;
        case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
            currency = MONEY(50000, 00);
            break;
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            currency = MONEY(1000, 00);
            break;
    }
    ConvertObjective(_type, year, minimumLength, currency, 0, _details);
}

void Objective::ConvertObjective(
    uint8_t _type, uint8_t _year, uint16_t _numGuestsRideIdMinLength, money64 _currencyMinExcitement,
    uint16_t _warningDaysParkRating, std::string _scenarioDetails)
{
    Reset();
    switch (_type)
    {
        case OBJECTIVE_GUESTS_BY:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::AtDate, 8, _year, false, _scenarioDetails);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveGuestNumGoal>(
                _numGuestsRideIdMinLength, Sign::BiggerThan, GoalType::Goal, 4);
            group.AddGoal(goal, true);
            ObjectiveGoalPtr goal2 = std::make_shared<ObjectiveParkRatingGoal>(600, Sign::BiggerThan, GoalType::Goal, 4);
            group.AddGoal(goal2, true);
            AddPhasedGoalGroup(group);
        }
        break;
        case OBJECTIVE_PARK_VALUE_BY:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::AtDate, 8, _year, false, _scenarioDetails);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveParkValueGoal>(
                _currencyMinExcitement, Sign::BiggerThan, GoalType::Goal, 4);
            group.AddGoal(goal, true);
            AddPhasedGoalGroup(group);
        }
        break;
        case OBJECTIVE_10_ROLLERCOASTERS:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::Dateless);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveCoasterGoal>(10, 0, 0, 0, 6, 0, 0, 0, 0, 0, true, false);
            group.AddGoal(goal, true);
            AddPhasedGoalGroup(group);
        }
        break;
        case OBJECTIVE_GUESTS_AND_RATING:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::Dateless);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveGuestNumGoal>(
                _numGuestsRideIdMinLength, Sign::BiggerThan, GoalType::Goal, 4);
            group.AddGoal(goal, true);
            ObjectiveGoalPtr goal2 = std::make_shared<ObjectiveParkRatingGoal>(700, Sign::BiggerThan, GoalType::Restriction, 4);
            group.AddGoal(goal2, true);
            gScenarioObjectiveWarningDays[0] = _warningDaysParkRating;
            AddPhasedGoalGroup(group);
        }
        break;
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::Dateless);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveRideTicketProfitGoal>(
                _currencyMinExcitement, Sign::BiggerThan, GoalType::Goal, 4);
            group.AddGoal(goal, true);
            AddPhasedGoalGroup(group);
        }
        break;
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::Dateless);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveCoasterGoal>(
                10, 0, _numGuestsRideIdMinLength, 0, 7, 0, 0, 0, 0, 0, true, false);
            group.AddGoal(goal, true);
            AddPhasedGoalGroup(group);
        }
        break;
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::Dateless);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveCoasterGoal>(
                5, 0, _numGuestsRideIdMinLength, 0, ((float)_currencyMinExcitement) / 100, 0, 0, 0, 0, 0, true, true);
            group.AddGoal(goal, true);
            AddPhasedGoalGroup(group);
        }
        break;
        case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::Dateless);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveParkValueGoal>(
                _currencyMinExcitement, Sign::BiggerThan, GoalType::Goal, 0);
            group.AddGoal(goal, true);
            ObjectiveGoalPtr goal2 = std::make_shared<ObjectiveRepayLoanGoal>();
            group.AddGoal(goal2, true);
            AddPhasedGoalGroup(group);
        }
        break;
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::Dateless);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveStallProfitGoal>(
                _currencyMinExcitement, Sign::BiggerThan, GoalType::Goal, 4);
            group.AddGoal(goal, true);
            AddPhasedGoalGroup(group);
        }
        break;
        case OBJECTIVE_BUILD_THE_BEST:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::Dateless);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveSpecificRideGoal>(_numGuestsRideIdMinLength, 0, false, true);
            group.AddGoal(goal, true);
            AddPhasedGoalGroup(group);
        }
        break;
        case OBJECTIVE_HAVE_FUN:
        {
            auto group = ObjectiveGoalGroup(GoalGroupType::Dateless);
            ObjectiveGoalPtr goal = std::make_shared<ObjectiveFunGoal>();
            group.AddGoal(goal, true);
            AddPhasedGoalGroup(group);
        }
        break;
    }
    LegacyType = _type;
    Type = OBJECTIVE_MODULAR_SYSTEM_V1;
    if (gScenarioCompletedCompanyValue != MONEY64_UNDEFINED)
    {
        auto& goalGroup = PermanentGoals.Initialized() ? PermanentGoals : PhasedGoals[0];
        goalGroup.completed = true;
        if (gScenarioCompletedCompanyValue != COMPANY_VALUE_ON_FAILED_OBJECTIVE)
        {
            for (auto goal : goalGroup.goals)
            {
                if (goal->GetGoalType() == GoalType::Goal)
                    goal->SetTrueOnLastCheck(true);
            }
        }
    }
}

void Objective::Reset()
{
    PermanentGoals = ObjectiveGoalGroup();
    PhasedGoals = std::vector<ObjectiveGoalGroup>();
    gScenarioObjectiveWarningDays = std::vector<uint16_t>();
    PhasedGoalIndex = 0;
    gScenarioObjectiveDescription = "";
}

/**
 * Checks the win/lose conditions of the current objective.
 *  rct2: 0x0066A4B2
 */
ObjectiveStatus Objective::Check()
{
    if (gScenarioCompletedCompanyValue != MONEY64_UNDEFINED)
    {
        return ObjectiveStatus::Undecided;
    }

    ObjectiveStatus untimed = ObjectiveStatus::Undecided;
    if (PermanentGoals.Initialized())
    {
        untimed = PermanentGoals.CheckTotalGroup();
        if (untimed == ObjectiveStatus::Failure)
        {
            PermanentGoals.completed = true;
            return ObjectiveStatus::Failure;
        }
    }
    if (PhasedGoals.size() == 0 || PhasedGoals[PhasedGoals.size() - 1].completed)
    {
        if (untimed == ObjectiveStatus::Success)
        {
            PermanentGoals.completed = true;
        }
        return untimed;
    }
    switch (PhasedGoals[PhasedGoalIndex].CheckTotalGroup())
    {
        default:
        case ObjectiveStatus::Undecided:
            return ObjectiveStatus::Undecided;
            break;
        case ObjectiveStatus::Success:
            PhasedGoals[PhasedGoalIndex].completed = true;
            SetPhasedGoalIndex(PhasedGoalIndex + 1);
            if (PhasedGoals[PhasedGoals.size() - 1].completed)
            {
                if (!PermanentGoals.Initialized() || (PermanentGoals.Initialized() && untimed == ObjectiveStatus::Success))
                {
                    PermanentGoals.completed = true;
                    return ObjectiveStatus::Success;
                }
            }
            else
            {
                scenario_end_phase();
            }
            return ObjectiveStatus::Undecided;

        case ObjectiveStatus::Failure:
            PhasedGoals[PhasedGoalIndex].completed = true;
            return ObjectiveStatus::Failure;
    }
}

bool Objective::SetPermanentGoalGroup(ObjectiveGoalGroup _group)
{
    LegacyType = OBJECTIVE_MODULAR_SYSTEM_V1;
    if (!PermanentGoals.Initialized())
    {
        PermanentGoals = _group;
        return true;
    }
    else
        return false;
}

rct_string_id Objective::AddPhasedGoalGroup(ObjectiveGoalGroup _group)
{
    LegacyType = OBJECTIVE_MODULAR_SYSTEM_V1;
    switch (_group.GetGoalGroupType())
    {
        case GoalGroupType::Dateless: // can always be added
            PhasedGoals.push_back(_group);
            return 0;
        case GoalGroupType::AtEndOfPeriod:
        case GoalGroupType::AfterPeriod:
        case GoalGroupType::DuringPeriod:
            if (_group.monthPeriod == 0 && _group.yearPeriod == 0)
                return STR_ERROR_PERIOD_OF_0;
            PhasedGoals.push_back(_group);
            return 0;

        case GoalGroupType::AfterDate: // These cannot be added after a Dateless, Period, or AfterDate group, as it could result
                                       // in impossilbe objectives, as the duedate has passed before the group is started.
        case GoalGroupType::AtDate:
        case GoalGroupType::BeforeDate:
        default:
            if (PhasedGoals.empty()
                || ((PhasedGoals.back().groupType == GoalGroupType::AtDate
                     || PhasedGoals.back().groupType == GoalGroupType::BeforeDate)
                    && (PhasedGoals.back().yearDate < _group.yearDate
                        || (PhasedGoals.back().yearDate == _group.yearDate
                            && PhasedGoals.back().monthDate < _group.monthDate))))
            {
                PhasedGoals.push_back(_group);
                return 0;
            }
            else if (
                PhasedGoals.back().groupType != GoalGroupType::AtDate
                && PhasedGoals.back().groupType != GoalGroupType::BeforeDate)
                return STR_ERROR_DATE_AFTER_PERIOD;
            else
                return STR_ERROR_DATE_AFTER_EALIER_DATE;
    }
}

rct_string_id Objective::SetPhasedGoalGroup(ObjectiveGoalGroup _group, size_t index)
{
    rct_string_id okay = 0;
    if ((_group.groupType == GoalGroupType::AfterPeriod || _group.groupType == GoalGroupType::DuringPeriod
         || _group.groupType == GoalGroupType::AtEndOfPeriod)
        && _group.monthPeriod == 0 && _group.yearPeriod == 0)
    {
        okay = STR_ERROR_PERIOD_OF_0; // can't have a period of 0 length.
    }
    if (okay == 0 && index >= 1) // there is a previous group
    {
        if (PhasedGoals[index - 1].groupType == GoalGroupType::Dateless
            || PhasedGoals[index - 1].groupType == GoalGroupType::AfterPeriod
            || PhasedGoals[index - 1].groupType == GoalGroupType::AtEndOfPeriod
            || PhasedGoals[index - 1].groupType == GoalGroupType::DuringPeriod)
        {
            if (_group.groupType == GoalGroupType::AfterDate || _group.groupType == GoalGroupType::AtDate
                || _group.groupType == GoalGroupType::BeforeDate)
                okay = STR_ERROR_DATE_AFTER_PERIOD; // Can't have fixed dates after a period or dateless or afterdate.
        }
        else
        {
            if (_group.groupType == GoalGroupType::AfterDate || _group.groupType == GoalGroupType::AtDate
                || _group.groupType == GoalGroupType::BeforeDate)
            {
                if (!(PhasedGoals[index - 1].yearDate < _group.yearDate
                      || (PhasedGoals[index - 1].yearDate == _group.yearDate
                          && PhasedGoals[index - 1].monthDate < _group.monthDate)))
                    okay = STR_ERROR_DATE_AFTER_EALIER_DATE; // Can't have fixed dates after a later fixed date.
            }
        }
    }
    if (okay == 0 && index + 1 <= gScenarioObjective.PhasedGoals.size() - 1) // there is a next group
    {
        if (_group.groupType == GoalGroupType::Dateless || _group.groupType == GoalGroupType::AfterPeriod
            || _group.groupType == GoalGroupType::AtEndOfPeriod || _group.groupType == GoalGroupType::DuringPeriod)
        {
            if (PhasedGoals[index + 1].groupType == GoalGroupType::AfterDate
                || PhasedGoals[index + 1].groupType == GoalGroupType::AtDate
                || PhasedGoals[index + 1].groupType == GoalGroupType::BeforeDate)
                okay = STR_ERROR_DATE_AFTER_PERIOD; // Can't have fixed dates after a period or dateless or afterdate.
        }
        else
        {
            if (PhasedGoals[index + 1].groupType == GoalGroupType::AfterDate
                || PhasedGoals[index + 1].groupType == GoalGroupType::AtDate
                || PhasedGoals[index + 1].groupType == GoalGroupType::BeforeDate)
            {
                if (!(_group.yearDate < PhasedGoals[index + 1].yearDate
                      || (_group.yearDate == PhasedGoals[index + 1].yearDate
                          && _group.monthDate < PhasedGoals[index + 1].monthDate)))
                    okay = STR_ERROR_DATE_AFTER_EALIER_DATE; // Can't have fixed dates after a later fixed date.
            }
        }
    }
    if (okay == 0)
        PhasedGoals[index] = _group;
    return okay;
}

void Objective::RemovePhasedGoalGroup(uint32_t number)
{
    LegacyType = OBJECTIVE_MODULAR_SYSTEM_V1;
    if (number >= PhasedGoals.size())
        return;
    PhasedGoals.erase(PhasedGoals.begin() + number);
}

void Objective::SetPhasedGoalIndex(uint32_t _newIndex, bool reset)
{
    uint32_t oldIndex = PhasedGoalIndex;
    PhasedGoalIndex = _newIndex;
    allowParkOpening = true;
    if (PhasedGoals.size() > _newIndex)
        PhasedGoals[PhasedGoalIndex].Start();
    else if (PhasedGoalIndex != 0)
        PhasedGoalIndex = (uint32_t)PhasedGoals.size() - 1;

    CalculateAllowParkOpening();
    if (reset)
    {
        gScenarioCompletedBy = "?";
        gScenarioCompletedCompanyValue = MONEY64_UNDEFINED;
        gParkFlags &= ~PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;

        for (int32_t i = (int32_t)oldIndex; i >= (int32_t)_newIndex; i--)
        {
            PhasedGoals[i].completed = false;
        }
    }
}

void Objective::CalculateAllowParkOpening()
{
    if (PhasedGoals.size() > PhasedGoalIndex)
    {
        for (auto goal : PhasedGoals[PhasedGoalIndex].goals)
        {
            if (goal->GetGoalType() == GoalType::Restriction)
            {
                gScenarioObjective.allowParkOpening = false;
                gParkFlags |= PARK_FLAGS_PARK_OPEN; // open the park as now players can't do that themselves.
            }
        }
    }

    if (PermanentGoals.initialized) // phased goals doing this happens in the Start()
    {
        for (auto goal : PermanentGoals.goals)
        {
            if (goal->GetGoalType() == GoalType::Restriction)
            {
                allowParkOpening = false;
                gParkFlags |= PARK_FLAGS_PARK_OPEN; // open the park as now players can't do that themselves.
            }
        }
    }
}

bool Objective::RequiresMoney()
{
    bool okay = false;
    for (auto goal : PermanentGoals.goals)
    {
        okay |= goal->GetUsesMoney();
    }
    for (auto group : PhasedGoals)
    {
        for (auto goal : group.goals)
        {
            okay |= goal->GetUsesMoney();
        }
    }
    return okay;
}

bool Objective::RequiresRidePrices()
{
    bool okay = false;
    for (auto goal : PermanentGoals.goals)
    {
        okay |= (goal->GetGoalID() == GoalID::RideTicketProfitGoal);
    }
    for (auto group : PhasedGoals)
    {
        for (auto goal : group.goals)
        {
            okay |= (goal->GetGoalID() == GoalID::RideTicketProfitGoal);
        }
    }
    return okay;
}

bool Objective::RequiresParkEntryPrices()
{
    bool okay = false;
    for (auto goal : PermanentGoals.goals)
    {
        okay |= (goal->GetGoalID() == GoalID::ParkEntryProfitGoal);
    }
    for (auto group : PhasedGoals)
    {
        for (auto goal : group.goals)
        {
            okay |= (goal->GetGoalID() == GoalID::ParkEntryProfitGoal);
        }
    }
    return okay;
}

bool Objective::MoneySettingsOkay()
{
    // Check if objective is allowed by money and pay-per-ride settings.
    const bool objectiveAllowedByMoneyUsage = !(((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                                                 && (gParkFlags & PARK_FLAGS_NO_MONEY_SCENARIO))
                                                || (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                                                    && (gParkFlags & PARK_FLAGS_NO_MONEY)))
        || !RequiresMoney();

    // This objective can only work if the player can ask money for rides.
    const bool objectiveAllowedByPaymentSettings = (!RequiresRidePrices() || park_ride_prices_unlocked())
        && (!RequiresParkEntryPrices() || park_entry_price_unlocked());

    return objectiveAllowedByMoneyUsage && objectiveAllowedByPaymentSettings;
}
