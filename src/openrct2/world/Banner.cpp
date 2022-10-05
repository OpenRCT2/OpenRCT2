/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Banner.h"

#include "../Context.h"
#include "../Game.h"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../interface/Window.h"
#include "../localisation/Formatter.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../windows/Intent.h"
#include "../world/TileElementsView.h"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <limits>

static std::vector<Banner> _banners;

std::string Banner::GetText() const
{
    Formatter ft;
    FormatTextTo(ft);
    return format_string(STR_STRINGID, ft.Data());
}

void Banner::FormatTextTo(Formatter& ft, bool addColour) const
{
    if (addColour)
    {
        auto formatToken = FormatTokenFromTextColour(text_colour);
        auto tokenText = FormatTokenToString(formatToken, true);
        ft.Add<StringId>(STR_STRING_STRINGID);
        ft.Add<const char*>(tokenText.data());
    }

    FormatTextTo(ft);
}

void Banner::FormatTextTo(Formatter& ft) const
{
    if (flags & BANNER_FLAG_NO_ENTRY)
    {
        ft.Add<StringId>(STR_NO_ENTRY);
    }
    else if (flags & BANNER_FLAG_LINKED_TO_RIDE)
    {
        auto ride = get_ride(ride_index);
        if (ride != nullptr)
        {
            ride->FormatNameTo(ft);
        }
        else
        {
            ft.Add<StringId>(STR_DEFAULT_SIGN);
        }
    }
    else if (text.empty())
    {
        ft.Add<StringId>(STR_DEFAULT_SIGN);
    }
    else
    {
        ft.Add<StringId>(STR_STRING).Add<const char*>(text.c_str());
    }
}

/**
 *
 *  rct2: 0x006B7EAB
 */
static RideId BannerGetRideIndexAt(const CoordsXYZ& bannerCoords)
{
    TileElement* tileElement = map_get_first_element_at(bannerCoords);
    RideId resultRideIndex = RideId::GetNull();
    if (tileElement == nullptr)
        return resultRideIndex;
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
            continue;

        RideId rideIndex = tileElement->AsTrack()->GetRideIndex();
        auto ride = get_ride(rideIndex);
        if (ride == nullptr || ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
            continue;

        if ((tileElement->GetClearanceZ()) + (4 * COORDS_Z_STEP) <= bannerCoords.z)
            continue;

        resultRideIndex = rideIndex;
    } while (!(tileElement++)->IsLastForTile());

    return resultRideIndex;
}

static BannerIndex BannerGetNewIndex()
{
    for (BannerIndex::UnderlyingType bannerIndex = 0; bannerIndex < MAX_BANNERS; bannerIndex++)
    {
        if (bannerIndex < _banners.size())
        {
            if (_banners[bannerIndex].IsNull())
            {
                return BannerIndex::FromUnderlying(bannerIndex);
            }
        }
        else
        {
            _banners.emplace_back();
            return BannerIndex::FromUnderlying(bannerIndex);
        }
    }
    return BannerIndex::GetNull();
}

/**
 *
 *  rct2: 0x006B9CB0
 */
void BannerInit()
{
    _banners.clear();
}

TileElement* BannerGetTileElement(BannerIndex bannerIndex)
{
    auto banner = GetBanner(bannerIndex);
    if (banner != nullptr)
    {
        auto tileElement = map_get_first_element_at(banner->position);
        if (tileElement != nullptr)
        {
            do
            {
                if (tileElement->GetBannerIndex() == bannerIndex)
                {
                    return tileElement;
                }
            } while (!(tileElement++)->IsLastForTile());
        }
    }
    return nullptr;
}

