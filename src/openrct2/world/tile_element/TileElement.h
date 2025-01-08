/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Identifiers.h"
#include "../Banner.h"
#include "TileElementBase.h"

#pragma pack(push, 1)
/**
 * Map element structure
 * size: 0x10
 */
struct TileElement : public TileElementBase
{
    uint8_t Pad05[3];
    uint8_t Pad08[8];

    void ClearAs(TileElementType newType);

    RideId GetRideIndex() const;

    void SetBannerIndex(BannerIndex newIndex);
    void RemoveBannerEntry();
    BannerIndex GetBannerIndex() const;
};
static_assert(sizeof(TileElement) == kTileElementSize);

bool TileElementIsUnderground(TileElement* tileElement);

#pragma pack(pop)
