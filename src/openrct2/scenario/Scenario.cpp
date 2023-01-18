/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../core/BitSet.hpp"
#include "../core/Guard.hpp"
#include "../core/Path.hpp"
#include "../core/Random.hpp"
#include "../entity/Duck.h"
#include "../entity/Guest.h"
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
#include "../object/Object.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../platform/Platform.h"
#include "../profiling/Profiling.h"
#include "../rct1/RCT1.h"
#include "../rct12/RCT12.h"
#include "../ride/Ride.h"
#include "../ride/Track.h"
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

const StringId ScenarioCategoryStringIds[SCENARIO_CATEGORY_COUNT] = {
    STR_BEGINNER_PARKS, STR_CHALLENGING_PARKS,    STR_EXPERT_PARKS, STR_REAL_PARKS, STR_OTHER_PARKS,
    STR_DLC_PARKS,      STR_BUILD_YOUR_OWN_PARKS, STR_COMPETITIONS, STR_UCES_TM,    STR_UCES_KD,
};

SCENARIO_CATEGORY gScenarioCategory;
std::string gScenarioName;
std::string gScenarioDetails;
std::string gScenarioCompletedBy;
std::string gScenarioSavePath;
bool gFirstTimeSaving = true;
uint16_t gSavedAge;
uint32_t gLastAutoSaveUpdate = 0;

random_engine_t gScenarioRand;

Objective gScenarioObjective;

bool gAllowEarlyCompletionInNetworkPlay;
uint16_t gScenarioParkRatingWarningDays;
money64 gScenarioCompletedCompanyValue;
money64 gScenarioCompanyValueRecord;

std::string gScenarioFileName;

static void ScenarioCheckObjective();

using namespace OpenRCT2;

void ScenarioBegin()
{
    GameLoadInit();
    ScenarioReset();

    if (gScenarioObjective.Type != OBJECTIVE_NONE && !gLoadKeepWindowsOpen)
        ContextOpenWindowView(WV_PARK_OBJECTIVE);

    gScreenAge = 0;
}

