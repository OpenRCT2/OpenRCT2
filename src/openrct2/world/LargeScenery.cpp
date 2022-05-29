/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LargeScenery.h"

#include "../Context.h"
#include "../common.h"
#include "../object/LargeSceneryObject.h"
#include "../object/ObjectManager.h"
#include "../world/Banner.h"
#include "TileElement.h"

colour_t LargeSceneryElement::GetPrimaryColour() const
{
    return Colour[0];
}

colour_t LargeSceneryElement::GetSecondaryColour() const
{
    return Colour[1];
}

colour_t LargeSceneryElement::GetTertiaryColour() const
{
    return Colour[2];
}

void LargeSceneryElement::SetPrimaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    Colour[0] = newColour;
}

void LargeSceneryElement::SetSecondaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    Colour[1] = newColour;
}

void LargeSceneryElement::SetTertiaryColour(colour_t newColour)
{
    assert(newColour <= 31);
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

LargeSceneryEntry* LargeSceneryElement::GetEntry() const
{
    return get_large_scenery_entry(GetEntryIndex());
}

const LargeSceneryObject* LargeSceneryElement::GetObject() const
{
    return static_cast<const LargeSceneryObject*>(object_entry_get_object(ObjectType::LargeScenery, GetEntryIndex()));
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

LargeSceneryEntry* get_large_scenery_entry(ObjectEntryIndex entryIndex)
{
    LargeSceneryEntry* result = nullptr;
    auto* objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr->GetLoadedObject(ObjectType::LargeScenery, entryIndex);
    if (obj != nullptr)
    {
        result = static_cast<LargeSceneryEntry*>(obj->GetLegacyData());
    }
    return result;
}
