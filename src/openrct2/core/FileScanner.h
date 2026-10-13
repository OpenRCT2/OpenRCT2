/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
        u8string name;
        uint64_t size;
        uint64_t lastModified;
    };
} // namespace OpenRCT2::FileScanner

struct IFileScanner
{
    virtual ~IFileScanner() = default;

    virtual const OpenRCT2::FileScanner::FileInfo& getFileInfo() const = 0;
    virtual const u8string& getPath() const = 0;
    virtual u8string getPathRelative() const = 0;

    virtual void reset() = 0;
    virtual bool next() = 0;
};

struct QueryDirectoryResult
{
    uint32_t totalFiles;
    uint64_t totalFileSize;
    uint32_t fileDateModifiedChecksum;
    uint32_t pathChecksum;
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
    [[nodiscard]] std::unique_ptr<IFileScanner> scanDirectory(const std::string& pattern, bool recurse);

    /**
     * Scans a directory and all sub directories
     * @param result The query result to modify.
     * @param pattern The path followed by a semi-colon delimited list of wildcard patterns.
     */
    void queryDirectory(QueryDirectoryResult* result, const std::string& pattern);

    [[nodiscard]] std::vector<std::string> getDirectories(const std::string& path);
} // namespace OpenRCT2::Path
