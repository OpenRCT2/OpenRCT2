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
#include "Math.hpp"

enum
{
    FILE_MODE_OPEN,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
};

/**
 * A stream for reading and writing to files. Wraps an SDL_RWops, SDL2's cross platform file stream.
 */
class FileStream final : public IStream
{
private:
    SDL_RWops * _file           = 0;
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

        _file = SDL_RWFromFile(path, mode);
        if (_file == nullptr)
        {
            throw IOException(SDL_GetError());
        }
        _fileSize = SDL_RWsize(_file);
        _ownsFilePtr = true;
    }

    FileStream(SDL_RWops * ops, sint32 fileMode)
    {
        _file = ops;
        switch (fileMode) {
        case FILE_MODE_OPEN:
            _canRead = true;
            _canWrite = false;
            break;
        case FILE_MODE_WRITE:
            _canRead = true;
            _canWrite = true;
            break;
        default:
            throw;
        }
        _fileSize = SDL_RWsize(_file);
    }

    ~FileStream()
    {
        if (!_disposed)
        {
            _disposed = true;
            if (_ownsFilePtr)
            {
                SDL_RWclose(_file);
            }
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

    void Seek(sint64 offset, sint32 origin) override
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

        uint64 position = GetPosition();
        _fileSize = Math::Max(_fileSize, position);
    }

    uint64 TryRead(void * buffer, uint64 length) override
    {
        size_t readBytes = SDL_RWread(_file, buffer, 1, (size_t)length);
        return readBytes;
    }
};
