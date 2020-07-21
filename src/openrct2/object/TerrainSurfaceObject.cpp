/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "TerrainSurfaceObject.h"

#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../world/Location.hpp"
#include "ObjectJsonHelpers.h"

void TerrainSurfaceObject::Load()
{
    GetStringTable().Sort();
    NameStringId = language_allocate_object_string(GetName());
    IconImageId = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
    if ((Flags & SMOOTH_WITH_SELF) || (Flags & SMOOTH_WITH_OTHER))
    {
        PatternBaseImageId = IconImageId + 1;
        EntryBaseImageId = PatternBaseImageId + 6;
    }
    else
    {
        EntryBaseImageId = IconImageId + 1;
    }
    NumEntries = (GetImageTable().GetCount() - EntryBaseImageId) / NUM_IMAGES_IN_ENTRY;
}

void TerrainSurfaceObject::Unload()
{
    language_free_object_string(NameStringId);
    gfx_object_free_images(IconImageId, GetImageTable().GetCount());

    NameStringId = 0;
    IconImageId = 0;
    PatternBaseImageId = 0;
    EntryBaseImageId = 0;
    NumEntries = 0;
}

void TerrainSurfaceObject::DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const
{
    uint32_t imageId = GetImageId({}, 1, 0, 0, false, false);
    if (Colour != 255)
    {
        imageId |= SPRITE_ID_PALETTE_COLOUR_1(Colour);
    }

    ScreenCoordsXY screenCoords{};
    int32_t x0 = 0;
    screenCoords.y = -16;
    for (int32_t i = 0; i < 8; i++)
    {
        screenCoords.x = x0;
        if (i % 2 == 0)
        {
            screenCoords.x -= 32;
        }
        for (int32_t j = 0; j < 4; j++)
        {
            gfx_draw_sprite(dpi, imageId, screenCoords, 0);
            screenCoords.x += 64;
        }
        screenCoords.y += 16;
    }
}

void TerrainSurfaceObject::ReadJson(IReadObjectContext* context, const json_t* root)
{
    auto properties = json_object_get(root, "properties");
    Colour = ObjectJsonHelpers::ParseColour(ObjectJsonHelpers::GetString(properties, "colour"), 255);
    Rotations = ObjectJsonHelpers::GetInteger(properties, "rotations", 1);
    Price = ObjectJsonHelpers::GetInteger(properties, "price", 0);
    Flags = ObjectJsonHelpers::GetFlags<TERRAIN_SURFACE_FLAGS>(
        properties,
        { { "smoothWithSelf", TERRAIN_SURFACE_FLAGS::SMOOTH_WITH_SELF },
          { "smoothWithOther", TERRAIN_SURFACE_FLAGS::SMOOTH_WITH_OTHER },
          { "canGrow", TERRAIN_SURFACE_FLAGS::CAN_GROW } });

    auto jDefault = json_object_get(root, "default");
    if (json_is_object(jDefault))
    {
        DefaultEntry = ObjectJsonHelpers::GetInteger(properties, "normal");
        DefaultGridEntry = ObjectJsonHelpers::GetInteger(properties, "grid");
        DefaultUndergroundEntry = ObjectJsonHelpers::GetInteger(properties, "underground");
    }
    else
    {
        DefaultEntry = 0;
        DefaultGridEntry = 1;
        DefaultUndergroundEntry = 2;
    }

    auto jSpecialArray = json_object_get(properties, "special");
    if (json_is_array(jSpecialArray))
    {
        size_t i;
        json_t* el;
        json_array_foreach(jSpecialArray, i, el)
        {
            SpecialEntry entry;
            entry.Index = ObjectJsonHelpers::GetInteger(el, "index");
            entry.Length = ObjectJsonHelpers::GetInteger(el, "length", -1);
            entry.Rotation = ObjectJsonHelpers::GetInteger(el, "rotation", -1);
            entry.Variation = ObjectJsonHelpers::GetInteger(el, "variation", -1);
            entry.Grid = ObjectJsonHelpers::GetBoolean(el, "grid");
            entry.Underground = ObjectJsonHelpers::GetBoolean(el, "underground");
            SpecialEntries.push_back(std::move(entry));
        }
    }

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}

uint32_t TerrainSurfaceObject::GetImageId(
    const CoordsXY& position, int32_t length, int32_t rotation, int32_t offset, bool grid, bool underground) const
{
    uint32_t result = (underground ? DefaultUndergroundEntry : (grid ? DefaultGridEntry : DefaultEntry));

    // Look for a matching special
    auto variation = ((position.x << 1) & 0b10) | (position.y & 0b01);
    for (const auto& special : SpecialEntries)
    {
        if ((special.Length == -1 || special.Length == length) && (special.Rotation == -1 || special.Rotation == rotation)
            && (special.Variation == -1 || special.Variation == variation) && special.Grid == grid
            && special.Underground == underground)
        {
            result = special.Index;
            break;
        }
    }
    return EntryBaseImageId + (result * NUM_IMAGES_IN_ENTRY) + offset;
}
