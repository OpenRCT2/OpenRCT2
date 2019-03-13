/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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

static money32 SmallScenerySetColour(
    int16_t x, int16_t y, uint8_t baseHeight, uint8_t quadrant, uint8_t sceneryType, uint8_t primaryColour,
    uint8_t secondaryColour, uint8_t flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    int32_t z = baseHeight * 8;
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
    {
        if (!map_is_location_owned(x, y, z))
        {
            return MONEY32_UNDEFINED;
        }
    }

    TileElement* tileElement = map_get_small_scenery_element_at(x, y, baseHeight, sceneryType, quadrant);

    if (tileElement == nullptr)
    {
        return 0;
    }

    if ((flags & GAME_COMMAND_FLAG_GHOST) && !(tileElement->IsGhost()))
    {
        return 0;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        tileElement->AsSmallScenery()->SetPrimaryColour(primaryColour);
        tileElement->AsSmallScenery()->SetSecondaryColour(secondaryColour);

        map_invalidate_tile_full(x, y);
    }

    return 0;
}

/**
 *
 *  rct2: 0x006E0F26
 */
void game_command_set_scenery_colour(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, [[maybe_unused]] int32_t* edi,
    int32_t* ebp)
{
    *ebx = SmallScenerySetColour(
        *eax & 0xFFFF, *ecx & 0xFFFF, *edx & 0xFF, ((*ebx >> 8) & 0xFF), (*edx >> 8) & 0xFF, *ebp & 0xFF, (*ebp >> 8) & 0xFF,
        *ebx & 0xFF);
}

/**
 *
 *  rct2: 0x006E0D6E, 0x006B8D88
 */
int32_t map_place_scenery_clear_func(TileElement** tile_element, int32_t x, int32_t y, uint8_t flags, money32* price)
{
    if ((*tile_element)->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
        return 1;

    if (!(flags & GAME_COMMAND_FLAG_PATH_SCENERY))
        return 1;

    rct_scenery_entry* scenery = (*tile_element)->AsSmallScenery()->GetEntry();

    if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
    {
        if (scenery->small_scenery.height > 64)
            return 1;
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        *price += scenery->small_scenery.removal_price * 10;

    if (flags & GAME_COMMAND_FLAG_GHOST)
        return 0;

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
        return 0;

    map_invalidate_tile(x, y, (*tile_element)->base_height * 8, (*tile_element)->clearance_height * 8);

    tile_element_remove(*tile_element);

    (*tile_element)--;
    return 0;
}

/**
 *
 *  rct2: 0x006C5A4F, 0x006CDE57, 0x006A6733, 0x0066637E
 */
int32_t map_place_non_scenery_clear_func(TileElement** tile_element, int32_t x, int32_t y, uint8_t flags, money32* price)
{
    if ((*tile_element)->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
        return 1;

    rct_scenery_entry* scenery = (*tile_element)->AsSmallScenery()->GetEntry();

    if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
    {
        if (scenery->small_scenery.height > 64)
            return 1;
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        *price += scenery->small_scenery.removal_price * 10;

    if (flags & GAME_COMMAND_FLAG_GHOST)
        return 0;

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
        return 0;

    map_invalidate_tile(x, y, (*tile_element)->base_height * 8, (*tile_element)->clearance_height * 8);

    tile_element_remove(*tile_element);

    (*tile_element)--;
    return 0;
}

bool scenery_small_entry_has_flag(const rct_scenery_entry* sceneryEntry, uint32_t flags)
{
    return (bool)(sceneryEntry->small_scenery.flags & flags);
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

uint8_t SmallSceneryElement::GetEntryIndex() const
{
    return this->entryIndex;
}

void SmallSceneryElement::SetEntryIndex(uint8_t newIndex)
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

void SmallSceneryElement::IncreaseAge(int32_t x, int32_t y)
{
    if (IsGhost())
        return;

    if (age < 255)
    {
        uint8_t newAge = age++;

        // Only invalidate tiles when scenery crosses the withering threshholds, and can be withered.
        if (newAge == SCENERY_WITHER_AGE_THRESHOLD_1 || newAge == SCENERY_WITHER_AGE_THRESHOLD_2)
        {
            rct_scenery_entry* entry = GetEntry();

            if (scenery_small_entry_has_flag(entry, SMALL_SCENERY_FLAG_CAN_WITHER))
            {
                map_invalidate_tile_zoom1(x, y, base_height * 8, clearance_height * 8);
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
    return (bool)(colour_1 & MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS);
}

void SmallSceneryElement::SetNeedsSupports()
{
    colour_1 |= MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS;
}

rct_scenery_entry* SmallSceneryElement::GetEntry() const
{
    return get_small_scenery_entry(entryIndex);
}

rct_scenery_entry* get_small_scenery_entry(int32_t entryIndex)
{
    rct_scenery_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_SMALL_SCENERY, entryIndex);
    if (obj != nullptr)
    {
        result = (rct_scenery_entry*)obj->GetLegacyData();
    }
    return result;
}
