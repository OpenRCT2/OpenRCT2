/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../windows/Intent.h"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <limits>

static Banner _banners[MAX_BANNERS];

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
        ft.Add<rct_string_id>(STR_STRING_STRINGID);
        ft.Add<const char*>(tokenText.data());
    }

    FormatTextTo(ft);
}

void Banner::FormatTextTo(Formatter& ft) const
{
    if (flags & BANNER_FLAG_NO_ENTRY)
    {
        ft.Add<rct_string_id>(STR_NO_ENTRY);
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
            ft.Add<rct_string_id>(STR_DEFAULT_SIGN);
        }
    }
    else if (text.empty())
    {
        ft.Add<rct_string_id>(STR_DEFAULT_SIGN);
    }
    else
    {
        ft.Add<rct_string_id>(STR_STRING).Add<const char*>(text.c_str());
    }
}

/**
 *
 *  rct2: 0x006B7EAB
 */
static uint8_t banner_get_ride_index_at(const CoordsXYZ& bannerCoords)
{
    TileElement* tileElement = map_get_first_element_at(bannerCoords);
    ride_id_t resultRideIndex = RIDE_ID_NULL;
    if (tileElement == nullptr)
        return resultRideIndex;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
        auto ride = get_ride(rideIndex);
        if (ride == nullptr || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            continue;

        if ((tileElement->GetClearanceZ()) + (4 * COORDS_Z_STEP) <= bannerCoords.z)
            continue;

        resultRideIndex = rideIndex;
    } while (!(tileElement++)->IsLastForTile());

    return resultRideIndex;
}

static BannerIndex BannerGetNewIndex()
{
    for (BannerIndex bannerIndex = 0; bannerIndex < MAX_BANNERS; bannerIndex++)
    {
        if (_banners[bannerIndex].IsNull())
        {
            return bannerIndex;
        }
    }
    return BANNER_INDEX_NULL;
}

/**
 *
 *  rct2: 0x006B9CB0
 */
void banner_init()
{
    for (auto& banner : _banners)
    {
        banner = {};
    }
}

/**
 * Creates a new banner and returns the index of the banner
 * If the flag GAME_COMMAND_FLAG_APPLY is NOT set then returns
 * the first unused index but does NOT mark the banner as created.
 * returns 0xFF on failure.
 *
 *  rct2: 0x006BA278
 */
BannerIndex create_new_banner(uint8_t flags)
{
    BannerIndex bannerIndex = BannerGetNewIndex();

    if (bannerIndex == BANNER_INDEX_NULL)
    {
        gGameCommandErrorText = STR_TOO_MANY_BANNERS_IN_GAME;
        return bannerIndex;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        auto banner = &_banners[bannerIndex];

        banner->flags = 0;
        banner->type = 0;
        banner->text = {};
        banner->colour = 2;
        banner->text_colour = 2;
    }
    return bannerIndex;
}

