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

#include <stack>
#include "../common.h"

struct file_info;
class FileEnumerator final
{
private:
    struct DirectoryState
    {
        utf8 *  Directory;
        int     Handle;
    };

    // Enumeration options
    utf8 *  _rootPath;
    utf8 *  _pattern;
    bool    _recurse;

    // Enumeration state
    int                         _fileHandle;
    std::stack<DirectoryState>  _directoryStack;

    // Current enumeration
    file_info * _fileInfo;
    utf8      * _path;

public:
    FileEnumerator(const utf8 * pattern, bool recurse);
    ~FileEnumerator();

    const file_info *   GetFileInfo() const { return _fileInfo; }
    const utf8 *        GetPath() const { return _path; }

    void Reset();
    bool Next();

private:
    void CloseHandles();
};
