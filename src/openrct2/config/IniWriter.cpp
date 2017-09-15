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

#include <sstream>
#include "../core/FileStream.hpp"
#include "IniWriter.hpp"

#include "../platform/platform.h"

class IniWriter final : public IIniWriter
{
private:
    IStream *   _stream;
    bool        _firstSection = true;

public:
    IniWriter(IStream * stream)
        : _stream(stream)
    {
    }

    void WriteSection(const std::string &name) override
    {
        if (!_firstSection)
        {
            WriteLine();
        }
        _firstSection = false;

        WriteLine("[" + name + "]");
    }

    void WriteBoolean(const std::string &name, bool value) override
    {
        WriteProperty(name, value ? "true" : "false");
    }

    void WriteSint32(const std::string &name, sint32 value) override
    {
        WriteProperty(name, std::to_string(value));
    }

    void WriteFloat(const std::string &name, float value) override
    {
        WriteProperty(name, std::to_string(value));
    }

    void WriteString(const std::string &name, const std::string &value) override
    {
        std::ostringstream buffer;
        buffer << '"';
        for (char c : value)
        {
            if (c == '\\' || c == '"')
            {
                buffer << '\\';
            }
            buffer << c;
        }
        buffer << '"';

        WriteProperty(name, buffer.str());
    }

    void WriteEnum(const std::string &name, const std::string &key) override
    {
        WriteProperty(name, key);
    }

private:
    void WriteProperty(const std::string &name, const std::string &value)
    {
        WriteLine(name + " = " + value);
    }

    void WriteLine()
    {
        _stream->Write(PLATFORM_NEWLINE, String::SizeOf(PLATFORM_NEWLINE));
    }

    void WriteLine(const std::string &line)
    {
        _stream->Write(line.c_str(), line.size());
        WriteLine();
    }
};

void IIniWriter::WriteString(const std::string &name, const utf8 * value)
{
    WriteString(name, String::ToStd(value));
}

IIniWriter * CreateIniWriter(IStream * stream)
{
    return new IniWriter(stream);
}
