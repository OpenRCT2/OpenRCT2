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

#include "../core/FileStream.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "EntranceObject.h"
#include "Object.h"
#include "ObjectFactory.h"

extern "C"
{
    #include "../object.h"
    #include "../util/sawyercoding.h"
}

namespace ObjectFactory
{
    Object * CreateObjectFromLegacyFile(utf8 * path)
    {
        Object * result = nullptr;

        SDL_RWops * file = SDL_RWFromFile(path, "rb");
        if (file != nullptr)
        {
            rct_object_entry entry;
            if (SDL_RWread(file, &entry, sizeof(entry), 1) == 1)
            {
                result = CreateObject(entry);
                if (result != nullptr)
                {
                    size_t bufferSize = 0x600000;
                    void * buffer = Memory::Allocate<void>(bufferSize);
                    bufferSize = sawyercoding_read_chunk_with_size(file, (uint8 *)buffer, bufferSize);
                    buffer = Memory::Reallocate(buffer, bufferSize);
                    auto ms = MemoryStream(buffer, bufferSize);
                    result->ReadLegacy(&ms);
                }
            }
            SDL_RWclose(file);
        }
        return result;
    }

    Object * CreateObject(const rct_object_entry &entry)
    {
        Object * result = nullptr;

        uint8 objectType = entry.flags & 0x0F;
        switch (objectType) {
        case OBJECT_TYPE_PARK_ENTRANCE:
            result = new EntranceObject(entry);
            break;
        }

        return result;
    }
}
