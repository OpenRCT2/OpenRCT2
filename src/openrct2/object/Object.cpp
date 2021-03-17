/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Object.h"

#include "../Context.h"
#include "../core/File.h"
#include "../core/FileStream.h"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../core/Zip.h"
#include "../localisation/Language.h"
#include "../localisation/LocalisationService.h"
#include "../localisation/StringIds.h"
#include "../world/Scenery.h"
#include "ObjectLimits.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

using namespace OpenRCT2;

ObjectType& operator++(ObjectType& d, int)
{
    return d = (d == ObjectType::Count) ? ObjectType::Ride : static_cast<ObjectType>(static_cast<uint8_t>(d) + 1);
}

Object::Object(const rct_object_entry& entry)
{
    _objectEntry = entry;
}

void* Object::GetLegacyData()
{
    throw std::runtime_error("Not supported.");
}

void Object::ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    throw std::runtime_error("Not supported.");
}

void Object::PopulateTablesFromJson(IReadObjectContext* context, json_t& root)
{
    _stringTable.ReadJson(root);
    _imageTable.ReadJson(context, root);
}

rct_object_entry Object::ParseObjectEntry(const std::string& s)
{
    rct_object_entry entry = {};
    std::fill_n(entry.name, sizeof(entry.name), ' ');
    auto copyLen = std::min<size_t>(8, s.size());
    std::copy_n(s.c_str(), copyLen, entry.name);
    return entry;
}

std::string Object::GetOverrideString(uint8_t index) const
{
    auto legacyIdentifier = GetLegacyIdentifier();
    const auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    auto stringId = localisationService.GetObjectOverrideStringId(legacyIdentifier, index);

    const utf8* result = nullptr;
    if (stringId != STR_NONE)
    {
        result = language_get_string(stringId);
    }
    return String::ToStd(result);
}

std::string Object::GetString(ObjectStringID index) const
{
    auto sz = GetOverrideString(static_cast<uint8_t>(index));
    if (sz.empty())
    {
        sz = GetStringTable().GetString(index);
    }
    return sz;
}

std::string Object::GetString(int32_t language, ObjectStringID index) const
{
    return GetStringTable().GetString(language, index);
}

ObjectEntryDescriptor Object::GetScgWallsHeader() const
{
    return ObjectEntryDescriptor("rct2.scgwalls");
}

ObjectEntryDescriptor Object::GetScgPathXHeader() const
{
    return ObjectEntryDescriptor("rct2.scgpathx");
}

rct_object_entry Object::CreateHeader(const char name[DAT_NAME_LENGTH + 1], uint32_t flags, uint32_t checksum)
{
    rct_object_entry header = {};
    header.flags = flags;
    std::copy_n(name, DAT_NAME_LENGTH, header.name);
    header.checksum = checksum;
    return header;
}

std::vector<ObjectSourceGame> Object::GetSourceGames()
{
    return _sourceGames;
}

void Object::SetSourceGames(const std::vector<ObjectSourceGame>& sourceGames)
{
    _sourceGames = sourceGames;
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#endif

std::string Object::GetName() const
{
    return GetString(ObjectStringID::NAME);
}

std::string Object::GetName(int32_t language) const
{
    return GetString(language, ObjectStringID::NAME);
}

void rct_object_entry::SetName(std::string_view value)
{
    std::memset(name, ' ', sizeof(name));
    std::memcpy(name, value.data(), std::min(sizeof(name), value.size()));
}

const std::vector<std::string>& Object::GetAuthors() const
{
    return _authors;
}

void Object::SetAuthors(std::vector<std::string>&& authors)
{
    _authors = std::move(authors);
}

std::optional<uint8_t> rct_object_entry::GetSceneryType() const
{
    switch (GetType())
    {
        case ObjectType::SmallScenery:
            return SCENERY_TYPE_SMALL;
        case ObjectType::LargeScenery:
            return SCENERY_TYPE_LARGE;
        case ObjectType::Walls:
            return SCENERY_TYPE_WALL;
        case ObjectType::Banners:
            return SCENERY_TYPE_BANNER;
        case ObjectType::PathBits:
            return SCENERY_TYPE_PATH_ITEM;
        default:
            return std::nullopt;
    }
}

/**
 * Couples a zip archive and a zip item stream to ensure the lifetime of the zip archive is maintained
 * for the lifetime of the stream.
 */
class ZipStreamWrapper final : public IStream
{
private:
    std::unique_ptr<IZipArchive> _zipArchive;
    std::unique_ptr<IStream> _base;

public:
    ZipStreamWrapper(std::unique_ptr<IZipArchive> zipArchive, std::unique_ptr<IStream> base)
        : _zipArchive(std::move(zipArchive))
        , _base(std::move(base))
    {
    }

    bool CanRead() const override
    {
        return _base->CanRead();
    }

    bool CanWrite() const override
    {
        return _base->CanWrite();
    }

    uint64_t GetLength() const override
    {
        return _base->GetLength();
    }

    uint64_t GetPosition() const override
    {
        return _base->GetPosition();
    }

    void SetPosition(uint64_t position) override
    {
        _base->SetPosition(position);
    }

    void Seek(int64_t offset, int32_t origin) override
    {
        _base->Seek(offset, origin);
    }

    void Read(void* buffer, uint64_t length) override
    {
        _base->Read(buffer, length);
    }

    void Write(const void* buffer, uint64_t length) override
    {
        _base->Write(buffer, length);
    }

    uint64_t TryRead(void* buffer, uint64_t length) override
    {
        return _base->TryRead(buffer, length);
    }

    const void* GetData() const override
    {
        return _base->GetData();
    }
};

bool ObjectAsset::IsAvailable() const
{
    if (_zipPath.empty())
    {
        return File::Exists(_path);
    }
    else
    {
        auto zipArchive = Zip::TryOpen(_zipPath, ZIP_ACCESS::READ);
        return zipArchive != nullptr && zipArchive->Exists(_path);
    }
}

size_t ObjectAsset::GetSize() const
{
    if (_zipPath.empty())
    {
        try
        {
            return File::ReadAllBytes(_path).size();
        }
        catch (...)
        {
            return 0;
        }
    }
    else
    {
        auto zipArchive = Zip::TryOpen(_zipPath, ZIP_ACCESS::READ);
        if (zipArchive != nullptr)
        {
            auto index = zipArchive->GetIndexFromPath(_path);
            if (index)
            {
                auto size = zipArchive->GetFileSize(*index);
                return size;
            }
        }
    }
    return 0;
}

std::unique_ptr<IStream> ObjectAsset::GetStream() const
{
    if (_zipPath.empty())
    {
        return std::make_unique<FileStream>(_path, FILE_MODE_OPEN);
    }
    else
    {
        auto zipArchive = Zip::TryOpen(_zipPath, ZIP_ACCESS::READ);
        if (zipArchive != nullptr)
        {
            auto stream = zipArchive->GetFileStream(_path);
            if (stream != nullptr)
            {
                return std::make_unique<ZipStreamWrapper>(std::move(zipArchive), std::move(stream));
            }
        }
    }
    return {};
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif
