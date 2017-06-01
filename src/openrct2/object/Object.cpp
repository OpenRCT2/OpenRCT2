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

#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "Object.h"

extern "C"
{
    #include "../localisation/localisation.h"
}

Object::Object(const rct_object_entry &entry)
{
    _objectEntry = entry;

    char name[9] = { 0 };
    Memory::Copy(name, entry.name, 8);
    _identifier = String::Duplicate(name);
}

Object::~Object()
{
    Memory::Free(_identifier);
}

const utf8 * Object::GetOverrideString(uint8 index) const
{
    const char * identifier = GetIdentifier();
    rct_string_id stringId = language_get_object_override_string_id(identifier, index);

    const utf8 * result = nullptr;
    if (stringId != STR_NONE)
    {
        result = language_get_string(stringId);
    }
    return result;
}

const utf8 * Object::GetString(uint8 index) const
{
    const utf8 * sz = GetOverrideString(index);
    if (sz == nullptr)
    {
        sz = GetStringTable()->GetString(index);
    }
    return sz != nullptr ? sz : "";
}

const utf8 * Object::GetName() const
{
    return GetString(OBJ_STRING_ID_NAME);
}
