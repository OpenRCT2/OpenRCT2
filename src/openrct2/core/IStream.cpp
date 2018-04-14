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

#include <vector>
#include "IStream.hpp"
#include "Memory.hpp"
#include "String.hpp"

utf8 * IStream::ReadString()
{
    std::vector<utf8> result;

    uint8 ch;
    while ((ch = ReadValue<uint8>()) != 0)
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
    uint8 ch;
    while ((ch = ReadValue<uint8>()) != 0)
    {
        result.push_back(ch);
    }
    return result;
}

void IStream::WriteString(const utf8 * str)
{
    if (str == nullptr)
    {
        WriteValue<uint8>(0);
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
