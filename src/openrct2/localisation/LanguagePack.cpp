/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LanguagePack.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../core/FileStream.h"
#include "../core/Memory.hpp"
#include "../core/RTL.h"
#include "../core/String.hpp"
#include "../core/StringBuilder.h"
#include "../core/StringReader.h"
#include "Language.h"
#include "LocalisationService.h"
#include "StringIds.h"

#include <memory>
#include <string>
#include <vector>

using namespace OpenRCT2;

// Don't try to load more than language files that exceed 64 MiB
constexpr uint64_t MAX_LANGUAGE_SIZE = 64 * 1024 * 1024;

class LanguagePack final : public ILanguagePack
{
private:
    uint16_t const _id;
    std::vector<std::string> _strings;

public:
    static std::unique_ptr<LanguagePack> FromFile(uint16_t id, const utf8* path)
    {
        Guard::ArgumentNotNull(path);

        // Load file directly into memory
        u8string fileData;

        try
        {
            OpenRCT2::FileStream fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);

            size_t fileLength = static_cast<size_t>(fs.GetLength());
            if (fileLength > MAX_LANGUAGE_SIZE)
            {
                throw IOException("Language file too large.");
            }

            fileData.resize(fileLength);
            fs.Read(fileData.data(), fileLength);
        }
        catch (const std::exception& ex)
        {
            LOG_ERROR("Unable to open %s: %s", path, ex.what());
            return nullptr;
        }

        return FromText(id, fileData.data());
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
        if ((_strings.size() > static_cast<size_t>(stringId)) && !_strings[stringId].empty())
        {
            return _strings[stringId].c_str();
        }

        return nullptr;
    }

private:
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
        if (sscanf(identifier, "STR_%4d", &stringId) != 1)
        {
            // Ignore line entirely
            return;
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

        // Make sure the list is big enough to contain this string id
        if (static_cast<size_t>(stringId) >= _strings.size())
        {
            _strings.resize(stringId + 1);
        }
        _strings[stringId] = s;
    }
};

namespace OpenRCT2::LanguagePackFactory
{
    std::unique_ptr<ILanguagePack> FromFile(uint16_t id, const utf8* path)
    {
        auto languagePack = LanguagePack::FromFile(id, path);
        return languagePack;
    }

    std::unique_ptr<ILanguagePack> FromLanguageId(uint16_t id)
    {
        auto path = OpenRCT2::GetContext()->GetLocalisationService().GetLanguagePath(id);
        return LanguagePack::FromFile(id, path.c_str());
    }

    std::unique_ptr<ILanguagePack> FromText(uint16_t id, const utf8* text)
    {
        auto languagePack = LanguagePack::FromText(id, text);
        return languagePack;
    }
} // namespace OpenRCT2::LanguagePackFactory
