/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "TerrainSurfaceObject.h"

#include "../Context.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../world/Location.hpp"
#include "ObjectManager.h"

using namespace OpenRCT2;

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
    NumEntries = (GetImageTable().GetCount() - EntryBaseImageId) / kNumImagesInEntry;
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
    if (Colour != kNoValue)
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
            GfxDrawSprite(dpi, imageId, screenCoords);
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
        Colour = Colour::FromString(Json::GetString(properties["colour"]), kNoValue);
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
                entry.Index = Json::GetNumber<uint8_t>(el["index"]);
                entry.Length = Json::GetNumber<uint8_t>(el["length"], kNoValue);
                entry.Rotation = Json::GetNumber<uint8_t>(el["rotation"], kNoValue);
                entry.Variation = Json::GetNumber<uint8_t>(el["variation"], kNoValue);

                if (Json::GetBoolean(el["underground"]))
                    SpecialEntriesUnderground.push_back(entry);
                else if (Json::GetBoolean(el["grid"]))
                    SpecialEntriesGrid.push_back(entry);
                else
                    SpecialEntries.push_back(entry);
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

ImageId TerrainSurfaceObject::GetImageId(
    const CoordsXY& position, uint8_t length, uint8_t rotation, uint8_t offset, bool grid, bool underground) const
{
    uint32_t result = DefaultEntry;
    std::span<const SpecialEntry> entries(SpecialEntries);
    if (underground)
    {
        result = DefaultUndergroundEntry;
        entries = std::span<const SpecialEntry>(SpecialEntriesUnderground);
    }
    else if (grid)
    {
        result = DefaultGridEntry;
        entries = std::span<const SpecialEntry>(SpecialEntriesGrid);
    }

    TileCoordsXY tilePos(position);
    uint8_t variation = ((tilePos.x << 1) & 0b10) | (tilePos.y & 0b01);

    // Look for a matching special
    for (const SpecialEntry& special : entries)
    {
        if ((special.Length == kNoValue || special.Length == length)
            && (special.Rotation == kNoValue || special.Rotation == rotation)
            && (special.Variation == kNoValue || special.Variation == variation))
        {
            result = special.Index;
            break;
        }
    }

    ImageId image(EntryBaseImageId + (result * kNumImagesInEntry) + offset);
    if (Colour != kNoValue)
    {
        image = image.WithPrimary(Colour);
    }
    return image;
}

TerrainSurfaceObject* TerrainSurfaceObject::GetById(ObjectEntryIndex entryIndex)
{
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto* obj = objMgr.GetLoadedObject(ObjectType::TerrainSurface, entryIndex);
    return static_cast<TerrainSurfaceObject*>(obj);
}