WallElement* BannerGetScrollingWallTileElement(BannerIndex bannerIndex)
{
    auto banner = GetBanner(bannerIndex);
    if (banner == nullptr)
        return nullptr;

    auto tileElement = map_get_first_element_at(banner->position);
    if (tileElement == nullptr)
        return nullptr;

    do
    {
        auto wallElement = tileElement->AsWall();

        if (wallElement == nullptr)
            continue;

        auto* wallEntry = wallElement->GetEntry();
        if (wallEntry->scrolling_mode == SCROLLING_MODE_NONE)
            continue;
        if (wallElement->GetBannerIndex() != bannerIndex)
            continue;
        return wallElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

/**
 *
 *  rct2: 0x006B7D86
 */
RideId BannerGetClosestRideIndex(const CoordsXYZ& mapPos)
{
    static constexpr const std::array NeighbourCheckOrder = {
        CoordsXY{ COORDS_XY_STEP, 0 },
        CoordsXY{ -COORDS_XY_STEP, 0 },
        CoordsXY{ 0, COORDS_XY_STEP },
        CoordsXY{ 0, -COORDS_XY_STEP },
        CoordsXY{ -COORDS_XY_STEP, +COORDS_XY_STEP },
        CoordsXY{ +COORDS_XY_STEP, -COORDS_XY_STEP },
        CoordsXY{ +COORDS_XY_STEP, +COORDS_XY_STEP },
        CoordsXY{ -COORDS_XY_STEP, +COORDS_XY_STEP },
        CoordsXY{ 0, 0 },
    };

    for (const auto& neighhbourCoords : NeighbourCheckOrder)
    {
        RideId rideIndex = BannerGetRideIndexAt({ CoordsXY{ mapPos } + neighhbourCoords, mapPos.z });
        if (!rideIndex.IsNull())
        {
            return rideIndex;
        }
    }

    auto rideIndex = RideId::GetNull();
    auto resultDistance = std::numeric_limits<int32_t>::max();
    for (auto& ride : GetRideManager())
    {
        if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
            continue;

        auto rideCoords = ride.overall_view;
        if (rideCoords.IsNull())
            continue;

        int32_t distance = abs(mapPos.x - rideCoords.x) + abs(mapPos.y - rideCoords.y);
        if (distance < resultDistance)
        {
            resultDistance = distance;
            rideIndex = ride.id;
        }
    }
    return rideIndex;
}

void BannerResetBrokenIndex()
{
    for (BannerIndex::UnderlyingType index = 0; index < _banners.size(); index++)
    {
        const auto bannerId = BannerIndex::FromUnderlying(index);
        auto tileElement = BannerGetTileElement(bannerId);
        if (tileElement == nullptr)
        {
            auto banner = GetBanner(bannerId);
            if (banner != nullptr)
            {
                banner->type = BANNER_NULL;
            }
        }
    }
}

void BannerFixDuplicates()
{
    // For each banner in the map, check if the banner index is in use already, and if so, create a new entry for it
    std::vector<bool> activeBanners;
    activeBanners.resize(MAX_BANNERS);

    for (int y = 0; y < gMapSize.y; y++)
    {
        for (int x = 0; x < gMapSize.x; x++)
        {
            const auto bannerPos = TileCoordsXY{ x, y }.ToCoordsXY();
            for (auto* bannerElement : OpenRCT2::TileElementsView<BannerElement>(bannerPos))
            {
                auto bannerIndex = bannerElement->GetIndex();
                if (bannerIndex == BannerIndex::GetNull())
                    continue;

                const auto index = bannerIndex.ToUnderlying();
                if (activeBanners[index])
                {
                    log_info(
                        "Duplicated banner with index %d found at x = %d, y = %d and z = %d.", index, x, y,
                        bannerElement->base_height);

                    // Banner index is already in use by another banner, so duplicate it
                    auto newBanner = CreateBanner();
                    if (newBanner == nullptr)
                    {
                        log_error("Failed to create new banner.");
                        continue;
                    }
                    Guard::Assert(!activeBanners[newBanner->id.ToUnderlying()]);

                    // Copy over the original banner, but update the location
                    const auto* oldBanner = GetBanner(bannerIndex);
                    if (oldBanner != nullptr)
                    {
                        auto newBannerId = newBanner->id;

                        *newBanner = *oldBanner;
                        newBanner->id = newBannerId;
                        newBanner->position = { x, y };
                    }

                    bannerElement->SetIndex(newBanner->id);
                }

                // Mark banner index as in-use
                activeBanners[index] = true;
            }
        }
    }
}

Banner* BannerElement::GetBanner() const
{
    return ::GetBanner(GetIndex());
}

BannerSceneryEntry* BannerElement::GetEntry() const
{
    auto banner = GetBanner();
    if (banner != nullptr)
    {
        return get_banner_entry(banner->type);
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

void UnlinkAllRideBanners()
{
    for (auto& banner : _banners)
    {
        if (!banner.IsNull())
        {
            banner.flags &= ~BANNER_FLAG_LINKED_TO_RIDE;
            banner.ride_index = RideId::GetNull();
        }
    }
}

void UnlinkAllBannersForRide(RideId rideId)
{
    for (auto& banner : _banners)
    {
        if (!banner.IsNull() && (banner.flags & BANNER_FLAG_LINKED_TO_RIDE) && banner.ride_index == rideId)
        {
            banner.flags &= ~BANNER_FLAG_LINKED_TO_RIDE;
            banner.ride_index = RideId::GetNull();
            banner.text = {};
        }
    }
}

Banner* GetBanner(BannerIndex id)
{
    const auto index = id.ToUnderlying();
    if (index < _banners.size())
    {
        auto banner = &_banners[index];
        if (banner != nullptr && !banner->IsNull())
        {
            return banner;
        }
    }
    return nullptr;
}

Banner* GetOrCreateBanner(BannerIndex id)
{
    const auto index = id.ToUnderlying();
    if (index < MAX_BANNERS)
    {
        if (index >= _banners.size())
        {
            _banners.resize(index + 1);
        }
        // Create the banner
        auto& banner = _banners[index];
        banner.id = id;
        return &banner;
    }
    return nullptr;
}

Banner* CreateBanner()
{
    auto bannerIndex = BannerGetNewIndex();
    auto banner = GetOrCreateBanner(bannerIndex);
    if (banner != nullptr)
    {
        banner->id = bannerIndex;
        banner->flags = 0;
        banner->type = 0;
        banner->text = {};
        banner->colour = COLOUR_WHITE;
        banner->text_colour = COLOUR_WHITE;
    }
    return banner;
}

void DeleteBanner(BannerIndex id)
{
    auto* const banner = GetBanner(id);
    if (banner != nullptr)
    {
        *banner = {};
    }
}

void TrimBanners()
{
    if (_banners.size() > 0)
    {
        auto lastBannerId = _banners.size() - 1;
        while (lastBannerId != std::numeric_limits<size_t>::max() && _banners[lastBannerId].IsNull())
        {
            lastBannerId--;
        }
        _banners.resize(lastBannerId + 1);
        _banners.shrink_to_fit();
    }
}

size_t GetNumBanners()
{
    size_t count = 0;
    for (const auto& banner : _banners)
    {
        if (!banner.IsNull())
        {
            count++;
        }
    }
    return count;
}

bool HasReachedBannerLimit()
{
    auto numBanners = GetNumBanners();
    return numBanners >= MAX_BANNERS;
}
