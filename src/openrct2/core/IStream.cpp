/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "IStream.hpp"

#include "String.hpp"

#include <vector>

namespace OpenRCT2
{
    std::string IStream::ReadStdString()
    {
        std::string result;
        uint8_t ch;
        while ((ch = ReadValue<uint8_t>()) != 0)
        {
            result.push_back(ch);
        }
        return result;
    }

    void IStream::WriteString(const utf8* str)
    {
        if (str == nullptr)
        {
            WriteValue<uint8_t>(0);
        }
        else
        {
            size_t numBytes = String::sizeOf(str) + 1;
            Write(str, numBytes);
        }
    }

    void IStream::WriteString(const std::string_view str)
    {
        for (const auto c : str)
        {
            if (c == '\0')
                break;
            WriteValue<uint8_t>(c);
        }
        WriteValue<uint8_t>(0);
    }

    void IStream::WriteString(const std::string& str)
    {
        WriteString(str.c_str());
    }
} // namespace OpenRCT2
