/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"

#include <memory>
#include <string>
#include <vector>

namespace OpenRCT2::FileScanner
{
    struct FileInfo
    {
        u8string Name;
        uint64_t Size;
        uint64_t LastModified;
    };
} // namespace OpenRCT2::FileScanner

struct IFileScanner
{
    virtual ~IFileScanner() = default;

    virtual const OpenRCT2::FileScanner::FileInfo& GetFileInfo() const = 0;
    virtual const u8string& GetPath() const = 0;
    virtual u8string GetPathRelative() const = 0;

    virtual void Reset() = 0;
    virtual bool Next() = 0;
};

struct QueryDirectoryResult
{
    uint32_t TotalFiles;
    uint64_t TotalFileSize;
    uint32_t FileDateModifiedChecksum;
    uint32_t PathChecksum;
};

namespace OpenRCT2::Path
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
} // namespace OpenRCT2::Path
