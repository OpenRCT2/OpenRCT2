/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <unordered_map>
#include "../core/IStream.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Cursors.h"
#include "../localisation/Localisation.h"
#include "../object/Object.h"
#include "../object/ObjectRepository.h"
#include "ObjectList.h"
#include "FootpathItemObject.h"
#include "ObjectJsonHelpers.h"

void FootpathItemObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.path_bit.flags = stream->ReadValue<uint16_t>();
    _legacyType.path_bit.draw_type = stream->ReadValue<uint8_t>();
    _legacyType.path_bit.tool_id = stream->ReadValue<uint8_t>();
    _legacyType.path_bit.price = stream->ReadValue<int16_t>();
    _legacyType.path_bit.scenery_tab_id = stream->ReadValue<uint8_t>();
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
    auto identifier = GetIdentifier();

    auto& objectRepository = context->GetObjectRepository();
    auto item = objectRepository.FindObject(identifier);
    if (item != nullptr)
    {
        auto sourceGame = object_entry_get_source_game(&item->ObjectEntry);
        if (sourceGame == OBJECT_SOURCE_WACKY_WORLDS ||
            sourceGame == OBJECT_SOURCE_TIME_TWISTER ||
            sourceGame == OBJECT_SOURCE_CUSTOM)
        {
            auto scgPathX = Object::GetScgPathXHeader();
            SetPrimarySceneryGroup(&scgPathX);
        }
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

void FootpathItemObject::DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const
{
    int32_t x = width / 2;
    int32_t y = height / 2;
    gfx_draw_sprite(dpi, _legacyType.image, x - 22, y - 24, 0);
}

static uint8_t ParseDrawType(const std::string &s)
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
    _legacyType.path_bit.flags = ObjectJsonHelpers::GetFlags<uint16_t>(properties, {
        { "isBin", PATH_BIT_FLAG_IS_BIN },
        { "isBench", PATH_BIT_FLAG_IS_BENCH },
        { "isBreakable", PATH_BIT_FLAG_BREAKABLE },
        { "isLamp", PATH_BIT_FLAG_LAMP },
        { "isJumpingFountainWater", PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER },
        { "isJumpingFountainSnow", PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW },
        { "isTelevision", PATH_BIT_FLAG_IS_QUEUE_SCREEN }});

    // HACK To avoid 'negated' properties in JSON, handle these separately until
    //      flags are inverted in this code base.
    if (!ObjectJsonHelpers::GetBoolean(properties, "isAllowedOnQueue", false))
    {
        _legacyType.path_bit.flags |= PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE;
    }
    if (!ObjectJsonHelpers::GetBoolean(properties, "isAllowedOnSlope", false))
    {
        _legacyType.path_bit.flags |= PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE;
    }

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}
