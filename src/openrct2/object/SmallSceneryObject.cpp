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

#include "../core/IStream.hpp"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Cursors.h"
#include "../localisation/Language.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "SmallSceneryObject.h"
#include "ObjectJsonHelpers.h"

void SmallSceneryObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.small_scenery.flags = stream->ReadValue<uint32>();
    _legacyType.small_scenery.height = stream->ReadValue<uint8>();
    _legacyType.small_scenery.tool_id = stream->ReadValue<uint8>();
    _legacyType.small_scenery.price = stream->ReadValue<sint16>();
    _legacyType.small_scenery.removal_price = stream->ReadValue<sint16>();
    stream->Seek(4, STREAM_SEEK_CURRENT);
    _legacyType.small_scenery.animation_delay = stream->ReadValue<uint16>();
    _legacyType.small_scenery.animation_mask = stream->ReadValue<uint16>();
    _legacyType.small_scenery.num_frames = stream->ReadValue<uint16>();
    _legacyType.small_scenery.scenery_tab_id = 0xFF;

    GetStringTable().Read(context, stream, OBJ_STRING_ID_NAME);

    rct_object_entry sgEntry = stream->ReadValue<rct_object_entry>();
    SetPrimarySceneryGroup(&sgEntry);

    if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
    {
        _frameOffsets = ReadFrameOffsets(stream);
    }

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.small_scenery.price <= 0)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Price can not be free or negative.");
    }
    if (_legacyType.small_scenery.removal_price <= 0)
    {
        // Make sure you don't make a profit when placing then removing.
        money16 reimbursement = _legacyType.small_scenery.removal_price;
        if (reimbursement > _legacyType.small_scenery.price)
        {
            context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Sell price can not be more than buy price.");
        }
    }
}

void SmallSceneryObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());

    _legacyType.small_scenery.scenery_tab_id = 0xFF;

    if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
    {
        _legacyType.small_scenery.frame_offsets = _frameOffsets.data();
    }

    PerformFixes();
}

void SmallSceneryObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void SmallSceneryObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    uint32 imageId = _legacyType.image;
    if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
    {
        imageId |= 0x20D00000;
        if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
        {
            imageId |= 0x92000000;
        }
    }

    sint32 x = width / 2;
    sint32 y = (height / 2) + (_legacyType.small_scenery.height / 2);
    y = Math::Min(y, height - 16);

    if ((scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_FULL_TILE)) &&
        (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_VOFFSET_CENTRE)))
    {
        y -= 12;
    }

    gfx_draw_sprite(dpi, imageId, x, y, 0);

    if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_GLASS))
    {
        imageId = _legacyType.image + 0x44500004;
        if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
        {
            imageId |= 0x92000000;
        }
        gfx_draw_sprite(dpi, imageId, x, y, 0);
    }

    if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_ANIMATED_FG))
    {
        imageId = _legacyType.image + 4;
        if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
        {
            imageId |= 0x92000000;
        }
        gfx_draw_sprite(dpi, imageId, x, y, 0);
    }
}

std::vector<uint8> SmallSceneryObject::ReadFrameOffsets(IStream * stream)
{
    uint8 frameOffset;
    auto data = std::vector<uint8>();
    data.push_back(stream->ReadValue<uint8>());
    while ((frameOffset = stream->ReadValue<uint8>()) != 0xFF)
    {
        data.push_back(frameOffset);
    }
    data.push_back(frameOffset);
    return data;
}

// clang-format off
void SmallSceneryObject::PerformFixes()
{
    std::string identifier = GetIdentifier();
    static const rct_object_entry scgWalls = Object::GetScgWallsHeader();

    // ToonTowner's base blocks. Make them allow supports on top and put them in the Walls and Roofs group.
    if (String::Equals(identifier, "XXBBCL01") ||
        String::Equals(identifier, "XXBBMD01") ||
        String::Equals(identifier, "XXBBBR01") ||
        String::Equals(identifier, "ARBASE2 "))
    {
        SetPrimarySceneryGroup(&scgWalls);

        _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_BUILD_DIRECTLY_ONTOP;
    }

    // ToonTowner's regular roofs. Put them in the Walls and Roofs group.
    if (String::Equals(identifier, "TTRFTL02") ||
        String::Equals(identifier, "TTRFTL03") ||
        String::Equals(identifier, "TTRFTL04") ||
        String::Equals(identifier, "TTRFTL07") ||
        String::Equals(identifier, "TTRFTL08"))
    {
        SetPrimarySceneryGroup(&scgWalls);
    }

    // ToonTowner's Pirate roofs. Make them show up in the Pirate Theming.
    if (String::Equals(identifier, "TTPIRF02") ||
        String::Equals(identifier, "TTPIRF03") ||
        String::Equals(identifier, "TTPIRF04") ||
        String::Equals(identifier, "TTPIRF05") ||
        String::Equals(identifier, "TTPIRF07") ||
        String::Equals(identifier, "TTPIRF08") ||
        String::Equals(identifier, "TTPRF09 ") ||
        String::Equals(identifier, "TTPRF10 ") ||
        String::Equals(identifier, "TTPRF11 "))
    {
        static const rct_object_entry scgPirat = GetScgPiratHeader();
        SetPrimarySceneryGroup(&scgPirat);
    }

    // ToonTowner's wooden roofs. Make them show up in the Mine Theming.
    if (String::Equals(identifier, "TTRFWD01") ||
        String::Equals(identifier, "TTRFWD02") ||
        String::Equals(identifier, "TTRFWD03") ||
        String::Equals(identifier, "TTRFWD04") ||
        String::Equals(identifier, "TTRFWD05") ||
        String::Equals(identifier, "TTRFWD06") ||
        String::Equals(identifier, "TTRFWD07") ||
        String::Equals(identifier, "TTRFWD08"))
    {
        static const rct_object_entry scgMine = GetScgMineHeader();
        SetPrimarySceneryGroup(&scgMine);
    }

    // ToonTowner's glass roofs. Make them show up in the Abstract Theming.
    if (String::Equals(identifier, "TTRFGL01") ||
        String::Equals(identifier, "TTRFGL02") ||
        String::Equals(identifier, "TTRFGL03"))
    {
        static const rct_object_entry scgAbstr = GetScgAbstrHeader();
        SetPrimarySceneryGroup(&scgAbstr);
    }
}
// clang-format on

