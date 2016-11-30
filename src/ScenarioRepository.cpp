#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <algorithm>
#include <memory>
#include <vector>
#include "core/Console.hpp"
#include "core/FileScanner.h"
#include "core/FileStream.hpp"
#include "core/Math.hpp"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "ScenarioRepository.h"
#include "ScenarioSources.h"

extern "C"
{
    #include "config.h"
    #include "localisation/localisation.h"
    #include "scenario.h"
    #include "rct2.h"
}

static int ScenarioCategoryCompare(int categoryA, int categoryB)
{
    if (categoryA == categoryB) return 0;
    if (categoryA == SCENARIO_CATEGORY_DLC) return -1;
    if (categoryB == SCENARIO_CATEGORY_DLC) return 1;
    if (categoryA == SCENARIO_CATEGORY_BUILD_YOUR_OWN) return -1;
    if (categoryB == SCENARIO_CATEGORY_BUILD_YOUR_OWN) return 1;
    return Math::Sign(categoryA - categoryB);
}

static int scenario_index_entry_CompareByCategory(const scenario_index_entry &entryA,
                                                  const scenario_index_entry &entryB)
{
    // Order by category
    if (entryA.category != entryB.category)
    {
        return ScenarioCategoryCompare(entryA.category, entryB.category);
    }

    // Then by source game / name
    switch (entryA.category) {
    default:
        if (entryA.source_game != entryB.source_game)
        {
            return entryA.source_game - entryB.source_game;
        }
        return strcmp(entryA.name, entryB.name);
    case SCENARIO_CATEGORY_REAL:
    case SCENARIO_CATEGORY_OTHER:
        return strcmp(entryA.name, entryB.name);
    }
}

static int scenario_index_entry_CompareByIndex(const scenario_index_entry &entryA,
                                               const scenario_index_entry &entryB)
{
    // Order by source game
    if (entryA.source_game != entryB.source_game)
    {
        return entryA.source_game - entryB.source_game;
    }

    // Then by index / category / name
    uint8 sourceGame = entryA.source_game;
    switch (sourceGame) {
    default:
        if (entryA.source_index == -1 && entryB.source_index == -1)
        {
            if (entryA.category == entryB.category)
            {
                return scenario_index_entry_CompareByCategory(entryA, entryB);
            }
            else
            {
                return ScenarioCategoryCompare(entryA.category, entryB.category);
            }
        }
        else if (entryA.source_index == -1)
        {
            return 1;
        }
        else if (entryB.source_index == -1)
        {
            return -1;
        }
        else
        {
            return entryA.source_index - entryB.source_index;
        }
    case SCENARIO_SOURCE_REAL:
        return scenario_index_entry_CompareByCategory(entryA, entryB);
    }
}

static void scenario_highscore_free(scenario_highscore_entry * highscore)
{
    SafeFree(highscore->fileName);
    SafeFree(highscore->name);
    SafeDelete(highscore);
}

class ScenarioRepository final : public IScenarioRepository
{
private:
    static constexpr uint32 HighscoreFileVersion = 1;

    std::vector<scenario_index_entry> _scenarios;
    std::vector<scenario_highscore_entry*> _highscores;

public:
    virtual ~ScenarioRepository()
    {
        ClearHighscores();
    }

    void Scan() override
    {
        utf8 directory[MAX_PATH];

        _scenarios.clear();

        // Scan RCT2 directory
        GetRCT2Directory(directory, sizeof(directory));
        Scan(directory);

        // Scan user directory
        GetUserDirectory(directory, sizeof(directory));
        Scan(directory);

        Sort();
        LoadScores();
        LoadLegacyScores();
        AttachHighscores();
    }

    size_t GetCount() const override
    {
        return _scenarios.size();
    }

    const scenario_index_entry * GetByIndex(size_t index) const override
    {
        const scenario_index_entry * result = nullptr;
        if (index < _scenarios.size())
        {
            result = &_scenarios[index];
        }
        return result;
    }

    const scenario_index_entry * GetByFilename(const utf8 * filename) const override
    {
        for (size_t i = 0; i < _scenarios.size(); i++)
        {
            const scenario_index_entry * scenario = &_scenarios[i];
            const utf8 * scenarioFilename = Path::GetFileName(scenario->path);

            // Note: this is always case insensitive search for cross platform consistency
            if (String::Equals(filename, scenarioFilename, true))
            {
                return &_scenarios[i];
            }
        }
        return nullptr;
    }

