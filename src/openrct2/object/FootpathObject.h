/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Object.h"

#include "../world/Footpath.h"

class FootpathObject final : public Object
{
private:
    rct_footpath_entry _legacyType = {};

public:
    explicit FootpathObject(const rct_object_entry &entry) : Object(entry) { }

    void * GetLegacyData()  override { return &_legacyType; }

    void ReadLegacy(IReadObjectContext * context, IStream * stream) override;
    void ReadJson(IReadObjectContext * context, const json_t * root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const override;
};
