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

static money32 WallSetColour(
    int16_t x, int16_t y, uint8_t baseHeight, uint8_t direction, uint8_t primaryColour, uint8_t secondaryColour,
    uint8_t tertiaryColour, uint8_t flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    int32_t z = baseHeight * 8;

    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !map_is_location_in_park({ x, y }) && !gCheatsSandboxMode)
    {
        return MONEY32_UNDEFINED;
    }

    auto wallElement = map_get_wall_element_at(x, y, baseHeight, direction);
    if (wallElement == nullptr)
    {
        return 0;
    }

    if ((flags & GAME_COMMAND_FLAG_GHOST) && !(wallElement->IsGhost()))
    {
        return 0;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        rct_scenery_entry* scenery_entry = wallElement->GetEntry();
        wallElement->SetPrimaryColour(primaryColour);
        wallElement->SetSecondaryColour(secondaryColour);

        if (scenery_entry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
        {
            wallElement->SetTertiaryColour(tertiaryColour);
        }
        map_invalidate_tile_zoom1(x, y, z, z + 72);
    }

    return 0;
}

/**
 *
 *  rct2: 0x006E588E
 */
void wall_remove_at(int32_t x, int32_t y, int32_t z0, int32_t z1)
{
    TileElement* tileElement;

    z0 /= 8;
    z1 /= 8;
repeat:
    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (z0 >= tileElement->clearance_height)
            continue;
        if (z1 <= tileElement->base_height)
            continue;

        tile_element_remove_banner_entry(tileElement);
        map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->base_height * 8 + 72);
        tile_element_remove(tileElement);
        goto repeat;
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006E57E6
 */
void wall_remove_at_z(int32_t x, int32_t y, int32_t z)
{
    wall_remove_at(x, y, z, z + 48);
}

/**
 *
 *  rct2: 0x006E5935
 */
void wall_remove_intersecting_walls(int32_t x, int32_t y, int32_t z0, int32_t z1, int32_t direction)
{
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;

        if (tileElement->clearance_height <= z0 || tileElement->base_height >= z1)
            continue;

        if (direction != tileElement->GetDirection())
            continue;

        tile_element_remove_banner_entry(tileElement);
        map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->base_height * 8 + 72);
        tile_element_remove(tileElement);
        tileElement--;
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006E56B5
 */
void game_command_set_wall_colour(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, [[maybe_unused]] int32_t* edi,
    int32_t* ebp)
{
    *ebx = WallSetColour(
        *eax & 0xFFFF, *ecx & 0xFFFF, (*edx >> 8) & 0xFF, *edx & 0xFF, (*ebx >> 8) & 0xFF, *ebp & 0xFF, (*ebp >> 8) & 0xFF,
        *ebx & 0xFF);
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
