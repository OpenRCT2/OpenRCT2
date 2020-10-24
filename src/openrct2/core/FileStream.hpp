/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../localisation/Language.h"
#include "IStream.hpp"
#include "String.hpp"

#include <algorithm>
#ifndef _WIN32
#    include <sys/stat.h>
#endif

#if defined(__linux__) && !defined(__ANDROID__)
#    define ftello ftello64
#    define fseeko fseeko64
#endif

#ifdef _MSC_VER
#    define ftello _ftelli64
#    define fseeko _fseeki64
#endif

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
        FileStream(const std::string& path, int32_t fileMode)
            : FileStream(path.c_str(), fileMode)
        {
        }

        FileStream(const utf8* path, int32_t fileMode)
        {
            const char* mode;
            switch (fileMode)
            {
                case FILE_MODE_OPEN:
                    mode = "rb";
                    _canRead = true;
                    _canWrite = false;
                    break;
                case FILE_MODE_WRITE:
                    mode = "w+b";
                    _canRead = true;
                    _canWrite = true;
                    break;
                case FILE_MODE_APPEND:
                    mode = "a";
                    _canRead = false;
                    _canWrite = true;
                    break;
                default:
                    throw;
            }

#ifdef _WIN32
            auto pathW = String::ToWideChar(path);
            auto modeW = String::ToWideChar(mode);
            _file = _wfopen(pathW.c_str(), modeW.c_str());
#else
            if (fileMode == FILE_MODE_OPEN)
            {
                struct stat fileStat;
                // Only allow regular files to be opened as its possible to open directories.
                if (stat(path, &fileStat) == 0 && S_ISREG(fileStat.st_mode))
                {
                    _file = fopen(path, mode);
                }
            }
            else
            {
                _file = fopen(path, mode);
            }
#endif
            if (_file == nullptr)
            {
                throw IOException(String::StdFormat("Unable to open '%s'", path));
            }

            Seek(0, STREAM_SEEK_END);
            _fileSize = GetPosition();
            Seek(0, STREAM_SEEK_BEGIN);

            _ownsFilePtr = true;
        }

        ~FileStream() override
        {
            if (!_disposed)
            {
                _disposed = true;
                if (_ownsFilePtr)
                {
                    fclose(_file);
                }
            }
        }

        bool CanRead() const override
        {
            return _canRead;
        }
        bool CanWrite() const override
        {
            return _canWrite;
        }

        uint64_t GetLength() const override
        {
            return _fileSize;
        }
        uint64_t GetPosition() const override
        {
            return ftello(_file);
        }

        void SetPosition(uint64_t position) override
        {
            Seek(position, STREAM_SEEK_BEGIN);
        }

        void Seek(int64_t offset, int32_t origin) override
        {
            switch (origin)
            {
                case STREAM_SEEK_BEGIN:
                    fseeko(_file, offset, SEEK_SET);
                    break;
                case STREAM_SEEK_CURRENT:
                    fseeko(_file, offset, SEEK_CUR);
                    break;
                case STREAM_SEEK_END:
                    fseeko(_file, offset, SEEK_END);
                    break;
            }
        }

        void Read(void* buffer, uint64_t length) override
        {
            uint64_t remainingBytes = GetLength() - GetPosition();
            if (length <= remainingBytes)
            {
                if (fread(buffer, static_cast<size_t>(length), 1, _file) == 1)
                {
                    return;
                }
            }
            throw IOException("Attempted to read past end of file.");
        }

        void Write(const void* buffer, uint64_t length) override
        {
            if (fwrite(buffer, static_cast<size_t>(length), 1, _file) != 1)
            {
                throw IOException("Unable to write to file.");
            }

            uint64_t position = GetPosition();
            _fileSize = std::max(_fileSize, position);
        }

        uint64_t TryRead(void* buffer, uint64_t length) override
        {
            size_t readBytes = fread(buffer, 1, static_cast<size_t>(length), _file);
            return readBytes;
        }

        const void* GetData() const override
        {
            return nullptr;
        }
    };

} // namespace OpenRCT2
