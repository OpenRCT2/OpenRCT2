/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioRepository.h"

#include "../Context.h"
#include "../Game.h"
#include "../ParkImporter.h"
#include "../PlatformEnvironment.h"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/FileIndex.hpp"
#include "../core/FileStream.h"
#include "../core/MemoryStream.h"
#include "../core/Numerics.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../localisation/Language.h"
#include "../localisation/Localisation.h"
#include "../localisation/LocalisationService.h"
#include "../platform/Platform.h"
#include "../rct12/RCT12.h"
#include "../rct12/SawyerChunkReader.h"
#include "Scenario.h"
#include "ScenarioSources.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

using namespace OpenRCT2;

static int32_t ScenarioCategoryCompare(int32_t categoryA, int32_t categoryB)
{
    if (categoryA == categoryB)
        return 0;
    if (categoryA == SCENARIO_CATEGORY_DLC)
        return -1;
    if (categoryB == SCENARIO_CATEGORY_DLC)
        return 1;
    if (categoryA == SCENARIO_CATEGORY_BUILD_YOUR_OWN)
        return -1;
    if (categoryB == SCENARIO_CATEGORY_BUILD_YOUR_OWN)
        return 1;
    if (categoryA < categoryB)
        return -1;
    return 1;
}

static int32_t scenario_index_entry_CompareByCategory(const scenario_index_entry& entryA, const scenario_index_entry& entryB)
{
    // Order by category
    if (entryA.category != entryB.category)
    {
        return ScenarioCategoryCompare(entryA.category, entryB.category);
    }

    // Then by source game / name
    switch (entryA.category)
    {
        default:
            if (entryA.source_game != entryB.source_game)
            {
                return static_cast<int32_t>(entryA.source_game) - static_cast<int32_t>(entryB.source_game);
            }
            return strcmp(entryA.name, entryB.name);
        case SCENARIO_CATEGORY_REAL:
        case SCENARIO_CATEGORY_OTHER:
            return strcmp(entryA.name, entryB.name);
    }
}

static int32_t scenario_index_entry_CompareByIndex(const scenario_index_entry& entryA, const scenario_index_entry& entryB)
{
    // Order by source game
    if (entryA.source_game != entryB.source_game)
    {
        return static_cast<int32_t>(entryA.source_game) - static_cast<int32_t>(entryB.source_game);
    }

    // Then by index / category / name
    ScenarioSource sourceGame = ScenarioSource{ entryA.source_game };
    switch (sourceGame)
    {
        default:
            if (entryA.source_index == -1 && entryB.source_index == -1)
            {
                if (entryA.category == entryB.category)
                {
                    return scenario_index_entry_CompareByCategory(entryA, entryB);
                }

                return ScenarioCategoryCompare(entryA.category, entryB.category);
            }
            if (entryA.source_index == -1)
            {
                return 1;
            }
            if (entryB.source_index == -1)
            {
                return -1;
            }
            return entryA.source_index - entryB.source_index;

        case ScenarioSource::Real:
            return scenario_index_entry_CompareByCategory(entryA, entryB);
    }
}

static void scenario_highscore_free(scenario_highscore_entry* highscore)
{
    SafeFree(highscore->fileName);
    SafeFree(highscore->name);
    SafeDelete(highscore);
}

class ScenarioFileIndex final : public FileIndex<scenario_index_entry>
{
private:
    static constexpr uint32_t MAGIC_NUMBER = 0x58444953; // SIDX
    static constexpr uint16_t VERSION = 5;
    static constexpr auto PATTERN = "*.sc4;*.sc6;*.sea;*.park";

public:
    explicit ScenarioFileIndex(const IPlatformEnvironment& env)
        : FileIndex(
            "scenario index", MAGIC_NUMBER, VERSION, env.GetFilePath(PATHID::CACHE_SCENARIOS), std::string(PATTERN),
            std::vector<std::string>({
                env.GetDirectoryPath(DIRBASE::RCT1, DIRID::SCENARIO),
                env.GetDirectoryPath(DIRBASE::RCT2, DIRID::SCENARIO),
                env.GetDirectoryPath(DIRBASE::USER, DIRID::SCENARIO),
            }))
    {
    }

protected:
    std::tuple<bool, scenario_index_entry> Create(int32_t, const std::string& path) const override
    {
        scenario_index_entry entry;
        auto timestamp = File::GetLastModified(path);
        if (GetScenarioInfo(path, timestamp, &entry))
        {
            return std::make_tuple(true, entry);
        }

        return std::make_tuple(true, scenario_index_entry());
    }

