/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Zip.h"

#include "IStream.hpp"

#include <algorithm>
#ifndef __ANDROID__
#    include <zip.h>
#endif

using namespace OpenRCT2;

static std::string NormalisePath(std::string_view path)
{
    std::string result;
    if (!path.empty())
    {
        result.reserve(path.size());
        for (auto ch : path)
        {
            if (ch == '\\')
            {
                result += '/';
            }
            else
            {
                result += ch;
            }
        }
    }
    return result;
}

/**
 * Normalises both the given path and the stored paths and finds the first match.
 */
std::optional<size_t> IZipArchive::GetIndexFromPath(std::string_view path) const
{
    auto normalisedPath = NormalisePath(path);
    if (!normalisedPath.empty())
    {
        auto numFiles = GetNumFiles();
        for (size_t i = 0; i < numFiles; i++)
        {
            auto normalisedZipPath = NormalisePath(GetFileName(i));
            if (normalisedZipPath == normalisedPath)
            {
                return i;
            }
        }
    }
    return std::nullopt;
}

bool IZipArchive::Exists(std::string_view path) const
{
    return GetIndexFromPath(path).has_value();
}

#ifndef __ANDROID__

class ZipArchive final : public IZipArchive
{
private:
    zip_t* _zip;
    ZIP_ACCESS _access;
    std::vector<std::vector<uint8_t>> _writeBuffers;

public:
    ZipArchive(std::string_view path, ZIP_ACCESS access)
    {
        auto zipOpenMode = ZIP_RDONLY;
        if (access == ZIP_ACCESS::WRITE)
        {
            zipOpenMode = ZIP_CREATE;
        }

        int32_t error;
        _zip = zip_open(std::string(path).c_str(), zipOpenMode, &error);
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
        return zip_get_num_entries(_zip, 0);
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

    uint64_t GetFileSize(size_t index) const override
    {
        zip_stat_t zipFileStat;
        if (zip_stat_index(_zip, index, 0, &zipFileStat) == ZIP_ER_OK)
        {
            return zipFileStat.size;
        }

        return 0;
    }

    std::vector<uint8_t> GetFileData(std::string_view path) const override
    {
        std::vector<uint8_t> result;
        auto index = GetIndexFromPath(path);
        if (index.has_value())
        {
            auto dataSize = GetFileSize(index.value());
            if (dataSize > 0 && dataSize < SIZE_MAX)
            {
                auto zipFile = zip_fopen_index(_zip, index.value(), 0);
                if (zipFile != nullptr)
                {
                    result.resize(static_cast<size_t>(dataSize));
                    uint64_t readBytes = zip_fread(zipFile, result.data(), dataSize);
                    if (readBytes != dataSize)
                    {
                        result = {};
                    }
                    zip_fclose(zipFile);
                }
            }
        }
        return result;
    }

    std::unique_ptr<IStream> GetFileStream(std::string_view path) const override
    {
        auto index = GetIndexFromPath(path);
        if (index.has_value())
        {
            return std::make_unique<ZipItemStream>(_zip, index.value());
        }
        return {};
    }

    void SetFileData(std::string_view path, std::vector<uint8_t>&& data) override
    {
        // Push buffer to an internal list as libzip requires access to it until the zip
        // handle is closed.
        _writeBuffers.push_back(std::move(data));
        const auto& writeBuffer = *_writeBuffers.rbegin();

        auto source = zip_source_buffer(_zip, writeBuffer.data(), writeBuffer.size(), 0);
        auto index = GetIndexFromPath(path);
        if (index.has_value())
        {
            zip_replace(_zip, index.value(), source);
        }
        else
        {
            zip_add(_zip, path.data(), source);
        }
    }

    void DeleteFile(std::string_view path) override
    {
        auto index = GetIndexFromPath(path);
        if (index.has_value())
        {
            zip_delete(_zip, index.value());
        }
        else
        {
            throw std::runtime_error("File does not exist.");
        }
    }

    void RenameFile(std::string_view path, std::string_view newPath) override
    {
        auto index = GetIndexFromPath(path);
        if (index)
        {
            zip_file_rename(_zip, *index, newPath.data(), ZIP_FL_ENC_GUESS);
        }
        else
        {
            throw std::runtime_error("File does not exist.");
        }
    }

private:
    class ZipItemStream final : public IStream
    {
    private:
        zip* _zip;
        zip_int64_t _index;
        zip_file_t* _zipFile{};
        zip_uint64_t _len{};
        zip_uint64_t _pos{};

    public:
        ZipItemStream(zip* zip, zip_int64_t index)
            : _zip(zip)
            , _index(index)
        {
        }

        ~ZipItemStream() override
        {
            Close();
        }

        bool CanRead() const override
        {
            return true;
        }

        bool CanWrite() const override
        {
            return false;
        }

        uint64_t GetLength() const override
        {
            return _len;
        }

        uint64_t GetPosition() const override
        {
            return _pos;
        }

        void SetPosition(uint64_t position) override
        {
            if (position > _pos)
            {
                // Read to seek forwards
                Skip(position - _pos);
            }
            else if (position < _pos)
            {
                // Can not seek backwards, start from the beginning
                Reset();
                Skip(position);
            }
        }

        void Seek(int64_t offset, int32_t origin) override
        {
            switch (origin)
            {
                case STREAM_SEEK_BEGIN:
                    SetPosition(offset);
                    break;
                case STREAM_SEEK_CURRENT:
                    SetPosition(_pos + offset);
                    break;
                case STREAM_SEEK_END:
                    SetPosition(_len - offset);
                    break;
            }
        }

        void Read(void* buffer, uint64_t length) override
        {
            size_t readBytes = TryRead(buffer, length);
            if (readBytes != length)
            {
                throw IOException("Attempted to read past end of file.");
            }
        }

        void Write(const void* buffer, uint64_t length) override
        {
            throw IOException("Stream is read-only.");
        }

        uint64_t TryRead(void* buffer, uint64_t length) override
        {
            if (_zipFile == nullptr && !Reset())
            {
                return 0;
            }

            auto readBytes = zip_fread(_zipFile, buffer, length);
            if (readBytes < 0)
            {
                return 0;
            }

            _pos += readBytes;
            return static_cast<uint64_t>(readBytes);
        }

        const void* GetData() const override
        {
            return nullptr;
        }

    private:
        void Close()
        {
            if (_zipFile != nullptr)
            {
                zip_fclose(_zipFile);
                _zipFile = nullptr;
            }
        }

        bool Reset()
        {
            Close();

            _pos = 0;
            _len = 0;
            _zipFile = zip_fopen_index(_zip, _index, 0);
            if (_zipFile == nullptr)
            {
                return false;
            }

            zip_stat_t zipFileStat{};
            if (zip_stat_index(_zip, _index, 0, &zipFileStat) != ZIP_ER_OK)
            {
                return false;
            }

            _len = zipFileStat.size;
            return true;
        }

        void Skip(zip_int64_t len)
        {
            // zip_fseek can not be used on compressed data, so skip bytes by
            // reading into a temporary buffer
            char buffer[2048]{};
            while (len > 0)
            {
                auto readLen = std::min<zip_int64_t>(len, sizeof(buffer));
                auto read = zip_fread(_zipFile, buffer, readLen);
                if (read <= 0)
                {
                    break;
                }
                _pos += read;
                len -= read;
            }
        }
    };
};

namespace Zip
{
    std::unique_ptr<IZipArchive> Open(std::string_view path, ZIP_ACCESS access)
    {
        return std::make_unique<ZipArchive>(path, access);
    }

    std::unique_ptr<IZipArchive> TryOpen(std::string_view path, ZIP_ACCESS access)
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

#endif
