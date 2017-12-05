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

#include <algorithm>
#include <stdexcept>
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../OpenRCT2.h"
#include "ImageTable.h"
#include "Object.h"

ImageTable::~ImageTable()
{
    Memory::Free(_data);
    _data = nullptr;
    _dataSize = 0;
}

void ImageTable::Read(IReadObjectContext * context, IStream * stream)
{
    if (gOpenRCT2NoGraphics)
    {
        return;
    }

    try
    {
        uint32 numImages = stream->ReadValue<uint32>();
        uint32 imageDataSize = stream->ReadValue<uint32>();

        uint64 headerTableSize = numImages * 16;
        uint64 remainingBytes = stream->GetLength() - stream->GetPosition() - headerTableSize;
        if (remainingBytes > imageDataSize)
        {
            context->LogWarning(OBJECT_ERROR_BAD_IMAGE_TABLE, "Image table size longer than expected.");
            imageDataSize = (uint32)remainingBytes;
        }

        _dataSize = imageDataSize;
        _data = Memory::Reallocate(_data, _dataSize);
        if (_data == nullptr)
        {
            context->LogError(OBJECT_ERROR_BAD_IMAGE_TABLE, "Image table too large.");
            throw std::runtime_error("Image table too large.");
        }

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
            uint8 * ptr = (uint8*)(((uintptr_t)_data) + readBytes);
            std::fill_n(ptr, unreadBytes, 0);

            context->LogWarning(OBJECT_ERROR_BAD_IMAGE_TABLE, "Image table size shorter than expected.");
        }

        // TODO validate the image data to prevent crashes in-game
    }
    catch (const std::exception &)
    {
        context->LogError(OBJECT_ERROR_BAD_IMAGE_TABLE, "Bad image table.");
        throw;
    }
}

void ImageTable::AddImage(const rct_g1_element * g1)
{
    rct_g1_element newg1 = *g1;
    auto length = g1_calculate_data_size(g1);
    if (length == 0)
    {
        newg1.offset = 0;
    }
    else
    {
        auto dstOffset = _dataSize;
        _dataSize += length;
        _data = Memory::Reallocate(_data, _dataSize);
        auto dst = (uint8 *)((size_t)_data + dstOffset);
        Memory::Copy(dst, g1->offset, length);
        newg1.offset = dst;
    }
    _entries.push_back(newg1);
}
