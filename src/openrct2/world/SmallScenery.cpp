/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_CAN_WITHER))
            {
                MapInvalidateTileZoom1({ sceneryPos, GetBaseZ(), GetClearanceZ() });
            }
        }
    }
}

colour_t SmallSceneryElement::GetPrimaryColour() const
{
    return Colour[0] & TILE_ELEMENT_COLOUR_MASK;
}

colour_t SmallSceneryElement::GetSecondaryColour() const
{
    return Colour[1] & TILE_ELEMENT_COLOUR_MASK;
}

colour_t SmallSceneryElement::GetTertiaryColour() const
{
    return Colour[2] & TILE_ELEMENT_COLOUR_MASK;
}

void SmallSceneryElement::SetPrimaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    Colour[0] &= ~TILE_ELEMENT_COLOUR_MASK;
    Colour[0] |= newColour;
}

void SmallSceneryElement::SetSecondaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    Colour[1] &= ~TILE_ELEMENT_COLOUR_MASK;
    Colour[1] |= newColour;
}

void SmallSceneryElement::SetTertiaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    Colour[2] &= ~TILE_ELEMENT_COLOUR_MASK;
    Colour[2] |= newColour;
}

bool SmallSceneryElement::NeedsSupports() const
{
    return static_cast<bool>(Colour[0] & MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS);
}

void SmallSceneryElement::SetNeedsSupports()
{
    Colour[0] |= MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS;
}

SmallSceneryEntry* SmallSceneryElement::GetEntry() const
{
    return GetSmallSceneryEntry(entryIndex);
}

SmallSceneryEntry* GetSmallSceneryEntry(ObjectEntryIndex entryIndex)
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
