/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../drawing/ImageId.hpp"
#include "Object.h"

namespace OpenRCT2
{
    namespace StationObjectFlags
    {
        const uint32_t hasPrimaryColour = 1 << 0;
        const uint32_t hasSecondaryColour = 1 << 1;
        const uint32_t isTransparent = 1 << 2;
        const uint32_t noPlatforms = 1 << 3;
        const uint32_t hasShelter = (1 << 4);
    } // namespace StationObjectFlags

    class StationObject final : public Object
    {
    public:
        static constexpr ObjectType kObjectType = ObjectType::station;

        StringId NameStringId{};
        ImageIndex BaseImageId = kImageIndexUndefined;
        ImageIndex ShelterImageId = kImageIndexUndefined;
        uint32_t Flags{};
        int32_t Height{};
        uint8_t ScrollingMode{};

        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void Load() override;
        void Unload() override;

        void DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const override;
    };
} // namespace OpenRCT2
