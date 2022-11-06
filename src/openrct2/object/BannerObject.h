/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/IStream.hpp"
#include "SceneryObject.h"

enum
{
    BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR = (1 << 0),
};

struct BannerSceneryEntry
{
    StringId name;
    uint32_t image;
    uint8_t scrolling_mode;
    uint8_t flags;
    int16_t price;
    ObjectEntryIndex scenery_tab_id;
};

class BannerObject final : public SceneryObject
{
private:
    BannerSceneryEntry _legacyType = {};

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
