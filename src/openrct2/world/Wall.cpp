/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Wall.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../common.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "Banner.h"
#include "LargeScenery.h"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"
#include "SmallScenery.h"
#include "Surface.h"
#include "Wall.h"
namespace OpenRCT2
{
    /**
     *
     *  rct2: 0x006E588E
     */
    void WallRemoveAt(const CoordsXYRangedZ& wallPos)
    {
        for (auto wallElement = MapGetWallElementAt(wallPos); wallElement != nullptr;
             wallElement = MapGetWallElementAt(wallPos))
        {
            reinterpret_cast<TileElement*>(wallElement)->RemoveBannerEntry();
            MapInvalidateTileZoom1({ wallPos, wallElement->GetBaseZ(), wallElement->GetBaseZ() + 72 });
            TileElementRemove(reinterpret_cast<TileElement*>(wallElement));
        }
    }

    /**
     *
     *  rct2: 0x006E57E6
     */
    void WallRemoveAtZ(const CoordsXYZ& wallPos)
    {
        WallRemoveAt({ wallPos, wallPos.z, wallPos.z + 48 });
    }

    /**
     *
     *  rct2: 0x006E5935
     */
    void WallRemoveIntersectingWalls(const CoordsXYRangedZ& wallPos, Direction direction)
    {
        TileElement* tileElement;

        tileElement = MapGetFirstElementAt(wallPos);
        if (tileElement == nullptr)
            return;
        do
        {
            if (tileElement->GetType() != TileElementType::Wall)
                continue;

            if (tileElement->GetClearanceZ() <= wallPos.baseZ || tileElement->GetBaseZ() >= wallPos.clearanceZ)
                continue;

            if (direction != tileElement->GetDirection())
                continue;

            tileElement->RemoveBannerEntry();
            MapInvalidateTileZoom1({ wallPos, tileElement->GetBaseZ(), tileElement->GetBaseZ() + 72 });
            TileElementRemove(tileElement);
            tileElement--;
        } while (!(tileElement++)->IsLastForTile());
    }

    uint8_t WallElement::GetSlope() const
    {
        return (Type & TILE_ELEMENT_QUADRANT_MASK) >> 6;
    }

    void WallElement::SetSlope(uint8_t newSlope)
    {
        Type &= ~TILE_ELEMENT_QUADRANT_MASK;
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

    WallSceneryEntry* WallElement::GetEntry() const
    {
        return GetWallEntry(entryIndex);
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
} // namespace OpenRCT2