TileElement* banner_get_tile_element(BannerIndex bannerIndex)
{
    auto banner = GetBanner(bannerIndex);
    if (banner != nullptr)
    {
        auto tileElement = map_get_first_element_at(banner->position.ToCoordsXY());
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

WallElement* banner_get_scrolling_wall_tile_element(BannerIndex bannerIndex)
{
    auto banner = GetBanner(bannerIndex);
    if (banner == nullptr)
        return nullptr;

    auto tileElement = map_get_first_element_at(banner->position.ToCoordsXY());
    if (tileElement == nullptr)
        return nullptr;

    do
    {
        auto wallElement = tileElement->AsWall();

        if (wallElement == nullptr)
            continue;

        rct_scenery_entry* scenery_entry = wallElement->GetEntry();
        if (scenery_entry->wall.scrolling_mode == SCROLLING_MODE_NONE)
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
uint8_t banner_get_closest_ride_index(const CoordsXYZ& mapPos)
{
    static constexpr const std::array<CoordsXY, 9> NeighbourCheckOrder = { CoordsXY{ COORDS_XY_STEP, 0 },
                                                                           CoordsXY{ -COORDS_XY_STEP, 0 },
                                                                           { 0, COORDS_XY_STEP },
                                                                           CoordsXY{ 0, -COORDS_XY_STEP },
                                                                           CoordsXY{ -COORDS_XY_STEP, +COORDS_XY_STEP },
                                                                           CoordsXY{ +COORDS_XY_STEP, -COORDS_XY_STEP },
                                                                           CoordsXY{ +COORDS_XY_STEP, +COORDS_XY_STEP },
                                                                           CoordsXY{ -COORDS_XY_STEP, +COORDS_XY_STEP },
                                                                           CoordsXY{ 0, 0 } };

    for (const auto& neighhbourCoords : NeighbourCheckOrder)
    {
        ride_id_t rideIndex = banner_get_ride_index_at({ CoordsXY{ mapPos } + neighhbourCoords, mapPos.z });
        if (rideIndex != RIDE_ID_NULL)
        {
            return rideIndex;
        }
    }

    auto rideIndex = RIDE_ID_NULL;
    auto resultDistance = std::numeric_limits<int32_t>::max();
    for (auto& ride : GetRideManager())
    {
        if (ride_type_has_flag(ride.type, RIDE_TYPE_FLAG_IS_SHOP))
            continue;

        auto rideCoords = ride.overall_view;
        if (rideCoords.isNull())
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

void banner_reset_broken_index()
{
    for (BannerIndex bannerIndex = 0; bannerIndex < MAX_BANNERS; bannerIndex++)
    {
        auto tileElement = banner_get_tile_element(bannerIndex);
        if (tileElement == nullptr)
        {
            _banners[bannerIndex].type = BANNER_NULL;
        }
    }
}

void fix_duplicated_banners()
{
    // For each banner in the map, check if the banner index is in use already, and if so, create a new entry for it
    bool activeBanners[std::size(_banners)]{};
    for (int y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            auto tileElement = map_get_first_element_at(TileCoordsXY{ x, y }.ToCoordsXY());
            if (tileElement != nullptr)
            {
                do
                {
                    // TODO: Handle walls and large-scenery that use banner indices too. Large scenery can be tricky, as they
                    // occupy multiple tiles that should both refer to the same banner index.
                    if (tileElement->GetType() == TILE_ELEMENT_TYPE_BANNER)
                    {
                        auto bannerIndex = tileElement->AsBanner()->GetIndex();
                        if (bannerIndex == BANNER_INDEX_NULL)
                            continue;

                        if (activeBanners[bannerIndex])
                        {
                            log_info(
                                "Duplicated banner with index %d found at x = %d, y = %d and z = %d.", bannerIndex, x, y,
                                tileElement->base_height);

                            // Banner index is already in use by another banner, so duplicate it
                            auto newBannerIndex = create_new_banner(GAME_COMMAND_FLAG_APPLY);
                            if (newBannerIndex == BANNER_INDEX_NULL)
                            {
                                log_error("Failed to create new banner.");
                                continue;
                            }
                            Guard::Assert(!activeBanners[newBannerIndex]);

                            // Copy over the original banner, but update the location
                            auto newBanner = GetBanner(newBannerIndex);
                            auto oldBanner = GetBanner(bannerIndex);
                            if (oldBanner != nullptr && newBanner != nullptr)
                            {
                                *newBanner = *oldBanner;
                                newBanner->position = { x, y };
                            }

                            tileElement->AsBanner()->SetIndex(newBannerIndex);
                        }

                        // Mark banner index as in-use
                        activeBanners[bannerIndex] = true;
                    }
                } while (!(tileElement++)->IsLastForTile());
            }
        }
    }
}

Banner* BannerElement::GetBanner() const
{
    return ::GetBanner(GetIndex());
}

rct_scenery_entry* BannerElement::GetEntry() const
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

Banner* GetBanner(BannerIndex id)
{
    if (id < std::size(_banners))
    {
        return &_banners[id];
    }
    return nullptr;
}
