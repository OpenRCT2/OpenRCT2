/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include <unordered_map>
#include "../Context.h"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Language.h"
#include "../peep/Staff.h"
#include "ObjectJsonHelpers.h"
#include "ObjectManager.h"
#include "ObjectRepository.h"
#include "SceneryGroupObject.h"

using namespace OpenRCT2;

void SceneryGroupObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    stream->Seek(0x80 * 2, STREAM_SEEK_CURRENT);
    _legacyType.entry_count = stream->ReadValue<uint8_t>();
    _legacyType.pad_107 = stream->ReadValue<uint8_t>();
    _legacyType.priority = stream->ReadValue<uint8_t>();
    _legacyType.pad_109 = stream->ReadValue<uint8_t>();
    _legacyType.entertainer_costumes = stream->ReadValue<uint32_t>();

    GetStringTable().Read(context, stream, OBJ_STRING_ID_NAME);
    _items = ReadItems(stream);
    GetImageTable().Read(context, stream);
}

void SceneryGroupObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
    _legacyType.entry_count = 0;
}

void SceneryGroupObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void SceneryGroupObject::DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const
{
    int32_t x = width / 2;
    int32_t y = height / 2;

    uint32_t imageId = _legacyType.image + 0x20600001;
    gfx_draw_sprite(dpi, imageId, x - 15, y - 14, 0);
}

void SceneryGroupObject::UpdateEntryIndexes()
{
    auto context = GetContext();
    auto objectRepository = context->GetObjectRepository();
    auto objectManager = context->GetObjectManager();

    _legacyType.entry_count = 0;
    for (const auto &objectEntry : _items)
    {
        auto ori = objectRepository->FindObject(&objectEntry);
        if (ori == nullptr) continue;
        if (ori->LoadedObject == nullptr) continue;

        uint16_t sceneryEntry = objectManager->GetLoadedObjectEntryIndex(ori->LoadedObject);
        Guard::Assert(sceneryEntry != UINT8_MAX, GUARD_LINE);

        auto objectType = ori->ObjectEntry.flags & 0x0F;
        switch (objectType) {
        case OBJECT_TYPE_SMALL_SCENERY:                        break;
        case OBJECT_TYPE_PATH_BITS:     sceneryEntry |= 0x100; break;
        case OBJECT_TYPE_WALLS:         sceneryEntry |= 0x200; break;
        case OBJECT_TYPE_LARGE_SCENERY: sceneryEntry |= 0x300; break;
        case OBJECT_TYPE_BANNERS:       sceneryEntry |= 0x400; break;
        default:                        sceneryEntry = 0xFFFF; break;
        }
        if (sceneryEntry != 0xFFFF)
        {
            _legacyType.scenery_entries[_legacyType.entry_count] = sceneryEntry;
            _legacyType.entry_count++;
        }
    }
}

void SceneryGroupObject::SetRepositoryItem(ObjectRepositoryItem * item) const
{
    item->SceneryGroupInfo.Entries = _items;
}

std::vector<rct_object_entry> SceneryGroupObject::ReadItems(IStream * stream)
{
    auto items = std::vector<rct_object_entry>();
    while (stream->ReadValue<uint8_t>() != 0xFF)
    {
        stream->Seek(-1, STREAM_SEEK_CURRENT);
        auto entry = stream->ReadValue<rct_object_entry>();
        items.push_back(entry);
    }
    return items;
}

void SceneryGroupObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");
    _legacyType.priority = json_integer_value(json_object_get(properties, "priority"));

    // Entertainer cosumes
    auto jCostumes = json_object_get(properties, "entertainerCostumes");
    if (jCostumes != nullptr)
    {
        _legacyType.entertainer_costumes = ReadJsonEntertainerCostumes(jCostumes);
    }

    auto jEntries = json_object_get(properties, "entries");
    if (jEntries != nullptr)
    {
        _items = ReadJsonEntries(jEntries);
    }

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}

uint32_t SceneryGroupObject::ReadJsonEntertainerCostumes(const json_t * jCostumes)
{
    uint32_t costumes = 0;
    auto szCostumes = ObjectJsonHelpers::GetJsonStringArray(jCostumes);
    for (const auto& szCostume : szCostumes)
    {
        auto entertainer = ParseEntertainerCostume(szCostume);

        // For some reason the flags are +4 from the actual costume IDs
        // See staff_get_available_entertainer_costumes
        costumes |= 1 << (entertainer + 4);
    }
    return costumes;
}

uint32_t SceneryGroupObject::ParseEntertainerCostume(const std::string &s)
{
    if (s == "panda")       return ENTERTAINER_COSTUME_PANDA;
    if (s == "tiger")       return ENTERTAINER_COSTUME_TIGER;
    if (s == "elephant")    return ENTERTAINER_COSTUME_ELEPHANT;
    if (s == "roman")       return ENTERTAINER_COSTUME_ROMAN;
    if (s == "gorilla")     return ENTERTAINER_COSTUME_GORILLA;
    if (s == "snowman")     return ENTERTAINER_COSTUME_SNOWMAN;
    if (s == "knight")      return ENTERTAINER_COSTUME_KNIGHT;
    if (s == "astronaut")   return ENTERTAINER_COSTUME_ASTRONAUT;
    if (s == "bandit")      return ENTERTAINER_COSTUME_BANDIT;
    if (s == "sheriff")     return ENTERTAINER_COSTUME_SHERIFF;
    if (s == "pirate")      return ENTERTAINER_COSTUME_PIRATE;
    return ENTERTAINER_COSTUME_PANDA;
}

std::vector<rct_object_entry> SceneryGroupObject::ReadJsonEntries(const json_t * jEntries)
{
    std::vector<rct_object_entry> entries;
    size_t index;
    json_t * jEntry;
    json_array_foreach(jEntries, index, jEntry)
    {
        auto entryId = json_string_value(jEntry);
        if (entryId != nullptr)
        {
            auto entry = ObjectJsonHelpers::ParseObjectEntry(entryId);
            entries.push_back(entry);
        }
    }
    return entries;
}
