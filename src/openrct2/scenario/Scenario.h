/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../core/Random.hpp"
#include "../core/String.hpp"

struct ResultWithMessage;

namespace OpenRCT2
{
    struct GameState_t;
}

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
