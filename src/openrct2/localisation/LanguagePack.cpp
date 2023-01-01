/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LanguagePack.h"

#include "../common.h"
#include "../core/FileStream.h"
#include "../core/Memory.hpp"
#include "../core/RTL.h"
#include "../core/String.hpp"
#include "../core/StringBuilder.h"
#include "../core/StringReader.h"
#include "Language.h"
#include "Localisation.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

// Don't try to load more than language files that exceed 64 MiB
constexpr uint64_t MAX_LANGUAGE_SIZE = 64 * 1024 * 1024;
constexpr uint64_t MAX_OBJECT_OVERRIDES = 4096;
constexpr uint64_t MAX_SCENARIO_OVERRIDES = 4096;

constexpr StringId ObjectOverrideBase = 0x6000;
constexpr int32_t ObjectOverrideMaxStringCount = 3;

constexpr StringId ScenarioOverrideBase = 0x7000;
constexpr int32_t ScenarioOverrideMaxStringCount = 3;

struct ObjectOverride
{
    char name[8] = { 0 };
    std::string strings[ObjectOverrideMaxStringCount];
};

struct ScenarioOverride
{
    std::string filename;
    std::string strings[ScenarioOverrideMaxStringCount];
};

class LanguagePack final : public ILanguagePack
{
private:
    uint16_t const _id;
    std::vector<std::string> _strings;
    std::vector<ObjectOverride> _objectOverrides;
    std::vector<ScenarioOverride> _scenarioOverrides;

    ///////////////////////////////////////////////////////////////////////////
    // Parsing work data
    ///////////////////////////////////////////////////////////////////////////
    std::string _currentGroup;
    ObjectOverride* _currentObjectOverride = nullptr;
    ScenarioOverride* _currentScenarioOverride = nullptr;

public:
    static std::unique_ptr<LanguagePack> FromFile(uint16_t id, const utf8* path)
    {
        Guard::ArgumentNotNull(path);

        // Load file directly into memory
        utf8* fileData = nullptr;
        try
        {
            OpenRCT2::FileStream fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);

            size_t fileLength = static_cast<size_t>(fs.GetLength());
            if (fileLength > MAX_LANGUAGE_SIZE)
            {
                throw IOException("Language file too large.");
            }

            fileData = Memory::Allocate<utf8>(fileLength + 1);
            fs.Read(fileData, fileLength);
            fileData[fileLength] = '\0';
        }
        catch (const std::exception& ex)
        {
            Memory::Free(fileData);
            log_error("Unable to open %s: %s", path, ex.what());
            return nullptr;
        }

        // Parse the memory as text
        auto result = FromText(id, fileData);

        Memory::Free(fileData);
        return result;
    }

    static std::unique_ptr<LanguagePack> FromText(uint16_t id, const utf8* text)
    {
        return std::make_unique<LanguagePack>(id, text);
    }

    LanguagePack(uint16_t id, const utf8* text)
        : _id(id)
    {
        Guard::ArgumentNotNull(text);

        auto reader = UTF8StringReader(text);
        while (reader.CanRead())
        {
            ParseLine(&reader);
        }

        // Clean up the parsing work data
        _currentGroup.clear();
        _currentObjectOverride = nullptr;
        _currentScenarioOverride = nullptr;
    }

    uint16_t GetId() const override
    {
        return _id;
    }

    uint32_t GetCount() const override
    {
        return static_cast<uint32_t>(_strings.size());
    }

    void RemoveString(StringId stringId) override
    {
        if (_strings.size() > static_cast<size_t>(stringId))
        {
            _strings[stringId].clear();
        }
    }

    void SetString(StringId stringId, const std::string& str) override
    {
        if (_strings.size() > static_cast<size_t>(stringId))
        {
            _strings[stringId] = str;
        }
    }

    const utf8* GetString(StringId stringId) const override
    {
        if (stringId >= ScenarioOverrideBase)
        {
            int32_t offset = stringId - ScenarioOverrideBase;
            int32_t ooIndex = offset / ScenarioOverrideMaxStringCount;
            int32_t ooStringIndex = offset % ScenarioOverrideMaxStringCount;

            if (_scenarioOverrides.size() > static_cast<size_t>(ooIndex)
                && !_scenarioOverrides[ooIndex].strings[ooStringIndex].empty())
            {
                return _scenarioOverrides[ooIndex].strings[ooStringIndex].c_str();
            }

            return nullptr;
        }

        if (stringId >= ObjectOverrideBase)
        {
            int32_t offset = stringId - ObjectOverrideBase;
            int32_t ooIndex = offset / ObjectOverrideMaxStringCount;
            int32_t ooStringIndex = offset % ObjectOverrideMaxStringCount;

            if (_objectOverrides.size() > static_cast<size_t>(ooIndex)
                && !_objectOverrides[ooIndex].strings[ooStringIndex].empty())
            {
                return _objectOverrides[ooIndex].strings[ooStringIndex].c_str();
            }

            return nullptr;
        }

        if ((_strings.size() > static_cast<size_t>(stringId)) && !_strings[stringId].empty())
        {
            return _strings[stringId].c_str();
        }

        return nullptr;
    }

    StringId GetObjectOverrideStringId(std::string_view legacyIdentifier, uint8_t index) override
    {
        Guard::Assert(index < ObjectOverrideMaxStringCount);

        int32_t ooIndex = 0;
        for (const ObjectOverride& objectOverride : _objectOverrides)
        {
            if (std::string_view(objectOverride.name, 8) == legacyIdentifier)
            {
                if (objectOverride.strings[index].empty())
                {
                    return STR_NONE;
                }
                return ObjectOverrideBase + (ooIndex * ObjectOverrideMaxStringCount) + index;
            }
            ooIndex++;
        }

        return STR_NONE;
    }

    StringId GetScenarioOverrideStringId(const utf8* scenarioFilename, uint8_t index) override
    {
        Guard::ArgumentNotNull(scenarioFilename);
        Guard::Assert(index < ScenarioOverrideMaxStringCount);

        int32_t ooIndex = 0;
        for (const ScenarioOverride& scenarioOverride : _scenarioOverrides)
        {
            if (String::Equals(scenarioOverride.filename.c_str(), scenarioFilename, true))
            {
                if (scenarioOverride.strings[index].empty())
                {
                    return STR_NONE;
                }
                return ScenarioOverrideBase + (ooIndex * ScenarioOverrideMaxStringCount) + index;
            }
            ooIndex++;
        }

        return STR_NONE;
    }

