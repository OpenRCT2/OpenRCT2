#include "BannerElement.h"

#include "../../object/BannerSceneryEntry.h"
#include "../../object/ObjectEntryManager.h"
#include "../Banner.h"

namespace OpenRCT2
{
    Banner* BannerElement::getBanner() const
    {
        return ::GetBanner(getIndex());
    }

    const BannerSceneryEntry* BannerElement::getEntry() const
    {
        auto banner = getBanner();
        if (banner != nullptr)
        {
            return OpenRCT2::ObjectEntryManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
        }
        return nullptr;
    }

    BannerIndex BannerElement::getIndex() const
    {
        return index;
    }

    void BannerElement::setIndex(BannerIndex newIndex)
    {
        index = newIndex;
    }

    uint8_t BannerElement::getPosition() const
    {
        return position;
    }

    void BannerElement::setPosition(uint8_t newPosition)
    {
        position = newPosition;
    }

    uint8_t BannerElement::getAllowedEdges() const
    {
        return allowedEdges & 0b00001111;
    }

    void BannerElement::setAllowedEdges(uint8_t newEdges)
    {
        allowedEdges &= ~0b00001111;
        allowedEdges |= (newEdges & 0b00001111);
    }

    void BannerElement::resetAllowedEdges()
    {
        allowedEdges |= 0b00001111;
    }
} // namespace OpenRCT2
