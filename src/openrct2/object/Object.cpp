/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../core/ZipStream.hpp"
#include "../drawing/Image.h"
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

ObjectEntryDescriptor::ObjectEntryDescriptor(const RCTObjectEntry& newEntry)
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

std::string ObjectEntryDescriptor::ToString() const
{
    if (Generation == ObjectGeneration::DAT)
    {
        char buffer[32];
        std::snprintf(&buffer[0], 9, "%08X", Entry.flags);
        buffer[8] = '|';
        std::memcpy(&buffer[9], Entry.name, 8);
        buffer[17] = '|';
        std::snprintf(&buffer[18], 9, "%8X", Entry.checksum);
        return std::string(buffer);
    }
    else
    {
        return std::string(GetName());
    }
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
    _usesFallbackImages = _imageTable.ReadJson(context, root);
}

std::string Object::GetOverrideString(uint8_t index) const
{
    auto legacyIdentifier = GetLegacyIdentifier();
    const auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    auto stringId = localisationService.GetObjectOverrideStringId(legacyIdentifier, index);

    const utf8* result = nullptr;
    if (stringId != STR_NONE)
    {
        result = LanguageGetString(stringId);
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

ObjectEntryDescriptor Object::GetScgPathXHeader() const
{
    return ObjectEntryDescriptor("rct2.scenery_group.scgpathx");
}

RCTObjectEntry Object::CreateHeader(const char name[DAT_NAME_LENGTH + 1], uint32_t flags, uint32_t checksum)
{
    RCTObjectEntry header = {};
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

ImageIndex Object::LoadImages()
{
    if (_baseImageId == ImageIndexUndefined)
    {
        _baseImageId = GfxObjectAllocateImages(GetImageTable().GetImages(), GetImageTable().GetCount());
    }
    return _baseImageId;
}

void Object::UnloadImages()
{
    if (_baseImageId != ImageIndexUndefined)
    {
        GfxObjectFreeImages(_baseImageId, GetImageTable().GetCount());
        _baseImageId = ImageIndexUndefined;
    }
}

void RCTObjectEntry::SetName(std::string_view value)
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

bool Object::IsCompatibilityObject() const
{
    return _isCompatibilityObject;
}
void Object::SetIsCompatibilityObject(const bool on)
{
    _isCompatibilityObject = on;
}

bool RCTObjectEntry::IsEmpty() const
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

bool RCTObjectEntry::operator==(const RCTObjectEntry& rhs) const
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

bool RCTObjectEntry::operator!=(const RCTObjectEntry& rhs) const
{
    return !(*this == rhs);
}

bool ObjectAsset::IsAvailable() const
{
    if (_zipPath.empty())
    {
        return File::Exists(_path);
    }

    auto zipArchive = Zip::TryOpen(_zipPath, ZIP_ACCESS::READ);
    return zipArchive != nullptr && zipArchive->Exists(_path);
}

uint64_t ObjectAsset::GetSize() const
{
    if (_zipPath.empty())
    {
        return File::GetSize(_path);
    }

    auto zipArchive = Zip::TryOpen(_zipPath, ZIP_ACCESS::READ);
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

    auto zipArchive = Zip::TryOpen(_zipPath, ZIP_ACCESS::READ);
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
            return std::make_unique<FileStream>(_path, FILE_MODE_OPEN);
        }

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
    catch (...)
    {
    }
    return {};
}

u8string VersionString(const ObjectVersion& version)
{
    return std::to_string(std::get<0>(version)) + "." + std::to_string(std::get<1>(version)) + "."
        + std::to_string(std::get<2>(version));
}

ObjectVersion VersionTuple(std::string_view version)
{
    if (version.empty())
    {
        return std::make_tuple(0, 0, 0);
    }

    auto nums = String::Split(version, ".");
    uint16_t versions[VersionNumFields] = {};
    if (nums.size() > VersionNumFields)
    {
        LOG_WARNING("%i fields found in version string '%s', expected X.Y.Z", nums.size(), version);
    }
    if (nums.size() == 0)
    {
        LOG_WARNING("No fields found in version string '%s', expected X.Y.Z", version);
        return std::make_tuple(0, 0, 0);
    }
    try
    {
        size_t highestIndex = std::min(nums.size(), VersionNumFields);
        for (size_t i = 0; i < highestIndex; i++)
        {
            auto value = stoi(nums.at(i));
            constexpr auto maxValue = std::numeric_limits<uint16_t>().max();
            if (value > maxValue)
            {
                LOG_WARNING(
                    "Version value too high in version string '%s', version value will be capped to %i.", version, maxValue);
                value = maxValue;
            }
            versions[i] = value;
        }
    }
    catch (const std::exception&)
    {
        LOG_WARNING("Malformed version string '%s', expected X.Y.Z", version);
    }

    return std::make_tuple(versions[0], versions[1], versions[2]);
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif
