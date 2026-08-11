/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SmallSceneryElement.h"

#include "../../drawing/Colour.h"
#include "../../object/ObjectEntryManager.h"
#include "../../object/SmallSceneryEntry.h"
#include "../Map.h"
#include "../Scenery.h"

#include <cassert>

namespace OpenRCT2
{
    uint8_t SmallSceneryElement::getSceneryQuadrant() const
    {
        return (this->type & kTileElementQuadrantMask) >> 6;
    }

    void SmallSceneryElement::setSceneryQuadrant(uint8_t newQuadrant)
    {
        type &= ~kTileElementQuadrantMask;
        type |= (newQuadrant << 6);
    }

    uint16_t SmallSceneryElement::getEntryIndex() const
    {
        return this->entryIndex;
    }

    void SmallSceneryElement::setEntryIndex(uint16_t newIndex)
    {
        this->entryIndex = newIndex;
    }

    uint8_t SmallSceneryElement::getAge() const
    {
        return this->age;
    }

    void SmallSceneryElement::setAge(uint8_t newAge)
    {
        this->age = newAge;
    }

    void SmallSceneryElement::increaseAge(const CoordsXY& sceneryPos)
    {
        if (isGhost())
            return;

        if (age < 255)
        {
            uint8_t newAge = age++;

            // Only invalidate tiles when scenery crosses the withering thresholds, and can be withered.
            if (newAge == kSceneryWitherAgeThreshold1 || newAge == kSceneryWitherAgeThreshold2)
            {
                auto* sceneryEntry = getEntry();

                if (sceneryEntry->flags.has(SmallSceneryFlag::canWither))
                {
                    MapInvalidateTileZoom1({ sceneryPos, getBaseZ(), getClearanceZ() });
                }
            }
        }
    }

    Drawing::Colour SmallSceneryElement::getPrimaryColour() const
    {
        return colour[0];
    }

    Drawing::Colour SmallSceneryElement::getSecondaryColour() const
    {
        return colour[1];
    }

    Drawing::Colour SmallSceneryElement::getTertiaryColour() const
    {
        return colour[2];
    }

    void SmallSceneryElement::setPrimaryColour(Drawing::Colour newColour)
    {
        assert(Drawing::colourIsValid(newColour));
        colour[0] = newColour;
    }

    void SmallSceneryElement::setSecondaryColour(Drawing::Colour newColour)
    {
        assert(Drawing::colourIsValid(newColour));
        colour[1] = newColour;
    }

    void SmallSceneryElement::setTertiaryColour(Drawing::Colour newColour)
    {
        assert(Drawing::colourIsValid(newColour));
        colour[2] = newColour;
    }

    bool SmallSceneryElement::needsSupports() const
    {
        return static_cast<bool>(flags2 & MAP_ELEM_SMALL_SCENERY_FLAGS2_NEEDS_SUPPORTS);
    }

    void SmallSceneryElement::setNeedsSupports()
    {
        flags2 |= MAP_ELEM_SMALL_SCENERY_FLAGS2_NEEDS_SUPPORTS;
    }

    const SmallSceneryEntry* SmallSceneryElement::getEntry() const
    {
        return ObjectEntryManager::GetObjectEntry<SmallSceneryEntry>(entryIndex);
    }
} // namespace OpenRCT2
