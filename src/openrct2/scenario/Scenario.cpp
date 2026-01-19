/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Scenario.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Date.h"
#include "../FileClassifier.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../PlatformEnvironment.h"
#include "../actions/ResultWithMessage.h"
#include "../audio/Audio.h"
#include "../config/Config.h"
#include "../core/BitSet.hpp"
#include "../core/EnumUtils.hpp"
#include "../core/Guard.hpp"
#include "../core/Path.hpp"
#include "../core/Random.hpp"
#include "../core/UnitConversion.h"
#include "../drawing/Drawing.h"
#include "../entity/Duck.h"
#include "../entity/Guest.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../localisation/Formatter.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../management/Research.h"
#include "../network/Network.h"
#include "../object/ObjectEntryManager.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectManager.h"
#include "../object/ScenarioMetaObject.h"
#include "../object/WaterEntry.h"
#include "../platform/Platform.h"
#include "../profiling/Profiling.h"
#include "../rct1/RCT1.h"
#include "../rct12/RCT12.h"
#include "../ride/Ride.h"
#include "../ride/RideManager.hpp"
#include "../ride/Track.h"
#include "../sawyer_coding/SawyerCoding.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "ScenarioRepository.h"
#include "ScenarioSources.h"

#include <chrono>

using namespace OpenRCT2;
using namespace OpenRCT2::Scenario;

std::string gScenarioSavePath;
bool gFirstTimeSaving = true;
uint32_t gLastAutoSaveUpdate = 0;

bool gAllowEarlyCompletionInNetworkPlay;

static void ScenarioCheckObjective(GameState_t& gameState);

void ScenarioBegin(GameState_t& gameState)
{
    GameLoadInit();
    ScenarioReset(gameState);

    if (gameState.scenarioOptions.objective.Type != ObjectiveType::none && !gLoadKeepWindowsOpen)
        ContextOpenWindowView(WindowView::parkObjective);

    gScreenAge = 0;
}

void ScenarioReset(GameState_t& gameState)
{
    // Set the scenario pseudo-random seeds
    Random::RCT2::Seed s{ 0x1234567F ^ Platform::GetTicks(), 0x789FABCD ^ Platform::GetTicks() };
    gameState.scenarioRand.seed(s);

    ResearchResetCurrentItem();

    auto intent = Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG);
    ContextBroadcastIntent(&intent);

    News::InitQueue(gameState);

    auto& park = gameState.park;
    park.rating = Park::CalculateParkRating(park, gameState);
    park.value = Park::CalculateParkValue(park, gameState);
    park.companyValue = Park::CalculateCompanyValue(park);
    park.historicalProfit = gameState.scenarioOptions.initialCash - park.bankLoan;
    park.cash = gameState.scenarioOptions.initialCash;

    auto& objManager = GetContext()->GetObjectManager();
    if (auto* textObject = objManager.GetLoadedObject<ScenarioMetaObject>(0); textObject != nullptr)
    {
        gameState.scenarioOptions.name = textObject->GetScenarioName();
        park.name = textObject->GetParkName();
        gameState.scenarioOptions.details = textObject->GetScenarioDetails();
    }

    // Set the last saved game path
    auto& env = GetContext()->GetPlatformEnvironment();
    auto savePath = env.GetDirectoryPath(DirBase::user, DirId::saves);
    gScenarioSavePath = Path::Combine(savePath, park.name + u8".park");

    park.currentExpenditure = 0;
    park.currentProfit = 0;
    park.weeklyProfitAverageDividend = 0;
    park.weeklyProfitAverageDivisor = 0;
    park.totalAdmissions = 0;
    park.totalIncomeFromAdmissions = 0;

    park.flags &= ~PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;
    gameState.scenarioCompletedCompanyValue = kMoney64Undefined;
    gameState.scenarioCompletedBy = "?";

    Park::ResetHistories(park);
    FinanceResetHistory();
    AwardReset();
    ResetAllRideBuildDates();
    ResetDate();
    Duck::RemoveAll();
    Park::UpdateSize(park);
    MapCountRemainingLandRights();
    Staff::ResetStats();

    gameState.lastEntranceStyle = objManager.GetLoadedObjectEntryIndex("rct2.station.plain");
    if (gameState.lastEntranceStyle == kObjectEntryIndexNull)
    {
        // Fall back to first entrance object
        gameState.lastEntranceStyle = 0;
    }

    park.marketingCampaigns.clear();
    park.ratingCasualtyPenalty = 0;

    // Open park with free entry when there is no money
    if (park.flags & PARK_FLAGS_NO_MONEY)
    {
        park.flags |= PARK_FLAGS_PARK_OPEN;
        park.entranceFee = 0;
    }

    park.flags |= PARK_FLAGS_SPRITES_INITIALISED;
    gGamePaused = false;
}