private:
    ObjectOverride* GetObjectOverride(const std::string& objectIdentifier)
    {
        for (auto& oo : _objectOverrides)
        {
            if (strncmp(oo.name, objectIdentifier.c_str(), 8) == 0)
            {
                return &oo;
            }
        }
        return nullptr;
    }

    ScenarioOverride* GetScenarioOverride(const std::string& scenarioIdentifier)
    {
        for (auto& so : _scenarioOverrides)
        {
            if (String::Equals(so.strings[0], scenarioIdentifier.c_str(), true))
            {
                return &so;
            }
        }
        return nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Parsing
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Partial support to open an uncompiled language file which parses tokens and converts them to the corresponding character
    // code. Due to resource strings (strings in scenarios and objects) being written to the original game's string table,
    // get_string will use those if the same entry in the loaded language is empty.
    //
    // Unsure at how the original game decides which entries to write resource strings to, but this could affect adding new
    // strings for the time being. Further investigation is required.
    //
    // When reading the language files, the STR_XXXX part is read and XXXX becomes the string id number. Everything after the
    // colon and before the new line will be saved as the string. Tokens are written with inside curly braces {TOKEN}. Use # at
    // the beginning of a line to leave a comment.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static bool IsWhitespace(codepoint_t codepoint)
    {
        return codepoint == '\t' || codepoint == ' ' || codepoint == '\r' || codepoint == '\n';
    }

    static bool IsNewLine(codepoint_t codepoint)
    {
        return codepoint == '\r' || codepoint == '\n';
    }

    static void SkipWhitespace(IStringReader* reader)
    {
        codepoint_t codepoint;
        while (reader->TryPeek(&codepoint))
        {
            if (IsWhitespace(codepoint))
            {
                reader->Skip();
            }
            else
            {
                break;
            }
        }
    }

    static void SkipNewLine(IStringReader* reader)
    {
        codepoint_t codepoint;
        while (reader->TryPeek(&codepoint))
        {
            if (IsNewLine(codepoint))
            {
                reader->Skip();
            }
            else
            {
                break;
            }
        }
    }

    static void SkipToEndOfLine(IStringReader* reader)
    {
        codepoint_t codepoint;
        while (reader->TryPeek(&codepoint))
        {
            if (codepoint != '\r' && codepoint != '\n')
            {
                reader->Skip();
            }
            else
            {
                break;
            }
        }
    }

    void ParseLine(IStringReader* reader)
    {
        SkipWhitespace(reader);

        codepoint_t codepoint;
        if (reader->TryPeek(&codepoint))
        {
            switch (codepoint)
            {
                case '#':
                    SkipToEndOfLine(reader);
                    break;
                case '[':
                    ParseGroupObject(reader);
                    break;
                case '<':
                    ParseGroupScenario(reader);
                    break;
                case '\r':
                case '\n':
                    break;
                default:
                    ParseString(reader);
                    break;
            }
            SkipToEndOfLine(reader);
            SkipNewLine(reader);
        }
    }

    void ParseGroupObject(IStringReader* reader)
    {
        auto sb = StringBuilder();
        codepoint_t codepoint;

        // Should have already deduced that the next codepoint is a [
        reader->Skip();

        // Read string up to ] or line end
        bool closedCorrectly = false;
        while (reader->TryPeek(&codepoint))
        {
            if (IsNewLine(codepoint))
                break;

            reader->Skip();
            if (codepoint == ']')
            {
                closedCorrectly = true;
                break;
            }
            sb.Append(codepoint);
        }

        if (closedCorrectly)
        {
            while (sb.GetLength() < 8)
            {
                sb.Append(' ');
            }
            if (sb.GetLength() == 8)
            {
                _currentGroup = sb.GetStdString();
                _currentObjectOverride = GetObjectOverride(_currentGroup);
                _currentScenarioOverride = nullptr;
                if (_currentObjectOverride == nullptr)
                {
                    if (_objectOverrides.size() == MAX_OBJECT_OVERRIDES)
                    {
                        log_warning("Maximum number of localised object strings exceeded.");
                    }

                    _objectOverrides.emplace_back();
                    _currentObjectOverride = &_objectOverrides[_objectOverrides.size() - 1];
                    std::copy_n(_currentGroup.c_str(), 8, _currentObjectOverride->name);
                }
            }
        }
    }

    void ParseGroupScenario(IStringReader* reader)
    {
        auto sb = StringBuilder();
        codepoint_t codepoint;

        // Should have already deduced that the next codepoint is a <
        reader->Skip();

        // Read string up to > or line end
        bool closedCorrectly = false;
        while (reader->TryPeek(&codepoint))
        {
            if (IsNewLine(codepoint))
                break;

            reader->Skip();
            if (codepoint == '>')
            {
                closedCorrectly = true;
                break;
            }
            sb.Append(codepoint);
        }

        if (closedCorrectly)
        {
            _currentGroup = sb.GetStdString();
            _currentObjectOverride = nullptr;
            _currentScenarioOverride = GetScenarioOverride(_currentGroup);
            if (_currentScenarioOverride == nullptr)
            {
                if (_scenarioOverrides.size() == MAX_SCENARIO_OVERRIDES)
                {
                    log_warning("Maximum number of scenario strings exceeded.");
                }

                _scenarioOverrides.emplace_back();
                _currentScenarioOverride = &_scenarioOverrides[_scenarioOverrides.size() - 1];
                _currentScenarioOverride->filename = std::string(sb.GetBuffer());
            }
        }
    }

    void ParseString(IStringReader* reader)
    {
        auto sb = StringBuilder();
        codepoint_t codepoint;

        // Parse string identifier
        while (reader->TryPeek(&codepoint))
        {
            if (IsNewLine(codepoint))
            {
                // Unexpected new line, ignore line entirely
                return;
            }

            if (!IsWhitespace(codepoint) && codepoint != ':')
            {
                reader->Skip();
                sb.Append(codepoint);
            }
            else
            {
                break;
            }
        }

        SkipWhitespace(reader);

        // Parse a colon
        if (!reader->TryPeek(&codepoint) || codepoint != ':')
        {
            // Expected a colon, ignore line entirely
            return;
        }
        reader->Skip();

        // Validate identifier
        const utf8* identifier = sb.GetBuffer();

        int32_t stringId;
        if (_currentGroup.empty())
        {
            if (sscanf(identifier, "STR_%4d", &stringId) != 1)
            {
                // Ignore line entirely
                return;
            }
        }
        else
        {
            if (String::Equals(identifier, "STR_NAME"))
            {
                stringId = 0;
            }
            else if (String::Equals(identifier, "STR_DESC"))
            {
                stringId = 1;
            }
            else if (String::Equals(identifier, "STR_CPTY"))
            {
                stringId = 2;
            }

            else if (String::Equals(identifier, "STR_SCNR"))
            {
                stringId = 0;
            }
            else if (String::Equals(identifier, "STR_PARK"))
            {
                stringId = 1;
            }
            else if (String::Equals(identifier, "STR_DTLS"))
            {
                stringId = 2;
            }
            else
            {
                // Ignore line entirely
                return;
            }
        }

        // Rest of the line is the actual string
        sb.Clear();
        while (reader->TryPeek(&codepoint) && !IsNewLine(codepoint))
        {
            reader->Skip();
            sb.Append(codepoint);
        }

        std::string s;
        if (LanguagesDescriptors[_id].isRtl)
        {
            auto ts = std::string(sb.GetBuffer(), sb.GetLength());
            s = FixRTL(ts);
        }
        else
        {
            s = std::string(sb.GetBuffer(), sb.GetLength());
        }

        if (_currentGroup.empty())
        {
            // Make sure the list is big enough to contain this string id
            if (static_cast<size_t>(stringId) >= _strings.size())
            {
                _strings.resize(stringId + 1);
            }
            _strings[stringId] = s;
        }
        else
        {
            if (_currentObjectOverride != nullptr)
            {
                _currentObjectOverride->strings[stringId] = s;
            }
            else
            {
                _currentScenarioOverride->strings[stringId] = std::move(s);
            }
        }
    }
};

namespace LanguagePackFactory
{
    std::unique_ptr<ILanguagePack> FromFile(uint16_t id, const utf8* path)
    {
        auto languagePack = LanguagePack::FromFile(id, path);
        return languagePack;
    }

    std::unique_ptr<ILanguagePack> FromText(uint16_t id, const utf8* text)
    {
        auto languagePack = LanguagePack::FromText(id, text);
        return languagePack;
    }
} // namespace LanguagePackFactory
