/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Object.h"
#include "WaterEntry.h"

#include <tuple>

class WaterObject final : public Object
{
private:
    WaterObjectEntry _legacyType = {};

public:
    static constexpr ObjectType kObjectType = ObjectType::Water;

    void* GetLegacyData() override
    {
        return &_legacyType;
    }

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const override;

private:
    void ReadJsonPalette(json_t& jPalette);
    uint32_t ParseColour(const std::string& s) const;
};