    const scenario_index_entry * GetByPath(const utf8 * path) const override
    {
        for (size_t i = 0; i < _scenarios.size(); i++)
        {
            const scenario_index_entry * scenario = &_scenarios[i];
            if (Path::Equals(path, scenario->path))
            {
                return scenario;
            }
        }
        return nullptr;
    }

    bool TryRecordHighscore(const utf8 * scenarioFileName, money32 companyValue, const utf8 * name) override
    {
        scenario_index_entry * scenario = GetByFilename(scenarioFileName);
        if (scenario != nullptr)
        {
            // Check if record company value has been broken or the highscore is the same but no name is registered
            scenario_highscore_entry * highscore = scenario->highscore;
            if (highscore == nullptr || companyValue > highscore->company_value ||
                (highscore->name == nullptr && companyValue == highscore->company_value))
            {
                if (highscore == nullptr)
                {
                    highscore = InsertHighscore();
                    highscore->timestamp = platform_get_datetime_now_utc();
                    scenario->highscore = highscore;
                }
                else
                {
                    if (highscore->name != nullptr)
                    {
                        highscore->timestamp = platform_get_datetime_now_utc();
                    }
                    SafeFree(highscore->fileName);
                    SafeFree(highscore->name);
                }
                highscore->fileName = String::Duplicate(Path::GetFileName(scenario->path));
                highscore->name = String::Duplicate(name);
                highscore->company_value = companyValue;
                SaveHighscores();
                return true;
            }
        }
        return false;
    }

private:
    scenario_index_entry * GetByFilename(const utf8 * filename)
    {
        const ScenarioRepository * repo = this;
        return (scenario_index_entry *)repo->GetByFilename(filename);
    }

    scenario_index_entry * GetByPath(const utf8 * path)
    {
        const ScenarioRepository * repo = this;
        return (scenario_index_entry *)repo->GetByPath(path);
    }

    void Scan(const utf8 * directory)
    {
        utf8 pattern[MAX_PATH];
        String::Set(pattern, sizeof(pattern), directory);
        Path::Append(pattern, sizeof(pattern), "*.sc6");

        IFileScanner * scanner = Path::ScanDirectory(pattern, true);
        while (scanner->Next())
        {
            auto path = scanner->GetPath();
            auto fileInfo = scanner->GetFileInfo();
            AddScenario(path, fileInfo->LastModified);
        }
        delete scanner;
    }

    void AddScenario(const utf8 * path, uint64 timestamp)
    {
        rct_s6_header s6Header;
        rct_s6_info s6Info;
        if (!scenario_load_basic(path, &s6Header, &s6Info))
        {
            Console::Error::WriteLine("Unable to read scenario: '%s'", path);
            return;
        }

        const utf8 * filename = Path::GetFileName(path);
        scenario_index_entry * existingEntry = GetByFilename(filename);
        if (existingEntry != nullptr)
        {
            const utf8 * conflictPath;
            if (existingEntry->timestamp > timestamp)
            {
                // Existing entry is more recent
                conflictPath = existingEntry->path;

                // Overwrite existing entry with this one
                *existingEntry = CreateNewScenarioEntry(path, timestamp, &s6Info);
            }
            else
            {
                // This entry is more recent
                conflictPath = path;
            }
            Console::WriteLine("Scenario conflict: '%s' ignored because it is newer.", conflictPath);
        }
        else
        {
            scenario_index_entry entry = CreateNewScenarioEntry(path, timestamp, &s6Info);
            _scenarios.push_back(entry);
        }
    }

