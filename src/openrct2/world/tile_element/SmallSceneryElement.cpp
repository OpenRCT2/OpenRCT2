/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../../network/Network.h"
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

namespace OpenRCT2
{
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

    Drawing::Colour SmallSceneryElement::GetPrimaryColour() const
    {
        return Colour[0];
    }

    Drawing::Colour SmallSceneryElement::GetSecondaryColour() const
    {
        return Colour[1];
    }

    Drawing::Colour SmallSceneryElement::GetTertiaryColour() const
    {
        return Colour[2];
    }

    void SmallSceneryElement::SetPrimaryColour(Drawing::Colour newColour)
    {
        assert(Drawing::colourIsValid(newColour));
        Colour[0] = newColour;
    }

    void SmallSceneryElement::SetSecondaryColour(Drawing::Colour newColour)
    {
        assert(Drawing::colourIsValid(newColour));
        Colour[1] = newColour;
    }

    void SmallSceneryElement::SetTertiaryColour(Drawing::Colour newColour)
    {
        assert(Drawing::colourIsValid(newColour));
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
        return ObjectManager::GetObjectEntry<SmallSceneryEntry>(entryIndex);
    }
} // namespace OpenRCT2
