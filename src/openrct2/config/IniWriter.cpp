/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "IniWriter.hpp"

#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../platform/Platform.h"

#include <sstream>

using namespace OpenRCT2;

class IniWriter final : public IIniWriter
{
private:
    OpenRCT2::IStream* _stream;
    bool _firstSection = true;

public:
    explicit IniWriter(OpenRCT2::IStream* stream)
        : _stream(stream)
    {
    }

    void WriteSection(const std::string& name) override
    {
        if (!_firstSection)
        {
            WriteLine();
        }
        _firstSection = false;

        WriteLine("[" + name + "]");
    }

    void WriteBoolean(const std::string& name, bool value) override
    {
        WriteProperty(name, value ? "true" : "false");
    }

    void WriteInt32(const std::string& name, int32_t value) override
    {
        WriteProperty(name, std::to_string(value));
    }

    void WriteInt64(const std::string& name, int64_t value) override
    {
        WriteProperty(name, std::to_string(value));
    }

    void WriteFloat(const std::string& name, float value) override
    {
        WriteProperty(name, std::to_string(value));
    }

    void WriteString(const std::string& name, const std::string& value) override
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

    void WriteEnum(const std::string& name, const std::string& key) override
    {
        WriteProperty(name, key);
    }

private:
    void WriteProperty(const std::string& name, const std::string& value)
    {
        WriteLine(name + " = " + value);
    }

    void WriteLine()
    {
        _stream->Write(PLATFORM_NEWLINE, String::sizeOf(PLATFORM_NEWLINE));
    }

    void WriteLine(const std::string& line)
    {
        _stream->Write(line.c_str(), line.size());
        WriteLine();
    }
};

void IIniWriter::WriteString(const std::string& name, const utf8* value)
{
    WriteString(name, String::toStd(value));
}

std::unique_ptr<IIniWriter> CreateIniWriter(OpenRCT2::IStream* stream)
{
    return std::make_unique<IniWriter>(stream);
}
