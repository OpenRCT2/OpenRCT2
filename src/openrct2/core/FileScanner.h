/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"

#include <memory>
#include <string>
#include <vector>

struct FileInfo
{
    const utf8* Name;
    uint64_t Size;
    uint64_t LastModified;
};

struct IFileScanner
{
    virtual ~IFileScanner() = default;

    virtual const FileInfo* GetFileInfo() const abstract;
    virtual const utf8* GetPath() const abstract;
    virtual const utf8* GetPathRelative() const abstract;

    virtual void Reset() abstract;
    virtual bool Next() abstract;
};

struct QueryDirectoryResult
{
    uint32_t TotalFiles;
    uint64_t TotalFileSize;
    uint32_t FileDateModifiedChecksum;
    uint32_t PathChecksum;
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
    [[nodiscard]] std::unique_ptr<IFileScanner> ScanDirectory(const std::string& pattern, bool recurse);

    /**
     * Scans a directory and all sub directories
     * @param result The query result to modify.
     * @param pattern The path followed by a semi-colon delimited list of wildcard patterns.
     */
    void QueryDirectory(QueryDirectoryResult* result, const std::string& pattern);

    [[nodiscard]] std::vector<std::string> GetDirectories(const std::string& path);
} // namespace Path
