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

#include "FileEnumerator.h"
#include "Memory.hpp"
#include "Path.hpp"
#include "String.hpp"

extern "C"
{
    #include "../platform/platform.h"
}

FileEnumerator::FileEnumerator(const utf8 * pattern, bool recurse)
{
    _rootPath = Memory::Allocate<utf8>(MAX_PATH);
    Path::GetDirectory(_rootPath, MAX_PATH, pattern);
    _pattern = String::Duplicate(Path::GetFileName(pattern));
    _recurse = recurse;

    _fileInfo = Memory::Allocate<file_info>();
    Memory::Set(_fileInfo, 0, sizeof(file_info));
    _path = Memory::Allocate<utf8>(MAX_PATH);

    _fileHandle = INVALID_HANDLE;

    Reset();
}

FileEnumerator::~FileEnumerator()
{
    CloseHandles();
    Memory::Free(_path);
    Memory::Free(_fileInfo);
    Memory::Free(_pattern);
    Memory::Free(_rootPath);
}

void FileEnumerator::Reset()
{
    CloseHandles();

    DirectoryState directoryState;
    directoryState.Directory = String::Duplicate(_rootPath);
    directoryState.Handle = INVALID_HANDLE;
    _directoryStack.push(directoryState);
}

bool FileEnumerator::Next()
{
    while (true)
    {
        while (_fileHandle == INVALID_HANDLE)
        {
            if (_directoryStack.size() == 0)
            {
                return false;
            }

            DirectoryState directoryState = _directoryStack.top();
            if (directoryState.Handle == INVALID_HANDLE)
            {
                // Start enumerating files for this directory
                utf8 pattern[MAX_PATH];
                String::Set(pattern, sizeof(pattern), directoryState.Directory);
                Path::Append(pattern, sizeof(pattern), _pattern);

                _fileHandle = platform_enumerate_files_begin(pattern);
                break;
            }
            else
            {
                // Next directory
                utf8 name[MAX_PATH];
                if (platform_enumerate_directories_next(directoryState.Handle, name))
                {
                    DirectoryState newDirectoryState;
                    newDirectoryState.Handle = INVALID_HANDLE;
                    newDirectoryState.Directory = Memory::Allocate<utf8>(MAX_PATH);

                    String::Set(newDirectoryState.Directory, MAX_PATH, directoryState.Directory);
                    Path::Append(newDirectoryState.Directory, MAX_PATH, name);

                    _directoryStack.push(newDirectoryState);
                }
                else
                {
                    platform_enumerate_directories_end(directoryState.Handle);
                    Memory::Free(directoryState.Directory);
                    _directoryStack.pop();
                }
            }
        }

        // Next file
        if (_fileHandle != INVALID_HANDLE)
        {
            if (platform_enumerate_files_next(_fileHandle, _fileInfo))
            {
                String::Set(_path, MAX_PATH, _directoryStack.top().Directory);
                Path::Append(_path, MAX_PATH, _fileInfo->path);
                return true;
            }
            platform_enumerate_files_end(_fileHandle);
            _fileHandle = INVALID_HANDLE;
        }

        if (_recurse)
        {
            // Start enumerating sub-directories
            DirectoryState * directoryState = &_directoryStack.top();
            directoryState->Handle = platform_enumerate_directories_begin(directoryState->Directory);
            if (directoryState->Handle == INVALID_HANDLE)
            {
                Memory::Free(directoryState->Directory);
                _directoryStack.pop();
            }
        }
        else
        {
            Memory::Free(_directoryStack.top().Directory);
            _directoryStack.pop();
            return false;
        }
    }
}

void FileEnumerator::CloseHandles()
{
    if (_fileHandle != INVALID_HANDLE)
    {
        platform_enumerate_files_end(_fileHandle);
        _fileHandle = INVALID_HANDLE;
    }
    while (_directoryStack.size() > 0)
    {
        DirectoryState directoryState = _directoryStack.top();
        if (directoryState.Handle != INVALID_HANDLE)
        {
            platform_enumerate_directories_end(directoryState.Handle);
        }
        Memory::Free(directoryState.Directory);
        _directoryStack.pop();
    }
}