    scenario_index_entry CreateNewScenarioEntry(const utf8 * path, uint64 timestamp, rct_s6_info * s6Info)
    {
        scenario_index_entry entry = { 0 };

        // Set new entry
        String::Set(entry.path, sizeof(entry.path), path);
        entry.timestamp = timestamp;
        entry.category = s6Info->category;
        entry.objective_type = s6Info->objective_type;
        entry.objective_arg_1 = s6Info->objective_arg_1;
        entry.objective_arg_2 = s6Info->objective_arg_2;
        entry.objective_arg_3 = s6Info->objective_arg_3;
        entry.highscore = nullptr;
        String::Set(entry.name, sizeof(entry.name), s6Info->name);
        String::Set(entry.details, sizeof(entry.details), s6Info->details);

        // Normalise the name to make the scenario as recognisable as possible.
        ScenarioSources::NormaliseName(entry.name, sizeof(entry.name), entry.name);

        // Look up and store information regarding the origins of this scenario.
        source_desc desc;
        if (ScenarioSources::TryGetByName(entry.name, &desc))
        {
            entry.sc_id = desc.id;
            entry.source_index = desc.index;
            entry.source_game = desc.source;
            entry.category = desc.category;
        }
        else
        {
            entry.sc_id = SC_UNIDENTIFIED;
            entry.source_index = -1;
            if (entry.category == SCENARIO_CATEGORY_REAL)
            {
                entry.source_game = SCENARIO_SOURCE_REAL;
            }
            else
            {
                entry.source_game = SCENARIO_SOURCE_OTHER;
            }
        }

        scenario_translate(&entry, &s6Info->entry);
        return entry;
    }

