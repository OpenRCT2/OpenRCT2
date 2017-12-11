#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../core/IStream.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Language.h"
#include "../object/Object.h"
#include "BannerObject.h"
#include "ObjectJsonHelpers.h"
#include "ObjectList.h"

void BannerObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.banner.scrolling_mode = stream->ReadValue<uint8>();
    _legacyType.banner.flags = stream->ReadValue<uint8>();
    _legacyType.banner.price = stream->ReadValue<sint16>();
    _legacyType.banner.scenery_tab_id = stream->ReadValue<uint8>();
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
    const std::string identifier = GetIdentifier();
    const rct_object_entry * objectEntry = object_list_find_by_name(identifier.c_str());
    static const rct_object_entry scgPathX = Object::GetScgPathXHeader();

    if (objectEntry != nullptr &&
            (object_entry_get_source_game(objectEntry) == OBJECT_SOURCE_WACKY_WORLDS ||
             object_entry_get_source_game(objectEntry) == OBJECT_SOURCE_TIME_TWISTER ||
             object_entry_get_source_game(objectEntry) == OBJECT_SOURCE_CUSTOM))
    {
        SetPrimarySceneryGroup(&scgPathX);
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

void BannerObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    sint32 x = width / 2;
    sint32 y = height / 2;

    uint32 imageId = 0x20D00000 | _legacyType.image;
    gfx_draw_sprite(dpi, imageId + 0, x - 12, y + 8, 0);
    gfx_draw_sprite(dpi, imageId + 1, x - 12, y + 8, 0);
}

void BannerObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");

    _legacyType.banner.scrolling_mode = json_integer_value(json_object_get(properties, "scrollingMode"));
    _legacyType.banner.price = json_integer_value(json_object_get(properties, "price"));
    _legacyType.banner.flags = ObjectJsonHelpers::GetFlags<uint8>(properties, {
        { "hasPrimaryColour", BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR }});

    SetPrimarySceneryGroup(ObjectJsonHelpers::GetString(json_object_get(properties, "sceneryGroup")));

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(root, GetImageTable());
}
