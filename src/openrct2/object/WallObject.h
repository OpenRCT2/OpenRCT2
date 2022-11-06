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

enum class CursorID : uint8_t;

enum WALL_SCENERY_FLAGS
{
    WALL_SCENERY_HAS_PRIMARY_COLOUR = (1 << 0),   // 0x1
    WALL_SCENERY_HAS_GLASS = (1 << 1),            // 0x2
    WALL_SCENERY_CANT_BUILD_ON_SLOPE = (1 << 2),  // 0x4
    WALL_SCENERY_IS_DOUBLE_SIDED = (1 << 3),      // 0x8
    WALL_SCENERY_IS_DOOR = (1 << 4),              // 0x10
    WALL_SCENERY_LONG_DOOR_ANIMATION = (1 << 5),  // 0x20
    WALL_SCENERY_HAS_SECONDARY_COLOUR = (1 << 6), // 0x40
    WALL_SCENERY_HAS_TERTIARY_COLOUR = (1 << 7),  // 0x80
};

enum WALL_SCENERY_2_FLAGS
{
    WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR = (1 << 0), // 0x1
    WALL_SCENERY_2_DOOR_SOUND_MASK = 0x6,
    WALL_SCENERY_2_DOOR_SOUND_SHIFT = 1,
    WALL_SCENERY_2_IS_OPAQUE = (1 << 3), // 0x8
    WALL_SCENERY_2_ANIMATED = (1 << 4),  // 0x10
};

struct WallSceneryEntry
{
    StringId name;
    uint32_t image;
    CursorID tool_id;
    uint8_t flags;
    uint8_t height;
    uint8_t flags2;
    money16 price;
    ObjectEntryIndex scenery_tab_id;
    uint8_t scrolling_mode;
};

class WallObject final : public SceneryObject
{
private:
    WallSceneryEntry _legacyType = {};

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
};
