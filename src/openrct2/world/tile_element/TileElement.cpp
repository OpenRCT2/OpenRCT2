/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TileElement.h"

#include "../../Diagnostic.h"
#include "../../core/Guard.hpp"
#include "../../drawing/ScrollingText.h"
#include "../../interface/WindowClasses.h"
#include "../../object/WallSceneryEntry.h"
#include "../../ui/WindowManager.h"
#include "BannerElement.h"
#include "EntranceElement.h"
#include "LargeSceneryElement.h"
#include "PathElement.h"
#include "TrackElement.h"
#include "WallElement.h"

namespace OpenRCT2
{
    BannerIndex TileElement::getBannerIndex() const
    {
        switch (getType())
        {
            case TileElementType::largeScenery:
            {
                auto* sceneryEntry = asLargeScenery()->GetEntry();
                if (sceneryEntry == nullptr || sceneryEntry->scrolling_mode == kScrollingModeNone)
                    return BannerIndex::GetNull();

                return asLargeScenery()->GetBannerIndex();
            }
            case TileElementType::wall:
            {
                auto* wallEntry = asWall()->GetEntry();
                if (wallEntry == nullptr || wallEntry->scrolling_mode == kScrollingModeNone)
                    return BannerIndex::GetNull();

                return asWall()->GetBannerIndex();
            }
            case TileElementType::banner:
                return asBanner()->GetIndex();
            default:
                return BannerIndex::GetNull();
        }
    }

    void TileElement::setBannerIndex(BannerIndex bannerIndex)
    {
        switch (getType())
        {
            case TileElementType::wall:
                asWall()->SetBannerIndex(bannerIndex);
                break;
            case TileElementType::largeScenery:
                asLargeScenery()->SetBannerIndex(bannerIndex);
                break;
            case TileElementType::banner:
                asBanner()->SetIndex(bannerIndex);
                break;
            default:
                LOG_ERROR("Tried to set banner index on unsuitable tile element!");
                Guard::Assert(false);
        }
    }

    void TileElement::removeBannerEntry()
    {
        auto bannerIndex = getBannerIndex();
        auto banner = GetBanner(bannerIndex);
        if (banner != nullptr)
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseByNumber(WindowClass::banner, bannerIndex.ToUnderlying());
            DeleteBanner(banner->id);
        }
    }

    RideId TileElement::getRideIndex() const
    {
        switch (getType())
        {
            case TileElementType::track:
                return asTrack()->GetRideIndex();
            case TileElementType::entrance:
                return asEntrance()->GetRideIndex();
            case TileElementType::path:
                return asPath()->GetRideIndex();
            default:
                return RideId::GetNull();
        }
    }

    void TileElement::clearAs(TileElementType newType)
    {
        type = 0;
        setType(newType);
        flags = 0;
        baseHeight = kMinimumLandHeight;
        clearanceHeight = kMinimumLandHeight;
        owner = 0;
        std::fill_n(pad05, sizeof(pad05), 0x00);
        std::fill_n(pad08, sizeof(pad08), 0x00);
    }

    bool tileElementIsUnderground(TileElement* tileElement)
    {
        do
        {
            tileElement++;
            if ((tileElement - 1)->isLastForTile())
                return false;
        } while (tileElement->getType() != TileElementType::surface);
        return true;
    }
} // namespace OpenRCT2
