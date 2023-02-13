/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../scenario/Scenario.h"

#include <memory>

struct RCTObjectEntry;

struct ScenarioHighscoreEntry
{
    utf8* fileName;
    utf8* name;
    money64 company_value;
    datetime64 timestamp;
};

enum class ScenarioSource : uint8_t
{
    RCT1,
    RCT1_AA,
    RCT1_LL,
    RCT2,
    RCT2_WW,
    RCT2_TT,
    UCES,
    Real,
    Extras,
    Other
};

struct ScenarioIndexEntry
{
    utf8 Path[MAX_PATH];
    uint64_t Timestamp;

    // Category / sequence
    uint8_t Category;
    ScenarioSource SourceGame;
    int16_t SourceIndex = -1;
    uint16_t ScenarioId;

    // Objective
    uint8_t ObjectiveType;
    uint8_t ObjectiveArg1;
    int64_t ObjectiveArg2;
    int16_t ObjectiveArg3;
    ScenarioHighscoreEntry* Highscore = nullptr;

    utf8 InternalName[64]; // Untranslated name
    utf8 Name[64];          // Translated name
    utf8 Details[256];
};

namespace OpenRCT2
{
    struct IPlatformEnvironment;
}

struct IScenarioRepository
{
    virtual ~IScenarioRepository() = default;

    /**
     * Scans the scenario directories and grabs the metadata for all the scenarios.
     */
    virtual void Scan(int32_t language) abstract;

    virtual size_t GetCount() const abstract;
    virtual const ScenarioIndexEntry* GetByIndex(size_t index) const abstract;
    virtual const ScenarioIndexEntry* GetByFilename(u8string_view filename) const abstract;
    /**
     * Does not return custom scenarios due to the fact that they may have the same name.
     */
    virtual const ScenarioIndexEntry* GetByInternalName(const utf8* name) const abstract;
    virtual const ScenarioIndexEntry* GetByPath(const utf8* path) const abstract;

    virtual bool TryRecordHighscore(
        int32_t language, const utf8* scenarioFileName, money64 companyValue, const utf8* name) abstract;
};

[[nodiscard]] std::unique_ptr<IScenarioRepository> CreateScenarioRepository(
    const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
[[nodiscard]] IScenarioRepository* GetScenarioRepository();

void ScenarioRepositoryScan();
[[nodiscard]] size_t ScenarioRepositoryGetCount();
[[nodiscard]] const ScenarioIndexEntry* ScenarioRepositoryGetByIndex(size_t index);
[[nodiscard]] bool ScenarioRepositoryTryRecordHighscore(const utf8* scenarioFileName, money64 companyValue, const utf8* name);
void ScenarioTranslate(ScenarioIndexEntry* scenarioEntry);
