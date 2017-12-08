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
#include "../world/Footpath.h"
#include "FootpathObject.h"
#include "ObjectJsonHelpers.h"

void FootpathObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(10, STREAM_SEEK_CURRENT);
    _legacyType.support_type = stream->ReadValue<uint8>();
    _legacyType.flags = stream->ReadValue<uint8>();
    _legacyType.scrolling_mode = stream->ReadValue<uint8>();
    stream->Seek(1, STREAM_SEEK_CURRENT);

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_NAME);
    GetImageTable()->Read(context, stream);

    // Validate properties
    if (_legacyType.support_type >= FOOTPATH_ENTRY_SUPPORT_TYPE_COUNT)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "SUPPORT_TYPE not supported.");
    }
}

void FootpathObject::Load()
{
    GetStringTable()->Sort();
    _legacyType.string_idx = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable()->GetImages(), GetImageTable()->GetCount());
    _legacyType.bridge_image = _legacyType.image + 109;
}

void FootpathObject::Unload()
{
    language_free_object_string(_legacyType.string_idx);
    gfx_object_free_images(_legacyType.image, GetImageTable()->GetCount());

    _legacyType.string_idx = 0;
    _legacyType.image = 0;
}

void FootpathObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    sint32 x = width / 2;
    sint32 y = height / 2;
    gfx_draw_sprite(dpi, _legacyType.image + 71, x - 49, y - 17, 0);
    gfx_draw_sprite(dpi, _legacyType.image + 72, x + 4, y - 17, 0);
}

static uint8 ParseSupportType(const std::string &s)
{
    if (s == "pole") return FOOTPATH_ENTRY_SUPPORT_TYPE_POLE;
    else /* if (s == "box") */ return FOOTPATH_ENTRY_SUPPORT_TYPE_BOX;
}

void FootpathObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    // Strings
    auto stringTable = GetStringTable();
    auto jsonStrings = json_object_get(root, "strings");
    auto jsonName = json_object_get(jsonStrings, "name");
    stringTable->SetString(0, 0, json_string_value(json_object_get(jsonName, "en-GB")));

    auto properties = json_object_get(root, "properties");
    _legacyType.support_type = ParseSupportType(ObjectJsonHelpers::GetString(json_object_get(properties, "supportType")));
    _legacyType.scrolling_mode = json_integer_value(json_object_get(properties, "scrollingMode"));

    // Flags
    _legacyType.flags = 0;
    if (json_boolean_value(json_object_get(properties, "hasSupportImages")))
    {
        _legacyType.flags |= FOOTPATH_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE;
    }
    if (json_boolean_value(json_object_get(properties, "hasElevatedPathImages")))
    {
        _legacyType.flags |= FOOTPATH_ENTRY_FLAG_HAS_PATH_BASE_SPRITE;
    }
    if (json_boolean_value(json_object_get(properties, "editorOnly")))
    {
        _legacyType.flags |= FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR;
    }

    auto imageTable = GetImageTable();
    ObjectJsonHelpers::LoadImages(root, *imageTable);
}