    void Serialise(DataSerialiser& ds, scenario_index_entry& item) const override
    {
        ds << item.path;
        ds << item.timestamp;
        ds << item.category;
        ds << item.source_game;
        ds << item.source_index;
        ds << item.sc_id;
        ds << item.objective_type;
        ds << item.objective_arg_1;
        ds << item.objective_arg_2;
        ds << item.objective_arg_3;

        ds << item.internal_name;
        ds << item.name;
        ds << item.details;
    }

private:
    static std::unique_ptr<IStream> GetStreamFromRCT2Scenario(const std::string& path)
    {
        if (String::Equals(Path::GetExtension(path), ".sea", true))
        {
            auto data = DecryptSea(fs::u8path(path));
            auto ms = std::make_unique<MemoryStream>();
            // Need to copy the data into MemoryStream as the overload will borrow instead of copy.
            ms->Write(data.data(), data.size());
            ms->SetPosition(0);
            return ms;
        }

        auto fs = std::make_unique<FileStream>(path, FILE_MODE_OPEN);
        return fs;
    }

    /**
     * Reads basic information from a scenario file.
     */
    static bool GetScenarioInfo(const std::string& path, uint64_t timestamp, scenario_index_entry* entry)
    {
        log_verbose("GetScenarioInfo(%s, %d, ...)", path.c_str(), timestamp);
        try
        {
            std::string extension = Path::GetExtension(path);
            if (String::Equals(extension, ".park", true))
            {
                // OpenRCT2 park
                bool result = false;
                try
                {
                    auto& objRepository = OpenRCT2::GetContext()->GetObjectRepository();
                    auto importer = ParkImporter::CreateParkFile(objRepository);
                    importer->LoadScenario(path.c_str(), true);
                    if (importer->GetDetails(entry))
                    {
                        String::Set(entry->path, sizeof(entry->path), path.c_str());
                        entry->timestamp = timestamp;
                        result = true;
                    }
                }
                catch (const std::exception&)
                {
                }
                return result;
            }

            if (String::Equals(extension, ".sc4", true))
            {
                // RCT1 scenario
                bool result = false;
                try
                {
                    auto s4Importer = ParkImporter::CreateS4();
                    s4Importer->LoadScenario(path.c_str(), true);
                    if (s4Importer->GetDetails(entry))
                    {
                        String::Set(entry->path, sizeof(entry->path), path.c_str());
                        entry->timestamp = timestamp;
                        result = true;
                    }
                }
                catch (const std::exception&)
                {
                }
                return result;
            }

            // RCT2 or RCTC scenario
            auto stream = GetStreamFromRCT2Scenario(path);
            auto chunkReader = SawyerChunkReader(stream.get());

            const auto header = chunkReader.ReadChunkAs<RCT2::S6Header>();
            if (header.type == S6_TYPE_SCENARIO)
            {
                auto info = chunkReader.ReadChunkAs<RCT2::S6Info>();
                // If the name or the details contain a colour code, they might be in UTF-8 already.
                // This is caused by a bug that was in OpenRCT2 for 3 years.
                if (!IsLikelyUTF8(info.name) && !IsLikelyUTF8(info.details))
                {
                    rct2_to_utf8_self(info.name, sizeof(info.name));
                    rct2_to_utf8_self(info.details, sizeof(info.details));
                }

                *entry = CreateNewScenarioEntry(path, timestamp, &info);
                return true;
            }

            log_verbose("%s is not a scenario", path.c_str());
        }
        catch (const std::exception&)
        {
            Console::Error::WriteLine("Unable to read scenario: '%s'", path.c_str());
        }
        return false;
    }

