#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include <zip.h>
#include "IStream.hpp"
#include "Zip.h"

class ZipArchive : public IZipArchive
{
private:
    zip_t * _zip;

public:
    ZipArchive(const utf8 * path)
    {
        int error;
        _zip = zip_open(path, ZIP_RDONLY, &error);
        if (_zip == nullptr)
        {
            throw IOException("Unable to open zip file.");
        }
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

    const uint64 GetFileSize(size_t index) const override
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
                data = malloc(dataSize);
                size_t readBytes = zip_fread(zipFile, data, dataSize);
                if (readBytes != dataSize)
                {
                    free(data);
                    data = nullptr;
                    dataSize = 0;
                }
                zip_fclose(zipFile);
            }
        }

        if (outSize != nullptr) *outSize = dataSize;
        return data;
    }
};

namespace Zip
{
    IZipArchive * Open(const utf8 * path)
    {
        return new ZipArchive(path);
    }

    IZipArchive * TryOpen(const utf8 * path)
    {
        IZipArchive * result = nullptr;
        try
        {
            result = new ZipArchive(path);
        }
        catch (Exception)
        {
        }
        return result;
    }
}
