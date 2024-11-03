#include "BannerElement.h"

#include "../../object/BannerSceneryEntry.h"
#include "../../object/ObjectEntryManager.h"
#include "../../object/ObjectManager.h"
#include "../Banner.h"

Banner* BannerElement::GetBanner() const
{
    return ::GetBanner(GetIndex());
}

const BannerSceneryEntry* BannerElement::GetEntry() const
{
    auto banner = GetBanner();
    if (banner != nullptr)
    {
        return OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
    }
    return nullptr;
}

BannerIndex BannerElement::GetIndex() const
{
    return index;
}

void BannerElement::SetIndex(BannerIndex newIndex)
{
    index = newIndex;
}

uint8_t BannerElement::GetPosition() const
{
    return position;
}

void BannerElement::SetPosition(uint8_t newPosition)
{
    position = newPosition;
}

uint8_t BannerElement::GetAllowedEdges() const
{
    return AllowedEdges & 0b00001111;
}

void BannerElement::SetAllowedEdges(uint8_t newEdges)
{
    AllowedEdges &= ~0b00001111;
    AllowedEdges |= (newEdges & 0b00001111);
}

void BannerElement::ResetAllowedEdges()
{
    AllowedEdges |= 0b00001111;
}
