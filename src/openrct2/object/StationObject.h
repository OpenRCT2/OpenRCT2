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

namespace OpenRCT2
{
    enum class StationObjectFlag : uint8_t
    {
        hasPrimaryColour,
        hasSecondaryColour,
        isTransparent,
        noPlatforms,
        hasShelter,
    };
    using StationObjectFlags = FlagHolder<uint32_t, StationObjectFlag>;

    enum
    {
        SPR_STATION_COVER_OFFSET_NE_SW_BACK_0 = 0,
        SPR_STATION_COVER_OFFSET_NE_SW_BACK_1,
        SPR_STATION_COVER_OFFSET_NE_SW_FRONT,
        SPR_STATION_COVER_OFFSET_SE_NW_BACK_0,
        SPR_STATION_COVER_OFFSET_SE_NW_BACK_1,
        SPR_STATION_COVER_OFFSET_SE_NW_FRONT,
        SPR_STATION_COVER_OFFSET_TALL
    };

    class StationObject final : public Object
    {
    public:
        static constexpr ObjectType kObjectType = ObjectType::station;

        StringId NameStringId{};

        ImageIndex baseImageIndex = kImageIndexUndefined;

        ImageIndex entranceBackIndex = kImageIndexUndefined;
        ImageIndex entranceFrontIndex = kImageIndexUndefined;
        ImageIndex exitBackIndex = kImageIndexUndefined;
        ImageIndex exitFrontIndex = kImageIndexUndefined;

        ImageIndex entranceBackGlassIndex = kImageIndexUndefined;
        ImageIndex entranceFrontGlassIndex = kImageIndexUndefined;
        ImageIndex exitBackGlassIndex = kImageIndexUndefined;
        ImageIndex exitFrontGlassIndex = kImageIndexUndefined;

        ImageIndex shelterIndex = kImageIndexUndefined;
        ImageIndex shelterGlassIndex = kImageIndexUndefined;

        ImageIndex iconIndex = kImageIndexUndefined;

        StationObjectFlags Flags{};
        int32_t Height{};
        uint8_t ScrollingMode{};

        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void Load() override;
        void Unload() override;

        void DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const override;
    };
} // namespace OpenRCT2
