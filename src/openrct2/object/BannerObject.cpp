/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/IStream.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Language.h"
#include "../object/Object.h"
#include "../object/ObjectRepository.h"
#include "BannerObject.h"
#include "ObjectJsonHelpers.h"
#include "ObjectList.h"

void BannerObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.banner.scrolling_mode = stream->ReadValue<uint8_t>();
    _legacyType.banner.flags = stream->ReadValue<uint8_t>();
    _legacyType.banner.price = stream->ReadValue<int16_t>();
    _legacyType.banner.scenery_tab_id = stream->ReadValue<uint8_t>();
    stream->Seek(1, STREAM_SEEK_CURRENT);

    GetStringTable().Read(context, stream, OBJ_STRING_ID_NAME);

    rct_object_entry sgEntry = stream->ReadValue<rct_object_entry>();
    SetPrimarySceneryGroup(&sgEntry);

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.large_scenery.price <= 0)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Price can not be free or negative.");
    }

    // Add banners to 'Signs and items for footpaths' group, rather than lumping them in the Miscellaneous tab.
    // Since this is already done the other way round for original items, avoid adding those to prevent duplicates.
    auto identifier = GetIdentifier();

    auto& objectRepository = context->GetObjectRepository();
    auto item = objectRepository.FindObject(identifier);
    if (item != nullptr)
    {
        auto objectEntry = &item->ObjectEntry;
        if (object_entry_get_source_game(objectEntry) == OBJECT_SOURCE_WACKY_WORLDS ||
            object_entry_get_source_game(objectEntry) == OBJECT_SOURCE_TIME_TWISTER ||
            object_entry_get_source_game(objectEntry) == OBJECT_SOURCE_CUSTOM)
        {
            auto scgPathX = Object::GetScgPathXHeader();
            SetPrimarySceneryGroup(&scgPathX);
        }
    }
}

void BannerObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
}

void BannerObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void BannerObject::DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const
{
    int32_t x = width / 2;
    int32_t y = height / 2;

    uint32_t imageId = 0x20D00000 | _legacyType.image;
    gfx_draw_sprite(dpi, imageId + 0, x - 12, y + 8, 0);
    gfx_draw_sprite(dpi, imageId + 1, x - 12, y + 8, 0);
}

void BannerObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");

    _legacyType.banner.scrolling_mode = json_integer_value(json_object_get(properties, "scrollingMode"));
    _legacyType.banner.price = json_integer_value(json_object_get(properties, "price"));
    _legacyType.banner.flags = ObjectJsonHelpers::GetFlags<uint8_t>(properties, {
        { "hasPrimaryColour", BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR }});

    SetPrimarySceneryGroup(ObjectJsonHelpers::GetString(json_object_get(properties, "sceneryGroup")));

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}
