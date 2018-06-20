/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <string_view>
#include <vector>
#include "../common.h"

/**
 * Represents a zip file.
 */
interface IZipArchive
{
    virtual ~IZipArchive() { }

    virtual size_t GetNumFiles() const abstract;
    virtual std::string GetFileName(size_t index) const abstract;
    virtual uint64_t GetFileSize(size_t index) const abstract;
    virtual std::vector<uint8_t> GetFileData(const std::string_view& path) const abstract;

    /**
     * Creates or overwrites a file within the zip archive to the given data buffer.
     * @param path The path of the file within the zip.
     * @param data The data to write.
     */
    virtual void SetFileData(const std::string_view& path, std::vector<uint8_t>&& data) abstract;

    virtual void DeleteFile(const std::string_view& path) abstract;
    virtual void RenameFile(const std::string_view& path, const std::string_view& newPath) abstract;
};

enum class ZIP_ACCESS
{
    READ,
    WRITE,
};

namespace Zip
{
    std::unique_ptr<IZipArchive> Open(const std::string_view& path, ZIP_ACCESS zipAccess);
    std::unique_ptr<IZipArchive> TryOpen(const std::string_view& path, ZIP_ACCESS zipAccess);
}
