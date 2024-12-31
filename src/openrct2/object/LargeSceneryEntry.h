/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../localisation/StringIdType.h"
#include "../world/Location.hpp"
#include "ObjectTypes.h"

#include <span>
#include <string_view>

enum class CursorID : uint8_t;

struct LargeSceneryText;

struct LargeSceneryTile
{
    CoordsXYZ offset;
    int32_t zClearance; // BigZ
    bool hasSupports;
    bool allowSupportsAbove;
    uint8_t corners; // occupied corners of the tile
    uint8_t walls;   // sides that walls can be placed on
    uint8_t index;   // Purely to save having to look this up all the time
};

struct LargeSceneryTextGlyph
{
    uint8_t image_offset;
    uint8_t width;
    uint8_t height;
    uint8_t Pad3;
};

// // TODO: Remove not required
struct RCTLargeSceneryText
{
    struct
    {
        int16_t x, y;
    } offset[2];                       // 0x0
    uint16_t max_width;                // 0x8
    uint16_t PadA;                     // 0xA
    uint8_t flags;                     // 0xC
    uint8_t num_images;                // 0xD
    LargeSceneryTextGlyph glyphs[256]; // 0xE
};

enum LARGE_SCENERY_TEXT_FLAGS
{
    LARGE_SCENERY_TEXT_FLAG_VERTICAL = (1 << 0), // 0x1
    LARGE_SCENERY_TEXT_FLAG_TWO_LINE = (1 << 1), // 0x2
};

struct LargeSceneryEntry
{
    static constexpr auto kObjectType = ObjectType::LargeScenery;

    StringId name;
    uint32_t image;
    CursorID tool_id;
    uint16_t flags;
    money64 price;
    money64 removal_price;
    std::span<const LargeSceneryTile> tiles;
    ObjectEntryIndex scenery_tab_id;
    uint8_t scrolling_mode;
    LargeSceneryText* text;
    uint32_t text_image;

    constexpr bool HasFlag(const uint16_t _flags) const
    {
        return (flags & _flags) != 0;
    }
};

enum LARGE_SCENERY_FLAGS : uint16_t
{
    LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR = (1 << 0),          // 0x1
    LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR = (1 << 1),        // 0x2
    LARGE_SCENERY_FLAG_3D_TEXT = (1 << 2),                     // 0x4
    LARGE_SCENERY_FLAG_ANIMATED = (1 << 3),                    // 0x8
    LARGE_SCENERY_FLAG_PHOTOGENIC = (1 << 4),                  // 0x10
    LARGE_SCENERY_FLAG_IS_TREE = (1 << 5),                     // 0x20
    LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR = (1 << 6),         // 0x40
    LARGE_SCENERY_FLAG_HIDE_PRIMARY_REMAP_BUTTON = (1 << 7),   // 0x80
    LARGE_SCENERY_FLAG_HIDE_SECONDARY_REMAP_BUTTON = (1 << 8), // 0x100
};

struct LargeSceneryText
{
    CoordsXY offset[2];
    uint16_t max_width;
    uint8_t flags;
    uint16_t num_images;
    LargeSceneryTextGlyph glyphs[256];

    LargeSceneryText() = default;
    explicit LargeSceneryText(const RCTLargeSceneryText& original);
    const LargeSceneryTextGlyph* GetGlyph(char32_t codepoint) const;
    const LargeSceneryTextGlyph& GetGlyph(char32_t codepoint, char32_t defaultCodepoint) const;
    int32_t MeasureWidth(std::string_view text) const;
    int32_t MeasureHeight(std::string_view text) const;
};
