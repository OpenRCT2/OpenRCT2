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

namespace OpenRCT2
{
    enum class TerrainSurfaceFlag : uint8_t
    {
        smoothWithSelf,
        smoothWithOther,
        canGrow,
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

    public:
        static constexpr ObjectType kObjectType = ObjectType::terrainSurface;

        static constexpr uint8_t kNoValue = 0xFF;
        StringId NameStringId{};
        uint32_t IconImageId{};
        uint32_t PatternBaseImageId{};
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
        Drawing::PaletteIndex MapColours[2]{};

        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void Load() override;
        void Unload() override;

        void DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const override;

        ImageId GetImageId(
            const CoordsXY& position, uint8_t length, uint8_t rotation, uint8_t offset, bool grid, bool underground) const;

        static TerrainSurfaceObject* GetById(ObjectEntryIndex entryIndex);
    };
} // namespace OpenRCT2
