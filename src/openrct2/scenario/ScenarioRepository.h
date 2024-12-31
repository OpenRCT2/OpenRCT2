
/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/DateTime.h"
#include "../core/StringTypes.h"
#include "../scenario/Scenario.h"

#include <memory>

struct RCTObjectEntry;

struct ScenarioHighscoreEntry
{
    u8string fileName;
    u8string name;
    money64 company_value{};
    datetime64 timestamp{};
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
    u8string Path;
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
    utf8 Name[64];         // Translated name
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
    virtual void Scan(int32_t language) = 0;

    virtual size_t GetCount() const = 0;
    virtual const ScenarioIndexEntry* GetByIndex(size_t index) const = 0;
    virtual const ScenarioIndexEntry* GetByFilename(u8string_view filename) const = 0;
    /**
     * Does not return custom scenarios due to the fact that they may have the same name.
     */
    virtual const ScenarioIndexEntry* GetByInternalName(const utf8* name) const = 0;
    virtual const ScenarioIndexEntry* GetByPath(const utf8* path) const = 0;

    virtual bool TryRecordHighscore(int32_t language, const utf8* scenarioFileName, money64 companyValue, const utf8* name) = 0;
};

[[nodiscard]] std::unique_ptr<IScenarioRepository> CreateScenarioRepository(
    const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
[[nodiscard]] IScenarioRepository* GetScenarioRepository();

void ScenarioRepositoryScan();
[[nodiscard]] size_t ScenarioRepositoryGetCount();
[[nodiscard]] const ScenarioIndexEntry* ScenarioRepositoryGetByIndex(size_t index);
[[nodiscard]] bool ScenarioRepositoryTryRecordHighscore(const utf8* scenarioFileName, money64 companyValue, const utf8* name);
