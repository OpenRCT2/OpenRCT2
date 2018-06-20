/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "EntranceObject.h"
#include "ObjectJsonHelpers.h"

void EntranceObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.scrolling_mode = stream->ReadValue<uint8_t>();
    _legacyType.text_height = stream->ReadValue<uint8_t>();

    GetStringTable().Read(context, stream, OBJ_STRING_ID_NAME);
    GetImageTable().Read(context, stream);

    // Fix issue #1705: The Medieval entrance from Time Twister has a straight banner,
    // but scrolls its text as if it a curved one.
    if (String::Equals(GetIdentifier(), "MEDIENTR"))
    {
        _legacyType.scrolling_mode = 32;
        _legacyType.text_height += 1;
    }
}

void EntranceObject::Load()
{
    GetStringTable().Sort();
    _legacyType.string_idx = language_allocate_object_string(GetName());
    _legacyType.image_id = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
}

void EntranceObject::Unload()
{
    language_free_object_string(_legacyType.string_idx);
    gfx_object_free_images(_legacyType.image_id, GetImageTable().GetCount());

    _legacyType.string_idx = 0;
    _legacyType.image_id = 0;
}

void EntranceObject::DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const
{
    int32_t x = width / 2;
    int32_t y = height / 2;

    uint32_t imageId = _legacyType.image_id;
    gfx_draw_sprite(dpi, imageId + 1, x - 32, y + 14, 0);
    gfx_draw_sprite(dpi, imageId + 0, x +  0, y + 28, 0);
    gfx_draw_sprite(dpi, imageId + 2, x + 32, y + 44, 0);
}

void EntranceObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");
    _legacyType.scrolling_mode = json_integer_value(json_object_get(properties, "scrollingMode"));
    _legacyType.text_height = json_integer_value(json_object_get(properties, "textHeight"));

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}
