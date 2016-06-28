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

#include "../core/Console.hpp"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "ImageTable.h"

ImageTable::~ImageTable()
{
    Memory::Free(_data);
    _data = nullptr;
    _dataSize = 0;
}

void ImageTable::Read(IStream * stream)
{
    uint32 numImages = stream->ReadValue<uint32>();
    uint32 imageDataSize = stream->ReadValue<uint32>();

    _dataSize = imageDataSize;
    _data = Memory::Reallocate(_data, _dataSize);

    // Read g1 element headers
    uintptr_t imageDataBase = (uintptr_t)_data;
    for (uint32 i = 0; i < numImages; i++)
    {
        rct_g1_element g1Element;

        uintptr_t imageDataOffset = (uintptr_t)stream->ReadValue<uint32>();
        g1Element.offset = (uint8*)(imageDataBase + imageDataOffset);

        g1Element.width = stream->ReadValue<sint16>();
        g1Element.height = stream->ReadValue<sint16>();
        g1Element.x_offset = stream->ReadValue<sint16>();
        g1Element.y_offset = stream->ReadValue<sint16>();
        g1Element.flags = stream->ReadValue<uint16>();
        g1Element.zoomed_offset = stream->ReadValue<uint16>();

        _entries.push_back(g1Element);
    }

    // Read g1 element data
    size_t readBytes = (size_t)stream->TryRead(_data, _dataSize);

    // If data is shorter than expected (some custom objects are unfortunately like that)
    size_t unreadBytes = _dataSize - readBytes;
    if (unreadBytes > 0)
    {
        void * ptr = (void*)(((uintptr_t)_data) + readBytes);
        Memory::Set(ptr, 0, unreadBytes);

        Console::Error::WriteFormat("Warning: Image table size shorter than expected.");
        Console::Error::WriteLine();
    }
}
