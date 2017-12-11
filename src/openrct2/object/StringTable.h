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
#include "../localisation/Language.h"

interface IReadObjectContext;
interface IStream;

enum OBJ_STRING_ID : uint8
{
    OBJ_STRING_ID_UNKNOWN = 255,
    OBJ_STRING_ID_NAME = 0,
    OBJ_STRING_ID_DESCRIPTION,
    OBJ_STRING_ID_SCENARIO_NAME = 0,
    OBJ_STRING_ID_PARK_NAME = 1,
    OBJ_STRING_ID_SCENARIO_DETAILS = 2,
    OBJ_STRING_ID_CAPACITY = 2,
    OBJ_STRING_ID_VEHICLE_NAME = 3,
};

struct StringTableEntry
{
    uint8       Id          = OBJ_STRING_ID_UNKNOWN;
    uint8       LanguageId  = LANGUAGE_UNDEFINED;
    std::string Text;
};

class StringTable
{
private:
    std::vector<StringTableEntry> _strings;

public:
    StringTable() = default;
    StringTable(const StringTable &) = delete;
    StringTable & operator=(const StringTable &) = delete;

    void            Read(IReadObjectContext * context, IStream * stream, uint8 id);
    void            Sort();
    std::string     GetString(uint8 id) const;
    void            SetString(uint8 id, uint8 language, const std::string &text);
};
