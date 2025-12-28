/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Gx.h"

#include <cstdint>
#include <cstring>

using namespace OpenRCT2;

GxFile::GxFile()
{
}

GxFile::GxFile(IStream& stream)
{
    // Load header
    auto numImages = stream.ReadValue<uint32_t>();
    auto dataSize = stream.ReadValue<uint32_t>();

    // Load image headers
    _elements.resize(numImages);
    for (size_t i = 0; i < numImages; i++)
    {
        auto src = stream.ReadValue<RCTG1Element>();
        auto& dst = _elements[i];
        dst.offset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(src.offset));
        dst.width = src.width;
        dst.height = src.height;
        dst.x_offset = src.x_offset;
        dst.y_offset = src.y_offset;
        dst.flags = src.flags;
        dst.zoomed_offset = src.zoomed_offset;
    }

    // Load image data
    _data.resize(dataSize);
    stream.Read(_data.data(), dataSize);

    // Fix image data pointers
    auto dataStart = reinterpret_cast<uintptr_t>(_data.data());
    for (size_t i = 0; i < numImages; i++)
    {
        auto& dst = _elements[i];
        dst.offset += dataStart;
    }
}

size_t GxFile::GetCount() const
{
    return _elements.size();
}

static G1Element DuplicateImageData(const G1Element& src)
{
    G1Element dst = src;
    auto size = G1CalculateDataSize(&src);
    dst.offset = new uint8_t[size];
    std::memcpy(dst.offset, src.offset, size);
    return dst;
}

const G1Element* GxFile::GetImage(size_t i) const
{
    const G1Element* result{};
    if (i < _elements.size())
    {
        result = &_elements[i];
    }
    return result;
}

G1Element GxFile::GetImageCopy(size_t i) const
{
    G1Element result{};
    if (i < _elements.size())
    {
        result = DuplicateImageData(_elements[i]);
    }
    return result;
}

std::optional<size_t> GxFile::GetNextZoomImage(size_t i) const
{
    auto& element = _elements[i];
    if (element.flags & G1_FLAG_HAS_ZOOM_SPRITE)
    {
        return i - element.zoomed_offset;
    }
    return {};
}

GxStream::GxStream(std::unique_ptr<IStream> stream)
    : _stream(std::move(stream))
{
    _count = _stream->ReadValue<uint32_t>();
    _dataSize = _stream->ReadValue<uint32_t>();
    _elementStart = _stream->GetPosition();
    _dataStart = _elementStart + (_count * sizeof(RCTG1Element));
}

GxStream::GxStream(std::unique_ptr<IStream> streamElements, std::unique_ptr<IStream> streamData)
    : _streamElements(std::move(streamElements))
    , _streamData(std::move(streamData))
{
    _count = _streamElements->GetLength() / sizeof(RCTG1Element);
    _dataSize = _streamData->GetLength();
}

size_t GxStream::GetCount() const
{
    return _count;
}

const G1Element* GxStream::GetImage(size_t i) const
{
    throw std::runtime_error("Invalid operation");
}

G1Element GxStream::GetImageCopy(size_t i) const
{
    G1Element result{};
    if (i < _count)
    {
        // Read two elements
        auto mut = const_cast<GxStream*>(this);
        auto src = mut->ReadElement(i);
        auto dataLength = _dataSize - src.offset;
        if (i < _count - 1)
        {
            // A dodgy assumption
            auto next = mut->ReadElement(i + 1);
            dataLength = next.offset - src.offset;
        }

        // Read the data
        auto data = mut->ReadData(src.offset, dataLength);

        result.offset = data.release();
        result.width = src.width;
        result.height = src.height;
        result.x_offset = src.x_offset;
        result.y_offset = src.y_offset;
        result.flags = src.flags;
        result.zoomed_offset = src.zoomed_offset;

        mut->_cachedIndex = static_cast<uint32_t>(i);
        mut->_cachedElement = src;
    }
    return result;
}

std::optional<size_t> GxStream::GetNextZoomImage(size_t i) const
{
    if (_cachedIndex != i)
    {
        auto mut = const_cast<GxStream*>(this);
        mut->_cachedElement = mut->ReadElement(i);
        mut->_cachedIndex = static_cast<uint32_t>(i);
    }

    const auto* element = &_cachedElement;
    if (element->flags & G1_FLAG_HAS_ZOOM_SPRITE)
    {
        return i - element->zoomed_offset;
    }
    return {};
}

RCTG1Element GxStream::ReadElement(size_t i)
{
    auto stream = _stream != nullptr ? _stream.get() : _streamElements.get();
    stream->SetPosition(_elementStart + (i * sizeof(RCTG1Element)));
    return stream->ReadValue<RCTG1Element>();
}

std::unique_ptr<uint8_t[]> GxStream::ReadData(size_t offset, size_t len)
{
    auto stream = _stream != nullptr ? _stream.get() : _streamData.get();
    stream->SetPosition(_dataStart + offset);
    return stream->ReadArray<uint8_t>(len);
}
