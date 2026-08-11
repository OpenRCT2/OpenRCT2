/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Object.h"
#include "WaterEntry.h"

namespace OpenRCT2
{
    class WaterObject final : public Object
    {
    private:
        WaterObjectEntry _legacyType = {};

    public:
        static constexpr ObjectType kObjectType = ObjectType::water;

        void* GetLegacyData() override
        {
            return &_legacyType;
        }

        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void ReadLegacy(IReadObjectContext* context, IStream* stream) override;
        void Load() override;
        void Unload() override;

        void DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const override;

    private:
        void ReadJsonPalette(json_t& jPalette);
    };
} // namespace OpenRCT2
