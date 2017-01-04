#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include <SDL.h>
#include "IStream.hpp"

enum
{
    FILE_MODE_OPEN,
    FILE_MODE_WRITE
};

/**
 * A stream for reading and writing to files. Wraps an SDL_RWops, SDL2's cross platform file stream.
 */
class FileStream final : public IStream
{
private:
    SDL_RWops * _file;
    bool        _canRead;
    bool        _canWrite;
    bool        _disposed;
    uint64      _fileSize;

public:
    FileStream(const std::string &path, int fileMode) :
        FileStream(path.c_str(), fileMode)
    {
    }

    FileStream(const utf8 * path, int fileMode)
    {
        const char * mode;
        switch (fileMode) {
        case FILE_MODE_OPEN:
            mode = "rb";
            _canRead = true;
            _canWrite = false;
            break;
        case FILE_MODE_WRITE:
            mode = "wb";
            _canRead = false;
            _canWrite = true;
            break;
        default:
            throw;
        }

        _file = SDL_RWFromFile(path, mode);
        if (_file == nullptr)
        {
            throw IOException(SDL_GetError());
        }

        _disposed = false;
        _fileSize = SDL_RWsize(_file);
    }

    ~FileStream()
    {
        if (!_disposed)
        {
            _disposed = true;
            SDL_RWclose(_file);
        }
    }

    bool CanRead()  const override { return _canRead;  }
    bool CanWrite() const override { return _canWrite; }

    uint64 GetLength()   const override { return _fileSize; }
    uint64 GetPosition() const override { return SDL_RWtell(_file); }

    void SetPosition(uint64 position) override
    {
        Seek(position, STREAM_SEEK_BEGIN);
    }

    void Seek(sint64 offset, int origin) override
    {
        switch (origin) {
        case STREAM_SEEK_BEGIN:
            SDL_RWseek(_file, offset, RW_SEEK_SET);
            break;
        case STREAM_SEEK_CURRENT:
            SDL_RWseek(_file, offset, RW_SEEK_CUR);
            break;
        case STREAM_SEEK_END:
            SDL_RWseek(_file, offset, RW_SEEK_END);
            break;
        }
    }

    void Read(void * buffer, uint64 length) override
    {
        uint64 remainingBytes = GetLength() - GetPosition();
        if (length <= remainingBytes)
        {
            if (SDL_RWread(_file, buffer, (size_t)length, 1) == 1)
            {
                return;
            }
        }
        throw IOException("Attempted to read past end of file.");
    }

    void Write(const void * buffer, uint64 length) override
    {
        if (SDL_RWwrite(_file, buffer, (size_t)length, 1) != 1)
        {
            throw IOException("Unable to write to file.");
        }
    }

    uint64 TryRead(void * buffer, uint64 length) override
    {
        size_t readBytes = SDL_RWread(_file, buffer, 1, (size_t)length);
        return readBytes;
    }
};
