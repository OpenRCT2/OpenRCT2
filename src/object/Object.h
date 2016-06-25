#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../common.h"
#include "ImageTable.h"
#include "StringTable.h"

extern "C"
{
    #include "../object.h"
}

interface IStream;

class Object
{
private:
    rct_object_entry    _objectEntry;

protected:
    StringTable         StringTable;
    ImageTable          ImageTable;

public:
    explicit Object(const rct_object_entry &entry);
    virtual ~Object() { }

    // Legacy data structures
    const rct_object_entry *    GetObjectEntry() const { return &_objectEntry; }
    virtual void *              GetLegacyData() abstract;

    virtual void ReadLegacy(IStream * stream) abstract;
    virtual void Load() abstract;
    virtual void Unload() abstract;

    virtual uint8           GetObjectType() { return _objectEntry.flags & 0x0F; }
    virtual const utf8 *    GetName() abstract;
};
