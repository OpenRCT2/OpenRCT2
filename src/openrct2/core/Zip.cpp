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

#ifndef __ANDROID__
#include <zip.h>
#include "IStream.hpp"
#include "Zip.h"

class ZipArchive final : public IZipArchive
{
private:
    zip_t * _zip;
    ZIP_ACCESS _access;
    std::vector<std::vector<uint8>> _writeBuffers;

public:
    ZipArchive(const std::string_view& path, ZIP_ACCESS access)
    {
        auto zipOpenMode = ZIP_RDONLY;
        if (access == ZIP_ACCESS::WRITE)
        {
            zipOpenMode = ZIP_CREATE;
        }

        sint32 error;
        _zip = zip_open(path.data(), zipOpenMode, &error);
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

    std::string GetFileName(size_t index) const override
    {
        std::string result;
        auto name = zip_get_name(_zip, index, ZIP_FL_ENC_GUESS);
        if (name != nullptr)
        {
            result = name;
        }
        return result;
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

    std::vector<uint8> GetFileData(const std::string_view& path) const override
    {
        std::vector<uint8> result;
        auto index = (size_t)zip_name_locate(_zip, path.data(), 0);
        auto dataSize = GetFileSize(index);
        if (dataSize > 0 && dataSize < SIZE_MAX)
        {
            auto zipFile = zip_fopen(_zip, path.data(), 0);
            if (zipFile != nullptr)
            {
                result.resize((size_t)dataSize);
                uint64 readBytes = zip_fread(zipFile, result.data(), dataSize);
                if (readBytes != dataSize)
                {
                    result.clear();
                    result.shrink_to_fit();
                }
                zip_fclose(zipFile);
            }
        }
        return result;
    }

    void SetFileData(const std::string_view& path, std::vector<uint8>&& data) override
    {
        // Push buffer to an internal list as libzip requires access to it until the zip
        // handle is closed.
        _writeBuffers.push_back(std::move(data));
        const auto& writeBuffer = *_writeBuffers.rbegin();

        auto source = zip_source_buffer(_zip, writeBuffer.data(), writeBuffer.size(), 0);
        auto index = zip_name_locate(_zip, path.data(), 0);
        if (index == -1)
        {
            zip_add(_zip, path.data(), source);
        }
        else
        {
            zip_replace(_zip, index, source);
        }
    }

    void DeleteFile(const std::string_view& path) override
    {
        auto index = zip_name_locate(_zip, path.data(), 0);
        zip_delete(_zip, index);
    }

    void RenameFile(const std::string_view& path, const std::string_view& newPath) override
    {
        auto index = zip_name_locate(_zip, path.data(), 0);
        zip_file_rename(_zip, index, newPath.data(), ZIP_FL_ENC_GUESS);
    }
};

namespace Zip
{
    std::unique_ptr<IZipArchive> Open(const std::string_view& path, ZIP_ACCESS access)
    {
        return std::make_unique<ZipArchive>(path, access);
    }

    std::unique_ptr<IZipArchive> TryOpen(const std::string_view& path, ZIP_ACCESS access)
    {
        std::unique_ptr<IZipArchive> result;
        try
        {
            result = std::make_unique<ZipArchive>(path, access);
        }
        catch (const std::exception&)
        {
        }
        return result;
    }
} // namespace Zip

# endif
