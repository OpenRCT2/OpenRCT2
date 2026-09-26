
/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectAsset.h"

#include "../core/File.h"
#include "../core/FileStream.h"
#include "../core/Zip.h"
#include "../core/ZipStream.hpp"

namespace OpenRCT2
{
    bool ObjectAsset::IsAvailable() const
    {
        if (_zipPath.empty())
        {
            return File::Exists(_path);
        }

        auto zipArchive = Zip::TryOpen(_zipPath, ZipAccess::read);
        return zipArchive != nullptr && zipArchive->Exists(_path);
    }

    uint64_t ObjectAsset::GetSize() const
    {
        if (_zipPath.empty())
        {
            return File::GetSize(_path);
        }

        auto zipArchive = Zip::TryOpen(_zipPath, ZipAccess::read);
        if (zipArchive != nullptr)
        {
            auto index = zipArchive->GetIndexFromPath(_path);
            if (index.has_value())
            {
                auto size = zipArchive->GetFileSize(index.value());
                return size;
            }
        }
        return 0;
    }

    std::vector<uint8_t> ObjectAsset::GetData() const
    {
        if (_zipPath.empty())
        {
            return File::ReadAllBytes(_path);
        }

        auto zipArchive = Zip::TryOpen(_zipPath, ZipAccess::read);
        if (zipArchive != nullptr)
        {
            return zipArchive->GetFileData(_path);
        }
        return {};
    }

    std::unique_ptr<IStream> ObjectAsset::GetStream() const
    {
        try
        {
            if (_zipPath.empty())
            {
                return std::make_unique<FileStream>(_path, FileMode::open);
            }

            auto zipArchive = Zip::TryOpen(_zipPath, ZipAccess::read);
            if (zipArchive != nullptr)
            {
                auto stream = zipArchive->GetFileStream(_path);
                if (stream != nullptr)
                {
                    return std::make_unique<ZipStreamWrapper>(std::move(zipArchive), std::move(stream));
                }
            }
        }
        catch (...)
        {
        }
        return {};
    }

    const std::string& ObjectAsset::GetZipPath() const
    {
        return _zipPath;
    }

    const std::string& ObjectAsset::GetPath() const
    {
        return _path;
    }

    size_t ObjectAsset::GetHash() const
    {
        // Combine hashes of zipPath and path
        std::hash<std::string> hasher;
        auto h1 = hasher(_zipPath);
        auto h2 = hasher(_path);
        // Combine the hashes based on example from https://en.cppreference.com/w/cpp/utility/hash.html
        return h1 ^ (h2 << 1);
    }
}
