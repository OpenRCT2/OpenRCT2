/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Object.h"

class TerrainEdgeObject final : public Object
{
private:
public:
    StringId NameStringId{};
    uint32_t IconImageId{};
    uint32_t BaseImageId{};
    bool HasDoors{};

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const override;

    static TerrainEdgeObject* GetById(ObjectEntryIndex entryIndex);
};
