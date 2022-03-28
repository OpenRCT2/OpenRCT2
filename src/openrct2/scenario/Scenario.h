/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/Random.hpp"
#include "../entity/EntityList.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../world/Banner.h"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "ObjectiveGoalGroup.h"

using random_engine_t = Random::Rct2::Engine;

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

#define S6_RCT2_VERSION 120001
#define S6_MAGIC_NUMBER 0x00031144

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
    OBJECTIVE_MODULAR_SYSTEM_V1,

    OBJECTIVE_COUNT
};

struct Objective
{
    uint8_t Type;
    uint8_t LegacyType;
    uint32_t PhasedGoalIndex = 0;
    std::vector<ObjectiveGoalGroup> PhasedGoals;
    ObjectiveGoalGroup PermanentGoals;
    bool allowParkOpening = true;

    bool IsValid() const
    {
        for (auto group : PhasedGoals)
        {
            if (group.goals.size() == 0)
                return false; // empty group
        }
        if (PhasedGoals.size() == 0)
        {
            if (!PermanentGoals.Initialized() || PermanentGoals.goals.size() == 0)
                return false; // no goals at all.
        }
        return true;
        // other stuff, like money, rides, etc, is checked in goals themselves.
    }

    ObjectiveStatus Check();
    void ConvertObjective(
        uint8_t _type, uint8_t _year, uint16_t _numGuestsRideIdMinLength, money64 _currencyMinExcitement,
        uint16_t _warningDaysParkRating, std::string _scenarioDetails);
    void ConvertObjective(uint8_t _type, std::string _details); // set the old default parameters
    void Reset();
    bool SetPermanentGoalGroup(ObjectiveGoalGroup _group);
    rct_string_id AddPhasedGoalGroup(ObjectiveGoalGroup _group);
    rct_string_id SetPhasedGoalGroup(ObjectiveGoalGroup _group, size_t index);
    void RemovePhasedGoalGroup(uint32_t number);
    void SetPhasedGoalIndex(uint32_t _newIndex, bool reset = false);
    void CalculateAllowParkOpening();
    bool RequiresMoney();
    bool RequiresRidePrices();
    bool RequiresParkEntryPrices();
    bool MoneySettingsOkay();
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

#define AUTOSAVE_PAUSE 0
#define DEFAULT_NUM_AUTOSAVES_TO_KEEP 10

static constexpr money64 COMPANY_VALUE_ON_FAILED_OBJECTIVE = 0x8000000000000001;

extern const rct_string_id ScenarioCategoryStringIds[SCENARIO_CATEGORY_COUNT];

extern random_engine_t gScenarioRand;

extern Objective gScenarioObjective;
extern bool gAllowEarlyCompletionInNetworkPlay;
extern std::vector<uint16_t> gScenarioObjectiveWarningDays;
extern uint16_t gScenarioParkRatingWarningDays;
extern money64 gScenarioCompletedCompanyValue;
extern money64 gScenarioCompanyValueRecord;

extern SCENARIO_CATEGORY gScenarioCategory;
extern std::string gScenarioName;
extern std::string gScenarioDetails;
extern std::string gScenarioObjectiveDescription;
extern std::string gScenarioCompletedBy;
extern std::string gScenarioSavePath;
extern bool gFirstTimeSaving;
extern uint16_t gSavedAge;
extern uint32_t gLastAutoSaveUpdate;

extern std::string gScenarioFileName;

void load_from_sc6(const char* path);
void scenario_begin();
void scenario_update();
bool scenario_create_ducks();
bool AllowEarlyCompletion();

const random_engine_t::state_type& scenario_rand_state();
void scenario_rand_seed(random_engine_t::result_type s0, random_engine_t::result_type s1);
random_engine_t::result_type scenario_rand();
uint32_t scenario_rand_max(uint32_t max);

bool scenario_prepare_for_save();
int32_t scenario_save(u8string_view path, int32_t flags);
void scenario_failure();
void scenario_success();
void scenario_success_submit_name(const char* name);
void scenario_autosave_check();
