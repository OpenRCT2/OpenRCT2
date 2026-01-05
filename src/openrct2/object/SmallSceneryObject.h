/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Scenery.h"
#include "SceneryObject.h"
#include "SmallSceneryEntry.h"

#include <vector>

namespace OpenRCT2
{
    class SmallSceneryObject final : public SceneryObject
    {
    private:
        SmallSceneryEntry _legacyType = {};
        std::vector<uint8_t> _frameOffsets;

    public:
        static constexpr ObjectType kObjectType = ObjectType::smallScenery;

        void* GetLegacyData() override
        {
            return &_legacyType;
        }

        void ReadLegacy(IReadObjectContext* context, IStream* stream) override;
        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void Load() override;
        void Unload() override;

        void DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const override;

    private:
        static std::vector<uint8_t> ReadFrameOffsets(IStream* stream);
        static std::vector<uint8_t> ReadJsonFrameOffsets(json_t& jFrameOffsets);
        void PerformFixes();
    };
} // namespace OpenRCT2
