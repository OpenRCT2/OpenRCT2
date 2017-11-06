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

/**
 * Dummy class to ease porting OpenRCT2.
 */

#ifdef NO_LIBZIP
#include "Zip.h"

class ZipArchive final : public IZipArchive
{
public:
    ZipArchive(const utf8 * path, ZIP_ACCESS access)
    {
        STUB();
    }

    ~ZipArchive() override
    {
    }

    size_t GetNumFiles() const override
    {
        return 0;
    }

    const utf8 * GetFileName(size_t index) const override
    {
        return nullptr;
    }

    uint64 GetFileSize(size_t index) const override
    {
        return 0;
    }

    void * GetFileData(const utf8 * path, size_t * outSize) const override
    {
        return nullptr;
    }

    IStream * GetFileStream(const utf8 * path) const override
    {
        return nullptr;
    }

    void SetFileData(const utf8 * path, void * data, size_t dataSize) override
    {
    }

    void DeleteFile(const utf8 * path) override
    {
    }

    void RenameFile(const utf8 * path, const utf8 * newPath) override
    {
    }
};

namespace Zip
{
    IZipArchive * Open(const utf8 * path, ZIP_ACCESS access)
    {
        return nullptr;
    }

    IZipArchive * TryOpen(const utf8 * path, ZIP_ACCESS access)
    {
        return nullptr;
    }
}

#endif // NO_LIBZIP
