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

#pragma warning(disable : 4706) // assignment within conditional expression

#include <algorithm>
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Util.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Cursors.h"
#include "../localisation/Language.h"
#include "LargeSceneryObject.h"
#include "ObjectJsonHelpers.h"

void LargeSceneryObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.large_scenery.tool_id = stream->ReadValue<uint8>();
    _legacyType.large_scenery.flags = stream->ReadValue<uint8>();
    _legacyType.large_scenery.price = stream->ReadValue<sint16>();
    _legacyType.large_scenery.removal_price = stream->ReadValue<sint16>();
    stream->Seek(5, STREAM_SEEK_CURRENT);
    _legacyType.large_scenery.scenery_tab_id = 0xFF;
    _legacyType.large_scenery.scrolling_mode = stream->ReadValue<uint8>();
    stream->Seek(4, STREAM_SEEK_CURRENT);

    GetStringTable().Read(context, stream, OBJ_STRING_ID_NAME);

    rct_object_entry sgEntry = stream->ReadValue<rct_object_entry>();
    SetPrimarySceneryGroup(&sgEntry);

    if (_legacyType.large_scenery.flags & LARGE_SCENERY_FLAG_3D_TEXT)
    {
        _3dFont = std::make_unique<rct_large_scenery_text>();
        stream->Read(_3dFont.get());
        _legacyType.large_scenery.text = _3dFont.get();
    }

    _tiles = ReadTiles(stream);

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.large_scenery.price <= 0)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Price can not be free or negative.");
    }
    if (_legacyType.large_scenery.removal_price <= 0)
    {
        // Make sure you don't make a profit when placing then removing.
        money16 reimbursement = _legacyType.large_scenery.removal_price;
        if (reimbursement > _legacyType.large_scenery.price)
        {
            context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Sell price can not be more than buy price.");
        }
    }
}

void LargeSceneryObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _baseImageId = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
    _legacyType.image = _baseImageId;

    _legacyType.large_scenery.tiles = _tiles.data();

    if (_legacyType.large_scenery.flags & LARGE_SCENERY_FLAG_3D_TEXT)
    {
        _legacyType.large_scenery.text_image = _legacyType.image;
        if (_3dFont->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL)
        {
            _legacyType.image += _3dFont->num_images * 2;
        }
        else
        {
            _legacyType.image += _3dFont->num_images * 4;
        }
        _legacyType.large_scenery.text = _3dFont.get();
    }
}

void LargeSceneryObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_baseImageId, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void LargeSceneryObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    sint32 x = width / 2;
    sint32 y = (height / 2) - 39;

    uint32 imageId = 0xB2D00000 | _legacyType.image;
    gfx_draw_sprite(dpi, imageId, x, y, 0);
}

std::vector<rct_large_scenery_tile> LargeSceneryObject::ReadTiles(IStream * stream)
{
    auto tiles = std::vector<rct_large_scenery_tile>();
    while (stream->ReadValue<uint16>() != 0xFFFF)
    {
        stream->Seek(-2, STREAM_SEEK_CURRENT);
        auto tile = stream->ReadValue<rct_large_scenery_tile>();
        tiles.push_back(tile);
    }
    tiles.push_back({ -1, -1, -1, 255, 0xFFFF });
    return tiles;
}

void LargeSceneryObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");

    _legacyType.large_scenery.tool_id = ObjectJsonHelpers::ParseCursor(ObjectJsonHelpers::GetString(properties, "cursor"), CURSOR_STATUE_DOWN);
    _legacyType.large_scenery.price = json_integer_value(json_object_get(properties, "price"));
    _legacyType.large_scenery.removal_price = json_integer_value(json_object_get(properties, "height"));

    auto jScrollingMode = json_object_get(properties, "scrollingMode");
    _legacyType.large_scenery.scrolling_mode = jScrollingMode != nullptr ?
        json_integer_value(jScrollingMode) :
        -1;

    // Flags
    _legacyType.large_scenery.flags = ObjectJsonHelpers::GetFlags<uint8>(properties, {
        { "hasPrimaryColour", LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR },
        { "hasSecondaryColour", LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR },
        { "isAnimated", LARGE_SCENERY_FLAG_ANIMATED },
        { "isPhotogenic", LARGE_SCENERY_FLAG_PHOTOGENIC } });

    // Tiles
    auto jTiles = json_object_get(properties, "tiles");
    if (jTiles != nullptr)
    {
        _tiles = ReadJsonTiles(jTiles);
    }

    // Read text
    auto j3dFont = json_object_get(properties, "3dFont");
    if (j3dFont != nullptr)
    {
        _3dFont = ReadJson3dFont(j3dFont);
        _legacyType.large_scenery.flags |= LARGE_SCENERY_FLAG_3D_TEXT;
    }

    SetPrimarySceneryGroup(ObjectJsonHelpers::GetString(json_object_get(properties, "sceneryGroup")));

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(root, GetImageTable());
}