void ScenarioReset()
{
    // Set the scenario pseudo-random seeds
    Random::Rct2::Seed s{ 0x1234567F ^ Platform::GetTicks(), 0x789FABCD ^ Platform::GetTicks() };
    gScenarioRand.seed(s);

    ResearchResetCurrentItem();
    ScenerySetDefaultPlacementConfiguration();
    News::InitQueue();

    auto& park = GetContext()->GetGameState()->GetPark();
    gParkRating = park.CalculateParkRating();
    gParkValue = park.CalculateParkValue();
    gCompanyValue = park.CalculateCompanyValue();
    gHistoricalProfit = gInitialCash - gBankLoan;
    gCash = gInitialCash;

    {
        utf8 normalisedName[64];
        ScenarioSources::NormaliseName(normalisedName, sizeof(normalisedName), gScenarioName.c_str());

        StringId localisedStringIds[3];
        if (LanguageGetLocalisedScenarioStrings(normalisedName, localisedStringIds))
        {
            if (localisedStringIds[0] != STR_NONE)
            {
                gScenarioName = LanguageGetString(localisedStringIds[0]);
            }
            if (localisedStringIds[1] != STR_NONE)
            {
                park.Name = LanguageGetString(localisedStringIds[1]);
            }
            if (localisedStringIds[2] != STR_NONE)
            {
                gScenarioDetails = LanguageGetString(localisedStringIds[2]);
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
    gTotalAdmissions = 0;
    gTotalIncomeFromAdmissions = 0;

    gParkFlags &= ~PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;
    gScenarioCompletedCompanyValue = MONEY64_UNDEFINED;
    gScenarioCompletedBy = "?";

    park.ResetHistories();
    FinanceResetHistory();
    AwardReset();
    ResetAllRideBuildDates();
    DateReset();
    Duck::RemoveAll();
    ParkCalculateSize();
    MapCountRemainingLandRights();
    Staff::ResetStats();

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
    gGamePaused = false;
}

static void ScenarioEnd()
{
    GameResetSpeed();
    WindowCloseByClass(WindowClass::Dropdown);
    WindowCloseAllExceptFlags(WF_STICK_TO_BACK | WF_STICK_TO_FRONT);
    ContextOpenWindowView(WV_PARK_OBJECTIVE);
}

/**
 *
 *  rct2: 0x0066A752
 */
void ScenarioFailure()
{
    gScenarioCompletedCompanyValue = COMPANY_VALUE_ON_FAILED_OBJECTIVE;
    ScenarioEnd();
}

/**
 *
 *  rct2: 0x0066A75E
 */
void ScenarioSuccess()
{
    auto companyValue = gCompanyValue;

    gScenarioCompletedCompanyValue = companyValue;
    PeepApplause();

    if (ScenarioRepositoryTryRecordHighscore(gScenarioFileName.c_str(), companyValue, nullptr))
    {
        // Allow name entry
        gParkFlags |= PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;
        gScenarioCompanyValueRecord = companyValue;
    }
    ScenarioEnd();
}

/**
 *
 *  rct2: 0x006695E8
 */
void ScenarioSuccessSubmitName(const char* name)
{
    if (ScenarioRepositoryTryRecordHighscore(gScenarioFileName.c_str(), gScenarioCompanyValueRecord, name))
    {
        gScenarioCompletedBy = name;
    }
    gParkFlags &= ~PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;
}

/**
 * Send a warning when entrance price is too high.
 *  rct2: 0x0066A80E
 */
static void ScenarioCheckEntranceFeeTooHigh()
{
    const auto max_fee = AddClamp_money16(gTotalRideValueForMoney, gTotalRideValueForMoney / 2);

    if ((gParkFlags & PARK_FLAGS_PARK_OPEN) && ParkGetEntranceFee() > max_fee)
    {
        if (!gParkEntrances.empty())
        {
            const auto& entrance = gParkEntrances[0];
            auto x = entrance.x + 16;
            auto y = entrance.y + 16;

            uint32_t packed_xy = (y << 16) | x;
            if (gConfigNotifications.ParkWarnings)
            {
                News::AddItemToQueue(News::ItemType::Blank, STR_ENTRANCE_FEE_TOO_HI, packed_xy, {});
            }
        }
    }
}

void ScenarioAutosaveCheck()
{
    if (gLastAutoSaveUpdate == AUTOSAVE_PAUSE)
        return;

    // Milliseconds since last save
    uint32_t timeSinceSave = Platform::GetTicks() - gLastAutoSaveUpdate;

    bool shouldSave = false;
    switch (gConfigGeneral.AutosaveFrequency)
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
        GameAutosave();
    }
}

static void ScenarioDayUpdate()
{
    FinanceUpdateDailyProfit();
    PeepUpdateDaysInQueue();
    switch (gScenarioObjective.Type)
    {
        case OBJECTIVE_10_ROLLERCOASTERS:
        case OBJECTIVE_GUESTS_AND_RATING:
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
        case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
            ScenarioCheckObjective();
            break;
        default:
            if (AllowEarlyCompletion())
                ScenarioCheckObjective();
            break;
    }

    // Lower the casualty penalty
    uint16_t casualtyPenaltyModifier = (gParkFlags & PARK_FLAGS_NO_MONEY) ? 40 : 7;
    gParkRatingCasualtyPenalty = std::max(0, gParkRatingCasualtyPenalty - casualtyPenaltyModifier);

    auto intent = Intent(INTENT_ACTION_UPDATE_DATE);
    ContextBroadcastIntent(&intent);
}

static void ScenarioWeekUpdate()
{
    int32_t month = DateGetMonth(gDateMonthsElapsed);

    FinancePayWages();
    FinancePayResearch();
    FinancePayInterest();
    MarketingUpdate();
    PeepProblemWarningsUpdate();
    RideCheckAllReachable();
    RideUpdateFavouritedStat();

    auto water_type = static_cast<rct_water_type*>(ObjectEntryGetChunk(ObjectType::Water, 0));

    if (month <= MONTH_APRIL && water_type != nullptr && water_type->flags & WATER_FLAGS_ALLOW_DUCKS)
    {
        // 100 attempts at finding some water to create a few ducks at
        for (int32_t i = 0; i < 100; i++)
        {
            if (ScenarioCreateDucks())
                break;
        }
    }
}

static void ScenarioFortnightUpdate()
{
    FinancePayRideUpkeep();
}

static void ScenarioMonthUpdate()
{
    FinanceShiftExpenditureTable();
    ScenarioCheckObjective();
    ScenarioCheckEntranceFeeTooHigh();
    AwardUpdateAll();
}

static void ScenarioUpdateDayNightCycle()
{
    float currentDayNightCycle = gDayNightCycle;
    gDayNightCycle = 0;

    if (gScreenFlags == SCREEN_FLAGS_PLAYING && gConfigGeneral.DayNightCycle)
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
void ScenarioUpdate()
{
    PROFILED_FUNCTION();

    if (gScreenFlags == SCREEN_FLAGS_PLAYING)
    {
        if (DateIsDayStart(gDateMonthTicks))
        {
            ScenarioDayUpdate();
        }
        if (DateIsWeekStart(gDateMonthTicks))
        {
            ScenarioWeekUpdate();
        }
        if (DateIsFortnightStart(gDateMonthTicks))
        {
            ScenarioFortnightUpdate();
        }
        if (DateIsMonthStart(gDateMonthTicks))
        {
            ScenarioMonthUpdate();
        }
    }
    ScenarioUpdateDayNightCycle();
}
/**
 *
 *  rct2: 0x006744A9
 */
bool ScenarioCreateDucks()
{
    // Check NxN area around centre tile defined by SquareSize
    constexpr int32_t SquareSize = 7;
    constexpr int32_t SquareCentre = SquareSize / 2;
    constexpr int32_t SquareRadiusSize = SquareCentre * 32;

    CoordsXY centrePos;
    centrePos.x = SquareRadiusSize + (ScenarioRandMax(gMapSize.x - SquareCentre) * 32);
    centrePos.y = SquareRadiusSize + (ScenarioRandMax(gMapSize.y - SquareCentre) * 32);

    Guard::Assert(MapIsLocationValid(centrePos));

    if (!MapIsLocationInPark(centrePos))
        return false;

    int32_t centreWaterZ = (TileElementWaterHeight(centrePos));
    if (centreWaterZ == 0)
        return false;

    CoordsXY innerPos{ centrePos.x - (32 * SquareCentre), centrePos.y - (32 * SquareCentre) };
    int32_t waterTiles = 0;
    for (int32_t y = 0; y < SquareSize; y++)
    {
        for (int32_t x = 0; x < SquareSize; x++)
        {
            if (!MapIsLocationValid(innerPos))
                continue;

            if (!MapIsLocationInPark(innerPos))
                continue;

            int32_t waterZ = (TileElementWaterHeight(innerPos));
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

    uint32_t duckCount = (ScenarioRand() % 4) + 2;
    for (uint32_t i = 0; i < duckCount; i++)
    {
        uint32_t r = ScenarioRand();
        innerPos.x = (r >> 16) % SquareRadiusSize;
        innerPos.y = (r & 0xFFFF) % SquareRadiusSize;

        CoordsXY targetPos{ centrePos.x + innerPos.x - SquareRadiusSize, centrePos.y + innerPos.y - SquareRadiusSize };

        Guard::Assert(MapIsLocationValid(targetPos));
        Duck::Create(targetPos);
    }

    return true;
}

const random_engine_t::state_type& ScenarioRandState()
{
    return gScenarioRand.state();
};

void ScenarioRandSeed(random_engine_t::result_type s0, random_engine_t::result_type s1)
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
random_engine_t::result_type ScenarioRand()
{
    return gScenarioRand();
}

uint32_t ScenarioRandMax(uint32_t max)
{
    if (max < 2)
        return 0;
    if ((max & (max - 1)) == 0)
        return ScenarioRand() & (max - 1);
    uint32_t rand, cap = ~(static_cast<uint32_t>(0)) - (~(static_cast<uint32_t>(0)) % max) - 1;
    do
    {
        rand = ScenarioRand();
    } while (rand > cap);
    return rand % max;
}

/**
 * Prepare rides, for the finish five rollercoasters objective.
 *  rct2: 0x006788F7
 */
static ResultWithMessage ScenarioPrepareRidesForSave()
{
    int32_t isFiveCoasterObjective = gScenarioObjective.Type == OBJECTIVE_FINISH_5_ROLLERCOASTERS;
    uint8_t rcs = 0;

    for (auto& ride : GetRideManager())
    {
        const auto* rideEntry = ride.GetRideEntry();
        if (rideEntry != nullptr)
        {
            // If there are more than 5 roller coasters, only mark the first five.
            if (isFiveCoasterObjective && (RideEntryHasCategory(*rideEntry, RIDE_CATEGORY_ROLLERCOASTER) && rcs < 5))
            {
                ride.lifecycle_flags |= RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
                rcs++;
            }
            else
            {
                ride.lifecycle_flags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
            }
        }
    }

    if (isFiveCoasterObjective && rcs < 5)
    {
        return { false, STR_NOT_ENOUGH_ROLLER_COASTERS };
    }

    bool markTrackAsIndestructible;
    tile_element_iterator it;
    TileElementIteratorBegin(&it);
    do
    {
        if (it.element->GetType() == TileElementType::Track)
        {
            markTrackAsIndestructible = false;

            if (isFiveCoasterObjective)
            {
                auto ride = GetRide(it.element->AsTrack()->GetRideIndex());

                // In the previous step, this flag was set on the first five roller coasters.
                if (ride != nullptr && ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
                {
                    markTrackAsIndestructible = true;
                }
            }

            it.element->AsTrack()->SetIsIndestructible(markTrackAsIndestructible);
        }
    } while (TileElementIteratorNext(&it));

    return { true };
}

/**
 *
 *  rct2: 0x006726C7
 */
ResultWithMessage ScenarioPrepareForSave()
{
    // This can return false if the goal is 'Finish 5 roller coaster' and there are too few.
    const auto prepareRidesResult = ScenarioPrepareRidesForSave();
    if (!prepareRidesResult.Successful)
    {
        return { false, prepareRidesResult.Message };
    }

    if (gScenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
        gParkFlags |= PARK_FLAGS_PARK_OPEN;

    ScenarioReset();

    return { true };
}

ObjectiveStatus Objective::CheckGuestsBy() const
{
    auto parkRating = gParkRating;
    auto currentMonthYear = gDateMonthsElapsed;

    if (currentMonthYear == MONTH_COUNT * Year || AllowEarlyCompletion())
    {
        if (parkRating >= 600 && gNumGuestsInPark >= NumGuests)
        {
            return ObjectiveStatus::Success;
        }

        if (currentMonthYear == MONTH_COUNT * Year)
        {
            return ObjectiveStatus::Failure;
        }
    }

    return ObjectiveStatus::Undecided;
}

ObjectiveStatus Objective::CheckParkValueBy() const
{
    int32_t currentMonthYear = gDateMonthsElapsed;
    money32 objectiveParkValue = Currency;
    money32 parkValue = gParkValue;

    if (currentMonthYear == MONTH_COUNT * Year || AllowEarlyCompletion())
    {
        if (parkValue >= objectiveParkValue)
        {
            return ObjectiveStatus::Success;
        }

        if (currentMonthYear == MONTH_COUNT * Year)
        {
            return ObjectiveStatus::Failure;
        }
    }

    return ObjectiveStatus::Undecided;
}

/**
 * Checks if there are 10 rollercoasters of different subtype with
 * excitement >= 600 .
 * rct2:
 **/
ObjectiveStatus Objective::Check10RollerCoasters() const
{
    auto rcs = 0;
    BitSet<MAX_RIDE_OBJECTS> type_already_counted;
    for (const auto& ride : GetRideManager())
    {
        if (ride.status == RideStatus::Open && ride.excitement >= RIDE_RATING(6, 00) && ride.subtype != OBJECT_ENTRY_INDEX_NULL)
        {
            auto rideEntry = ride.GetRideEntry();
            if (rideEntry != nullptr)
            {
                if (RideEntryHasCategory(*rideEntry, RIDE_CATEGORY_ROLLERCOASTER) && !type_already_counted[ride.subtype])
                {
                    type_already_counted[ride.subtype] = true;
                    rcs++;
                }
            }
        }
    }
    if (rcs >= 10)
    {
        return ObjectiveStatus::Success;
    }

    return ObjectiveStatus::Undecided;
}

/**
 *
 *  rct2: 0x0066A13C
 */
ObjectiveStatus Objective::CheckGuestsAndRating() const
{
    if (gParkRating < 700 && gDateMonthsElapsed >= 1)
    {
        gScenarioParkRatingWarningDays++;
        if (gScenarioParkRatingWarningDays == 1)
        {
            if (gConfigNotifications.ParkRatingWarnings)
            {
                News::AddItemToQueue(News::ItemType::Graph, STR_PARK_RATING_WARNING_4_WEEKS_REMAINING, 0, {});
            }
        }
        else if (gScenarioParkRatingWarningDays == 8)
        {
            if (gConfigNotifications.ParkRatingWarnings)
            {
                News::AddItemToQueue(News::ItemType::Graph, STR_PARK_RATING_WARNING_3_WEEKS_REMAINING, 0, {});
            }
        }
        else if (gScenarioParkRatingWarningDays == 15)
        {
            if (gConfigNotifications.ParkRatingWarnings)
            {
                News::AddItemToQueue(News::ItemType::Graph, STR_PARK_RATING_WARNING_2_WEEKS_REMAINING, 0, {});
            }
        }
        else if (gScenarioParkRatingWarningDays == 22)
        {
            if (gConfigNotifications.ParkRatingWarnings)
            {
                News::AddItemToQueue(News::ItemType::Graph, STR_PARK_RATING_WARNING_1_WEEK_REMAINING, 0, {});
            }
        }
        else if (gScenarioParkRatingWarningDays == 29)
        {
            News::AddItemToQueue(News::ItemType::Graph, STR_PARK_HAS_BEEN_CLOSED_DOWN, 0, {});
            gParkFlags &= ~PARK_FLAGS_PARK_OPEN;
            gGuestInitialHappiness = 50;
            return ObjectiveStatus::Failure;
        }
    }
    else if (gScenarioCompletedCompanyValue != COMPANY_VALUE_ON_FAILED_OBJECTIVE)
    {
        gScenarioParkRatingWarningDays = 0;
    }

    if (gParkRating >= 700)
        if (gNumGuestsInPark >= NumGuests)
            return ObjectiveStatus::Success;

    return ObjectiveStatus::Undecided;
}

ObjectiveStatus Objective::CheckMonthlyRideIncome() const
{
    money32 lastMonthRideIncome = gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ParkRideTickets)];
    if (lastMonthRideIncome >= Currency)
    {
        return ObjectiveStatus::Success;
    }

    return ObjectiveStatus::Undecided;
}

/**
 * Checks if there are 10 rollercoasters of different subtype with
 * excitement > 700 and a minimum length;
 *  rct2: 0x0066A6B5
 */
ObjectiveStatus Objective::Check10RollerCoastersLength() const
{
    BitSet<MAX_RIDE_OBJECTS> type_already_counted;
    auto rcs = 0;
    for (const auto& ride : GetRideManager())
    {
        if (ride.status == RideStatus::Open && ride.excitement >= RIDE_RATING(7, 00) && ride.subtype != OBJECT_ENTRY_INDEX_NULL)
        {
            auto rideEntry = ride.GetRideEntry();
            if (rideEntry != nullptr)
            {
                if (RideEntryHasCategory(*rideEntry, RIDE_CATEGORY_ROLLERCOASTER) && !type_already_counted[ride.subtype])
                {
                    if ((ride.GetTotalLength() >> 16) >= MinimumLength)
                    {
                        type_already_counted[ride.subtype] = true;
                        rcs++;
                    }
                }
            }
        }
    }
    if (rcs >= 10)
    {
        return ObjectiveStatus::Success;
    }

    return ObjectiveStatus::Undecided;
}

ObjectiveStatus Objective::CheckFinish5RollerCoasters() const
{
    // Originally, this did not check for null rides, neither did it check if
    // the rides are even rollercoasters, never mind the right rollercoasters to be finished.
    auto rcs = 0;
    for (const auto& ride : GetRideManager())
    {
        if (ride.status != RideStatus::Closed && ride.excitement >= MinimumExcitement)
        {
            auto rideEntry = ride.GetRideEntry();
            if (rideEntry != nullptr)
            {
                if ((ride.lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
                    && RideEntryHasCategory(*rideEntry, RIDE_CATEGORY_ROLLERCOASTER))
                {
                    rcs++;
                }
            }
        }
    }
    if (rcs >= 5)
    {
        return ObjectiveStatus::Success;
    }

    return ObjectiveStatus::Undecided;
}

ObjectiveStatus Objective::CheckRepayLoanAndParkValue() const
{
    money32 parkValue = gParkValue;
    money32 currentLoan = gBankLoan;

    if (currentLoan <= 0 && parkValue >= Currency)
    {
        return ObjectiveStatus::Success;
    }

    return ObjectiveStatus::Undecided;
}

ObjectiveStatus Objective::CheckMonthlyFoodIncome() const
{
    const auto* lastMonthExpenditure = gExpenditureTable[1];
    auto lastMonthProfit = lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::ShopSales)]
        + lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::ShopStock)]
        + lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::FoodDrinkSales)]
        + lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::FoodDrinkStock)];

    if (lastMonthProfit >= Currency)
    {
        return ObjectiveStatus::Success;
    }

    return ObjectiveStatus::Undecided;
}

