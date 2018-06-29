/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/Guard.hpp"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../ride/Track.h"
#include "Banner.h"
#include "LargeScenery.h"
#include "TileElement.h"
#include "Scenery.h"

uint8_t rct_tile_element::GetType() const
{
    return this->type & TILE_ELEMENT_TYPE_MASK;
}

void rct_tile_element::SetType(uint8_t newType)
{
    this->type &= ~TILE_ELEMENT_TYPE_MASK;
    this->type |= (newType & TILE_ELEMENT_TYPE_MASK);
}

uint8_t rct_tile_element::GetDirection() const
{
    return this->type & TILE_ELEMENT_DIRECTION_MASK;
}

void rct_tile_element::SetDirection(uint8_t direction)
{
    this->type &= ~TILE_ELEMENT_DIRECTION_MASK;
    this->type |= (direction & TILE_ELEMENT_DIRECTION_MASK);
}

uint8_t rct_tile_element::GetDirectionWithOffset(uint8_t offset) const
{
    return ((this->type & TILE_ELEMENT_DIRECTION_MASK) + offset) & TILE_ELEMENT_DIRECTION_MASK;
}

bool rct_tile_element::IsLastForTile() const
{
    return (this->flags & TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

bool rct_tile_element::IsGhost() const
{
    return (this->flags & TILE_ELEMENT_FLAG_GHOST) != 0;
}

uint8_t rct_tile_element::GetSceneryQuadrant() const
{
    return (this->type & TILE_ELEMENT_QUADRANT_MASK) >> 6;
}

int32_t tile_element_get_direction(const rct_tile_element * element)
{
    return element->GetDirection();
}

int32_t tile_element_get_direction_with_offset(const rct_tile_element * element, uint8_t offset)
{
    return element->GetDirectionWithOffset(offset);
}

bool tile_element_is_ghost(const rct_tile_element * element)
{
    return element->IsGhost();
}

bool tile_element_is_underground(rct_tile_element * tileElement)
{
    do {
        tileElement++;
        if ((tileElement - 1)->IsLastForTile())
            return false;
    } while (tileElement->GetType() != TILE_ELEMENT_TYPE_SURFACE);
    return true;
}

BannerIndex tile_element_get_banner_index(rct_tile_element* tileElement)
{
    rct_scenery_entry* sceneryEntry;

    switch (tileElement->GetType()) {
    case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        sceneryEntry = get_large_scenery_entry(scenery_large_get_type(tileElement));
        if (sceneryEntry->large_scenery.scrolling_mode == 0xFF)
            return BANNER_INDEX_NULL;

        return scenery_large_get_banner_id(tileElement);
    case TILE_ELEMENT_TYPE_WALL:
        sceneryEntry = get_wall_entry(tileElement->properties.wall.type);
        if (sceneryEntry == nullptr || sceneryEntry->wall.scrolling_mode == 0xFF)
            return BANNER_INDEX_NULL;

        return tileElement->properties.wall.banner_index;
    case TILE_ELEMENT_TYPE_BANNER:
        return tileElement->properties.banner.index;
    default:
        return BANNER_INDEX_NULL;
    }
}

void tile_element_set_banner_index(rct_tile_element* tileElement, BannerIndex bannerIndex)
{
    switch (tileElement->GetType())
    {
    case TILE_ELEMENT_TYPE_WALL:
        tileElement->properties.wall.banner_index = bannerIndex;
        break;
    case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        scenery_large_set_banner_id(tileElement, bannerIndex);
        break;
    case TILE_ELEMENT_TYPE_BANNER:
        tileElement->properties.banner.index = bannerIndex;
        break;
    default:
        log_error("Tried to set banner index on unsuitable tile element!");
        Guard::Assert(false);
    }
}

void tile_element_remove_banner_entry(rct_tile_element * tileElement)
{
    BannerIndex bannerIndex = tile_element_get_banner_index(tileElement);
    if (bannerIndex == BANNER_INDEX_NULL)
        return;

    rct_banner* banner = &gBanners[bannerIndex];
    if (banner->type != BANNER_NULL) {
        rct_windownumber windowNumber = bannerIndex;
        window_close_by_number(WC_BANNER, windowNumber);
        banner->type = BANNER_NULL;
        user_string_free(banner->string_idx);
    }
}

uint8_t tile_element_get_ride_index(const rct_tile_element * tileElement)
{
    switch (tileElement->GetType())
    {
    case TILE_ELEMENT_TYPE_TRACK:
        return track_element_get_ride_index(tileElement);
    case TILE_ELEMENT_TYPE_ENTRANCE:
        return tileElement->properties.entrance.ride_index;
    case TILE_ELEMENT_TYPE_PATH:
        return tileElement->properties.path.ride_index;
    default:
        return 0xFF;
    }
}
