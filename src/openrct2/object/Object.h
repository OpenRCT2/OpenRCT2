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

#include "../common.h"
#include "ImageTable.h"
#include "StringTable.h"

extern "C"
{
    #include "../object.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
    OBJ_STRING_ID_DESCRIPTION,
    OBJ_STRING_ID_SCENARIO_NAME = 0,
    OBJ_STRING_ID_PARK_NAME = 1,
    OBJ_STRING_ID_SCENARIO_DETAILS = 2,
    OBJ_STRING_ID_CAPACITY = 2,
};

interface IStream;
struct    ObjectRepositoryItem;
struct    rct_drawpixelinfo;

interface IReadObjectContext
{
    virtual ~IReadObjectContext() { }

    virtual void LogWarning(uint32 code, const utf8 * text) abstract;
    virtual void LogError(uint32 code, const utf8 * text) abstract;
};

class Object
{
private:
    char *              _identifier;
    rct_object_entry    _objectEntry;
    StringTable         _stringTable;
    ImageTable          _imageTable;

protected:
    StringTable *       GetStringTable() { return &_stringTable; }
    const StringTable * GetStringTable() const { return &_stringTable; }
    ImageTable  *       GetImageTable() { return &_imageTable; }

    const utf8 *        GetOverrideString(uint8 index) const;
    const utf8 *        GetString(uint8 index) const;

public:
    explicit Object(const rct_object_entry &entry);
    virtual ~Object();

    // Legacy data structures
    const char *                GetIdentifier() const { return _identifier; }
    const rct_object_entry *    GetObjectEntry() const { return &_objectEntry; }
    virtual void *              GetLegacyData() abstract;

    virtual void ReadLegacy(IReadObjectContext * context, IStream * stream) abstract;
    virtual void Load() abstract;
    virtual void Unload() abstract;

    virtual void DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const { }

    virtual uint8           GetObjectType() const final { return _objectEntry.flags & 0x0F; }
    virtual const utf8 *    GetName() const;

    virtual void SetRepositoryItem(ObjectRepositoryItem * item) const { }
};

enum OBJECT_ERROR : uint32
{
    OBJECT_ERROR_OK,
    OBJECT_ERROR_UNKNOWN,
    OBJECT_ERROR_BAD_ENCODING,
    OBJECT_ERROR_INVALID_PROPERTY,
    OBJECT_ERROR_BAD_STRING_TABLE,
    OBJECT_ERROR_BAD_IMAGE_TABLE,
    OBJECT_ERROR_UNEXPECTED_EOF,
};
