/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <vector>
#include "IStream.hpp"
#include "Memory.hpp"
#include "String.hpp"

utf8 * IStream::ReadString()
{
    std::vector<utf8> result;

    uint8_t ch;
    while ((ch = ReadValue<uint8_t>()) != 0)
    {
        result.push_back(ch);
    }
    result.push_back(0);

    utf8 * resultString = Memory::AllocateArray<utf8>(result.size());
    std::copy(result.begin(), result.end(), resultString);
    return resultString;
}

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

void IStream::WriteString(const utf8 * str)
{
    if (str == nullptr)
    {
        WriteValue<uint8_t>(0);
    }
    else
    {
        size_t numBytes = String::SizeOf(str) + 1;
        Write(str, numBytes);
    }
}

void IStream::WriteString(const std::string &str)
{
    WriteString(str.c_str());
}
