/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "TerrainSurfaceObject.h"

#include "../Context.h"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../world/Location.hpp"
#include "ObjectManager.h"

void TerrainSurfaceObject::Load()
{
    GetStringTable().Sort();
    NameStringId = LanguageAllocateObjectString(GetName());
    IconImageId = LoadImages();
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
    LanguageFreeObjectString(NameStringId);
    UnloadImages();

    NameStringId = 0;
    IconImageId = 0;
    PatternBaseImageId = 0;
    EntryBaseImageId = 0;
    NumEntries = 0;
}

void TerrainSurfaceObject::DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const
{
    auto imageId = ImageId(GetImageId({}, 1, 0, 0, false, false));
    if (Colour != 255)
    {
        imageId = imageId.WithPrimary(Colour);
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
            GfxDrawSprite(&dpi, imageId, screenCoords);
            screenCoords.x += 64;
        }
        screenCoords.y += 16;
    }
}

void TerrainSurfaceObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "TerrainSurfaceObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];

    if (properties.is_object())
    {
        Colour = Colour::FromString(Json::GetString(properties["colour"]), 255);
        Rotations = Json::GetNumber<int8_t>(properties["rotations"], 1);
        Price = Json::GetNumber<money64>(properties["price"]);
        Flags = Json::GetFlags<TERRAIN_SURFACE_FLAGS>(
            properties,
            { { "smoothWithSelf", TERRAIN_SURFACE_FLAGS::SMOOTH_WITH_SELF },
              { "smoothWithOther", TERRAIN_SURFACE_FLAGS::SMOOTH_WITH_OTHER },
              { "canGrow", TERRAIN_SURFACE_FLAGS::CAN_GROW } });

        const auto mapColours = properties["mapColours"];
        const bool mapColoursAreValid = mapColours.is_array() && mapColours.size() == std::size(MapColours);
        for (size_t i = 0; i < std::size(MapColours); i++)
        {
            if (mapColoursAreValid)
                MapColours[i] = mapColours[i];
            else
                MapColours[i] = PALETTE_INDEX_0;
        }

        for (auto& el : properties["special"])
        {
            if (el.is_object())
            {
                SpecialEntry entry;
                entry.Index = Json::GetNumber<uint32_t>(el["index"]);
                entry.Length = Json::GetNumber<int32_t>(el["length"], -1);
                entry.Rotation = Json::GetNumber<int32_t>(el["rotation"], -1);
                entry.Variation = Json::GetNumber<int32_t>(el["variation"], -1);
                entry.Grid = Json::GetBoolean(el["grid"]);
                entry.Underground = Json::GetBoolean(el["underground"]);
                SpecialEntries.push_back(std::move(entry));
            }
        }
    }

    auto jDefault = root["default"];
    if (jDefault.is_object())
    {
        DefaultEntry = Json::GetNumber<uint32_t>(jDefault["normal"]);
        DefaultGridEntry = Json::GetNumber<uint32_t>(jDefault["grid"]);
        DefaultUndergroundEntry = Json::GetNumber<uint32_t>(jDefault["underground"]);
    }
    else
    {
        DefaultEntry = 0;
        DefaultGridEntry = 1;
        DefaultUndergroundEntry = 2;
    }

    PopulateTablesFromJson(context, root);
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

TerrainSurfaceObject* TerrainSurfaceObject::GetById(ObjectEntryIndex entryIndex)
{
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto* obj = objMgr.GetLoadedObject(ObjectType::TerrainSurface, entryIndex);
    return static_cast<TerrainSurfaceObject*>(obj);
}
