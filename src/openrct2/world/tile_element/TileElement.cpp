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
#include "../../ui/WindowManager.h"
#include "BannerElement.h"
#include "EntranceElement.h"
#include "LargeSceneryElement.h"
#include "PathElement.h"
#include "TrackElement.h"
#include "WallElement.h"

namespace OpenRCT2
{
    BannerIndex TileElement::GetBannerIndex() const
    {
        switch (getType())
        {
            case TileElementType::LargeScenery:
            {
                auto* sceneryEntry = asLargeScenery()->GetEntry();
                if (sceneryEntry == nullptr || sceneryEntry->scrolling_mode == kScrollingModeNone)
                    return BannerIndex::GetNull();

                return asLargeScenery()->GetBannerIndex();
            }
            case TileElementType::Wall:
            {
                auto* wallEntry = asWall()->GetEntry();
                if (wallEntry == nullptr || wallEntry->scrolling_mode == kScrollingModeNone)
                    return BannerIndex::GetNull();

                return asWall()->GetBannerIndex();
            }
            case TileElementType::Banner:
                return asBanner()->GetIndex();
            default:
                return BannerIndex::GetNull();
        }
    }

    void TileElement::SetBannerIndex(BannerIndex bannerIndex)
    {
        switch (getType())
        {
            case TileElementType::Wall:
                asWall()->SetBannerIndex(bannerIndex);
                break;
            case TileElementType::LargeScenery:
                asLargeScenery()->SetBannerIndex(bannerIndex);
                break;
            case TileElementType::Banner:
                asBanner()->SetIndex(bannerIndex);
                break;
            default:
                LOG_ERROR("Tried to set banner index on unsuitable tile element!");
                Guard::Assert(false);
        }
    }

    void TileElement::RemoveBannerEntry()
    {
        auto bannerIndex = GetBannerIndex();
        auto banner = GetBanner(bannerIndex);
        if (banner != nullptr)
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseByNumber(WindowClass::banner, bannerIndex.ToUnderlying());
            DeleteBanner(banner->id);
        }
    }

    RideId TileElement::GetRideIndex() const
    {
        switch (getType())
        {
            case TileElementType::Track:
                return asTrack()->GetRideIndex();
            case TileElementType::Entrance:
                return asEntrance()->GetRideIndex();
            case TileElementType::Path:
                return asPath()->GetRideIndex();
            default:
                return RideId::GetNull();
        }
    }

    void TileElement::ClearAs(TileElementType newType)
    {
        type = 0;
        setType(newType);
        flags = 0;
        baseHeight = kMinimumLandHeight;
        clearanceHeight = kMinimumLandHeight;
        owner = 0;
        std::fill_n(Pad05, sizeof(Pad05), 0x00);
        std::fill_n(Pad08, sizeof(Pad08), 0x00);
    }

    bool TileElementIsUnderground(TileElement* tileElement)
    {
        do
        {
            tileElement++;
            if ((tileElement - 1)->isLastForTile())
                return false;
        } while (tileElement->getType() != TileElementType::Surface);
        return true;
    }
} // namespace OpenRCT2
