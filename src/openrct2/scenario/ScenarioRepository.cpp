/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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

static int32_t ScenarioIndexEntryCompareByCategory(const ScenarioIndexEntry& entryA, const ScenarioIndexEntry& entryB)
{
    // Order by category
    if (entryA.Category != entryB.Category)
    {
        return ScenarioCategoryCompare(entryA.Category, entryB.Category);
    }

    // Then by source game / name
    switch (entryA.Category)
    {
        default:
            if (entryA.SourceGame != entryB.SourceGame)
            {
                return static_cast<int32_t>(entryA.SourceGame) - static_cast<int32_t>(entryB.SourceGame);
            }
            return strcmp(entryA.Name, entryB.Name);
        case SCENARIO_CATEGORY_REAL:
        case SCENARIO_CATEGORY_OTHER:
            return strcmp(entryA.Name, entryB.Name);
    }
}

static int32_t ScenarioIndexEntryCompareByIndex(const ScenarioIndexEntry& entryA, const ScenarioIndexEntry& entryB)
{
    // Order by source game
    if (entryA.SourceGame != entryB.SourceGame)
    {
        return static_cast<int32_t>(entryA.SourceGame) - static_cast<int32_t>(entryB.SourceGame);
    }

    // Then by index / category / name
    ScenarioSource sourceGame = ScenarioSource{ entryA.SourceGame };
    switch (sourceGame)
    {
        default:
            if (entryA.SourceIndex == -1 && entryB.SourceIndex == -1)
            {
                if (entryA.Category == entryB.Category)
                {
                    return ScenarioIndexEntryCompareByCategory(entryA, entryB);
                }

                return ScenarioCategoryCompare(entryA.Category, entryB.Category);
            }
            if (entryA.SourceIndex == -1)
            {
                return 1;
            }
            if (entryB.SourceIndex == -1)
            {
                return -1;
            }
            return entryA.SourceIndex - entryB.SourceIndex;

        case ScenarioSource::Real:
            return ScenarioIndexEntryCompareByCategory(entryA, entryB);
    }
}

static void ScenarioHighscoreFree(ScenarioHighscoreEntry* highscore)
{
    SafeDelete(highscore);
}

class ScenarioFileIndex final : public FileIndex<ScenarioIndexEntry>
{
private:
    static constexpr uint32_t MAGIC_NUMBER = 0x58444953; // SIDX
    static constexpr uint16_t VERSION = 8;
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
    std::optional<ScenarioIndexEntry> Create(int32_t, const std::string& path) const override
    {
        ScenarioIndexEntry entry;
        auto timestamp = File::GetLastModified(path);
        if (GetScenarioInfo(path, timestamp, &entry))
        {
            return entry;
        }

        return std::nullopt;
    }

    void Serialise(DataSerialiser& ds, const ScenarioIndexEntry& item) const override
    {
        ds << item.Path;
        ds << item.Timestamp;
        ds << item.Category;
        ds << item.SourceGame;
        ds << item.SourceIndex;
        ds << item.ScenarioId;
        ds << item.ObjectiveType;
        ds << item.ObjectiveArg1;
        ds << item.ObjectiveArg2;
        ds << item.ObjectiveArg3;

        ds << item.InternalName;
        ds << item.Name;
        ds << item.Details;
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
    static bool GetScenarioInfo(const std::string& path, uint64_t timestamp, ScenarioIndexEntry* entry)
    {
        LOG_VERBOSE("GetScenarioInfo(%s, %d, ...)", path.c_str(), timestamp);
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
                        String::Set(entry->Path, sizeof(entry->Path), path.c_str());
                        entry->Timestamp = timestamp;
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
                        String::Set(entry->Path, sizeof(entry->Path), path.c_str());
                        entry->Timestamp = timestamp;
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
            if (header.Type == S6_TYPE_SCENARIO)
            {
                auto info = chunkReader.ReadChunkAs<RCT2::S6Info>();
                // If the name or the details contain a colour code, they might be in UTF-8 already.
                // This is caused by a bug that was in OpenRCT2 for 3 years.
                if (!IsLikelyUTF8(info.Name) && !IsLikelyUTF8(info.Details))
                {
                    RCT2StringToUTF8Self(info.Name, sizeof(info.Name));
                    RCT2StringToUTF8Self(info.Details, sizeof(info.Details));
                }

                *entry = CreateNewScenarioEntry(path, timestamp, &info);
                return true;
            }

            LOG_VERBOSE("%s is not a scenario", path.c_str());
        }
        catch (const std::exception&)
        {
            Console::Error::WriteLine("Unable to read scenario: '%s'", path.c_str());
        }
        return false;
    }

