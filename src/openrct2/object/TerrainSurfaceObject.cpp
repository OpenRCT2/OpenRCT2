/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TerrainSurfaceObject.h"

#include "../Context.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../drawing/ColourMap.h"
#include "../drawing/Drawing.Sprite.h"
#include "../drawing/Drawing.h"
#include "../drawing/FilterPaletteIds.h"
#include "../drawing/Image.h"
#include "../drawing/PaletteMap.h"
#include "../drawing/RenderTarget.h"
#include "../interface/ScreenCoords.hpp"
#include "../world/Location.hpp"
#include "ObjectManager.h"

using OpenRCT2::Drawing::FilterPaletteID;
using OpenRCT2::Drawing::PaletteIndex;
using OpenRCT2::Drawing::PaletteMap;

namespace OpenRCT2
{
    static constexpr MapColour kInvalidMapColour = { MapColourType::paletteIndex, Drawing::Colour::black,
                                                     PaletteIndex::transparent };

    void TerrainSurfaceObject::precolourPatternImages()
    {
        for (size_t colourNum = 0; colourNum < Drawing::kColourNumTotal; colourNum++)
        {
            for (uint8_t patternOffset = 0; patternOffset < kNumSmoothingPatternImages; patternOffset++)
            {
                // TODO (as requested by Aaron): this is a lot of ceremony for what on the face of it should be a
                // simple task: applying a colour to the primary remap and saving that in a G1Element. Especially the need
                // to get 3 structures (G1Element, RenderTarget and ImageId) involved.
                G1Element newElement = *(GfxGetG1Element(PatternBaseImageId + patternOffset));
                size_t numPixels = newElement.width * newElement.height;
                auto pixels8 = new uint8_t[numPixels];
                // Copy unmodified image so we can colour it
                std::copy_n(newElement.offset, numPixels, pixels8);
                newElement.offset = pixels8;

                auto primaryPaletteMap = GetPaletteMapForColour(static_cast<FilterPaletteID>(colourNum))
                                             .value_or(PaletteMap::GetDefault());

                Drawing::RenderTarget rt;
                rt.bits = reinterpret_cast<PaletteIndex*>(pixels8);
                rt.x = 0;
                rt.y = 0;
                rt.width = newElement.width;
                rt.height = newElement.height;
                rt.pitch = 0;
                rt.zoom_level = ZoomLevel{ 0 };

                auto dummyImage = ImageId().WithPrimary(static_cast<Drawing::Colour>(colourNum));
                DrawSpriteArgs args(
                    dummyImage, primaryPaletteMap, newElement, 0, 0, newElement.width, newElement.height, rt.bits);
                GfxSpriteToBuffer(rt, args);

                precolouredPatternImages.AddImage(&newElement);
                delete[] pixels8;
            }
        }

        precolouredPatternImageId = GfxObjectAllocateImages(
            precolouredPatternImages.GetImages(), precolouredPatternImages.GetCount());
    }

    void TerrainSurfaceObject::unloadPrecolouredPatternImages()
    {
        GfxObjectFreeImages(precolouredPatternImageId, precolouredPatternImages.GetCount());
        precolouredPatternImageId = 0;
    }

