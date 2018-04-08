#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <tuple>
#include "../world/Water.h"
#include "Object.h"

class WaterObject final : public Object
{
private:
    rct_water_type _legacyType = { 0 };

public:
    explicit WaterObject(const rct_object_entry &entry) : Object(entry) { }

    void * GetLegacyData()  override { return &_legacyType; }

    void ReadJson(IReadObjectContext * context, const json_t * root) override;
    void ReadLegacy(IReadObjectContext * context, IStream * stream) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const override;

private:
    void ReadJsonPalette(const json_t * jPalette);
    uint32 ParseColour(const std::string &s) const;
};
