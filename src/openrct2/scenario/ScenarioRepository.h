/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/DateTime.h"
#include "../core/Money.hpp"
#include "../core/StringTypes.h"
#include "ScenarioObjective.h"

#include <memory>

struct RCTObjectEntry;

namespace OpenRCT2::Scenario
{
    enum class Category : uint8_t;
}

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
    OpenRCT2::Scenario::Category Category;
    ScenarioSource SourceGame;
    int16_t SourceIndex = -1;
    uint16_t ScenarioId;

    // Objective
    OpenRCT2::Scenario::ObjectiveType ObjectiveType;
    uint8_t ObjectiveArg1;  // years
    int64_t ObjectiveArg2;  // money or excitement
    uint16_t ObjectiveArg3; // guests or rideID or coasterLength
    ScenarioHighscoreEntry* Highscore = nullptr;

    u8string InternalName; // Untranslated name
    u8string Name;         // Translated name
    u8string Details;
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
    virtual const ScenarioIndexEntry* GetByInternalName(u8string_view name) const = 0;
    virtual const ScenarioIndexEntry* GetByPath(const utf8* path) const = 0;

    virtual bool TryRecordHighscore(int32_t language, const utf8* scenarioFileName, money64 companyValue, const utf8* name) = 0;
};

[[nodiscard]] std::unique_ptr<IScenarioRepository> CreateScenarioRepository(OpenRCT2::IPlatformEnvironment& env);
[[nodiscard]] IScenarioRepository* GetScenarioRepository();

void ScenarioRepositoryScan();
[[nodiscard]] size_t ScenarioRepositoryGetCount();
[[nodiscard]] const ScenarioIndexEntry* ScenarioRepositoryGetByIndex(size_t index);
[[nodiscard]] bool ScenarioRepositoryTryRecordHighscore(const utf8* scenarioFileName, money64 companyValue, const utf8* name);
