/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/IStream.hpp"
#include "../drawing/Drawing.h"

#include <memory>
#include <optional>

namespace OpenRCT2
{
    class GxSource
    {
    public:
        virtual ~GxSource() = default;

        virtual size_t GetCount() const = 0;
        virtual const G1Element* GetImage(size_t i) const = 0;
        virtual G1Element GetImageCopy(size_t i) const = 0;
        virtual std::optional<size_t> GetNextZoomImage(size_t i) const = 0;
    };

    class GxFile final : public GxSource
    {
    private:
        std::vector<G1Element> _elements;
        std::vector<uint8_t> _data;

    public:
        GxFile();
        GxFile(IStream& stream);

        size_t GetCount() const override;
        const G1Element* GetImage(size_t i) const override;
        G1Element GetImageCopy(size_t i) const override;
        std::optional<size_t> GetNextZoomImage(size_t i) const override;
    };

    class GxStream final : public GxSource
    {
    private:
        std::unique_ptr<IStream> _stream;
        std::unique_ptr<IStream> _streamElements;
        std::unique_ptr<IStream> _streamData;

        uint32_t _count{};
        uint64_t _elementStart{};
        uint64_t _dataStart{};
        uint32_t _dataSize{};

        std::optional<uint32_t> _cachedIndex{};
        RCTG1Element _cachedElement{};

    public:
        GxStream(std::unique_ptr<IStream> stream);
        GxStream(std::unique_ptr<IStream> streamElements, std::unique_ptr<IStream> streamData);

        size_t GetCount() const override;
        const G1Element* GetImage(size_t i) const override;
        G1Element GetImageCopy(size_t i) const override;
        std::optional<size_t> GetNextZoomImage(size_t i) const override;

    private:
        RCTG1Element ReadElement(size_t i);
        std::unique_ptr<uint8_t[]> ReadData(size_t offset, size_t len);
    };
} // namespace OpenRCT2
