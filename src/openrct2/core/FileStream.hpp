/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "IStream.hpp"
#include "Math.hpp"
#include "String.hpp"

#include "../localisation/Language.h"

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
    FILE *      _file           = nullptr;
    bool        _ownsFilePtr    = false;
    bool        _canRead        = false;
    bool        _canWrite       = false;
    bool        _disposed       = false;
    uint64_t      _fileSize       = 0;

public:
    FileStream(const std::string &path, int32_t fileMode) :
        FileStream(path.c_str(), fileMode)
    {
    }

    FileStream(const utf8 * path, int32_t fileMode)
    {
        const char * mode;
        switch (fileMode) {
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
        wchar_t * pathW = utf8_to_widechar(path);
        wchar_t * modeW = utf8_to_widechar(mode);
        _file = _wfopen(pathW, modeW);
        free(pathW);
        free(modeW);
#else
        _file = fopen(path, mode);
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

    bool CanRead()  const override { return _canRead;  }
    bool CanWrite() const override { return _canWrite; }

    uint64_t GetLength()   const override { return _fileSize; }
    uint64_t GetPosition() const override
    {
#if defined(_MSC_VER)
        return _ftelli64(_file);
#elif (defined(__APPLE__) && defined(__MACH__)) || defined(__ANDROID__) || defined(__OpenBSD__) || defined(__FreeBSD__)
        return ftello(_file);
#else
        return ftello64(_file);
#endif
    }

    void SetPosition(uint64_t position) override
    {
        Seek(position, STREAM_SEEK_BEGIN);
    }

    void Seek(int64_t offset, int32_t origin) override
    {
#if defined(_MSC_VER)
        switch (origin) {
        case STREAM_SEEK_BEGIN:
            _fseeki64(_file, offset, SEEK_SET);
            break;
        case STREAM_SEEK_CURRENT:
            _fseeki64(_file, offset, SEEK_CUR);
            break;
        case STREAM_SEEK_END:
            _fseeki64(_file, offset, SEEK_END);
            break;
        }
#elif (defined(__APPLE__) && defined(__MACH__)) || defined(__ANDROID__) || defined(__OpenBSD__) || defined(__FreeBSD__)
        switch (origin) {
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
#else
        switch (origin) {
        case STREAM_SEEK_BEGIN:
            fseeko64(_file, offset, SEEK_SET);
            break;
        case STREAM_SEEK_CURRENT:
            fseeko64(_file, offset, SEEK_CUR);
            break;
        case STREAM_SEEK_END:
            fseeko64(_file, offset, SEEK_END);
            break;
    }
#endif
    }

    void Read(void * buffer, uint64_t length) override
    {
        uint64_t remainingBytes = GetLength() - GetPosition();
        if (length <= remainingBytes)
        {
            if (fread(buffer, (size_t)length, 1, _file) == 1)
            {
                return;
            }
        }
        throw IOException("Attempted to read past end of file.");
    }

    void Write(const void * buffer, uint64_t length) override
    {
        if (fwrite(buffer, (size_t)length, 1, _file) != 1)
        {
            throw IOException("Unable to write to file.");
        }

        uint64_t position = GetPosition();
        _fileSize = std::max(_fileSize, position);
    }

    uint64_t TryRead(void * buffer, uint64_t length) override
    {
        size_t readBytes = fread(buffer, 1, (size_t)length, _file);
        return readBytes;
    }
};