    void TerrainSurfaceObject::Load()
    {
        GetStringTable().Sort();
        NameStringId = LanguageAllocateObjectString(GetName());
        IconImageId = LoadImages();
        if (Flags.hasAny(TerrainSurfaceFlag::smoothWithSelf, TerrainSurfaceFlag::smoothWithOther))
        {
            PatternBaseImageId = IconImageId + 1;
            EntryBaseImageId = PatternBaseImageId + kNumSmoothingPatternImages;

            if (Flags.has(TerrainSurfaceFlag::hasPrimaryColour))
                precolourPatternImages();
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
        if (Flags.has(TerrainSurfaceFlag::hasPrimaryColour))
            unloadPrecolouredPatternImages();

        NameStringId = 0;
        IconImageId = 0;
        PatternBaseImageId = 0;
        EntryBaseImageId = 0;
        NumEntries = 0;
    }

    void TerrainSurfaceObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        auto imageId = GetImageId({}, 1, 0, 0, false, false, getPreviewColour());

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
                GfxDrawSprite(rt, imageId, screenCoords);
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
            Rotations = Json::GetNumber<int8_t>(properties["rotations"], 1);
            Price = Json::GetNumber<money64>(properties["price"]);
            Flags = Json::GetFlagHolder<TerrainSurfaceFlags, TerrainSurfaceFlag>(
                properties,
                { { "smoothWithSelf", TerrainSurfaceFlag::smoothWithSelf },
                  { "smoothWithOther", TerrainSurfaceFlag::smoothWithOther },
                  { "canGrow", TerrainSurfaceFlag::canGrow },
                  { "hasPrimaryColour", TerrainSurfaceFlag::hasPrimaryColour } });

            const auto colourSettings = properties["colourSettings"];
            if (colourSettings.is_object() && colourSettings["parkMap"].is_array())
            {
                readColourSettingsProperty(colourSettings);
            }
            else
            {
                if (Flags.has(TerrainSurfaceFlag::hasPrimaryColour))
                    throw std::runtime_error(
                        "Recolourable terrain surface objects need to have a valid colourSettings property!");

                readOldColourSettings(properties);
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

    void TerrainSurfaceObject::readColourSettingsProperty(const json_t& colourSettings)
    {
        Colour = Drawing::kColourNull;
        if (Flags.has(TerrainSurfaceFlag::hasPrimaryColour))
        {
            const auto primary = colourSettings["defaultPrimary"];
            Colour = colourFromString(Json::GetString(primary), Drawing::kColourNull);
            if (Colour == Drawing::kColourNull)
                throw std::runtime_error(
                    "Recolourable terrain surface object need to have a valid colourSettings property and set a default "
                    "primary colour.");
        }

        const auto mapColours = colourSettings["parkMap"];
        const auto mapColoursAreValid = mapColours.is_array() && mapColours.size() == std::size(MapColours);
        if (!mapColoursAreValid)
            throw std::runtime_error("colourSettings.parkMap is not valid!");

        for (size_t i = 0; i < std::size(MapColours); i++)
        {
            const auto& mapColour = mapColours[i];
            if (!mapColour.is_object())
                throw std::runtime_error("Park map colour entry is not a valid object!");

            const auto shadeOffset = Json::GetNumber(mapColour["shade"], -1);
            if (shadeOffset < 0 || shadeOffset > 11)
                throw std::runtime_error("Park map colour shade must be between 0 and 11!");

            MapColour entry{};
            entry.shadeOffset = shadeOffset;

            const auto parkMapColour = Json::GetString(mapColour["colour"]);
            if (parkMapColour == "primary")
            {
                entry.type = MapColourType::primaryColour;
            }
            else
            {
                const auto converted = Drawing::colourFromString(parkMapColour, Drawing::kColourNull);
                if (converted == Drawing::kColourNull)
                    throw std::runtime_error("Park map colour is not valid!");

                entry.fixedColour = converted;
            }

            MapColours[i] = entry;
        }
    }

    void TerrainSurfaceObject::readOldColourSettings(json_t& properties)
    {
        Colour = colourFromString(Json::GetString(properties["colour"]), Drawing::kColourNull);
        const auto mapColours = properties["mapColours"];
        const auto mapColoursAreValid = mapColours.is_array() && mapColours.size() == std::size(MapColours);
        for (size_t i = 0; i < std::size(MapColours); i++)
        {
            if (mapColoursAreValid && mapColours[i].is_number_integer())
                MapColours[i] = { .type = MapColourType::paletteIndex,
                                  .paletteIndex = static_cast<PaletteIndex>(mapColours[i]) };
            else
                MapColours[i] = kInvalidMapColour;
        }
    }

    ImageId TerrainSurfaceObject::GetImageId(
        const CoordsXY& position, uint8_t length, uint8_t rotation, uint8_t offset, bool grid, bool underground,
        Drawing::Colour selectedColour1) const
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
        const uint8_t variation = (tilePos.x & 0b01) | ((tilePos.y << 1) & 0b10);

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

        ImageId image(EntryBaseImageId + (result * kNumImagesInEntry) + offset, getPrimaryColour(selectedColour1));

        return image;
    }

    Drawing::Colour TerrainSurfaceObject::getPrimaryColour(Drawing::Colour selectedColour) const
    {
        if (Flags.has(TerrainSurfaceFlag::hasPrimaryColour))
            return selectedColour;

        if (Colour == Drawing::kColourNull)
            return selectedColour;

        return Colour;
    }

    Drawing::Colour TerrainSurfaceObject::getPreviewColour() const
    {
        if (Colour != Drawing::kColourNull)
            return Colour;

        return kDefaultTerrainSurfaceColour1;
    }

    std::array<PaletteIndex, 2> TerrainSurfaceObject::getMapColours(Drawing::Colour selectedColour) const
    {
        std::array<PaletteIndex, 2> result;
        assert(result.size() == std::size(MapColours));

        for (size_t i = 0; i < result.size(); i++)
        {
            switch (MapColours[i].type)
            {
                case MapColourType::paletteIndex:
                    result[i] = MapColours[i].paletteIndex;
                    break;
                case MapColourType::fixedColour:
                    result[i] = Drawing::getColourMapShade(MapColours[i].fixedColour, MapColours[i].shadeOffset);
                    break;
                case MapColourType::primaryColour:
                    result[i] = Drawing::getColourMapShade(selectedColour, MapColours[i].shadeOffset);
                    break;
            }
        }

        return result;
    }

    TerrainSurfaceObject* TerrainSurfaceObject::GetById(ObjectEntryIndex entryIndex)
    {
        auto& objMgr = GetContext()->GetObjectManager();
        return objMgr.GetLoadedObject<TerrainSurfaceObject>(entryIndex);
    }

    ImageIndex TerrainSurfaceObject::getPatternImage(Drawing::Colour selectedColour, uint8_t offset) const
    {
        assert(offset < kNumSmoothingPatternImages);
        if (Flags.has(TerrainSurfaceFlag::hasPrimaryColour))
            return precolouredPatternImageId + (EnumValue(selectedColour) * kNumSmoothingPatternImages) + offset;

        return PatternBaseImageId + offset;
    }
} // namespace OpenRCT2
