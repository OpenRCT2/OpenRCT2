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

#ifndef __ANDROID__
#include <zip.h>
#include "IStream.hpp"
#include "MemoryStream.h"
#include "Zip.h"

class ZipArchive final : public IZipArchive
{
private:
    zip_t *     _zip;
    ZIP_ACCESS  _access;

public:
    ZipArchive(const utf8 * path, ZIP_ACCESS access)
    {
        sint32 zipOpenMode = ZIP_RDONLY;
        if (access == ZIP_ACCESS_WRITE)
        {
            zipOpenMode = ZIP_CREATE;
        }

        sint32 error;
        _zip = zip_open(path, zipOpenMode, &error);
        if (_zip == nullptr)
        {
            throw IOException("Unable to open zip file.");
        }

        _access = access;
    }

    ~ZipArchive() override
    {
        zip_close(_zip);
    }

    size_t GetNumFiles() const override
    {
        return zip_get_num_files(_zip);
    }

    const utf8 * GetFileName(size_t index) const override
    {
        const utf8 * name = zip_get_name(_zip, index, ZIP_FL_ENC_GUESS);
        return name;
    }

    uint64 GetFileSize(size_t index) const override
    {
        zip_stat_t zipFileStat;
        if (zip_stat_index(_zip, index, 0, &zipFileStat) == ZIP_ER_OK)
        {
            return zipFileStat.size;
        }
        else
        {
            return 0;
        }
    }

    void * GetFileData(const utf8 * path, size_t * outSize) const override
    {
        void * data = nullptr;

        size_t index = (size_t)zip_name_locate(_zip, path, 0);
        uint64 dataSize = GetFileSize(index);
        if (dataSize > 0 && dataSize < SIZE_MAX)
        {
            zip_file_t * zipFile = zip_fopen(_zip, path, 0);
            if (zipFile != nullptr)
            {
                data = Memory::Allocate<void>((size_t)dataSize);
                uint64 readBytes = zip_fread(zipFile, data, dataSize);
                if (readBytes != dataSize)
                {
                    SafeFree(data);
                    dataSize = 0;
                }
                zip_fclose(zipFile);
            }
        }

        if (outSize != nullptr) *outSize = (size_t)dataSize;
        return data;
    }

    IStream * GetFileStream(const utf8 * path) const override
    {
        IStream * stream = nullptr;
        size_t dataSize;
        void * data = GetFileData(path, &dataSize);
        if (data != nullptr)
        {
            stream = new MemoryStream(data, dataSize, MEMORY_ACCESS::READ | MEMORY_ACCESS::OWNER);
        }
        return stream;
    }

    void SetFileData(const utf8 * path, void * data, size_t dataSize) override
    {
        zip_source_t * source = zip_source_buffer(_zip, data, dataSize, 0);
        sint64 index = zip_name_locate(_zip, path, 0);
        if (index == -1)
        {
            zip_add(_zip, path, source);
        }
        else
        {
            zip_replace(_zip, index, source);
        }
    }

    void DeleteFile(const utf8 * path) override
    {
        uint64 index = zip_name_locate(_zip, path, 0);
        zip_delete(_zip, index);
    }

    void RenameFile(const utf8 * path, const utf8 * newPath) override
    {
        uint64 index = zip_name_locate(_zip, path, 0);
        zip_file_rename(_zip, index, newPath, ZIP_FL_ENC_GUESS);
    }
};

namespace Zip
{
    IZipArchive * Open(const utf8 * path, ZIP_ACCESS access)
    {
        return new ZipArchive(path, access);
    }

    IZipArchive * TryOpen(const utf8 * path, ZIP_ACCESS access)
    {
        IZipArchive * result = nullptr;
        try
        {
            result = new ZipArchive(path, access);
        }
        catch (Exception)
        {
        }
        return result;
    }
}

# endif
