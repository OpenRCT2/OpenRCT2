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

#pragma once

#include <string>
#include <vector>
#include "../common.h"

interface IReadObjectContext;
interface IStream;

struct StringTableEntry
{
    uint8       Id;
    uint8       LanguageId;
    std::string Text;
};

class StringTable
{
private:
    std::vector<StringTableEntry> _strings;

public:
    void            Read(IReadObjectContext * context, IStream * stream, uint8 id);
    void            Sort();
    std::string     GetString(uint8 id) const;
    void            SetString(uint8 id, uint8 language, const std::string &text);
};
