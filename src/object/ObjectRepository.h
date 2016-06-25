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

extern "C"
{
    #include "../object.h"
}

struct ObjectRepositoryItem
{
    rct_object_entry_extended   ObjectEntry;
    utf8 *                      Path;
    uint32                      NumImages;
    utf8 *                      Name;
    size_t                      ChunkSize;
    uint16                      NumRequiredObjects;
    rct_object_entry_extended * RequiredObjects;
    union
    {
        struct
        {
            uint16                      NumThemeObjects;
            rct_object_entry_extended * ThemeObjects;
        };
        struct
        {
            uint8   RideFlags;
            uint8   RideCategory[2];
            uint8   RideType[3];
        };
    };
};

interface IObjectRepository
{
    virtual ~IObjectRepository() { }
    
    virtual const ObjectRepositoryItem *    FindObject(const rct_object_entry * objectEntry) abstract;
    virtual Object *                        LoadObject(const rct_object_entry * objectEntry) abstract;
};

IObjectRepository * GetObjectRepository();
