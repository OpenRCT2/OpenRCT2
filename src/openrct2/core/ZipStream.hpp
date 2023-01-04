/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once

#include "IStream.hpp"
#include "Zip.h"

#include <memory>

namespace OpenRCT2
{
    /**
     * Couples a zip archive and a zip item stream to ensure the lifetime of the zip archive is maintained
     * for the lifetime of the stream.
     */
    class ZipStreamWrapper final : public IStream
    {
    private:
        std::unique_ptr<IZipArchive> _zipArchive;
        std::unique_ptr<IStream> _base;

    public:
        ZipStreamWrapper(std::unique_ptr<IZipArchive> zipArchive, std::unique_ptr<IStream> base)
            : _zipArchive(std::move(zipArchive))
            , _base(std::move(base))
        {
        }

        bool CanRead() const override
        {
            return _base->CanRead();
        }

        bool CanWrite() const override
        {
            return _base->CanWrite();
        }

        uint64_t GetLength() const override
        {
            return _base->GetLength();
        }

        uint64_t GetPosition() const override
        {
            return _base->GetPosition();
        }

        void SetPosition(uint64_t position) override
        {
            _base->SetPosition(position);
        }

        void Seek(int64_t offset, int32_t origin) override
        {
            _base->Seek(offset, origin);
        }

        void Read(void* buffer, uint64_t length) override
        {
            _base->Read(buffer, length);
        }

        void Write(const void* buffer, uint64_t length) override
        {
            _base->Write(buffer, length);
        }

        uint64_t TryRead(void* buffer, uint64_t length) override
        {
            return _base->TryRead(buffer, length);
        }

        const void* GetData() const override
        {
            return _base->GetData();
        }
    };

} // namespace OpenRCT2
