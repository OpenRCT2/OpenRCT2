#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include <cctype>
#include <initializer_list>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "../common.h"
#include "../core/FileStream.hpp"
#include "../core/String.hpp"
#include "../core/StringBuilder.hpp"
#include "IniReader.hpp"

/**
 * Simple tuple (start, length) representing a text span in a buffer.
 */
struct Span
{
    size_t Start    = 0;
    size_t Length   = 0;

    Span() = default;
    Span(size_t start, size_t length)
        : Start(start),
          Length(length)
    {
    }
};

/**
 * Simple tuple (start, end) inclusive representing a range of lines.
 */
struct LineRange
{
    size_t Start    = 0;
    size_t End      = 0;

    LineRange() = default;
    LineRange(size_t start, size_t end)
        : Start(start),
          End(end)
    {
    }
};

struct StringIHash
{
    std::size_t operator()(const std::string &s) const
    {
        typedef std::char_traits<char> Traits;
        std::size_t seed = 0;
        for (const char &c : s)
        {
            const Traits::int_type value = std::toupper(Traits::to_int_type(c));
            // Simple Hash Combine as used by Boost.Functional/Hash
            seed ^= value + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
    }
};

struct StringICmp
{
    bool operator()(const std::string &a, const std::string &b) const
    {
        typedef std::char_traits<char> Traits;
        if (a.size() != b.size()) return false;
        const char *s1 = a.data(), *s2 = b.data();
        for (std::size_t i = a.size(); i > 0; --i, ++s1, ++s2)
        {
            const int c1 = std::toupper(Traits::to_int_type(*s1));
            const int c2 = std::toupper(Traits::to_int_type(*s2));
            if (c1 != c2) return false;
        }
        return true;
    }
};

class IniReader final : public IIniReader
{
private:
    std::vector<uint8>                                                      _buffer;
    std::vector<Span>                                                       _lines;
    std::unordered_map<std::string, LineRange, StringIHash, StringICmp>     _sections;
    std::unordered_map<std::string, std::string, StringIHash, StringICmp>   _values;

public:
    IniReader(IStream * stream)
    {
        uint64 length = stream->GetLength() - stream->GetPosition();
        _buffer.resize(length);
        stream->Read(_buffer.data(), length);

        RemoveBOM();

        // Ensure there is a null terminator on the end, this is
        // mainly for ParseLines's sake
        if (_buffer.empty() || _buffer[length - 1] != 0)
        {
            _buffer.push_back(0);
        }

        ParseLines();
        ParseSections();
    }

    bool ReadSection(const std::string &name) override
    {
        auto it = _sections.find(name);
        if (it == _sections.end())
        {
            return false;
        }

        ParseSectionValues(it->second);
        return true;
    }

    bool GetBoolean(const std::string &name, bool defaultValue) const override
    {
        bool result = defaultValue;
        std::string value;
        if (TryGetString(name, &value))
        {
            result = String::Equals(value, "true", true);
        }
        return result;
    }

    sint32 GetSint32(const std::string &name, sint32 defaultValue) const override
    {
        sint32 result = defaultValue;
        std::string value;
        if (TryGetString(name, &value))
        {
            try
            {
                result = std::stoi(value);
            }
            catch (std::exception)
            {
            }
        }
        return result;
    }

    float GetFloat(const std::string &name, float defaultValue) const override
    {
        float result = defaultValue;
        std::string value;
        if (TryGetString(name, &value))
        {
            try
            {
                result = std::stof(value);
            }
            catch (std::exception)
            {
            }
        }
        return result;
    }

    std::string GetString(const std::string &name, const std::string &defaultValue) const override
    {
        std::string result;
        if (!TryGetString(name, &result))
        {
            result = defaultValue;
        }
        return result;
    }

    bool TryGetString(const std::string &name, std::string * outValue) const override
    {
        auto it = _values.find(name);
        if (it == _values.end())
        {
            return false;
        }

        *outValue = it->second;
        return true;
    }

private:
    void RemoveBOM()
    {
        if (_buffer.size() < 3)
        {
            return;
        }
        utf8 * file = (utf8 *)_buffer.data();
        utf8 * content = String::SkipBOM(file);
        if (file != content)
        {
            size_t skipLength = content - file;
            _buffer.erase(_buffer.begin(), _buffer.begin() + skipLength);
        }
    }

