#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "SmallSceneryObject.h"

extern "C"
{
    #include "../drawing/drawing.h"
    #include "../localisation/localisation.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

SmallSceneryObject::~SmallSceneryObject()
{
    Memory::Free(_var10data);
}

void SmallSceneryObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    _legacyType.name = stream->ReadValue<rct_string_id>();
    _legacyType.image = stream->ReadValue<uint32>();

    _legacyType.small_scenery.flags = stream->ReadValue<uint32>();
    _legacyType.small_scenery.height = stream->ReadValue<uint8>();
    _legacyType.small_scenery.tool_id = stream->ReadValue<uint8>();
    _legacyType.small_scenery.price = stream->ReadValue<sint16>();
    _legacyType.small_scenery.removal_price = stream->ReadValue<sint16>();
    _legacyType.small_scenery.var_10 = stream->ReadValue<uint32>();
    _legacyType.small_scenery.var_14 = stream->ReadValue<uint16>();
    _legacyType.small_scenery.var_16 = stream->ReadValue<uint16>();
    _legacyType.small_scenery.var_18 = stream->ReadValue<uint16>();
    _legacyType.small_scenery.scenery_tab_id = 0xFF;

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_NAME);

    _sceneryTabEntry = stream->ReadValue<rct_object_entry>();

    if (_legacyType.small_scenery.flags & SMALL_SCENERY_FLAG16)
    {
        _var10data = ReadVar10(stream);
    }

    GetImageTable()->Read(context, stream);

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
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable()->GetImages(), GetImageTable()->GetCount());

    _legacyType.small_scenery.scenery_tab_id = 0xFF;
    if ((_sceneryTabEntry.flags & 0xFF) != 0xFF)
    {
        uint8 entryType, entryIndex;
        if (find_object_in_entry_group(&_sceneryTabEntry, &entryType, &entryIndex))
        {
            _legacyType.small_scenery.scenery_tab_id = entryIndex;
        }
    }

    if (_legacyType.small_scenery.flags & SMALL_SCENERY_FLAG16)
    {
        _legacyType.small_scenery.var_10 = (uint32)_var10data;
    }
}

void SmallSceneryObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable()->GetCount());
}

void SmallSceneryObject::DrawPreview(rct_drawpixelinfo * dpi) const
{
    // rct_drawpixelinfo clipDPI;
    // if (!clip_drawpixelinfo(&clipDPI, dpi, x - 56, y - 56, 112, 112)) {
    //     return;
    // }

    uint32 flags = _legacyType.small_scenery.flags;
    uint32 imageId = _legacyType.image;
    if (flags & SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
    {
        imageId |= 0x20D00000;
        if (flags & SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
        {
            imageId |= 0x92000000;
        }
    }

    int x = dpi->width / 2;
    int y = (dpi->height / 2) + (_legacyType.small_scenery.height / 2);
    y = Math::Min(y, dpi->height - 16);

    if ((flags & SMALL_SCENERY_FLAG_FULL_TILE) &&
        (flags & SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
    {
        y -= 12;
    }

    gfx_draw_sprite(dpi, imageId, x, y, 0);

    if (_legacyType.small_scenery.flags & SMALL_SCENERY_FLAG_HAS_GLASS)
    {
        imageId = _legacyType.image + 0x44500004;
        if (flags & SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
        {
            imageId |= 0x92000000;
        }
        gfx_draw_sprite(dpi, imageId, x, y, 0);
    }

    if (flags & SMALL_SCENERY_FLAG_ANIMATED_FG)
    {
        imageId = _legacyType.image + 4;
        if (flags & SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
        {
            imageId |= 0x92000000;
        }
        gfx_draw_sprite(dpi, imageId, x, y, 0);
    }
}


const utf8 * SmallSceneryObject::GetName() const
{
    const utf8 * name = GetStringTable()->GetString(OBJ_STRING_ID_NAME);
    return name != nullptr ? name : "";
}

uint8 * SmallSceneryObject::ReadVar10(IStream * stream)
{
    uint8 b;
    auto data = std::vector<uint8>();
    data.push_back(stream->ReadValue<uint8>());
    while ((b = stream->ReadValue<uint8>()) != 0xFF)
    {
        data.push_back(b);
    }
    data.push_back(b);
    return Memory::Duplicate(data.data(), data.size());
}