    static ScenarioIndexEntry CreateNewScenarioEntry(const std::string& path, uint64_t timestamp, RCT2::S6Info* s6Info)
    {
        ScenarioIndexEntry entry = {};

        // Set new entry
        String::Set(entry.Path, sizeof(entry.Path), path.c_str());
        entry.Timestamp = timestamp;
        entry.Category = s6Info->Category;
        entry.ObjectiveType = s6Info->ObjectiveType;
        entry.ObjectiveArg1 = s6Info->ObjectiveArg1;
        entry.ObjectiveArg2 = s6Info->ObjectiveArg2;
        entry.ObjectiveArg3 = s6Info->ObjectiveArg3;
        entry.Highscore = nullptr;
        if (String::IsNullOrEmpty(s6Info->Name))
        {
            // If the scenario doesn't have a name, set it to the filename
            String::Set(entry.Name, sizeof(entry.Name), Path::GetFileNameWithoutExtension(entry.Path).c_str());
        }
        else
        {
            String::Set(entry.Name, sizeof(entry.Name), s6Info->Name);
            // Normalise the name to make the scenario as recognisable as possible.
            ScenarioSources::NormaliseName(entry.Name, sizeof(entry.Name), entry.Name);
        }

        // entry.name will be translated later so keep the untranslated name here
        String::Set(entry.InternalName, sizeof(entry.InternalName), entry.Name);

        String::Set(entry.Details, sizeof(entry.Details), s6Info->Details);

        // Look up and store information regarding the origins of this scenario.
        SourceDescriptor desc;
        if (ScenarioSources::TryGetByName(entry.Name, &desc))
        {
            entry.ScenarioId = desc.id;
            entry.SourceIndex = desc.index;
            entry.SourceGame = ScenarioSource{ desc.source };
            entry.Category = desc.category;
        }
        else
        {
            entry.ScenarioId = SC_UNIDENTIFIED;
            entry.SourceIndex = -1;
            if (entry.Category == SCENARIO_CATEGORY_REAL)
            {
                entry.SourceGame = ScenarioSource::Real;
            }
            else
            {
                entry.SourceGame = ScenarioSource::Other;
            }
        }

        ScenarioTranslate(&entry);
        return entry;
    }
};

class ScenarioRepository final : public IScenarioRepository
{
private:
    static constexpr uint32_t HighscoreFileVersion = 2;

    std::shared_ptr<IPlatformEnvironment> const _env;
    ScenarioFileIndex const _fileIndex;
    std::vector<ScenarioIndexEntry> _scenarios;
    std::vector<ScenarioHighscoreEntry*> _highscores;

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

    const ScenarioIndexEntry* GetByIndex(size_t index) const override
    {
        const ScenarioIndexEntry* result = nullptr;
        if (index < _scenarios.size())
        {
            result = &_scenarios[index];
        }
        return result;
    }

    const ScenarioIndexEntry* GetByFilename(u8string_view filename) const override
    {
        for (const auto& scenario : _scenarios)
        {
            const auto scenarioFilename = Path::GetFileName(scenario.Path);

            // Note: this is always case insensitive search for cross platform consistency
            if (String::Equals(filename, scenarioFilename, true))
            {
                return &scenario;
            }
        }
        return nullptr;
    }

    const ScenarioIndexEntry* GetByInternalName(const utf8* name) const override
    {
        for (size_t i = 0; i < _scenarios.size(); i++)
        {
            const ScenarioIndexEntry* scenario = &_scenarios[i];

            if (scenario->SourceGame == ScenarioSource::Other && scenario->ScenarioId == SC_UNIDENTIFIED)
                continue;

            // Note: this is always case insensitive search for cross platform consistency
            if (String::Equals(name, scenario->InternalName, true))
            {
                return &_scenarios[i];
            }
        }
        return nullptr;
    }

