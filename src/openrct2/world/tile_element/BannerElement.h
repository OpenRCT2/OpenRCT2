/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Banner.h"
#include "TileElementBase.h"

struct BannerSceneryEntry;

#pragma pack(push, 1)
struct BannerElement : TileElementBase
{
    static constexpr TileElementType kElementType = TileElementType::Banner;

private:
    BannerIndex index;    // 5
    uint8_t position;     // 7
    uint8_t AllowedEdges; // 8
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t Pad09[7];
#pragma clang diagnostic pop
public:
    Banner* GetBanner() const;
    const BannerSceneryEntry* GetEntry() const;

    BannerIndex GetIndex() const;
    void SetIndex(BannerIndex newIndex);

    uint8_t GetPosition() const;
    void SetPosition(uint8_t newPosition);

    uint8_t GetAllowedEdges() const;
    void SetAllowedEdges(uint8_t newEdges);
    void ResetAllowedEdges();
};
static_assert(sizeof(BannerElement) == kTileElementSize);
#pragma pack(pop)
