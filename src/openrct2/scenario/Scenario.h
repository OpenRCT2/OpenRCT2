/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Random.hpp"
#include "../entity/EntityList.h"
#include "../management/Finance.h"
#include "../management/Research.h"
#include "../object/Object.h"
#include "../ride/RideRatings.h"
#include "../world/Banner.h"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"

struct ResultWithMessage;

using random_engine_t = OpenRCT2::Random::RCT2::Engine;

namespace OpenRCT2
{
    struct GameState_t;
}

enum
{
    SCENARIO_FLAGS_VISIBLE = (1 << 0),
    SCENARIO_FLAGS_COMPLETED = (1 << 1),
    SCENARIO_FLAGS_SIXFLAGS = (1 << 2)
};

enum
{
    S6_TYPE_SAVEDGAME,
    S6_TYPE_SCENARIO
};

enum SCENARIO_CATEGORY
{
    // RCT2 categories (keep order)
    SCENARIO_CATEGORY_BEGINNER,
    SCENARIO_CATEGORY_CHALLENGING,
    SCENARIO_CATEGORY_EXPERT,
    SCENARIO_CATEGORY_REAL,
    SCENARIO_CATEGORY_OTHER,

    // OpenRCT2 categories
    SCENARIO_CATEGORY_DLC,
    SCENARIO_CATEGORY_BUILD_YOUR_OWN,
    SCENARIO_CATEGORY_COMPETITIONS,
    SCENARIO_CATEGORY_TIME_MACHINE,
    SCENARIO_CATEGORY_KATYS_DREAMWORLD,

    SCENARIO_CATEGORY_COUNT
};

enum
{
    OBJECTIVE_NONE,
    OBJECTIVE_GUESTS_BY,
    OBJECTIVE_PARK_VALUE_BY,
    OBJECTIVE_HAVE_FUN,
    OBJECTIVE_BUILD_THE_BEST,
    OBJECTIVE_10_ROLLERCOASTERS,
    OBJECTIVE_GUESTS_AND_RATING,
    OBJECTIVE_MONTHLY_RIDE_INCOME,
    OBJECTIVE_10_ROLLERCOASTERS_LENGTH,
    OBJECTIVE_FINISH_5_ROLLERCOASTERS,
    OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE,
    OBJECTIVE_MONTHLY_FOOD_INCOME,

    OBJECTIVE_COUNT
};

bool ObjectiveNeedsMoney(const uint8_t objective);

enum class ObjectiveStatus : uint8_t
{
    Undecided,
    Success,
    Failure,
};

struct Objective
{
    uint8_t Type;
    uint8_t Year;
    union
    {
        uint16_t NumGuests;
        StringId RideId;
        uint16_t MinimumLength; // For the "Build 10 coasters of minimum length" objective.
    };
    union
    {
        money64 Currency;
        ride_rating MinimumExcitement; // For the "Finish 5 coaster with a minimum excitement rating" objective.
    };

    bool NeedsMoney() const
    {
        return ObjectiveNeedsMoney(Type);
    }

    bool IsValid(bool useMoney, bool canAskMoneyForRides) const
    {
        const bool objectiveAllowedByMoneyUsage = useMoney || !NeedsMoney();
        // This objective can only work if the player can ask money for rides.
        const bool objectiveAllowedByPaymentSettings = (Type != OBJECTIVE_MONTHLY_RIDE_INCOME) || canAskMoneyForRides;
        return objectiveAllowedByMoneyUsage && objectiveAllowedByPaymentSettings;
    }

    ObjectiveStatus Check(OpenRCT2::GameState_t& gameState) const;

private:
    ObjectiveStatus CheckGuestsBy() const;
    ObjectiveStatus CheckParkValueBy() const;
    ObjectiveStatus Check10RollerCoasters() const;
    ObjectiveStatus CheckGuestsAndRating() const;
    ObjectiveStatus CheckMonthlyRideIncome() const;
    ObjectiveStatus Check10RollerCoastersLength() const;
    ObjectiveStatus CheckFinish5RollerCoasters() const;
    ObjectiveStatus CheckRepayLoanAndParkValue() const;
    ObjectiveStatus CheckMonthlyFoodIncome() const;
};

enum
{
    SCENARIO_SELECT_MODE_DIFFICULTY,
    SCENARIO_SELECT_MODE_ORIGIN,
};

enum
{
    AUTOSAVE_EVERY_MINUTE,
    AUTOSAVE_EVERY_5MINUTES,
    AUTOSAVE_EVERY_15MINUTES,
    AUTOSAVE_EVERY_30MINUTES,
    AUTOSAVE_EVERY_HOUR,
    AUTOSAVE_NEVER
};

constexpr uint8_t kAutosavePause = 0;
constexpr uint8_t kDefaultNumAutosavesToKeep = 10;

static constexpr money64 kCompanyValueOnFailedObjective = 0x8000000000000001;

extern const StringId kScenarioCategoryStringIds[SCENARIO_CATEGORY_COUNT];

extern bool gAllowEarlyCompletionInNetworkPlay;

extern std::string gScenarioSavePath;
extern bool gFirstTimeSaving;
extern uint32_t gLastAutoSaveUpdate;

void ScenarioBegin(OpenRCT2::GameState_t& gameState);
void ScenarioReset(OpenRCT2::GameState_t& gameState);
void ScenarioUpdate(OpenRCT2::GameState_t& gameState);
bool ScenarioCreateDucks();
bool AllowEarlyCompletion();

const random_engine_t::state_type& ScenarioRandState();
void ScenarioRandSeed(random_engine_t::result_type s0, random_engine_t::result_type s1);
random_engine_t::result_type ScenarioRand();
uint32_t ScenarioRandMax(uint32_t max);

ResultWithMessage ScenarioPrepareForSave(OpenRCT2::GameState_t& gameState);
int32_t ScenarioSave(OpenRCT2::GameState_t& gameState, u8string_view path, int32_t flags);
void ScenarioFailure(OpenRCT2::GameState_t& gameState);
void ScenarioSuccess(OpenRCT2::GameState_t& gameState);
void ScenarioSuccessSubmitName(OpenRCT2::GameState_t& gameState, const char* name);
void ScenarioAutosaveCheck();
