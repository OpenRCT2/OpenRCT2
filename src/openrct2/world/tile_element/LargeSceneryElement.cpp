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
#include "../../object/ObjectManager.h"
#include "../Banner.h"

#include <cassert>

namespace OpenRCT2
{
    Drawing::Colour LargeSceneryElement::GetPrimaryColour() const
    {
        return Colour[0];
    }

    Drawing::Colour LargeSceneryElement::GetSecondaryColour() const
    {
        return Colour[1];
    }

    Drawing::Colour LargeSceneryElement::GetTertiaryColour() const
    {
        return Colour[2];
    }

    void LargeSceneryElement::SetPrimaryColour(Drawing::Colour newColour)
    {
        assert(EnumValue(newColour) < Drawing::kColourNumTotal);
        Colour[0] = newColour;
    }

    void LargeSceneryElement::SetSecondaryColour(Drawing::Colour newColour)
    {
        assert(EnumValue(newColour) < Drawing::kColourNumTotal);
        Colour[1] = newColour;
    }

    void LargeSceneryElement::SetTertiaryColour(Drawing::Colour newColour)
    {
        assert(EnumValue(newColour) < Drawing::kColourNumTotal);
        Colour[2] = newColour;
    }

    Banner* LargeSceneryElement::GetBanner() const
    {
        return ::GetBanner(GetBannerIndex());
    }

    BannerIndex LargeSceneryElement::GetBannerIndex() const
    {
        return BannerIndex;
    }

    void LargeSceneryElement::SetBannerIndex(::BannerIndex newIndex)
    {
        this->BannerIndex = newIndex;
    }

    bool LargeSceneryElement::IsAccounted() const
    {
        return (Flags2 & LARGE_SCENERY_ELEMENT_FLAGS2_ACCOUNTED) != 0;
    }

    void LargeSceneryElement::SetIsAccounted(bool isAccounted)
    {
        if (isAccounted)
        {
            Flags2 |= LARGE_SCENERY_ELEMENT_FLAGS2_ACCOUNTED;
        }
        else
        {
            Flags2 &= ~LARGE_SCENERY_ELEMENT_FLAGS2_ACCOUNTED;
        }
    }

    ObjectEntryIndex LargeSceneryElement::GetEntryIndex() const
    {
        return EntryIndex;
    }

    const LargeSceneryEntry* LargeSceneryElement::GetEntry() const
    {
        return OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(GetEntryIndex());
    }

    uint8_t LargeSceneryElement::GetSequenceIndex() const
    {
        return SequenceIndex;
    }

    void LargeSceneryElement::SetEntryIndex(ObjectEntryIndex newIndex)
    {
        EntryIndex = newIndex;
    }

    void LargeSceneryElement::SetSequenceIndex(uint8_t sequence)
    {
        SequenceIndex = sequence;
    }
} // namespace OpenRCT2
