#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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
    virtual uint64 GetFileSize(size_t index) const abstract;
    virtual std::vector<uint8> GetFileData(const std::string_view& path) const abstract;

    /**
     * Creates or overwrites a file within the zip archive to the given data buffer.
     * @param path The path of the file within the zip.
     * @param data The data to write.
     */
    virtual void SetFileData(const std::string_view& path, std::vector<uint8>&& data) abstract;

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
