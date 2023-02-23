/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Object.h"

struct CoordsXY;

enum TERRAIN_SURFACE_FLAGS
{
    NONE = 0,
    SMOOTH_WITH_SELF = 1 << 0,
    SMOOTH_WITH_OTHER = 1 << 1,
    CAN_GROW = 1 << 2,
};

class TerrainSurfaceObject final : public Object
{
private:
    struct SpecialEntry
    {
        uint32_t Index{};
        int32_t Length{};
        int32_t Rotation{};
        int32_t Variation{};
        bool Grid{};
        bool Underground{};
    };

    static constexpr auto NUM_IMAGES_IN_ENTRY = 19;

public:
    StringId NameStringId{};
    uint32_t IconImageId{};
    uint32_t PatternBaseImageId{};
    uint32_t EntryBaseImageId{};

    uint32_t NumEntries{};
    uint32_t DefaultEntry{};
    uint32_t DefaultGridEntry{};
    uint32_t DefaultUndergroundEntry{};
    std::vector<SpecialEntry> SpecialEntries;
    std::vector<uint32_t> SpecialEntryMap;

    colour_t Colour{};
    uint8_t Rotations{};
    money64 Price{};
    TERRAIN_SURFACE_FLAGS Flags{};
    PaletteIndex MapColours[2]{};

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(DrawPixelInfo* dpi, int32_t width, int32_t height) const override;

    uint32_t GetImageId(
        const CoordsXY& position, int32_t length, int32_t rotation, int32_t offset, bool grid, bool underground) const;

    static TerrainSurfaceObject* GetById(ObjectEntryIndex entryIndex);
};
