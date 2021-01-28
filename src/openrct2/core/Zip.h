/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <istream>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace OpenRCT2
{
    struct IStream;
}

/**
 * Represents a zip file.
 */
struct IZipArchive
{
    virtual ~IZipArchive()
    {
    }

    virtual size_t GetNumFiles() const abstract;
    virtual std::string GetFileName(size_t index) const abstract;
    virtual uint64_t GetFileSize(size_t index) const abstract;
    virtual std::vector<uint8_t> GetFileData(std::string_view path) const abstract;
    virtual std::unique_ptr<OpenRCT2::IStream> GetFileStream(std::string_view path) const abstract;

    /**
     * Creates or overwrites a file within the zip archive to the given data buffer.
     * @param path The path of the file within the zip.
     * @param data The data to write.
     */
    virtual void SetFileData(std::string_view path, std::vector<uint8_t>&& data) abstract;

    virtual void DeleteFile(std::string_view path) abstract;
    virtual void RenameFile(std::string_view path, std::string_view newPath) abstract;

    std::optional<size_t> GetIndexFromPath(std::string_view path) const;
    bool Exists(std::string_view path) const;
};

enum class ZIP_ACCESS
{
    READ,
    WRITE,
};

namespace Zip
{
    std::unique_ptr<IZipArchive> Open(std::string_view path, ZIP_ACCESS zipAccess);
    std::unique_ptr<IZipArchive> TryOpen(std::string_view path, ZIP_ACCESS zipAccess);
} // namespace Zip
