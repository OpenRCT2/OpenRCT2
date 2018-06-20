/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <tuple>
#include "../world/Water.h"
#include "Object.h"

class WaterObject final : public Object
{
private:
    rct_water_type _legacyType = {};

public:
    explicit WaterObject(const rct_object_entry &entry) : Object(entry) { }

    void * GetLegacyData()  override { return &_legacyType; }

    void ReadJson(IReadObjectContext * context, const json_t * root) override;
    void ReadLegacy(IReadObjectContext * context, IStream * stream) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const override;

private:
    void ReadJsonPalette(const json_t * jPalette);
    uint32_t ParseColour(const std::string &s) const;
};