rct_object_entry SmallSceneryObject::GetScgPiratHeader()
{
    return Object::CreateHeader("SCGPIRAT", 169381767, 132382977);
}

rct_object_entry SmallSceneryObject::GetScgMineHeader()
{
    return Object::CreateHeader("SCGMINE ", 207140231, 3638141733);
}

rct_object_entry SmallSceneryObject::GetScgAbstrHeader()
{
    return Object::CreateHeader("SCGABSTR", 207140231, 932253451);
}

void SmallSceneryObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");

    _legacyType.small_scenery.height = json_integer_value(json_object_get(properties, "height"));
    _legacyType.small_scenery.tool_id = ObjectJsonHelpers::ParseCursor(ObjectJsonHelpers::GetString(properties, "cursor"), CURSOR_STATUE_DOWN);
    _legacyType.small_scenery.price = json_integer_value(json_object_get(properties, "price"));
    _legacyType.small_scenery.removal_price = json_integer_value(json_object_get(properties, "removalPrice"));
    _legacyType.small_scenery.animation_delay = json_integer_value(json_object_get(properties, "animationDelay"));
    _legacyType.small_scenery.animation_mask = json_integer_value(json_object_get(properties, "animationMask"));
    _legacyType.small_scenery.num_frames = json_integer_value(json_object_get(properties, "numFrames"));

    // Flags
    _legacyType.small_scenery.flags = ObjectJsonHelpers::GetFlags<uint32>(properties, {
        { "SMALL_SCENERY_FLAG_VOFFSET_CENTRE", SMALL_SCENERY_FLAG_VOFFSET_CENTRE },
        { "requiresFlatSurface", SMALL_SCENERY_FLAG_REQUIRE_FLAT_SURFACE },
        { "isRotatable", SMALL_SCENERY_FLAG_ROTATABLE },
        { "isAnimated", SMALL_SCENERY_FLAG_ANIMATED },
        { "canWither", SMALL_SCENERY_FLAG_CAN_WITHER },
        { "canBeWatered", SMALL_SCENERY_FLAG_CAN_BE_WATERED },
        { "hasOverlayImage", SMALL_SCENERY_FLAG_ANIMATED_FG },
        { "hasGlass", SMALL_SCENERY_FLAG_HAS_GLASS },
        { "hasPrimaryColour", SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR },
        { "SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1", SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1 },
        { "SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4", SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4 },
        { "isClock", SMALL_SCENERY_FLAG_IS_CLOCK },
        { "SMALL_SCENERY_FLAG_SWAMP_GOO", SMALL_SCENERY_FLAG_SWAMP_GOO },
        { "SMALL_SCENERY_FLAG17", SMALL_SCENERY_FLAG17 },
        { "isStackable", SMALL_SCENERY_FLAG_STACKABLE },
        { "prohibitWalls", SMALL_SCENERY_FLAG_NO_WALLS },
        { "hasSecondaryColour", SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR },
        { "hasNoSupports", SMALL_SCENERY_FLAG_NO_SUPPORTS },
        { "SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED", SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED },
        { "SMALL_SCENERY_FLAG_COG", SMALL_SCENERY_FLAG_COG },
        { "allowSupportsAbove", SMALL_SCENERY_FLAG_BUILD_DIRECTLY_ONTOP },
        { "supportsHavePrimaryColour", SMALL_SCENERY_FLAG_PAINT_SUPPORTS },
        { "SMALL_SCENERY_FLAG27", SMALL_SCENERY_FLAG27 } });

    // Determine shape flags from a shape string
    auto shape = ObjectJsonHelpers::GetString(properties, "shape");
    if (!shape.empty())
    {
        auto quarters = shape.substr(0, 3);
        if (quarters == "2/4")
        {
            _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_FULL_TILE | SMALL_SCENERY_FLAG_HALF_SPACE;
        }
        else if (quarters == "3/4")
        {
            _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_FULL_TILE | SMALL_SCENERY_FLAG_THREE_QUARTERS;
        }
        else if (quarters == "4/4")
        {
            _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_FULL_TILE;
        }
        if (shape.size() >= 5)
        {
            if ((shape.substr(3) == "+D"))
            {
                _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_DIAGONAL;
            }
        }
    }

    auto jFrameOffsets = json_object_get(properties, "frameOffsets");
    if (jFrameOffsets != nullptr)
    {
        _frameOffsets = ReadJsonFrameOffsets(jFrameOffsets);
        _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS;
    }

    SetPrimarySceneryGroup(ObjectJsonHelpers::GetString(json_object_get(properties, "sceneryGroup")));

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(root, GetImageTable());
}

std::vector<uint8> SmallSceneryObject::ReadJsonFrameOffsets(const json_t * jFrameOffsets)
{
    std::vector<uint8> offsets;
    size_t index;
    const json_t * jOffset;
    json_array_foreach(jFrameOffsets, index, jOffset)
    {
        offsets.push_back(json_integer_value(jOffset));
    }
    return offsets;
}
