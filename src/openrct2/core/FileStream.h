/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    enum class FileMode : uint8_t
    {
        open,
        write,
        append,
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
        FileStream(const fs::path& path, FileMode fileMode);
        FileStream(const std::string& path, FileMode fileMode);
        FileStream(std::string_view path, FileMode fileMode);
        FileStream(const utf8* path, FileMode fileMode);
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
    };

} // namespace OpenRCT2
