/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SmallScenery.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../object/ObjectManager.h"
#include "../ride/TrackDesign.h"
#include "Footpath.h"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"
#include "Surface.h"

static int32_t map_place_clear_func(
    TileElement** tile_element, const CoordsXY& coords, uint8_t flags, money32* price, bool is_scenery)
{
    if ((*tile_element)->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
        return 1;

    if (is_scenery && !(flags & GAME_COMMAND_FLAG_PATH_SCENERY))
        return 1;

    auto* scenery = (*tile_element)->AsSmallScenery()->GetEntry();

    if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
    {
        if (scenery != nullptr && scenery_small_entry_has_flag(scenery, SMALL_SCENERY_FLAG_IS_TREE))
            return 1;
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY) && scenery != nullptr)
        *price += scenery->removal_price * 10;

    if (flags & GAME_COMMAND_FLAG_GHOST)
        return 0;

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
        return 0;

    map_invalidate_tile({ coords, (*tile_element)->GetBaseZ(), (*tile_element)->GetClearanceZ() });

    tile_element_remove(*tile_element);

    (*tile_element)--;
    return 0;
}

/**
 *
 *  rct2: 0x006E0D6E, 0x006B8D88
 */
int32_t map_place_scenery_clear_func(TileElement** tile_element, const CoordsXY& coords, uint8_t flags, money32* price)
{
    return map_place_clear_func(tile_element, coords, flags, price, /*is_scenery=*/true);
}

/**
 *
 *  rct2: 0x006C5A4F, 0x006CDE57, 0x006A6733, 0x0066637E
 */
int32_t map_place_non_scenery_clear_func(TileElement** tile_element, const CoordsXY& coords, uint8_t flags, money32* price)
{
    return map_place_clear_func(tile_element, coords, flags, price, /*is_scenery=*/false);
}

bool scenery_small_entry_has_flag(const SmallSceneryEntry* sceneryEntry, uint32_t flags)
{
    return static_cast<bool>(sceneryEntry->flags & flags);
}

uint8_t SmallSceneryElement::GetSceneryQuadrant() const
{
    return (this->type & TILE_ELEMENT_QUADRANT_MASK) >> 6;
}

void SmallSceneryElement::SetSceneryQuadrant(uint8_t newQuadrant)
{
    type &= ~TILE_ELEMENT_QUADRANT_MASK;
    type |= (newQuadrant << 6);
}

uint16_t SmallSceneryElement::GetEntryIndex() const
{
    return this->entryIndex;
}

void SmallSceneryElement::SetEntryIndex(uint16_t newIndex)
{
    this->entryIndex = newIndex;
}

uint8_t SmallSceneryElement::GetAge() const
{
    return this->age;
}

void SmallSceneryElement::SetAge(uint8_t newAge)
{
    this->age = newAge;
}

void SmallSceneryElement::IncreaseAge(const CoordsXY& sceneryPos)
{
    if (IsGhost())
        return;

    if (age < 255)
    {
        uint8_t newAge = age++;

        // Only invalidate tiles when scenery crosses the withering thresholds, and can be withered.
        if (newAge == SCENERY_WITHER_AGE_THRESHOLD_1 || newAge == SCENERY_WITHER_AGE_THRESHOLD_2)
        {
            auto* sceneryEntry = GetEntry();

            if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_CAN_WITHER))
            {
                map_invalidate_tile_zoom1({ sceneryPos, GetBaseZ(), GetClearanceZ() });
            }
        }
    }
}

colour_t SmallSceneryElement::GetPrimaryColour() const
{
    return colour_1 & TILE_ELEMENT_COLOUR_MASK;
}

colour_t SmallSceneryElement::GetSecondaryColour() const
{
    return colour_2 & TILE_ELEMENT_COLOUR_MASK;
}

void SmallSceneryElement::SetPrimaryColour(colour_t colour)
{
    assert(colour <= 31);
    colour_1 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= colour;
}

void SmallSceneryElement::SetSecondaryColour(colour_t colour)
{
    assert(colour <= 31);
    colour_2 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_2 |= colour;
}

bool SmallSceneryElement::NeedsSupports() const
{
    return static_cast<bool>(colour_1 & MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS);
}

void SmallSceneryElement::SetNeedsSupports()
{
    colour_1 |= MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS;
}

SmallSceneryEntry* SmallSceneryElement::GetEntry() const
{
    return get_small_scenery_entry(entryIndex);
}

SmallSceneryEntry* get_small_scenery_entry(ObjectEntryIndex entryIndex)
{
    SmallSceneryEntry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::SmallScenery, entryIndex);
    if (obj != nullptr)
    {
        result = static_cast<SmallSceneryEntry*>(obj->GetLegacyData());
    }
    return result;
}
