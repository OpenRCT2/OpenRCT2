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

#ifdef __cplusplus

#include "../common.h"

/**
 * Represents a zip file.
 */
interface IZipArchive
{
    virtual ~IZipArchive() { }

    virtual size_t          GetNumFiles() const abstract;
    virtual const utf8 *    GetFileName(size_t index) const abstract;
    virtual uint64          GetFileSize(size_t index) const abstract;
    virtual void *          GetFileData(const utf8 * path, size_t * outSize) const abstract;
    virtual IStream *       GetFileStream(const utf8 * path) const abstract;

    /**
     * Creates or overwrites a file within the zip archive to the given data buffer.
     * @param path The path of the file within the zip.
     * @param data The data to write, this buffer must not be mutated or disposed until
     *             the zip archive has been disposed.
     * @param dataSize The size of the data in bytes.
     */
    virtual void SetFileData(const utf8 * path, void * data, size_t dataSize) abstract;

    virtual void DeleteFile(const utf8 * path) abstract;
    virtual void RenameFile(const utf8 * path, const utf8 * newPath) abstract;
};

enum ZIP_ACCESS
{
    ZIP_ACCESS_READ,
    ZIP_ACCESS_WRITE,
};

namespace Zip
{
    IZipArchive * Open(const utf8 * path, ZIP_ACCESS zipAccess);
    IZipArchive * TryOpen(const utf8 * path, ZIP_ACCESS zipAccess);
}

#endif
