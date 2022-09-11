/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

    void WriteSection(std::string_view name) override
    {
        if (!_firstSection)
        {
            WriteLine();
        }
        _firstSection = false;

        WriteLine("[" + std::string{ name } + "]");
    }

    void WriteBoolean(std::string_view name, bool value) override
    {
        WriteProperty(name, value ? "true" : "false");
    }

    void WriteInt32(std::string_view name, int32_t value) override
    {
        WriteProperty(name, std::to_string(value));
    }

    void WriteInt64(std::string_view name, int64_t value) override
    {
        WriteProperty(name, std::to_string(value));
    }

    void WriteFloat(std::string_view name, float value) override
    {
        WriteProperty(name, std::to_string(value));
    }

    void WriteString(std::string_view name, std::string_view value) override
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

    void WriteEnum(std::string_view name, std::string_view key) override
    {
        WriteProperty(name, key);
    }

private:
    void WriteProperty(std::string_view name, std::string_view value)
    {
        WriteLine(std::string{ name } + " = " + std::string{ value });
    }

    void WriteLine()
    {
        _stream->Write(PLATFORM_NEWLINE, String::SizeOf(PLATFORM_NEWLINE));
    }

    void WriteLine(std::string_view line)
    {
        _stream->Write(line.data(), line.size());
        WriteLine();
    }
};

std::unique_ptr<IIniWriter> CreateIniWriter(OpenRCT2::IStream* stream)
{
    return std::make_unique<IniWriter>(stream);
}
