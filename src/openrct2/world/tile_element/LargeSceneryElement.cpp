/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LargeSceneryElement.h"

#include "../../drawing/Colour.h"
#include "../../object/LargeSceneryEntry.h"
#include "../../object/ObjectEntryManager.h"
#include "../Banner.h"

#include <cassert>

namespace OpenRCT2
{
    Drawing::Colour LargeSceneryElement::getPrimaryColour() const
    {
        return colour[0];
    }

    Drawing::Colour LargeSceneryElement::getSecondaryColour() const
    {
        return colour[1];
    }

    Drawing::Colour LargeSceneryElement::getTertiaryColour() const
    {
        return colour[2];
    }

    void LargeSceneryElement::setPrimaryColour(Drawing::Colour newColour)
    {
        assert(EnumValue(newColour) < Drawing::kColourNumTotal);
        colour[0] = newColour;
    }

    void LargeSceneryElement::setSecondaryColour(Drawing::Colour newColour)
    {
        assert(EnumValue(newColour) < Drawing::kColourNumTotal);
        colour[1] = newColour;
    }

    void LargeSceneryElement::setTertiaryColour(Drawing::Colour newColour)
    {
        assert(EnumValue(newColour) < Drawing::kColourNumTotal);
        colour[2] = newColour;
    }

    Banner* LargeSceneryElement::getBanner() const
    {
        return ::GetBanner(getBannerIndex());
    }

    BannerIndex LargeSceneryElement::getBannerIndex() const
    {
        return bannerIndex;
    }

    void LargeSceneryElement::setBannerIndex(::BannerIndex newIndex)
    {
        this->bannerIndex = newIndex;
    }

    bool LargeSceneryElement::isAccounted() const
    {
        return (flags2 & LARGE_SCENERY_ELEMENT_FLAGS2_ACCOUNTED) != 0;
    }

    void LargeSceneryElement::setIsAccounted(bool isAccounted)
    {
        if (isAccounted)
        {
            flags2 |= LARGE_SCENERY_ELEMENT_FLAGS2_ACCOUNTED;
        }
        else
        {
            flags2 &= ~LARGE_SCENERY_ELEMENT_FLAGS2_ACCOUNTED;
        }
    }

    ObjectEntryIndex LargeSceneryElement::getEntryIndex() const
    {
        return entryIndex;
    }

    const LargeSceneryEntry* LargeSceneryElement::getEntry() const
    {
        return OpenRCT2::ObjectEntryManager::GetObjectEntry<LargeSceneryEntry>(getEntryIndex());
    }

    uint8_t LargeSceneryElement::getSequenceIndex() const
    {
        return sequenceIndex;
    }

    void LargeSceneryElement::setEntryIndex(ObjectEntryIndex newIndex)
    {
        entryIndex = newIndex;
    }

    void LargeSceneryElement::setSequenceIndex(uint8_t newIndex)
    {
        sequenceIndex = newIndex;
    }
} // namespace OpenRCT2
