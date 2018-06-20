/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <memory>
#include <stdexcept>
#include "../core/IStream.hpp"
#include "../OpenRCT2.h"
#include "ImageTable.h"
#include "Object.h"

ImageTable::~ImageTable()
{
    if (_data == nullptr)
    {
        for (auto &entry : _entries)
        {
            delete [] entry.offset;
        }
    }
}

void ImageTable::Read(IReadObjectContext * context, IStream * stream)
{
    if (gOpenRCT2NoGraphics)
    {
        return;
    }

    try
    {
        uint32_t numImages = stream->ReadValue<uint32_t>();
        uint32_t imageDataSize = stream->ReadValue<uint32_t>();

        uint64_t headerTableSize = numImages * 16;
        uint64_t remainingBytes = stream->GetLength() - stream->GetPosition() - headerTableSize;
        if (remainingBytes > imageDataSize)
        {
            context->LogWarning(OBJECT_ERROR_BAD_IMAGE_TABLE, "Image table size longer than expected.");
            imageDataSize = (uint32_t)remainingBytes;
        }

        auto dataSize = (size_t)imageDataSize;
        auto data = std::make_unique<uint8_t[]>(dataSize);
        if (data == nullptr)
        {
            context->LogError(OBJECT_ERROR_BAD_IMAGE_TABLE, "Image table too large.");
            throw std::runtime_error("Image table too large.");
        }

        // Read g1 element headers
        uintptr_t imageDataBase = (uintptr_t)data.get();
        std::vector<rct_g1_element> newEntries;
        for (uint32_t i = 0; i < numImages; i++)
        {
            rct_g1_element g1Element;

            uintptr_t imageDataOffset = (uintptr_t)stream->ReadValue<uint32_t>();
            g1Element.offset = (uint8_t*)(imageDataBase + imageDataOffset);

            g1Element.width = stream->ReadValue<int16_t>();
            g1Element.height = stream->ReadValue<int16_t>();
            g1Element.x_offset = stream->ReadValue<int16_t>();
            g1Element.y_offset = stream->ReadValue<int16_t>();
            g1Element.flags = stream->ReadValue<uint16_t>();
            g1Element.zoomed_offset = stream->ReadValue<uint16_t>();

            newEntries.push_back(g1Element);
        }

        // Read g1 element data
        size_t readBytes = (size_t)stream->TryRead(data.get(), dataSize);

        // If data is shorter than expected (some custom objects are unfortunately like that)
        size_t unreadBytes = dataSize - readBytes;
        if (unreadBytes > 0)
        {
            std::fill_n(data.get() + readBytes, unreadBytes, 0);
            context->LogWarning(OBJECT_ERROR_BAD_IMAGE_TABLE, "Image table size shorter than expected.");
        }

        _data = std::move(data);
        _entries.insert(_entries.end(), newEntries.begin(), newEntries.end());
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
        newg1.offset = nullptr;
    }
    else
    {
        newg1.offset = new uint8_t[length];
        std::copy_n(g1->offset, length, newg1.offset);
    }
    _entries.push_back(newg1);
}
