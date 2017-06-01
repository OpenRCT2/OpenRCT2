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

#include <string>
#include "../common.h"

struct FileInfo
{
    const utf8 *    Name;
    uint64          Size;
    uint64          LastModified;
};

interface IFileScanner
{
    virtual ~IFileScanner() = default;

    virtual const FileInfo *    GetFileInfo() const abstract;
    virtual const utf8 *        GetPath() const abstract;
    virtual const utf8 *        GetPathRelative() const abstract;

    virtual void Reset() abstract;
    virtual bool Next() abstract;
};

struct QueryDirectoryResult
{
    uint32 TotalFiles;
    uint64 TotalFileSize;
    uint32 FileDateModifiedChecksum;
    uint32 PathChecksum;
};

namespace Path
{
    /**
     * Scans a directory and optionally sub directories for files that matches the
     * given pattern and returns an enumerator.
     * @param pattern The path followed by a semi-colon delimited list of wildcard patterns.
     * @param recurse Whether to scan sub directories or not.
     * @returns A new FileScanner, this must be deleted when no longer needed.
     */
    IFileScanner * ScanDirectory(const std::string &pattern, bool recurse);

    /**
     * Scans a directory and all sub directories
     * @param result The query result to modify.
     * @param pattern The path followed by a semi-colon delimited list of wildcard patterns.
     * @returns An aggregated result of all scanned files.
     */
    void QueryDirectory(QueryDirectoryResult * result, const std::string &pattern);
}
