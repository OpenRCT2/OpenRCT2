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
#include "ObjectRepository.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

using namespace OpenRCT2;

ObjectType& operator++(ObjectType& d, int)
{
    return d = (d == ObjectType::Count) ? ObjectType::Ride : static_cast<ObjectType>(static_cast<uint8_t>(d) + 1);
}

ObjectEntryDescriptor::ObjectEntryDescriptor(const rct_object_entry& newEntry)
{
    if (!newEntry.IsEmpty())
    {
        Generation = ObjectGeneration::DAT;
        Entry = newEntry;
    }
}

ObjectEntryDescriptor::ObjectEntryDescriptor(std::string_view newIdentifier)
{
    Generation = ObjectGeneration::JSON;
    Identifier = std::string(newIdentifier);
}

ObjectEntryDescriptor::ObjectEntryDescriptor(ObjectType type, std::string_view newIdentifier)
{
    Generation = ObjectGeneration::JSON;
    Identifier = std::string(newIdentifier);
    Type = type;
}

ObjectEntryDescriptor::ObjectEntryDescriptor(const ObjectRepositoryItem& ori)
{
    if (!ori.Identifier.empty())
    {
        Generation = ObjectGeneration::JSON;
        Identifier = std::string(ori.Identifier);
    }
    else
    {
        Generation = ObjectGeneration::DAT;
        Entry = ori.ObjectEntry;
    }
}

bool ObjectEntryDescriptor::HasValue() const
{
    return Generation != ObjectGeneration::JSON || !Identifier.empty();
};

ObjectType ObjectEntryDescriptor::GetType() const
{
    return Generation == ObjectGeneration::JSON ? Type : Entry.GetType();
}

std::string_view ObjectEntryDescriptor::GetName() const
{
    return Generation == ObjectGeneration::JSON ? Identifier : Entry.GetName();
}

bool ObjectEntryDescriptor::operator==(const ObjectEntryDescriptor& rhs) const
{
    if (Generation != rhs.Generation)
        return false;
    if (Generation == ObjectGeneration::DAT)
    {
        return Entry == rhs.Entry;
    }
    else
    {
        return Type == rhs.Type && Identifier == rhs.Identifier;
    }
}

bool ObjectEntryDescriptor::operator!=(const ObjectEntryDescriptor& rhs) const
{
    return !(*this == rhs);
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

bool rct_object_entry::IsEmpty() const
{
    uint64_t a, b;
    std::memcpy(&a, reinterpret_cast<const uint8_t*>(this), 8);
    std::memcpy(&b, reinterpret_cast<const uint8_t*>(this) + 8, 8);

    if (a == 0xFFFFFFFFFFFFFFFF && b == 0xFFFFFFFFFFFFFFFF)
        return true;
    if (a == 0 && b == 0)
        return true;
    return false;
}

bool rct_object_entry::operator==(const rct_object_entry& rhs) const
{
    const auto a = this;
    const auto b = &rhs;

    // If an official object don't bother checking checksum
    if ((a->flags & 0xF0) || (b->flags & 0xF0))
    {
        if (a->GetType() != b->GetType())
        {
            return false;
        }
        int32_t match = memcmp(a->name, b->name, 8);
        if (match)
        {
            return false;
        }
    }
    else
    {
        if (a->flags != b->flags)
        {
            return false;
        }
        int32_t match = memcmp(a->name, b->name, 8);
        if (match)
        {
            return false;
        }
        if (a->checksum != b->checksum)
        {
            return false;
        }
    }
    return true;
}

bool rct_object_entry::operator!=(const rct_object_entry& rhs) const
{
    return !(*this == rhs);
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