    static scenario_index_entry CreateNewScenarioEntry(const std::string& path, uint64_t timestamp, RCT2::S6Info* s6Info)
    {
        scenario_index_entry entry = {};

        // Set new entry
        String::Set(entry.path, sizeof(entry.path), path.c_str());
        entry.timestamp = timestamp;
        entry.category = s6Info->category;
        entry.objective_type = s6Info->objective_type;
        entry.objective_arg_1 = s6Info->objective_arg_1;
        entry.objective_arg_2 = s6Info->objective_arg_2;
        entry.objective_arg_3 = s6Info->objective_arg_3;
        entry.highscore = nullptr;
        if (String::IsNullOrEmpty(s6Info->name))
        {
            // If the scenario doesn't have a name, set it to the filename
            String::Set(entry.name, sizeof(entry.name), Path::GetFileNameWithoutExtension(entry.path).c_str());
        }
        else
        {
            String::Set(entry.name, sizeof(entry.name), s6Info->name);
            // Normalise the name to make the scenario as recognisable as possible.
            ScenarioSources::NormaliseName(entry.name, sizeof(entry.name), entry.name);
        }

        // entry.name will be translated later so keep the untranslated name here
        String::Set(entry.internal_name, sizeof(entry.internal_name), entry.name);

        String::Set(entry.details, sizeof(entry.details), s6Info->details);

        // Look up and store information regarding the origins of this scenario.
        source_desc desc;
        if (ScenarioSources::TryGetByName(entry.name, &desc))
        {
            entry.sc_id = desc.id;
            entry.source_index = desc.index;
            entry.source_game = ScenarioSource{ desc.source };
            entry.category = desc.category;
        }
        else
        {
            entry.sc_id = SC_UNIDENTIFIED;
            entry.source_index = -1;
            if (entry.category == SCENARIO_CATEGORY_REAL)
            {
                entry.source_game = ScenarioSource::Real;
            }
            else
            {
                entry.source_game = ScenarioSource::Other;
            }
        }

        scenario_translate(&entry);
        return entry;
    }
};

class ScenarioRepository final : public IScenarioRepository
{
private:
    static constexpr uint32_t HighscoreFileVersion = 2;

    std::shared_ptr<IPlatformEnvironment> const _env;
    ScenarioFileIndex const _fileIndex;
    std::vector<scenario_index_entry> _scenarios;
    std::vector<scenario_highscore_entry*> _highscores;

public:
    explicit ScenarioRepository(const std::shared_ptr<IPlatformEnvironment>& env)
        : _env(env)
        , _fileIndex(*env)
    {
    }

    virtual ~ScenarioRepository()
    {
        ClearHighscores();
    }

    void Scan(int32_t language) override
    {
        ImportMegaPark();

        // Reload scenarios from index
        _scenarios.clear();
        auto scenarios = _fileIndex.LoadOrBuild(language);
        for (const auto& scenario : scenarios)
        {
            AddScenario(scenario);
        }

        // Sort the scenarios and load the highscores
        Sort();
        LoadScores();
        LoadLegacyScores();
        AttachHighscores();
    }

    size_t GetCount() const override
    {
        return _scenarios.size();
    }

    const scenario_index_entry* GetByIndex(size_t index) const override
    {
        const scenario_index_entry* result = nullptr;
        if (index < _scenarios.size())
        {
            result = &_scenarios[index];
        }
        return result;
    }

    const scenario_index_entry* GetByFilename(u8string_view filename) const override
    {
        for (const auto& scenario : _scenarios)
        {
            const auto scenarioFilename = Path::GetFileName(scenario.path);

            // Note: this is always case insensitive search for cross platform consistency
            if (String::Equals(filename, scenarioFilename, true))
            {
                return &scenario;
            }
        }
        return nullptr;
    }

    const scenario_index_entry* GetByInternalName(const utf8* name) const override
    {
        for (size_t i = 0; i < _scenarios.size(); i++)
        {
            const scenario_index_entry* scenario = &_scenarios[i];

            if (scenario->source_game == ScenarioSource::Other && scenario->sc_id == SC_UNIDENTIFIED)
                continue;

            // Note: this is always case insensitive search for cross platform consistency
            if (String::Equals(name, scenario->internal_name, true))
            {
                return &_scenarios[i];
            }
        }
        return nullptr;
    }

    const scenario_index_entry* GetByPath(const utf8* path) const override
    {
        for (const auto& scenario : _scenarios)
        {
            if (Path::Equals(path, scenario.path))
            {
                return &scenario;
            }
        }
        return nullptr;
    }

    bool TryRecordHighscore(int32_t language, const utf8* scenarioFileName, money64 companyValue, const utf8* name) override
    {
        // Scan the scenarios so we have a fresh list to query. This is to prevent the issue of scenario completions
        // not getting recorded, see #4951.
        Scan(language);

        scenario_index_entry* scenario = GetByFilename(scenarioFileName);

        // Check if this is an RCTC scenario that corresponds to a known RCT1/2 scenario or vice versa, see #12626
        if (scenario == nullptr)
        {
            const std::string scenarioBaseName = Path::GetFileNameWithoutExtension(scenarioFileName);
            const std::string scenarioExtension = Path::GetExtension(scenarioFileName);

            if (String::Equals(scenarioExtension, ".sea", true))
            {
                // Get scenario using RCT2 style name of RCTC scenario
                scenario = GetByFilename((scenarioBaseName + ".sc6").c_str());
            }
            else if (String::Equals(scenarioExtension, ".sc6", true))
            {
                // Get scenario using RCTC style name of RCT2 scenario
                scenario = GetByFilename((scenarioBaseName + ".sea").c_str());
            }
        }

        if (scenario != nullptr)
        {
            // Check if record company value has been broken or the highscore is the same but no name is registered
            scenario_highscore_entry* highscore = scenario->highscore;
            if (highscore == nullptr || companyValue > highscore->company_value
                || (String::IsNullOrEmpty(highscore->name) && companyValue == highscore->company_value))
            {
                if (highscore == nullptr)
                {
                    highscore = InsertHighscore();
                    highscore->timestamp = Platform::GetDatetimeNowUTC();
                    scenario->highscore = highscore;
                }
                else
                {
                    if (!String::IsNullOrEmpty(highscore->name))
                    {
                        highscore->timestamp = Platform::GetDatetimeNowUTC();
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
    scenario_index_entry* GetByFilename(const utf8* filename)
    {
        const ScenarioRepository* repo = this;
        return const_cast<scenario_index_entry*>(repo->GetByFilename(filename));
    }

    scenario_index_entry* GetByPath(const utf8* path)
    {
        const ScenarioRepository* repo = this;
        return const_cast<scenario_index_entry*>(repo->GetByPath(path));
    }

    /**
     * Mega Park from RollerCoaster Tycoon 1 is stored in an encrypted hidden file: mp.dat.
     * Decrypt the file and save it as sc21.sc4 in the user's scenario directory.
     */
    void ImportMegaPark()
    {
        auto mpdatPath = _env->FindFile(DIRBASE::RCT1, DIRID::DATA, "mp.dat");
        if (File::Exists(mpdatPath))
        {
            auto scenarioDirectory = _env->GetDirectoryPath(DIRBASE::USER, DIRID::SCENARIO);
            auto expectedSc21Path = Path::Combine(scenarioDirectory, "sc21.sc4");
            auto sc21Path = Path::ResolveCasing(expectedSc21Path);
            if (!File::Exists(sc21Path))
            {
                ConvertMegaPark(mpdatPath, expectedSc21Path);
            }
        }
    }

    /**
     * Converts Mega Park to normalised file location (mp.dat to sc21.sc4)
     * @param srcPath Full path to mp.dat
     * @param dstPath Full path to sc21.dat
     */
    void ConvertMegaPark(const std::string& srcPath, const std::string& dstPath)
    {
        auto directory = Path::GetDirectory(dstPath);
        Platform::EnsureDirectoryExists(directory.c_str());

        auto mpdat = File::ReadAllBytes(srcPath);

        // Rotate each byte of mp.dat left by 4 bits to convert
        for (size_t i = 0; i < mpdat.size(); i++)
        {
            mpdat[i] = Numerics::rol8(mpdat[i], 4);
        }

        File::WriteAllBytes(dstPath, mpdat.data(), mpdat.size());
    }

    void AddScenario(const scenario_index_entry& entry)
    {
        auto filename = Path::GetFileName(entry.path);

        if (!String::Equals(filename, ""))
        {
            auto existingEntry = GetByFilename(filename.c_str());
            if (existingEntry != nullptr)
            {
                std::string conflictPath;
                if (existingEntry->timestamp > entry.timestamp)
                {
                    // Existing entry is more recent
                    conflictPath = String::ToStd(existingEntry->path);

                    // Overwrite existing entry with this one
                    *existingEntry = entry;
                }
                else
                {
                    // This entry is more recent
                    conflictPath = entry.path;
                }
                Console::WriteLine("Scenario conflict: '%s' ignored because it is newer.", conflictPath.c_str());
            }
            else
            {
                _scenarios.push_back(entry);
            }
        }
        else
        {
            log_error("Tried to add scenario with an empty filename!");
        }
    }

    void Sort()
    {
        if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN)
        {
            std::sort(
                _scenarios.begin(), _scenarios.end(), [](const scenario_index_entry& a, const scenario_index_entry& b) -> bool {
                    return scenario_index_entry_CompareByIndex(a, b) < 0;
                });
        }
        else
        {
            std::sort(
                _scenarios.begin(), _scenarios.end(), [](const scenario_index_entry& a, const scenario_index_entry& b) -> bool {
                    return scenario_index_entry_CompareByCategory(a, b) < 0;
                });
        }
    }

    void LoadScores()
    {
        std::string path = _env->GetFilePath(PATHID::SCORES);
        if (!File::Exists(path))
        {
            return;
        }

        try
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            uint32_t fileVersion = fs.ReadValue<uint32_t>();
            if (fileVersion != 1 && fileVersion != 2)
            {
                Console::Error::WriteLine("Invalid or incompatible highscores file.");
                return;
            }

            ClearHighscores();

            uint32_t numHighscores = fs.ReadValue<uint32_t>();
            for (uint32_t i = 0; i < numHighscores; i++)
            {
                scenario_highscore_entry* highscore = InsertHighscore();
                highscore->fileName = fs.ReadString();
                highscore->name = fs.ReadString();
                highscore->company_value = fileVersion == 1 ? fs.ReadValue<money32>() : fs.ReadValue<money64>();
                highscore->timestamp = fs.ReadValue<datetime64>();
            }
        }
        catch (const std::exception&)
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
        std::string rct2Path = _env->GetFilePath(PATHID::SCORES_RCT2);
        std::string legacyPath = _env->GetFilePath(PATHID::SCORES_LEGACY);
        LoadLegacyScores(legacyPath);
        LoadLegacyScores(rct2Path);
    }

    void LoadLegacyScores(const std::string& path)
    {
        if (!File::Exists(path))
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
            auto header = fs.ReadValue<RCT2::ScoresHeader>();
            for (uint32_t i = 0; i < header.ScenarioCount; i++)
            {
                // Read legacy entry
                auto scBasic = fs.ReadValue<RCT2::ScoresEntry>();

                // Ignore non-completed scenarios
                if (scBasic.Flags & SCENARIO_FLAGS_COMPLETED)
                {
                    bool notFound = true;
                    for (auto& highscore : _highscores)
                    {
                        if (String::Equals(scBasic.Path, highscore->fileName, true))
                        {
                            notFound = false;

                            // Check if legacy highscore is better
                            if (scBasic.CompanyValue > highscore->company_value)
                            {
                                SafeFree(highscore->name);
                                std::string name = rct2_to_utf8(scBasic.CompletedBy, RCT2LanguageId::EnglishUK);
                                highscore->name = String::Duplicate(name.c_str());
                                highscore->company_value = scBasic.CompanyValue;
                                highscore->timestamp = DATETIME64_MIN;
                                break;
                            }
                        }
                    }
                    if (notFound)
                    {
                        scenario_highscore_entry* highscore = InsertHighscore();
                        highscore->fileName = String::Duplicate(scBasic.Path);
                        std::string name = rct2_to_utf8(scBasic.CompletedBy, RCT2LanguageId::EnglishUK);
                        highscore->name = String::Duplicate(name.c_str());
                        highscore->company_value = scBasic.CompanyValue;
                        highscore->timestamp = DATETIME64_MIN;
                    }
                }
            }
        }
        catch (const std::exception&)
        {
            Console::Error::WriteLine("Error reading legacy scenario scores file: '%s'", path.c_str());
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

    scenario_highscore_entry* InsertHighscore()
    {
        auto highscore = new scenario_highscore_entry();
        std::memset(highscore, 0, sizeof(scenario_highscore_entry));
        _highscores.push_back(highscore);
        return highscore;
    }

    void AttachHighscores()
    {
        for (auto& highscore : _highscores)
        {
            scenario_index_entry* scenario = GetByFilename(highscore->fileName);
            if (scenario != nullptr)
            {
                scenario->highscore = highscore;
            }
        }
    }

    void SaveHighscores()
    {
        std::string path = _env->GetFilePath(PATHID::SCORES);
        try
        {
            auto fs = FileStream(path, FILE_MODE_WRITE);
            fs.WriteValue<uint32_t>(HighscoreFileVersion);
            fs.WriteValue<uint32_t>(static_cast<uint32_t>(_highscores.size()));
            for (size_t i = 0; i < _highscores.size(); i++)
            {
                const scenario_highscore_entry* highscore = _highscores[i];
                fs.WriteString(highscore->fileName);
                fs.WriteString(highscore->name);
                fs.WriteValue(highscore->company_value);
                fs.WriteValue(highscore->timestamp);
            }
        }
        catch (const std::exception&)
        {
            Console::Error::WriteLine("Unable to save highscores to '%s'", path.c_str());
        }
    }
};

std::unique_ptr<IScenarioRepository> CreateScenarioRepository(const std::shared_ptr<IPlatformEnvironment>& env)
{
    return std::make_unique<ScenarioRepository>(env);
}

IScenarioRepository* GetScenarioRepository()
{
    return GetContext()->GetScenarioRepository();
}

void scenario_repository_scan()
{
    auto context = OpenRCT2::GetContext();
    IScenarioRepository* repo = GetScenarioRepository();
    repo->Scan(LocalisationService_GetCurrentLanguage(context));
}

size_t scenario_repository_get_count()
{
    IScenarioRepository* repo = GetScenarioRepository();
    return repo->GetCount();
}

const scenario_index_entry* scenario_repository_get_by_index(size_t index)
{
    IScenarioRepository* repo = GetScenarioRepository();
    return repo->GetByIndex(index);
}

bool scenario_repository_try_record_highscore(const utf8* scenarioFileName, money64 companyValue, const utf8* name)
{
    auto context = OpenRCT2::GetContext();
    IScenarioRepository* repo = GetScenarioRepository();
    return repo->TryRecordHighscore(LocalisationService_GetCurrentLanguage(context), scenarioFileName, companyValue, name);
}
