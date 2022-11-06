/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "SceneryObject.h"

#include <memory>
#include <vector>

enum class CursorID : uint8_t;

struct rct_large_scenery_text_glyph
{
    uint8_t image_offset;
    uint8_t width;
    uint8_t height;
    uint8_t pad_3;
};

struct rct_large_scenery_text
{
    struct
    {
        int16_t x, y;
    } offset[2];                              // 0x0
    uint16_t max_width;                       // 0x8
    uint16_t pad_A;                           // 0xA
    uint8_t flags;                            // 0xC
    uint8_t num_images;                       // 0xD
    rct_large_scenery_text_glyph glyphs[256]; // 0xE
};

struct LargeSceneryText
{
    CoordsXY offset[2];
    uint16_t max_width;
    uint8_t flags;
    uint16_t num_images;
    rct_large_scenery_text_glyph glyphs[256];

    LargeSceneryText() = default;
    explicit LargeSceneryText(const rct_large_scenery_text& original);
    const rct_large_scenery_text_glyph* GetGlyph(char32_t codepoint) const;
    const rct_large_scenery_text_glyph& GetGlyph(char32_t codepoint, char32_t defaultCodepoint) const;
    int32_t MeasureWidth(std::string_view text) const;
    int32_t MeasureHeight(std::string_view text) const;
};

struct rct_large_scenery_tile
{
    int16_t x_offset;
    int16_t y_offset;
    int16_t z_offset;
    uint8_t z_clearance;
    // CCCC WWWW 0SS0 0000
    uint16_t flags;
};

enum
{
    LARGE_SCENERY_TILE_FLAG_NO_SUPPORTS = 0x20,
    LARGE_SCENERY_TILE_FLAG_ALLOW_SUPPORTS_ABOVE = 0x40,
};

enum LARGE_SCENERY_TEXT_FLAGS
{
    LARGE_SCENERY_TEXT_FLAG_VERTICAL = (1 << 0), // 0x1
    LARGE_SCENERY_TEXT_FLAG_TWO_LINE = (1 << 1), // 0x2
};

struct LargeSceneryEntry
{
    StringId name;
    uint32_t image;
    CursorID tool_id;
    uint8_t flags;
    money32 price;
    money32 removal_price;
    rct_large_scenery_tile* tiles;
    ObjectEntryIndex scenery_tab_id;
    uint8_t scrolling_mode;
    LargeSceneryText* text;
    uint32_t text_image;
};

enum LARGE_SCENERY_FLAGS
{
    LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR = (1 << 0),   // 0x1
    LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR = (1 << 1), // 0x2
    LARGE_SCENERY_FLAG_3D_TEXT = (1 << 2),              // 0x4
    LARGE_SCENERY_FLAG_ANIMATED = (1 << 3),             // 0x8
    LARGE_SCENERY_FLAG_PHOTOGENIC = (1 << 4),           // 0x10
    LARGE_SCENERY_FLAG_IS_TREE = (1 << 5),              // 0x20
    LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR = (1 << 6),  // 0x40
};

class LargeSceneryObject final : public SceneryObject
{
private:
    LargeSceneryEntry _legacyType = {};
    uint32_t _baseImageId = 0;
    std::vector<rct_large_scenery_tile> _tiles;
    std::unique_ptr<LargeSceneryText> _3dFont;

public:
    void* GetLegacyData() override
    {
        return &_legacyType;
    }

    void ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream) override;
    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const override;
    const rct_large_scenery_tile* GetTileForSequence(uint8_t SequenceIndex) const;

private:
    [[nodiscard]] static std::vector<rct_large_scenery_tile> ReadTiles(OpenRCT2::IStream* stream);
    [[nodiscard]] static std::vector<rct_large_scenery_tile> ReadJsonTiles(json_t& jTiles);
    [[nodiscard]] static std::unique_ptr<LargeSceneryText> ReadJson3dFont(json_t& j3dFont);
    [[nodiscard]] static std::vector<CoordsXY> ReadJsonOffsets(json_t& jOffsets);
    [[nodiscard]] static std::vector<rct_large_scenery_text_glyph> ReadJsonGlyphs(json_t& jGlyphs);
};
