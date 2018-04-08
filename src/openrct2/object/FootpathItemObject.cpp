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

#include <unordered_map>
#include "../core/IStream.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Cursors.h"
#include "../localisation/Localisation.h"
#include "../object/Object.h"
#include "ObjectList.h"
#include "FootpathItemObject.h"
#include "ObjectJsonHelpers.h"

void FootpathItemObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.path_bit.flags = stream->ReadValue<uint16>();
    _legacyType.path_bit.draw_type = stream->ReadValue<uint8>();
    _legacyType.path_bit.tool_id = stream->ReadValue<uint8>();
    _legacyType.path_bit.price = stream->ReadValue<sint16>();
    _legacyType.path_bit.scenery_tab_id = stream->ReadValue<uint8>();
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

    // Add path bits to 'Signs and items for footpaths' group, rather than lumping them in the Miscellaneous tab.
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

void FootpathItemObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());

    _legacyType.path_bit.scenery_tab_id = 0xFF;
}

void FootpathItemObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void FootpathItemObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    sint32 x = width / 2;
    sint32 y = height / 2;
    gfx_draw_sprite(dpi, _legacyType.image, x - 22, y - 24, 0);
}

static uint8 ParseDrawType(const std::string &s)
{
    if (s == "lamp") return PATH_BIT_DRAW_TYPE_LIGHTS;
    if (s == "bin") return PATH_BIT_DRAW_TYPE_BINS;
    if (s == "bench") return PATH_BIT_DRAW_TYPE_BENCHES;
    if (s == "fountain") return PATH_BIT_DRAW_TYPE_JUMPING_FOUNTAINS;
    return PATH_BIT_DRAW_TYPE_LIGHTS;
}

void FootpathItemObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");
    _legacyType.path_bit.draw_type = ParseDrawType(ObjectJsonHelpers::GetString(properties, "renderAs"));
    _legacyType.path_bit.tool_id = ObjectJsonHelpers::ParseCursor(ObjectJsonHelpers::GetString(properties, "cursor"), CURSOR_LAMPPOST_DOWN);
    _legacyType.path_bit.price = json_integer_value(json_object_get(properties, "price"));

    SetPrimarySceneryGroup(ObjectJsonHelpers::GetString(json_object_get(properties, "sceneryGroup")));

    // Flags
    _legacyType.path_bit.flags = ObjectJsonHelpers::GetFlags<uint16>(properties, {
        { "isBin", PATH_BIT_FLAG_IS_BIN },
        { "isBench", PATH_BIT_FLAG_IS_BENCH },
        { "isBreakable", PATH_BIT_FLAG_BREAKABLE },
        { "isLamp", PATH_BIT_FLAG_LAMP },
        { "isJumpingFountainWater", PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER },
        { "isJumpingFountainSnow", PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW },
        { "isAllowedOnQueue", PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE },
        { "isAllowedOnSlope", PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE },
        { "isTelevision", PATH_BIT_FLAG_IS_QUEUE_SCREEN }});

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}
