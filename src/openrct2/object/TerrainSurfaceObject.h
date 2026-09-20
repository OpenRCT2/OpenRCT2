/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../drawing/Colour.h"
#include "../drawing/PaletteIndex.h"
#include "Object.h"

struct CoordsXY;
struct ImageId;

namespace OpenRCT2
{
    constexpr auto kNumSmoothingPatternImages = 6;

    constexpr auto kDefaultTerrainSurfaceColour1 = Drawing::Colour::brightPurple;

    struct MapColour
    {
        uint8_t isOffset = 0;
        union
        {
            Drawing::PaletteIndex paletteIndex{};
            uint8_t mapOffset;
        };
    };

    enum class TerrainSurfaceFlag : uint8_t
    {
        smoothWithSelf,
        smoothWithOther,
        canGrow,
        hasPrimaryColour,
    };
    using TerrainSurfaceFlags = FlagHolder<uint8_t, TerrainSurfaceFlag>;

    class TerrainSurfaceObject final : public Object
    {
    private:
        struct SpecialEntry
        {
            uint8_t Index{};
            uint8_t Length{};
            uint8_t Rotation{};
            uint8_t Variation{};
        };

        static constexpr auto kNumImagesInEntry = 19;

        void precolourPatternImages();
        void unloadPrecolouredPatternImages();

    public:
        static constexpr ObjectType kObjectType = ObjectType::terrainSurface;

        static constexpr uint8_t kNoValue = 0xFF;
        StringId NameStringId{};
        uint32_t IconImageId{};
        uint32_t PatternBaseImageId{};
        ImageIndex precolouredPatternImageId{};
        uint32_t EntryBaseImageId{};

        uint32_t NumEntries{};
        uint32_t DefaultEntry{};
        uint32_t DefaultGridEntry{};
        uint32_t DefaultUndergroundEntry{};
        std::vector<SpecialEntry> SpecialEntries;
        std::vector<SpecialEntry> SpecialEntriesUnderground;
        std::vector<SpecialEntry> SpecialEntriesGrid;

        Drawing::Colour Colour{};
        uint8_t Rotations{};
        money64 Price{};
        TerrainSurfaceFlags Flags{};
        MapColour MapColours[2]{};

        ImageTable precolouredPatternImages{};

        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void Load() override;
        void Unload() override;

        void DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const override;

        ImageId GetImageId(
            const CoordsXY& position, uint8_t length, uint8_t rotation, uint8_t offset, bool grid, bool underground,
            Drawing::Colour selectedColour1) const;
        Drawing::Colour getPrimaryColour(Drawing::Colour selectedColour) const;
        /**
         * Returns a colour suitable for use in previews, like the Object Selection and the land surfaces selector.
         */
        Drawing::Colour getPreviewColour() const;
        std::array<Drawing::PaletteIndex, 2> getMapColours(Drawing::Colour selectedColour) const;

        static TerrainSurfaceObject* GetById(ObjectEntryIndex entryIndex);
        ImageIndex getPatternImage(Drawing::Colour selectedColour, uint8_t offset) const;
    };
} // namespace OpenRCT2
