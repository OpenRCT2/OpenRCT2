/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "SceneryGroupObject.h"

#include "../Context.h"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../entity/Staff.h"
#include "../localisation/Language.h"
#include "../world/Scenery.h"
#include "ObjectLimits.h"
#include "ObjectManager.h"
#include "ObjectRepository.h"

#include <unordered_map>

using namespace OpenRCT2;

// Example entry: "$DAT:09F55406|00STBEN "
// 5 for $DAT:, 8 for the checksum, 1 for the vertical bar, 8 for the .DAT name.
static constexpr uint8_t kDatEntryPrefixLength = 5;
static constexpr uint8_t kDatEntryFlagsLength = 8;
static constexpr uint8_t kDatEntrySeparatorLength = 1;
static constexpr uint8_t kDatEntryLength = kDatEntryPrefixLength + kDatEntryFlagsLength + kDatEntrySeparatorLength
    + kDatNameLength;
static constexpr uint8_t kDatEntryFlagsStart = kDatEntryPrefixLength;
static constexpr uint8_t kDatEntryNameStart = kDatEntryPrefixLength + kDatEntryFlagsLength + kDatEntrySeparatorLength;

void SceneryGroupObject::ReadLegacy(IReadObjectContext* context, IStream* stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    stream->Seek(0x80 * 2, STREAM_SEEK_CURRENT);
    stream->Seek(1, STREAM_SEEK_CURRENT); // entry_count
    stream->Seek(1, STREAM_SEEK_CURRENT); // Pad107;
    _legacyType.priority = stream->ReadValue<uint8_t>();
    stream->Seek(1, STREAM_SEEK_CURRENT); // Pad109;
    _legacyType.entertainer_costumes = stream->ReadValue<uint32_t>();

    GetStringTable().Read(context, stream, ObjectStringID::NAME);
    _items = ReadItems(stream);
    GetImageTable().Read(context, stream);
}

void SceneryGroupObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = LanguageAllocateObjectString(GetName());
    _legacyType.image = LoadImages();
    _legacyType.SceneryEntries.clear();
}

void SceneryGroupObject::Unload()
{
    LanguageFreeObjectString(_legacyType.name);
    UnloadImages();

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void SceneryGroupObject::DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };

    const auto imageId = ImageId(_legacyType.image + 1, COLOUR_DARK_GREEN);
    GfxDrawSprite(dpi, imageId, screenCoords - ScreenCoordsXY{ 15, 14 });
}

static std::optional<uint8_t> GetSceneryType(const ObjectType type)
{
    switch (type)
    {
        case ObjectType::SmallScenery:
            return SCENERY_TYPE_SMALL;
        case ObjectType::LargeScenery:
            return SCENERY_TYPE_LARGE;
        case ObjectType::Walls:
            return SCENERY_TYPE_WALL;
        case ObjectType::Banners:
            return SCENERY_TYPE_BANNER;
        case ObjectType::PathAdditions:
            return SCENERY_TYPE_PATH_ITEM;
        default:
            return std::nullopt;
    }
}

void SceneryGroupObject::UpdateEntryIndexes()
{
    auto context = GetContext();
    auto& objectRepository = context->GetObjectRepository();
    auto& objectManager = context->GetObjectManager();

    _legacyType.SceneryEntries.clear();
    for (const auto& objectEntry : _items)
    {
        auto ori = objectRepository.FindObject(objectEntry);
        if (ori == nullptr)
            continue;
        if (ori->LoadedObject == nullptr)
            continue;

        auto entryIndex = objectManager.GetLoadedObjectEntryIndex(ori->LoadedObject.get());
        if (entryIndex == kObjectEntryIndexNull)
        {
            // Some parks have manually deleted objects from the save so they might not be loaded
            // silently remove the object from the SceneryGroupObject
            continue;
        }

        auto sceneryType = GetSceneryType(ori->Type);
        if (sceneryType.has_value())
        {
            _legacyType.SceneryEntries.push_back({ sceneryType.value(), entryIndex });
        }
    }
}

void SceneryGroupObject::SetRepositoryItem(ObjectRepositoryItem* item) const
{
    item->SceneryGroupInfo.Entries = _items;
}

std::vector<ObjectEntryDescriptor> SceneryGroupObject::ReadItems(IStream* stream)
{
    auto items = std::vector<ObjectEntryDescriptor>();
    while (stream->ReadValue<uint8_t>() != 0xFF)
    {
        stream->Seek(-1, STREAM_SEEK_CURRENT);
        auto entry = stream->ReadValue<RCTObjectEntry>();
        items.emplace_back(entry);
    }
    return items;
}

void SceneryGroupObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "SceneryGroupObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];

    if (properties.is_object())
    {
        _legacyType.priority = Json::GetNumber<uint8_t>(properties["priority"], 40);
        _legacyType.entertainer_costumes = 0;

        _items = ReadJsonEntries(context, properties["entries"]);
    }

    PopulateTablesFromJson(context, root);
}

std::vector<ObjectEntryDescriptor> SceneryGroupObject::ReadJsonEntries(IReadObjectContext* context, json_t& jEntries)
{
    std::vector<ObjectEntryDescriptor> entries;

    for (const auto& jEntry : jEntries)
    {
        auto entryName = Json::GetString(jEntry);
        if (String::startsWith(entryName, "$DAT:"))
        {
            if (entryName.length() != kDatEntryLength)
            {
                std::string errorMessage = "Malformed DAT entry in scenery group: " + entryName;
                context->LogError(ObjectError::InvalidProperty, errorMessage.c_str());
                continue;
            }

            try
            {
                RCTObjectEntry entry = {};
                entry.flags = std::stoul(entryName.substr(kDatEntryFlagsStart, kDatEntryFlagsLength), nullptr, 16);
                std::memcpy(entry.name, entryName.c_str() + kDatEntryNameStart, kDatNameLength);
                entry.checksum = 0;
                entries.emplace_back(entry);
            }
            catch (std::invalid_argument&)
            {
                std::string errorMessage = "Malformed flags in DAT entry in scenery group: " + entryName;
                context->LogError(ObjectError::InvalidProperty, errorMessage.c_str());
            }
        }
        else
        {
            entries.emplace_back(entryName);
        }
    }
    return entries;
}

uint16_t SceneryGroupObject::GetNumIncludedObjects() const
{
    return static_cast<uint16_t>(_items.size());
}

const std::vector<ObjectEntryDescriptor>& SceneryGroupObject::GetItems() const
{
    return _items;
}
