/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SmallSceneryElement.h"

#include "../../Cheats.h"
#include "../../Context.h"
#include "../../Game.h"
#include "../../OpenRCT2.h"
#include "../../management/Finance.h"
#include "../../network/network.h"
#include "../../object/ObjectEntryManager.h"
#include "../../object/ObjectManager.h"
#include "../../object/SmallSceneryEntry.h"
#include "../../ride/TrackDesign.h"
#include "../Footpath.h"
#include "../Map.h"
#include "../MapAnimation.h"
#include "../Park.h"
#include "../Scenery.h"

#include <cassert>

uint8_t SmallSceneryElement::GetSceneryQuadrant() const
{
    return (this->Type & kTileElementQuadrantMask) >> 6;
}

void SmallSceneryElement::SetSceneryQuadrant(uint8_t newQuadrant)
{
    Type &= ~kTileElementQuadrantMask;
    Type |= (newQuadrant << 6);
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
        if (newAge == kSceneryWitherAgeThreshold1 || newAge == kSceneryWitherAgeThreshold2)
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
    return Colour[0];
}

colour_t SmallSceneryElement::GetSecondaryColour() const
{
    return Colour[1];
}

colour_t SmallSceneryElement::GetTertiaryColour() const
{
    return Colour[2];
}

void SmallSceneryElement::SetPrimaryColour(colour_t newColour)
{
    assert(newColour < COLOUR_COUNT);
    Colour[0] = newColour;
}

void SmallSceneryElement::SetSecondaryColour(colour_t newColour)
{
    assert(newColour < COLOUR_COUNT);
    Colour[1] = newColour;
}

void SmallSceneryElement::SetTertiaryColour(colour_t newColour)
{
    assert(newColour < COLOUR_COUNT);
    Colour[2] = newColour;
}

bool SmallSceneryElement::NeedsSupports() const
{
    return static_cast<bool>(Flags2 & MAP_ELEM_SMALL_SCENERY_FLAGS2_NEEDS_SUPPORTS);
}

void SmallSceneryElement::SetNeedsSupports()
{
    Flags2 |= MAP_ELEM_SMALL_SCENERY_FLAGS2_NEEDS_SUPPORTS;
}

const SmallSceneryEntry* SmallSceneryElement::GetEntry() const
{
    return OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(entryIndex);
}
