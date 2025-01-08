/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "FileSystem.hpp"
#include "IStream.hpp"

namespace OpenRCT2
{
    enum
    {
        FILE_MODE_OPEN,
        FILE_MODE_WRITE,
        FILE_MODE_APPEND,
    };

    /**
     * A stream for reading and writing to files.
     */
    class FileStream final : public IStream
    {
    private:
        FILE* _file = nullptr;
        bool _ownsFilePtr = false;
        bool _canRead = false;
        bool _canWrite = false;
        bool _disposed = false;
        uint64_t _fileSize = 0;

    public:
        FileStream(const fs::path& path, int32_t fileMode);
        FileStream(const std::string& path, int32_t fileMode);
        FileStream(std::string_view path, int32_t fileMode);
        FileStream(const utf8* path, int32_t fileMode);
        ~FileStream() override;

        bool CanRead() const override;
        bool CanWrite() const override;

        uint64_t GetLength() const override;
        uint64_t GetPosition() const override;

        void SetPosition(uint64_t position) override;
        void Seek(int64_t offset, int32_t origin) override;
        void Read(void* buffer, uint64_t length) override;
        void Write(const void* buffer, uint64_t length) override;
        uint64_t TryRead(void* buffer, uint64_t length) override;
        const void* GetData() const override;
    };

} // namespace OpenRCT2
