/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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

rct_banner gBanners[MAX_BANNERS];

/**
 *
 *  rct2: 0x006B7EAB
 */
static uint8_t banner_get_ride_index_at(int32_t x, int32_t y, int32_t z)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    ride_id_t resultRideIndex = RIDE_ID_NULL;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
        Ride* ride = get_ride(rideIndex);
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            continue;

        if ((tileElement->clearance_height * 8) + 32 <= z)
            continue;

        resultRideIndex = rideIndex;
    } while (!(tileElement++)->IsLastForTile());

    return resultRideIndex;
}

static BannerIndex BannerGetNewIndex()
{
    for (BannerIndex bannerIndex = 0; bannerIndex < MAX_BANNERS; bannerIndex++)
    {
        if (gBanners[bannerIndex].type == BANNER_NULL)
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
    for (auto& banner : gBanners)
    {
        banner.type = BANNER_NULL;
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
        rct_banner* banner = &gBanners[bannerIndex];

        banner->flags = 0;
        banner->type = 0;
        banner->string_idx = STR_DEFAULT_SIGN;
        banner->colour = 2;
        banner->text_colour = 2;
    }
    return bannerIndex;
}

TileElement* banner_get_tile_element(BannerIndex bannerIndex)
{
    rct_banner* banner = &gBanners[bannerIndex];
    TileElement* tileElement = map_get_first_element_at(banner->x, banner->y);
    do
    {
        if (tile_element_get_banner_index(tileElement) == bannerIndex)
        {
            return tileElement;
        }
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

WallElement* banner_get_scrolling_wall_tile_element(BannerIndex bannerIndex)
{
    rct_banner* banner = &gBanners[bannerIndex];
    TileElement* tileElement = map_get_first_element_at(banner->x, banner->y);

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
        if (wallElement->GetBannerIndex() != (BannerIndex)bannerIndex)
            continue;
        return wallElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

/**
 *
 *  rct2: 0x006B7D86
 */
uint8_t banner_get_closest_ride_index(int32_t x, int32_t y, int32_t z)
{
    Ride* ride;

    static constexpr const LocationXY16 NeighbourCheckOrder[] = { { 32, 0 },    { -32, 0 },   { 0, 32 },
                                                                  { 0, -32 },   { -32, +32 }, { +32, -32 },
                                                                  { +32, +32 }, { -32, +32 }, { 0, 0 } };

    for (size_t i = 0; i < (int32_t)std::size(NeighbourCheckOrder); i++)
    {
        ride_id_t rideIndex = banner_get_ride_index_at(x + NeighbourCheckOrder[i].x, y + NeighbourCheckOrder[i].y, z);
        if (rideIndex != RIDE_ID_NULL)
        {
            return rideIndex;
        }
    }

    uint8_t index;
    ride_id_t rideIndex = RIDE_ID_NULL;
    int32_t resultDistance = std::numeric_limits<int32_t>::max();
    FOR_ALL_RIDES (index, ride)
    {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            continue;

        LocationXY8 location = ride->overall_view;
        if (location.xy == RCT_XY8_UNDEFINED)
            continue;

        int32_t rideX = location.x * 32;
        int32_t rideY = location.y * 32;
        int32_t distance = abs(x - rideX) + abs(y - rideY);
        if (distance < resultDistance)
        {
            resultDistance = distance;
            rideIndex = index;
        }
    }

    return rideIndex;
}

void banner_reset_broken_index()
{
    for (BannerIndex bannerIndex = 0; bannerIndex < MAX_BANNERS; bannerIndex++)
    {
        TileElement* tileElement = banner_get_tile_element(bannerIndex);
        if (tileElement == nullptr)
            gBanners[bannerIndex].type = BANNER_NULL;
    }
}

void fix_duplicated_banners()
{
    // For each banner in the map, check if the banner index is in use already, and if so, create a new entry for it
    bool activeBanners[std::size(gBanners)]{};
    TileElement* tileElement;
    for (int y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            tileElement = map_get_first_element_at(x, y);
            do
            {
                // TODO: Handle walls and large-scenery that use banner indices too. Large scenery can be tricky, as they occupy
                // multiple tiles that should both refer to the same banner index.
                if (tileElement->GetType() == TILE_ELEMENT_TYPE_BANNER)
                {
                    uint8_t bannerIndex = tileElement->AsBanner()->GetIndex();
                    if (activeBanners[bannerIndex])
                    {
                        log_info(
                            "Duplicated banner with index %d found at x = %d, y = %d and z = %d.", bannerIndex, x, y,
                            tileElement->base_height);

                        // Banner index is already in use by another banner, so duplicate it
                        BannerIndex newBannerIndex = create_new_banner(GAME_COMMAND_FLAG_APPLY);
                        if (newBannerIndex == BANNER_INDEX_NULL)
                        {
                            log_error("Failed to create new banner.");
                            continue;
                        }
                        Guard::Assert(!activeBanners[newBannerIndex]);

                        // Copy over the original banner, but update the location
                        rct_banner& newBanner = gBanners[newBannerIndex];
                        newBanner = gBanners[bannerIndex];
                        newBanner.x = x;
                        newBanner.y = y;

                        // Duplicate user string too
                        rct_string_id stringIdx = newBanner.string_idx;
                        if (is_user_string_id(stringIdx))
                        {
                            utf8 buffer[USER_STRING_MAX_LENGTH];
                            format_string(buffer, USER_STRING_MAX_LENGTH, stringIdx, nullptr);
                            rct_string_id newStringIdx = user_string_allocate(USER_STRING_DUPLICATION_PERMITTED, buffer);
                            if (newStringIdx == 0)
                            {
                                log_error("Failed to allocate user string for banner");
                                continue;
                            }
                            newBanner.string_idx = newStringIdx;
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
    return flags & 0b00001111;
}

void BannerElement::SetAllowedEdges(uint8_t newEdges)
{
    flags &= ~0b00001111;
    flags |= (newEdges & 0b00001111);
}

void BannerElement::ResetAllowedEdges()
{
    flags |= 0b00001111;
}