/*
 * Returns the AllowEarlyCompletion-Option to be used
 * depending on the Current Network-Mode.
 */
bool AllowEarlyCompletion()
{
    switch (NetworkGetMode())
    {
        case NETWORK_MODE_CLIENT:
            return gAllowEarlyCompletionInNetworkPlay;
        case NETWORK_MODE_NONE:
        case NETWORK_MODE_SERVER:
        default:
            return gConfigGeneral.AllowEarlyCompletion;
    }
}

static void ScenarioCheckObjective()
{
    auto status = gScenarioObjective.Check();
    if (status == ObjectiveStatus::Success)
    {
        ScenarioSuccess();
    }
    else if (status == ObjectiveStatus::Failure)
    {
        ScenarioFailure();
    }
}

/**
 * Checks the win/lose conditions of the current objective.
 *  rct2: 0x0066A4B2
 */
ObjectiveStatus Objective::Check() const
{
    if (gScenarioCompletedCompanyValue != MONEY64_UNDEFINED)
    {
        return ObjectiveStatus::Undecided;
    }

    switch (Type)
    {
        case OBJECTIVE_GUESTS_BY:
            return CheckGuestsBy();
        case OBJECTIVE_PARK_VALUE_BY:
            return CheckParkValueBy();
        case OBJECTIVE_10_ROLLERCOASTERS:
            return Check10RollerCoasters();
        case OBJECTIVE_GUESTS_AND_RATING:
            return CheckGuestsAndRating();
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
            return CheckMonthlyRideIncome();
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            return Check10RollerCoastersLength();
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
            return CheckFinish5RollerCoasters();
        case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
            return CheckRepayLoanAndParkValue();
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            return CheckMonthlyFoodIncome();
    }

    return ObjectiveStatus::Undecided;
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