static void ScenarioEnd()
{
    GameResetSpeed();

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->CloseByClass(WindowClass::dropdown);
    windowMgr->CloseAllExceptFlags({ WindowFlag::stickToBack, WindowFlag::stickToFront });

    ContextOpenWindowView(WindowView::parkObjective);
}

/**
 *
 *  rct2: 0x0066A752
 */
void ScenarioFailure(GameState_t& gameState)
{
    gameState.scenarioCompletedCompanyValue = kCompanyValueOnFailedObjective;
    ScenarioEnd();
}

/**
 *
 *  rct2: 0x0066A75E
 */
void ScenarioSuccess(GameState_t& gameState)
{
    auto companyValue = gameState.park.companyValue;

    gameState.scenarioCompletedCompanyValue = companyValue;
    PeepApplause();

    if (ScenarioRepositoryTryRecordHighscore(gameState.scenarioFileName.c_str(), companyValue, nullptr))
    {
        // Allow name entry
        gameState.park.flags |= PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;
        gameState.scenarioCompanyValueRecord = companyValue;
    }
    ScenarioEnd();
}

/**
 *
 *  rct2: 0x006695E8
 */
void ScenarioSuccessSubmitName(GameState_t& gameState, const char* name)
{
    if (ScenarioRepositoryTryRecordHighscore(gameState.scenarioFileName.c_str(), gameState.scenarioCompanyValueRecord, name))
    {
        gameState.scenarioCompletedBy = name;
    }
    gameState.park.flags &= ~PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;
}

/**
 * Send a warning when entrance price is too high.
 *  rct2: 0x0066A80E
 */
static void ScenarioCheckEntranceFeeTooHigh()
{
    const auto& park = getGameState().park;
    const auto max_fee = AddClamp(park.totalRideValueForMoney, park.totalRideValueForMoney / 2);

    if ((park.flags & PARK_FLAGS_PARK_OPEN) && Park::GetEntranceFee() > max_fee)
    {
        if (!park.entrances.empty())
        {
            const auto& entrance = park.entrances[0];
            auto x = entrance.x + 16;
            auto y = entrance.y + 16;

            uint32_t packed_xy = (y << 16) | x;
            if (Config::Get().notifications.parkWarnings)
            {
                News::AddItemToQueue(News::ItemType::blank, STR_ENTRANCE_FEE_TOO_HI, packed_xy, {});
            }
        }
    }
}

void ScenarioAutosaveCheck()
{
    if (gLastAutoSaveUpdate == kAutosavePause)
        return;

    // Milliseconds since last save
    uint32_t timeSinceSave = Platform::GetTicks() - gLastAutoSaveUpdate;

    bool shouldSave = false;
    using namespace std::chrono_literals;

    switch (Config::Get().general.autosaveFrequency)
    {
        case AUTOSAVE_EVERY_MINUTE:
            shouldSave = timeSinceSave >= std::chrono::milliseconds(1min).count();
            break;
        case AUTOSAVE_EVERY_5MINUTES:
            shouldSave = timeSinceSave >= std::chrono::milliseconds(5min).count();
            break;
        case AUTOSAVE_EVERY_15MINUTES:
            shouldSave = timeSinceSave >= std::chrono::milliseconds(15min).count();
            break;
        case AUTOSAVE_EVERY_30MINUTES:
            shouldSave = timeSinceSave >= std::chrono::milliseconds(30min).count();
            break;
        case AUTOSAVE_EVERY_HOUR:
            shouldSave = timeSinceSave >= std::chrono::milliseconds(1h).count();
            break;
    }

    if (shouldSave)
    {
        gLastAutoSaveUpdate = kAutosavePause;
        GameAutosave();
    }
}

static void ScenarioDayUpdate(GameState_t& gameState)
{
    FinanceUpdateDailyProfit();
    PeepUpdateDaysInQueue();
    switch (gameState.scenarioOptions.objective.Type)
    {
        case ObjectiveType::tenRollercoasters:
        case ObjectiveType::guestsAndRating:
        case ObjectiveType::tenRollercoastersLength:
        case ObjectiveType::finishFiveRollercoasters:
        case ObjectiveType::repayLoanAndParkValue:
            ScenarioCheckObjective(gameState);
            break;
        default:
            if (AllowEarlyCompletion())
                ScenarioCheckObjective(gameState);
            break;
    }

    auto& park = gameState.park;

    // Lower the casualty penalty
    uint16_t casualtyPenaltyModifier = (park.flags & PARK_FLAGS_NO_MONEY) ? 40 : 7;
    park.ratingCasualtyPenalty = std::max(0, park.ratingCasualtyPenalty - casualtyPenaltyModifier);

    auto intent = Intent(INTENT_ACTION_UPDATE_DATE);
    ContextBroadcastIntent(&intent);
}

