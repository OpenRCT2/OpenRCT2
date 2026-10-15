/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Identifiers.h"
#include "TileElementBase.h"

namespace OpenRCT2
{
#pragma pack(push, 1)
    /**
     * Map element structure
     * size: 0x10
     */
    struct TileElement : public TileElementBase
    {
        uint8_t pad05[3];
        uint8_t pad08[8];

        void clearAs(TileElementType newType);

        RideId getRideIndex() const;

        void setBannerIndex(BannerIndex newIndex);
        void removeBannerEntry();
        BannerIndex getBannerIndex() const;
    };
    static_assert(sizeof(TileElement) == kTileElementSize);

    bool tileElementIsUnderground(TileElement* tileElement);

#pragma pack(pop)
} // namespace OpenRCT2
