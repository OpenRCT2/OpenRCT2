/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LargeScenery.h"

#include "../Context.h"
#include "../common.h"
#include "../object/ObjectManager.h"
#include "TileElement.h"

colour_t LargeSceneryElement::GetPrimaryColour() const
{
    return colour[0] & TILE_ELEMENT_COLOUR_MASK;
}

colour_t LargeSceneryElement::GetSecondaryColour() const
{
    return colour[1] & TILE_ELEMENT_COLOUR_MASK;
}

void LargeSceneryElement::SetPrimaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour[0] &= ~TILE_ELEMENT_COLOUR_MASK;
    colour[0] |= newColour;
}

void LargeSceneryElement::SetSecondaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour[1] &= ~TILE_ELEMENT_COLOUR_MASK;
    colour[1] |= newColour;
}

BannerIndex LargeSceneryElement::GetBannerIndex() const
{
    return (entryIndex & 0xC0) | (((colour[0]) & ~TILE_ELEMENT_COLOUR_MASK) >> 2)
        | (((colour[1]) & ~TILE_ELEMENT_COLOUR_MASK) >> 5);
}

void LargeSceneryElement::SetBannerIndex(BannerIndex newIndex)
{
    entryIndex |= newIndex & 0xC0;
    colour[0] |= (newIndex & 0x38) << 2;
    colour[1] |= (newIndex & 7) << 5;
}

uint32_t LargeSceneryElement::GetEntryIndex() const
{
    return entryIndex & TILE_ELEMENT_LARGE_TYPE_MASK;
}

rct_scenery_entry* LargeSceneryElement::GetEntry() const
{
    return get_large_scenery_entry(GetEntryIndex());
}

uint16_t LargeSceneryElement::GetSequenceIndex() const
{
    return (entryIndex >> 10);
}

void LargeSceneryElement::SetEntryIndex(uint32_t newIndex)
{
    entryIndex &= ~TILE_ELEMENT_LARGE_TYPE_MASK;
    entryIndex |= (newIndex & TILE_ELEMENT_LARGE_TYPE_MASK);
}

void LargeSceneryElement::SetSequenceIndex(uint16_t sequence)
{
    entryIndex &= TILE_ELEMENT_LARGE_TYPE_MASK;
    entryIndex |= (sequence << 10);
}

rct_scenery_entry* get_large_scenery_entry(int32_t entryIndex)
{
    rct_scenery_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_LARGE_SCENERY, entryIndex);
    if (obj != nullptr)
    {
        result = (rct_scenery_entry*)obj->GetLegacyData();
    }
    return result;
}