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

enum
{
    PATH_BIT_FLAG_IS_BIN = 1 << 0,
    PATH_BIT_FLAG_IS_BENCH = 1 << 1,
    PATH_BIT_FLAG_BREAKABLE = 1 << 2,
    PATH_BIT_FLAG_LAMP = 1 << 3,
    PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER = 1 << 4,
    PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW = 1 << 5,
    PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE = 1 << 6,
    PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE = 1 << 7,
    PATH_BIT_FLAG_IS_QUEUE_SCREEN = 1 << 8
};

enum class PathBitDrawType : uint8_t
{
    Light,
    Bin,
    Bench,
    JumpingFountain,
};

struct PathBitEntry
{
    StringId name;
    uint32_t image;
    uint16_t flags;
    PathBitDrawType draw_type;
    CursorID tool_id;
    money16 price;
    ObjectEntryIndex scenery_tab_id;
};

class FootpathItemObject final : public SceneryObject
{
private:
    PathBitEntry _legacyType = {};

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