    const ScenarioIndexEntry* GetByPath(const utf8* path) const override
    {
        for (const auto& scenario : _scenarios)
        {
            if (Path::Equals(path, scenario.Path))
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

        ScenarioIndexEntry* scenario = GetByFilename(scenarioFileName);

        if (scenario == nullptr)
        {
            const std::string scenarioBaseName = Path::GetFileNameWithoutExtension(scenarioFileName);
            const std::string scenarioExtension = Path::GetExtension(scenarioFileName);

            // Check if this is an RCTC scenario that corresponds to a known RCT1/2 scenario or vice versa, see #12626
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
            // gScenarioFileName .Park scenarios is the full file path instead of just <scenarioName.park>, so need to convert
            else if (String::Equals(scenarioExtension, ".park", true))
            {
                scenario = GetByFilename((scenarioBaseName + ".park").c_str());
            }
        }

        if (scenario != nullptr)
        {
            // Check if record company value has been broken or the highscore is the same but no name is registered
            ScenarioHighscoreEntry* highscore = scenario->Highscore;
            if (highscore == nullptr || companyValue > highscore->company_value
                || (highscore->name.empty() && companyValue == highscore->company_value))
            {
                if (highscore == nullptr)
                {
                    highscore = InsertHighscore();
                    highscore->timestamp = Platform::GetDatetimeNowUTC();
                    scenario->Highscore = highscore;
                }
                else
                {
                    if (!highscore->name.empty())
                    {
                        highscore->timestamp = Platform::GetDatetimeNowUTC();
                    }
                }
                highscore->fileName = Path::GetFileName(scenario->Path);
                highscore->name = name != nullptr ? name : "";
                highscore->company_value = companyValue;
                SaveHighscores();
                return true;
            }
        }
        return false;
    }

private:
    ScenarioIndexEntry* GetByFilename(u8string_view filename)
    {
        for (auto& scenario : _scenarios)
        {
            const auto scenarioFilename = Path::GetFileName(scenario.Path);

            // Note: this is always case insensitive search for cross platform consistency
            if (String::Equals(filename, scenarioFilename, true))
            {
                return &scenario;
            }
        }
        return nullptr;
    }

    ScenarioIndexEntry* GetByPath(const utf8* path)
    {
        const ScenarioRepository* repo = this;
        return const_cast<ScenarioIndexEntry*>(repo->GetByPath(path));
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
        Path::CreateDirectory(Path::GetDirectory(dstPath));

        auto mpdat = File::ReadAllBytes(srcPath);

        // Rotate each byte of mp.dat left by 4 bits to convert
        for (size_t i = 0; i < mpdat.size(); i++)
        {
            mpdat[i] = Numerics::rol8(mpdat[i], 4);
        }

        File::WriteAllBytes(dstPath, mpdat.data(), mpdat.size());
    }

    void AddScenario(const ScenarioIndexEntry& entry)
    {
        auto filename = Path::GetFileName(entry.Path);

        if (!String::Equals(filename, ""))
        {
            auto existingEntry = GetByFilename(filename.c_str());
            if (existingEntry != nullptr)
            {
                std::string conflictPath;
                if (existingEntry->Timestamp > entry.Timestamp)
                {
                    // Existing entry is more recent
                    conflictPath = String::ToStd(existingEntry->Path);

                    // Overwrite existing entry with this one
                    *existingEntry = entry;
                }
                else
                {
                    // This entry is more recent
                    conflictPath = entry.Path;
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
            LOG_ERROR("Tried to add scenario with an empty filename!");
        }
    }

    void Sort()
    {
        if (gConfigGeneral.ScenarioSelectMode == SCENARIO_SELECT_MODE_ORIGIN)
        {
            std::sort(
                _scenarios.begin(), _scenarios.end(), [](const ScenarioIndexEntry& a, const ScenarioIndexEntry& b) -> bool {
                    return ScenarioIndexEntryCompareByIndex(a, b) < 0;
                });
        }
        else
        {
            std::sort(
                _scenarios.begin(), _scenarios.end(), [](const ScenarioIndexEntry& a, const ScenarioIndexEntry& b) -> bool {
                    return ScenarioIndexEntryCompareByCategory(a, b) < 0;
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
                ScenarioHighscoreEntry* highscore = InsertHighscore();
                highscore->fileName = fs.ReadStdString();
                highscore->name = fs.ReadStdString();
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
                                std::string name = RCT2StringToUTF8(scBasic.CompletedBy, RCT2LanguageId::EnglishUK);
                                highscore->name = name;
                                highscore->company_value = scBasic.CompanyValue;
                                highscore->timestamp = DATETIME64_MIN;
                                break;
                            }
                        }
                    }
                    if (notFound)
                    {
                        ScenarioHighscoreEntry* highscore = InsertHighscore();
                        highscore->fileName = scBasic.Path;
                        std::string name = RCT2StringToUTF8(scBasic.CompletedBy, RCT2LanguageId::EnglishUK);
                        highscore->name = name;
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
            ScenarioHighscoreFree(highscore);
        }
        _highscores.clear();
    }

    ScenarioHighscoreEntry* InsertHighscore()
    {
        auto highscore = new ScenarioHighscoreEntry();
        _highscores.push_back(highscore);
        return highscore;
    }

    void AttachHighscores()
    {
        for (auto& highscore : _highscores)
        {
            ScenarioIndexEntry* scenario = GetByFilename(highscore->fileName);
            if (scenario != nullptr)
            {
                scenario->Highscore = highscore;
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
                const ScenarioHighscoreEntry* highscore = _highscores[i];
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

void ScenarioRepositoryScan()
{
    IScenarioRepository* repo = GetScenarioRepository();
    repo->Scan(LocalisationService_GetCurrentLanguage());
}

size_t ScenarioRepositoryGetCount()
{
    IScenarioRepository* repo = GetScenarioRepository();
    return repo->GetCount();
}

const ScenarioIndexEntry* ScenarioRepositoryGetByIndex(size_t index)
{
    IScenarioRepository* repo = GetScenarioRepository();
    return repo->GetByIndex(index);
}

bool ScenarioRepositoryTryRecordHighscore(const utf8* scenarioFileName, money64 companyValue, const utf8* name)
{
    IScenarioRepository* repo = GetScenarioRepository();
    return repo->TryRecordHighscore(LocalisationService_GetCurrentLanguage(), scenarioFileName, companyValue, name);
}
