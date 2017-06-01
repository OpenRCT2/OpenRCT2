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

#pragma once

#include "../common.h"
#include "IStream.hpp"
#include "Math.hpp"
#include "String.hpp"

extern "C"
{
    #include "../localisation/language.h"
}

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
    uint64      _fileSize       = 0;

public:
    FileStream(const std::string &path, sint32 fileMode) :
        FileStream(path.c_str(), fileMode)
    {
    }

    FileStream(const utf8 * path, sint32 fileMode)
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

    uint64 GetLength()   const override { return _fileSize; }
    uint64 GetPosition() const override
    {
#if defined(_MSC_VER)
        return _ftelli64(_file);
#elif (defined(__APPLE__) && defined(__MACH__))
        return ftello(_file);
#else
        return ftello64(_file);
#endif
    }

    void SetPosition(uint64 position) override
    {
        Seek(position, STREAM_SEEK_BEGIN);
    }

    void Seek(sint64 offset, sint32 origin) override
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
#elif (defined(__APPLE__) && defined(__MACH__))
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

    void Read(void * buffer, uint64 length) override
    {
        uint64 remainingBytes = GetLength() - GetPosition();
        if (length <= remainingBytes)
        {
            if (fread(buffer, (size_t)length, 1, _file) == 1)
            {
                return;
            }
        }
        throw IOException("Attempted to read past end of file.");
    }

    void Write(const void * buffer, uint64 length) override
    {
        if (fwrite(buffer, (size_t)length, 1, _file) != 1)
        {
            throw IOException("Unable to write to file.");
        }

        uint64 position = GetPosition();
        _fileSize = Math::Max(_fileSize, position);
    }

    uint64 TryRead(void * buffer, uint64 length) override
    {
        size_t readBytes = fread(buffer, 1, (size_t)length, _file);
        return readBytes;
    }
};
