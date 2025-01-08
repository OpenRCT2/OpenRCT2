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
#include "../../interface/Colour.h"
#include "../../object/ObjectTypes.h"
#include "../../object/WallSceneryEntry.h"
#include "../Banner.h"
#include "TileElementBase.h"

#include <cstdint>

enum
{
    WALL_ANIMATION_FLAG_ACROSS_TRACK = (1 << 2),
    // 3 - 6 animation frame number
    WALL_ANIMATION_FLAG_DIRECTION_BACKWARD = (1 << 7),
    WALL_ANIMATION_FLAG_ALL_FLAGS = WALL_ANIMATION_FLAG_ACROSS_TRACK | WALL_ANIMATION_FLAG_DIRECTION_BACKWARD
};

#pragma pack(push, 1)
struct WallElement : TileElementBase
{
    static constexpr TileElementType kElementType = TileElementType::Wall;

private:
    ObjectEntryIndex entryIndex; // 05
    colour_t colour_1;           // 07
    colour_t colour_2;           // 08
    colour_t colour_3;           // 09
    BannerIndex banner_index;    // 0A
    uint8_t animation;           // 0C 0b_dfff_ft00 d = direction, f = frame num, t = across track flag (not used)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t Pad0D[3];
#pragma clang diagnostic pop

public:
    uint16_t GetEntryIndex() const;
    void SetEntryIndex(uint16_t newIndex);
    const WallSceneryEntry* GetEntry() const;

    uint8_t GetSlope() const;
    void SetSlope(uint8_t newslope);

    colour_t GetPrimaryColour() const;
    void SetPrimaryColour(colour_t newColour);
    colour_t GetSecondaryColour() const;
    void SetSecondaryColour(colour_t newColour);
    colour_t GetTertiaryColour() const;
    void SetTertiaryColour(colour_t newColour);

    uint8_t GetAnimationFrame() const;
    void SetAnimationFrame(uint8_t frameNum);

    Banner* GetBanner() const;
    BannerIndex GetBannerIndex() const;
    void SetBannerIndex(BannerIndex newIndex);

    bool IsAcrossTrack() const;
    void SetAcrossTrack(bool acrossTrack);
    bool AnimationIsBackwards() const;
    void SetAnimationIsBackwards(bool isBackwards);
};
static_assert(sizeof(WallElement) == kTileElementSize);

#pragma pack(pop)