    void Sort()
    {
        if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN)
        {
            std::sort(_scenarios.begin(), _scenarios.end(), [](const scenario_index_entry &a,
                                                               const scenario_index_entry &b) -> bool
            {
                return scenario_index_entry_CompareByIndex(a, b) < 0;
            });
        }
        else
        {
            std::sort(_scenarios.begin(), _scenarios.end(), [](const scenario_index_entry &a,
                                                               const scenario_index_entry &b) -> bool
            {
                return scenario_index_entry_CompareByCategory(a, b) < 0;
            });
        }
    }

    void LoadScores()
    {
        utf8 scoresPath[MAX_PATH];
        GetScoresPath(scoresPath, sizeof(scoresPath));
        if (!platform_file_exists(scoresPath))
        {
            return;
        }

        try
        {
            auto fs = FileStream(scoresPath, FILE_MODE_OPEN);
            uint32 fileVersion = fs.ReadValue<uint32>();
            if (fileVersion != 1)
            {
                Console::Error::WriteLine("Invalid or incompatible highscores file.");
                return;
            }

            ClearHighscores();

            uint32 numHighscores = fs.ReadValue<uint32>();
            for (uint32 i = 0; i < numHighscores; i++)
            {
                scenario_highscore_entry * highscore = InsertHighscore();
                highscore->fileName = fs.ReadString();
                highscore->name = fs.ReadString();
                highscore->company_value = fs.ReadValue<money32>();
                highscore->timestamp = fs.ReadValue<datetime64>();
            }
        }
        catch (Exception ex)
        {
            Console::Error::WriteLine("Error reading highscores.");
        }
    }

    /**
     * Loads the original scores.dat file and replaces any highscores that
     * are better for matching scenarios.
     */
    void LoadLegacyScores()
    {
        utf8 scoresPath[MAX_PATH];

        GetLegacyScoresPath(scoresPath, sizeof(scoresPath));
        LoadLegacyScores(scoresPath);

        GetRCT2ScoresPath(scoresPath, sizeof(scoresPath));
        LoadLegacyScores(scoresPath);
    }

    void LoadLegacyScores(const utf8 * path)
    {
        if (!platform_file_exists(path))
        {
            return;
        }

        bool highscoresDirty = false;
        try
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            if (fs.GetLength() <= 4)
            {
                // Initial value of scores for RCT2, just ignore
                return;
            }

            // Load header
            auto header = fs.ReadValue<rct_scenario_scores_header>();
            for (uint32 i = 0; i < header.scenario_count; i++)
            {
                // Read legacy entry
                auto scBasic = fs.ReadValue<rct_scenario_basic>();

                // Ignore non-completed scenarios
                if (scBasic.flags & SCENARIO_FLAGS_COMPLETED)
                {
                    bool notFound = true;
                    for (size_t i = 0; i < _highscores.size(); i++)
                    {
                        scenario_highscore_entry * highscore = _highscores[i];
                        if (String::Equals(scBasic.path, highscore->fileName, true))
                        {
                            notFound = false;

                            // Check if legacy highscore is better
                            if (scBasic.company_value > highscore->company_value)
                            {
                                SafeFree(highscore->name);
                                highscore->name = win1252_to_utf8_alloc(scBasic.completed_by);
                                highscore->company_value = scBasic.company_value;
                                highscore->timestamp = DATETIME64_MIN;
                                break;
                            }
                        }
                    }
                    if (notFound)
                    {
                        scenario_highscore_entry * highscore = InsertHighscore();
                        highscore->fileName = String::Duplicate(scBasic.path);
                        highscore->name = win1252_to_utf8_alloc(scBasic.completed_by);
                        highscore->company_value = scBasic.company_value;
                        highscore->timestamp = DATETIME64_MIN;
                    }
                }
            }
        }
        catch (Exception ex)
        {
            Console::Error::WriteLine("Error reading legacy scenario scores file: '%s'", path);
        }

        if (highscoresDirty)
        {
            SaveHighscores();
        }
    }

    void ClearHighscores()
    {
        for (auto highscore : _highscores)
        {
            scenario_highscore_free(highscore);
        }
        _highscores.clear();
    }

    scenario_highscore_entry * InsertHighscore()
    {
        auto highscore = new scenario_highscore_entry();
        memset(highscore, 0, sizeof(scenario_highscore_entry));
        _highscores.push_back(highscore);
        return highscore;
    }

    void AttachHighscores()
    {
        for (size_t i = 0; i < _highscores.size(); i++)
        {
            scenario_highscore_entry * highscore = _highscores[i];
            scenario_index_entry * scenerio = GetByFilename(highscore->fileName);
            if (scenerio != nullptr)
            {
                scenerio->highscore = highscore;
            }
        }
    }

    void SaveHighscores()
    {
        utf8 scoresPath[MAX_PATH];
        GetScoresPath(scoresPath, sizeof(scoresPath));

        try
        {
            auto fs = FileStream(scoresPath, FILE_MODE_WRITE);
            fs.WriteValue<uint32>(HighscoreFileVersion);
            fs.WriteValue<uint32>((uint32)_highscores.size());
            for (size_t i = 0; i < _highscores.size(); i++)
            {
                const scenario_highscore_entry * highscore = _highscores[i];
                fs.WriteString(highscore->fileName);
                fs.WriteString(highscore->name);
                fs.WriteValue(highscore->company_value);
                fs.WriteValue(highscore->timestamp);
            }
        }
        catch (Exception ex)
        {
            Console::Error::WriteLine("Unable to save highscores to '%s'", scoresPath);
        }
    }

    static utf8 * GetRCT2Directory(utf8 * buffer, size_t bufferSize)
    {
        String::Set(buffer, bufferSize, gRCT2AddressAppPath);
        Path::Append(buffer, bufferSize, "Scenarios");
        return buffer;
    }

    static utf8 * GetUserDirectory(utf8 * buffer, size_t bufferSize)
    {
        platform_get_user_directory(buffer, "scenario", bufferSize);
        return buffer;
    }

    static void GetScoresPath(utf8 * buffer, size_t bufferSize)
    {
        platform_get_user_directory(buffer, nullptr, bufferSize);
        Path::Append(buffer, bufferSize, "highscores.dat");
    }

    static void GetLegacyScoresPath(utf8 * buffer, size_t bufferSize)
    {
        platform_get_user_directory(buffer, nullptr, bufferSize);
        Path::Append(buffer, bufferSize, "scores.dat");
    }

    static void GetRCT2ScoresPath(utf8 * buffer, size_t bufferSize)
    {
        String::Set(buffer, bufferSize, get_file_path(PATH_ID_SCORES));
    }
};

static std::unique_ptr<ScenarioRepository> _scenarioRepository;

IScenarioRepository * GetScenarioRepository()
{
    if (_scenarioRepository == nullptr)
    {
        _scenarioRepository = std::unique_ptr<ScenarioRepository>(new ScenarioRepository());
    }
    return _scenarioRepository.get();
}

extern "C"
{
    void scenario_repository_scan()
    {
        IScenarioRepository * repo = GetScenarioRepository();
        repo->Scan();
    }

    size_t scenario_repository_get_count()
    {
        IScenarioRepository * repo = GetScenarioRepository();
        return repo->GetCount();
    }

    const scenario_index_entry *scenario_repository_get_by_index(size_t index)
    {
        IScenarioRepository * repo = GetScenarioRepository();
        return repo->GetByIndex(index);
    }

    bool scenario_repository_try_record_highscore(const utf8 * scenarioFileName, money32 companyValue, const utf8 * name)
    {
        IScenarioRepository * repo = GetScenarioRepository();
        return repo->TryRecordHighscore(scenarioFileName, companyValue, name);
    }
}
