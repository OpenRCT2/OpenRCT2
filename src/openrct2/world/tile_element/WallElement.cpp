/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "WallElement.h"

#include "../../object/ObjectEntryManager.h"
#include "../../object/WallSceneryEntry.h"
#include "../Banner.h"

uint8_t WallElement::GetSlope() const
{
    return (Type & kTileElementQuadrantMask) >> 6;
}

void WallElement::SetSlope(uint8_t newSlope)
{
    Type &= ~kTileElementQuadrantMask;
    Type |= (newSlope << 6);
}

colour_t WallElement::GetPrimaryColour() const
{
    return colour_1;
}

colour_t WallElement::GetSecondaryColour() const
{
    return colour_2;
}

colour_t WallElement::GetTertiaryColour() const
{
    return colour_3;
}

void WallElement::SetPrimaryColour(colour_t newColour)
{
    colour_1 = newColour;
}

void WallElement::SetSecondaryColour(colour_t newColour)
{
    colour_2 = newColour;
}

void WallElement::SetTertiaryColour(colour_t newColour)
{
    colour_3 = newColour;
}

uint8_t WallElement::GetAnimationFrame() const
{
    return (animation >> 3) & 0xF;
}

void WallElement::SetAnimationFrame(uint8_t frameNum)
{
    animation &= WALL_ANIMATION_FLAG_ALL_FLAGS;
    animation |= (frameNum & 0xF) << 3;
}

uint16_t WallElement::GetEntryIndex() const
{
    return entryIndex;
}

const WallSceneryEntry* WallElement::GetEntry() const
{
    return OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(entryIndex);
}

void WallElement::SetEntryIndex(uint16_t newIndex)
{
    entryIndex = newIndex;
}

Banner* WallElement::GetBanner() const
{
    return ::GetBanner(GetBannerIndex());
}

BannerIndex WallElement::GetBannerIndex() const
{
    return banner_index;
}

void WallElement::SetBannerIndex(BannerIndex newIndex)
{
    banner_index = newIndex;
}

bool WallElement::IsAcrossTrack() const
{
    return (animation & WALL_ANIMATION_FLAG_ACROSS_TRACK) != 0;
}

void WallElement::SetAcrossTrack(bool acrossTrack)
{
    animation &= ~WALL_ANIMATION_FLAG_ACROSS_TRACK;
    if (acrossTrack)
        animation |= WALL_ANIMATION_FLAG_ACROSS_TRACK;
}

bool WallElement::AnimationIsBackwards() const
{
    return (animation & WALL_ANIMATION_FLAG_DIRECTION_BACKWARD) != 0;
}

void WallElement::SetAnimationIsBackwards(bool isBackwards)
{
    animation &= ~WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
    if (isBackwards)
        animation |= WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
}
