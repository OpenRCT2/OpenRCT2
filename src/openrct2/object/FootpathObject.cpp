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
#include "../world/Footpath.h"
#include "FootpathObject.h"
#include "ObjectJsonHelpers.h"

void FootpathObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(10, STREAM_SEEK_CURRENT);
    _legacyType.support_type = stream->ReadValue<uint8_t>();
    _legacyType.flags = stream->ReadValue<uint8_t>();
    _legacyType.scrolling_mode = stream->ReadValue<uint8_t>();
    stream->Seek(1, STREAM_SEEK_CURRENT);

    GetStringTable().Read(context, stream, OBJ_STRING_ID_NAME);
    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.support_type >= FOOTPATH_ENTRY_SUPPORT_TYPE_COUNT)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "SUPPORT_TYPE not supported.");
    }
}

void FootpathObject::Load()
{
    GetStringTable().Sort();
    _legacyType.string_idx = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
    _legacyType.bridge_image = _legacyType.image + 109;
}

void FootpathObject::Unload()
{
    language_free_object_string(_legacyType.string_idx);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.string_idx = 0;
    _legacyType.image = 0;
}

void FootpathObject::DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const
{
    int32_t x = width / 2;
    int32_t y = height / 2;
    gfx_draw_sprite(dpi, _legacyType.image + 71, x - 49, y - 17, 0);
    gfx_draw_sprite(dpi, _legacyType.image + 72, x + 4, y - 17, 0);
}

static uint8_t ParseSupportType(const std::string &s)
{
    if (s == "pole") return FOOTPATH_ENTRY_SUPPORT_TYPE_POLE;
    else /* if (s == "box") */ return FOOTPATH_ENTRY_SUPPORT_TYPE_BOX;
}

void FootpathObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");
    _legacyType.support_type = ParseSupportType(ObjectJsonHelpers::GetString(json_object_get(properties, "supportType")));
    _legacyType.scrolling_mode = json_integer_value(json_object_get(properties, "scrollingMode"));

    // Flags
    _legacyType.flags = ObjectJsonHelpers::GetFlags<uint8_t>(properties, {
        { "hasSupportImages", FOOTPATH_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE },
        { "hasElevatedPathImages", FOOTPATH_ENTRY_FLAG_HAS_PATH_BASE_SPRITE },
        { "editorOnly", FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR } });

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}