std::vector<rct_large_scenery_tile> LargeSceneryObject::ReadJsonTiles(const json_t * jTiles)
{
    std::vector<rct_large_scenery_tile> tiles;
    size_t index;
    const json_t * jTile;
    json_array_foreach(jTiles, index, jTile)
    {
        rct_large_scenery_tile tile = { 0 };
        tile.x_offset = json_integer_value(json_object_get(jTile, "x"));
        tile.y_offset = json_integer_value(json_object_get(jTile, "y"));
        tile.z_offset = json_integer_value(json_object_get(jTile, "z"));
        tile.z_clearance = json_integer_value(json_object_get(jTile, "clearance"));
        if (!ObjectJsonHelpers::GetBoolean(jTile, "hasSupports"))
        {
            tile.flags |= LARGE_SCENERY_TILE_FLAG_NO_SUPPORTS;
        }
        if (ObjectJsonHelpers::GetBoolean(jTile, "allowSupportsAbove"))
        {
            tile.flags |= LARGE_SCENERY_TILE_FLAG_ALLOW_SUPPORTS_ABOVE;
        }

        // All corners are by default occupied
        auto jCorners = json_object_get(jTile, "corners");
        auto corners = 0xF;
        if (jCorners != nullptr)
        {
            corners = json_integer_value(jCorners);
        }
        tile.flags |= (corners & 0xFF) << 12;

        auto walls = json_integer_value(json_object_get(jTile, "walls"));
        tile.flags |= (walls & 0xFF) << 8;

        tiles.push_back(tile);
    }

    // HACK Add end of tiles marker
    //      We should remove this later by improving the code base to use tiles array length
    tiles.push_back({ -1, -1, -1, 0xFF, 0xFFFF });

    return tiles;
}

std::unique_ptr<rct_large_scenery_text> LargeSceneryObject::ReadJson3dFont(const json_t * j3dFont)
{
    auto font = std::make_unique<rct_large_scenery_text>();

    auto jOffsets = json_object_get(j3dFont, "offsets");
    if (jOffsets != nullptr)
    {
        auto offsets = ReadJsonOffsets(jOffsets);
        auto numOffsets = std::min(Util::CountOf(font->offset), offsets.size());
        std::copy_n(offsets.data(), numOffsets, font->offset);
    }

    font->max_width = json_integer_value(json_object_get(j3dFont, "maxWidth"));
    font->num_images = json_integer_value(json_object_get(j3dFont, "numImages"));
    font->flags = ObjectJsonHelpers::GetFlags<uint8>(j3dFont, {
        { "isVertical", LARGE_SCENERY_TEXT_FLAG_VERTICAL },
        { "isTwoLine", LARGE_SCENERY_TEXT_FLAG_TWO_LINE } });

    auto jGlyphs = json_object_get(j3dFont, "glyphs");
    if (jGlyphs != nullptr)
    {
        auto glyphs = ReadJsonGlyphs(jGlyphs);
        auto numGlyphs = std::min(Util::CountOf(font->glyphs), glyphs.size());
        std::copy_n(glyphs.data(), numGlyphs, font->glyphs);
    }

    return font;
}

std::vector<LocationXY16> LargeSceneryObject::ReadJsonOffsets(const json_t * jOffsets)
{
    std::vector<LocationXY16> offsets;
    size_t index;
    const json_t * jOffset;
    json_array_foreach(jOffsets, index, jOffset)
    {
        LocationXY16 offset = { 0 };
        offset.x = json_integer_value(json_object_get(jOffset, "x"));
        offset.y = json_integer_value(json_object_get(jOffset, "y"));
        offsets.push_back(offset);
    }
    return offsets;
}

std::vector<rct_large_scenery_text_glyph> LargeSceneryObject::ReadJsonGlyphs(const json_t * jGlpyhs)
{
    std::vector<rct_large_scenery_text_glyph> glyphs;
    size_t index;
    const json_t * jGlyph;
    json_array_foreach(jGlpyhs, index, jGlyph)
    {
        rct_large_scenery_text_glyph glyph;
        glyph.image_offset = json_integer_value(json_object_get(jGlyph, "image"));
        glyph.width = json_integer_value(json_object_get(jGlyph, "width"));
        glyph.height = json_integer_value(json_object_get(jGlyph, "height"));
        glyphs.push_back(glyph);
    }
    return glyphs;
}
