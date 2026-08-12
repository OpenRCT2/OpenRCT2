/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Banner.h"
#include "TileElementBase.h"

namespace OpenRCT2
{
    struct BannerSceneryEntry;

#pragma pack(push, 1)
    struct BannerElement : TileElementBase
    {
        static constexpr TileElementType kElementType = TileElementType::banner;

    private:
        BannerIndex index;    // 5
        uint8_t position;     // 7
        uint8_t allowedEdges; // 8
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
        uint8_t pad09[7];
#pragma clang diagnostic pop
    public:
        Banner* getBanner() const;
        const BannerSceneryEntry* getEntry() const;

        BannerIndex getIndex() const;
        void setIndex(BannerIndex newIndex);

        uint8_t getPosition() const;
        void setPosition(uint8_t newPosition);

        uint8_t getAllowedEdges() const;
        void setAllowedEdges(uint8_t newEdges);
        void resetAllowedEdges();
    };
    static_assert(sizeof(BannerElement) == kTileElementSize);
#pragma pack(pop)
} // namespace OpenRCT2