    void ParseLines()
    {
        size_t lineBegin = 0;
        bool onNewLineCh = false;
        for (size_t i = 0; i < _buffer.size(); i++)
        {
            char b = (char)_buffer[i];
            if (b == 0 || b == '\n' || b == '\r')
            {
                if (!onNewLineCh)
                {
                    onNewLineCh = true;
                    size_t lineEnd = i;
                    _lines.emplace_back(lineBegin, lineEnd - lineBegin);
                }
            }
            else if (onNewLineCh)
            {
                onNewLineCh = false;
                lineBegin = i;
            }
        }
    }

    void ParseSections()
    {
        std::string sectionName;
        LineRange lineRange;

        for (size_t i = 0; i < _lines.size(); i++)
        {
            std::string line = GetLine(i);
            line = String::Trim(line);
            if (line.size() > 3 && line[0] == '[')
            {
                size_t endIndex = line.find_first_of(']');
                if (endIndex != std::string::npos)
                {
                    // Add last section
                    if (!sectionName.empty())
                    {
                        lineRange.End = i - 1;
                        _sections[sectionName] = lineRange;
                    }

                    // Set up new section
                    sectionName = line.substr(1, endIndex - 1);
                    lineRange.Start = i;
                }
            }
        }

        // Add final section
        if (!sectionName.empty())
        {
            lineRange.End = _lines.size() - 1;
            _sections[sectionName] = lineRange;
        }
    }

    void ParseSectionValues(LineRange range)
    {
        for (size_t i = range.Start + 1; i <= range.End; i++)
        {
            ParseValue(i);
        }
    }

    void ParseValue(size_t lineIndex)
    {
        std::string line = GetLine(lineIndex);
        line = TrimComment(line);

        // Find assignment character
        size_t equalsIndex = line.find_first_of('=');
        if (equalsIndex == std::string::npos)
        {
            return;
        }

        // Get the key and value
        std::string key = String::Trim(line.substr(0, equalsIndex));
        std::string value = String::Trim(line.substr(equalsIndex + 1));

        value = UnquoteValue(value);
        value = UnescapeValue(value);
        _values[key] = value;
    }

    std::string TrimComment(const std::string &s)
    {
        char inQuotes = 0;
        bool escaped = false;
        for (size_t i = 0; i < s.size(); i++)
        {
            char c = s[i];
            if (inQuotes == 0 && c == '#' && !escaped)
            {
                return s.substr(0, i);
            }
            else if (c == inQuotes && !escaped)
            {
                inQuotes = 0;
            }
            else if ((c == '\'' || c == '"') && !escaped)
            {
                inQuotes = c;
            }
            escaped = (c == '\\' && !escaped);
        }
        return s;
    }

    std::string UnquoteValue(const std::string &s)
    {
        std::string result = s;
        size_t length = s.size();
        if (length >= 2)
        {
            if ((s[0] == '"' || s[0] == '\'') && s[0] == s[length - 1])
            {
                result = s.substr(1, length - 2);
            }
        }
        return result;
    }

    std::string UnescapeValue(const std::string &s)
    {
        if (s.find_first_of('\\') == std::string::npos)
        {
            return s;
        }

        bool escaped = false;
        auto sb = StringBuilder();
        for (char c : s)
        {
            if (c == '\\' && !escaped)
            {
                escaped = true;
            }
            else
            {
                escaped = false;
                sb.Append(&c, 1);
            }
        }
        return std::string(sb.GetString());
    }

    std::string GetLine(size_t index)
    {
        utf8 * szBuffer = (utf8 *)_buffer.data();
        auto span = _lines[index];
        auto line = std::string(szBuffer + span.Start, span.Length);
        return line;
    }
};

class DefaultIniReader final : public IIniReader
{
public:
    bool ReadSection(const std::string &name) override
    {
        return true;
    }

    bool GetBoolean(const std::string &name, bool defaultValue) const override
    {
        return defaultValue;
    }

    sint32 GetSint32(const std::string &name, sint32 defaultValue) const override
    {
        return defaultValue;
    }

    float GetFloat(const std::string &name, float defaultValue) const override
    {
        return defaultValue;
    }

    std::string GetString(const std::string &name, const std::string &defaultValue) const override
    {
        return defaultValue;
    }

    bool TryGetString(const std::string &name, std::string * outValue) const override
    {
        return false;
    }
};

utf8 * IIniReader::GetCString(const std::string &name, const utf8 * defaultValue) const
{
    std::string szValue;
    if (!TryGetString(name, &szValue))
    {
        return String::Duplicate(defaultValue);
    }

    return String::Duplicate(szValue.c_str());
}

IIniReader * CreateIniReader(IStream * stream)
{
    return new IniReader(stream);
}

IIniReader * CreateDefaultIniReader()
{
    return new DefaultIniReader();
}
