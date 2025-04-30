/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TileElement.h"

#include "../../Diagnostic.h"
#include "../../core/Guard.hpp"
#include "../../interface/WindowClasses.h"
#include "../../ui/WindowManager.h"
#include "../Map.h"
#include "BannerElement.h"
#include "EntranceElement.h"
#include "LargeSceneryElement.h"
#include "PathElement.h"
#include "TrackElement.h"
#include "WallElement.h"

using namespace OpenRCT2;

BannerIndex TileElement::GetBannerIndex() const
{
    switch (GetType())
    {
        case TileElementType::LargeScenery:
        {
            auto* sceneryEntry = AsLargeScenery()->GetEntry();
            if (sceneryEntry == nullptr || sceneryEntry->scrolling_mode == SCROLLING_MODE_NONE)
                return BannerIndex::GetNull();

            return AsLargeScenery()->GetBannerIndex();
        }
        case TileElementType::Wall:
        {
            auto* wallEntry = AsWall()->GetEntry();
            if (wallEntry == nullptr || wallEntry->scrolling_mode == SCROLLING_MODE_NONE)
                return BannerIndex::GetNull();

            return AsWall()->GetBannerIndex();
        }
        case TileElementType::Banner:
            return AsBanner()->GetIndex();
        default:
            return BannerIndex::GetNull();
    }
}

void TileElement::SetBannerIndex(BannerIndex bannerIndex)
{
    switch (GetType())
    {
        case TileElementType::Wall:
            AsWall()->SetBannerIndex(bannerIndex);
            break;
        case TileElementType::LargeScenery:
            AsLargeScenery()->SetBannerIndex(bannerIndex);
            break;
        case TileElementType::Banner:
            AsBanner()->SetIndex(bannerIndex);
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
        windowMgr->CloseByNumber(WindowClass::Banner, bannerIndex.ToUnderlying());
        DeleteBanner(banner->id);
    }
}

RideId TileElement::GetRideIndex() const
{
    switch (GetType())
    {
        case TileElementType::Track:
            return AsTrack()->GetRideIndex();
        case TileElementType::Entrance:
            return AsEntrance()->GetRideIndex();
        case TileElementType::Path:
            return AsPath()->GetRideIndex();
        default:
            return RideId::GetNull();
    }
}

void TileElement::ClearAs(TileElementType newType)
{
    Type = 0;
    SetType(newType);
    Flags = 0;
    BaseHeight = kMinimumLandHeight;
    ClearanceHeight = kMinimumLandHeight;
    Owner = 0;
    std::fill_n(Pad05, sizeof(Pad05), 0x00);
    std::fill_n(Pad08, sizeof(Pad08), 0x00);
}

bool TileElementIsUnderground(TileElement* tileElement)
{
    do
    {
        tileElement++;
        if ((tileElement - 1)->IsLastForTile())
            return false;
    } while (tileElement->GetType() != TileElementType::Surface);
    return true;
}
