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

#include "../common.h"
#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "ObjectCache.h"

extern "C"
{
    #include "../object.h"
}

struct ObjectCacheEntry
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

    void Dispose()
    {
        Memory::Free(RequiredObjects);
        Memory::Free(ThemeObjects);
        RequiredObjects = nullptr;
        ThemeObjects = nullptr;
    }

    void Read(IStream * stream)
    {
        Guard::Assert(RequiredObjects == nullptr);
        Guard::Assert(ThemeObjects == nullptr);

        ObjectEntry = stream->ReadValue<rct_object_entry_extended>();
        Path = stream->ReadString();
        NumImages = stream->ReadValue<uint32>();
        Name = stream->ReadString();
        ChunkSize = stream->ReadValue<size_t>();
        NumRequiredObjects = stream->ReadValue<uint16>();
            
        RequiredObjects = Memory::AllocateArray<rct_object_entry_extended>(NumRequiredObjects);
        for (uint16 i = 0; i < NumRequiredObjects; i++)
        {
            RequiredObjects[i] = stream->ReadValue<rct_object_entry_extended>();
        }

        switch (ObjectEntry.flags & 0x0F) {
        case OBJECT_TYPE_RIDE:
            RideFlags = stream->ReadValue<uint8>();
            for (int i = 0; i < 2; i++)
            {
                RideCategory[i] = stream->ReadValue<uint8>();
            }
            for (int i = 0; i < 3; i++)
            {
                RideType[i] = stream->ReadValue<uint8>();
            }
            break;
        case OBJECT_TYPE_SCENERY_SETS:
            NumThemeObjects = stream->ReadValue<uint16>();
            for (uint16 i = 0; i < NumThemeObjects; i++)
            {
                ThemeObjects[i] = stream->ReadValue<rct_object_entry_extended>();
            }
            break;
        }
    }

    void Write(IStream * stream)
    {
        stream->WriteValue(ObjectEntry);
        stream->WriteString(Path);
        stream->WriteValue(NumImages);
        stream->WriteString(Name);
        stream->WriteValue(ChunkSize);
            
        stream->WriteValue(NumRequiredObjects);
        for (uint16 i = 0; i < NumRequiredObjects; i++)
        {
            stream->WriteValue(RequiredObjects[i]);
        }

        switch (ObjectEntry.flags & 0x0F) {
        case OBJECT_TYPE_RIDE:
            stream->WriteValue<uint8>(RideFlags);
            for (int i = 0; i < 2; i++)
            {
                stream->WriteValue<uint8>(RideCategory[i]);
            }
            for (int i = 0; i < 3; i++)
            {
                stream->WriteValue<uint8>(RideType[i]);
            }
            break;
        case OBJECT_TYPE_SCENERY_SETS:
            stream->WriteValue<uint16>(NumThemeObjects);
            for (uint16 i = 0; i < NumThemeObjects; i++)
            {
                stream->WriteValue<rct_object_entry_extended>(ThemeObjects[i]);
            }
            break;
        }
    }
};

class ObjectCache : IObjectCache
{
    void Load() override
    {
        // TODO
    }

    void Save() override
    {
        // TODO
    }
};