static void ScenarioWeekUpdate()
{
    int32_t month = GetDate().GetMonth();

    FinancePayWages();
    FinancePayResearch();
    FinancePayInterest();
    MarketingUpdate();
    PeepProblemWarningsUpdate();
    RideCheckAllReachable();
    RideUpdateFavouritedStat();

    auto water_type = OpenRCT2::ObjectManager::GetObjectEntry<WaterObjectEntry>(0);

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
    ScenarioCheckObjective(getGameState());
    ScenarioCheckEntranceFeeTooHigh();
    AwardUpdateAll();
}

static void ScenarioUpdateDayNightCycle()
{
    float currentDayNightCycle = gDayNightCycle;
    gDayNightCycle = 0;

    if (gLegacyScene == LegacyScene::playing && Config::Get().general.dayNightCycle)
    {
        float monthFraction = GetDate().GetMonthTicks() / static_cast<float>(kTicksPerMonth);
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
        UpdatePalette(gGamePalette, Drawing::PaletteIndex::pi10, 236);
    }
}

/**
 * Scenario and finance related update iteration.
 *  rct2: 0x006C44B1
 */
void ScenarioUpdate(GameState_t& gameState)
{
    PROFILED_FUNCTION();

    if (gLegacyScene == LegacyScene::playing)
    {
        auto& date = GetDate();
        if (date.IsDayStart())
        {
            ScenarioDayUpdate(gameState);
        }
        if (date.IsWeekStart())
        {
            ScenarioWeekUpdate();
        }
        if (date.IsFortnightStart())
        {
            ScenarioFortnightUpdate();
        }
        if (date.IsMonthStart())
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
    auto& gameState = getGameState();
    centrePos.x = SquareRadiusSize + (ScenarioRandMax(gameState.mapSize.x - SquareCentre) * 32);
    centrePos.y = SquareRadiusSize + (ScenarioRandMax(gameState.mapSize.y - SquareCentre) * 32);

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
    return getGameState().scenarioRand.state();
}

void ScenarioRandSeed(random_engine_t::result_type s0, random_engine_t::result_type s1)
{
    Random::RCT2::Seed s{ s0, s1 };
    getGameState().scenarioRand.seed(s);
}

/**
 *
 *  rct2: 0x006E37D2
 *
 * @return eax
 */
random_engine_t::result_type ScenarioRand()
{
    return getGameState().scenarioRand();
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
static ResultWithMessage ScenarioPrepareRidesForSave(GameState_t& gameState)
{
    bool isFiveCoasterObjective = gameState.scenarioOptions.objective.Type == ObjectiveType::finishFiveRollercoasters;
    uint8_t rcs = 0;

    for (auto& ride : RideManager(gameState))
    {
        const auto* rideEntry = ride.getRideEntry();
        if (rideEntry != nullptr)
        {
            // If there are more than 5 roller coasters, only mark the first five.
            if (isFiveCoasterObjective && (RideEntryHasCategory(*rideEntry, RideCategory::rollerCoaster) && rcs < 5))
            {
                ride.lifecycleFlags |= RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
                rcs++;
            }
            else
            {
                ride.lifecycleFlags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
            }
        }
    }

    if (isFiveCoasterObjective && rcs < 5)
    {
        return { false, STR_NOT_ENOUGH_ROLLER_COASTERS };
    }

    bool markTrackAsIndestructible;
    TileElementIterator it;
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
                if (ride != nullptr && ride->lifecycleFlags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
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
ResultWithMessage ScenarioPrepareForSave(GameState_t& gameState)
{
    // This can return false if the goal is 'Finish 5 roller coaster' and there are too few.
    const auto prepareRidesResult = ScenarioPrepareRidesForSave(gameState);
    if (!prepareRidesResult.Successful)
    {
        return { false, prepareRidesResult.Message };
    }

    if (gameState.scenarioOptions.objective.Type == ObjectiveType::guestsAndRating)
        gameState.park.flags |= PARK_FLAGS_PARK_OPEN;

    ScenarioReset(gameState);

    return { true };
}

/*
 * Returns the AllowEarlyCompletion-Option to be used
 * depending on the Current Network-Mode.
 */
bool AllowEarlyCompletion()
{
    switch (Network::GetMode())
    {
        case Network::Mode::client:
            return gAllowEarlyCompletionInNetworkPlay;
        case Network::Mode::none:
        case Network::Mode::server:
        default:
            return Config::Get().general.allowEarlyCompletion;
    }
}

static void ScenarioCheckObjective(GameState_t& gameState)
{
    auto& park = gameState.park;
    auto status = gameState.scenarioOptions.objective.Check(park, gameState);
    if (status == ObjectiveStatus::Success)
    {
        ScenarioSuccess(gameState);
    }
    else if (status == ObjectiveStatus::Failure)
    {
        ScenarioFailure(gameState);
    }
}
