/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
    uint8_t WallElement::getSlope() const
    {
        return (type & kTileElementQuadrantMask) >> 6;
    }

    void WallElement::setSlope(uint8_t newSlope)
    {
        type &= ~kTileElementQuadrantMask;
        type |= (newSlope << 6);
    }

    Drawing::Colour WallElement::getPrimaryColour() const
    {
        return colour1;
    }

    Drawing::Colour WallElement::getSecondaryColour() const
    {
        return colour2;
    }

    Drawing::Colour WallElement::getTertiaryColour() const
    {
        return colour3;
    }

    void WallElement::setPrimaryColour(Drawing::Colour newColour)
    {
        colour1 = newColour;
    }

    void WallElement::setSecondaryColour(Drawing::Colour newColour)
    {
        colour2 = newColour;
    }

    void WallElement::setTertiaryColour(Drawing::Colour newColour)
    {
        colour3 = newColour;
    }

    uint8_t WallElement::getAnimationFrame() const
    {
        return (animation >> 3) & 0xF;
    }

    void WallElement::setAnimationFrame(uint8_t frameNum)
    {
        animation &= WALL_ANIMATION_FLAG_ALL_FLAGS;
        animation |= (frameNum & 0xF) << 3;
    }

    bool WallElement::isAnimating() const
    {
        return (animation & WALL_ANIMATION_FLAG_IS_ANIMATING) != 0;
    }

    void WallElement::setIsAnimating(const bool isAnimating)
    {
        if (isAnimating)
            animation |= WALL_ANIMATION_FLAG_IS_ANIMATING;
        else
            animation &= ~WALL_ANIMATION_FLAG_IS_ANIMATING;
    }

    uint16_t WallElement::getEntryIndex() const
    {
        return entryIndex;
    }

    const WallSceneryEntry* WallElement::getEntry() const
    {
        return ObjectEntryManager::GetObjectEntry<WallSceneryEntry>(entryIndex);
    }

    void WallElement::setEntryIndex(uint16_t newIndex)
    {
        entryIndex = newIndex;
    }

    Banner* WallElement::getBanner() const
    {
        return ::GetBanner(getBannerIndex());
    }

    BannerIndex WallElement::getBannerIndex() const
    {
        return bannerIndex;
    }

    void WallElement::setBannerIndex(BannerIndex newIndex)
    {
        bannerIndex = newIndex;
    }

    bool WallElement::isAcrossTrack() const
    {
        return (animation & WALL_ANIMATION_FLAG_ACROSS_TRACK) != 0;
    }

    void WallElement::setAcrossTrack(bool acrossTrack)
    {
        animation &= ~WALL_ANIMATION_FLAG_ACROSS_TRACK;
        if (acrossTrack)
            animation |= WALL_ANIMATION_FLAG_ACROSS_TRACK;
    }

    bool WallElement::animationIsBackwards() const
    {
        return (animation & WALL_ANIMATION_FLAG_DIRECTION_BACKWARD) != 0;
    }

    void WallElement::setAnimationIsBackwards(bool isBackwards)
    {
        animation &= ~WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
        if (isBackwards)
            animation |= WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
    }
} // namespace OpenRCT2
