/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
#include "../ride/RideGroupManager.h"
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

/**
 *
 *  rct2: 0x006E588E
 */
void wall_remove_at(const CoordsXYRangedZ& wallPos)
{
    TileElement* tileElement;

repeat:
    tileElement = map_get_first_element_at(wallPos);
    if (tileElement == nullptr)
        return;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (wallPos.baseZ >= tileElement->GetClearanceZ())
            continue;
        if (wallPos.clearanceZ <= tileElement->GetBaseZ())
            continue;

        tile_element_remove_banner_entry(tileElement);
        map_invalidate_tile_zoom1({ wallPos, tileElement->GetBaseZ(), tileElement->GetBaseZ() + 72 });
        tile_element_remove(tileElement);
        goto repeat;
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006E57E6
 */
void wall_remove_at_z(const CoordsXYZ& wallPos)
{
    wall_remove_at({ wallPos, wallPos.z, wallPos.z + 48 });
}

/**
 *
 *  rct2: 0x006E5935
 */
void wall_remove_intersecting_walls(const CoordsXYRangedZ& wallPos, Direction direction)
{
    TileElement* tileElement;

    tileElement = map_get_first_element_at(wallPos);
    if (tileElement == nullptr)
        return;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;

        if (tileElement->GetClearanceZ() <= wallPos.baseZ || tileElement->GetBaseZ() >= wallPos.clearanceZ)
            continue;

        if (direction != tileElement->GetDirection())
            continue;

        tile_element_remove_banner_entry(tileElement);
        map_invalidate_tile_zoom1({ wallPos, tileElement->GetBaseZ(), tileElement->GetBaseZ() + 72 });
        tile_element_remove(tileElement);
        tileElement--;
    } while (!(tileElement++)->IsLastForTile());
}

uint8_t WallElement::GetSlope() const
{
    return (type & TILE_ELEMENT_QUADRANT_MASK) >> 6;
}

void WallElement::SetSlope(uint8_t newSlope)
{
    type &= ~TILE_ELEMENT_QUADRANT_MASK;
    type |= (newSlope << 6);
}

colour_t WallElement::GetPrimaryColour() const
{
    return colour_1 & TILE_ELEMENT_COLOUR_MASK;
}

colour_t WallElement::GetSecondaryColour() const
{
    uint8_t secondaryColour = (colour_1 & ~TILE_ELEMENT_COLOUR_MASK) >> 5;
    secondaryColour |= (flags & 0x60) >> 2;
    return secondaryColour;
}

colour_t WallElement::GetTertiaryColour() const
{
    return colour_3 & TILE_ELEMENT_COLOUR_MASK;
}

void WallElement::SetPrimaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour_1 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= newColour;
}

void WallElement::SetSecondaryColour(colour_t newColour)
{
    colour_1 &= TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= (newColour & 0x7) << 5;
    flags &= ~0x60;
    flags |= (newColour & 0x18) << 2;
}

void WallElement::SetTertiaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour_3 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_3 |= newColour;
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

uint8_t WallElement::GetEntryIndex() const
{
    return entryIndex;
}

rct_scenery_entry* WallElement::GetEntry() const
{
    return get_wall_entry(entryIndex);
}

void WallElement::SetEntryIndex(uint8_t newIndex)
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

void WallElement::SetRawRCT1Data(uint32_t rawData)
{
    colour_3 = rawData & 0xFF;
    colour_1 = (rawData >> 8) & 0xFF;
    animation = (rawData >> 16) & 0xFF;
}
