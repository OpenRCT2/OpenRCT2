/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"
#include "LargeScenery.h"
#include "TileElement.h"

colour_t scenery_large_get_primary_colour(const rct_tile_element * tileElement)
{
    return tileElement->properties.scenerymultiple.colour[0] & TILE_ELEMENT_COLOUR_MASK;
}

colour_t scenery_large_get_secondary_colour(const rct_tile_element * tileElement)
{
    return tileElement->properties.scenerymultiple.colour[1] & TILE_ELEMENT_COLOUR_MASK;
}

void scenery_large_set_primary_colour(rct_tile_element * tileElement, colour_t colour)
{
    assert(colour <= 31);
    tileElement->properties.scenerymultiple.colour[0] &= ~TILE_ELEMENT_COLOUR_MASK;
    tileElement->properties.scenerymultiple.colour[0] |= colour;
}

void scenery_large_set_secondary_colour(rct_tile_element * tileElement, colour_t colour)
{
    assert(colour <= 31);
    tileElement->properties.scenerymultiple.colour[1] &= ~TILE_ELEMENT_COLOUR_MASK;
    tileElement->properties.scenerymultiple.colour[1] |= colour;
}

BannerIndex scenery_large_get_banner_id(const rct_tile_element* tileElement)
{
    return (tileElement->type & 0xC0) |
           (((tileElement->properties.scenerymultiple.colour[0]) &~ TILE_ELEMENT_COLOUR_MASK) >> 2) |
           (((tileElement->properties.scenerymultiple.colour[1]) &~ TILE_ELEMENT_COLOUR_MASK) >> 5);
}

void scenery_large_set_banner_id(rct_tile_element * tileElement, BannerIndex bannerIndex)
{
    tileElement->type |= bannerIndex & 0xC0;
    tileElement->properties.scenerymultiple.colour[0] |= (bannerIndex & 0x38) << 2;
    tileElement->properties.scenerymultiple.colour[1] |= (bannerIndex & 7) << 5;
}

int32_t scenery_large_get_type(const rct_tile_element * tileElement)
{
    return (tileElement->properties.scenerymultiple.type & TILE_ELEMENT_LARGE_TYPE_MASK);
}

int32_t scenery_large_get_sequence(const rct_tile_element * tileElement)
{
    return (tileElement->properties.scenerymultiple.type >> 10);
}

void scenery_large_set_type(rct_tile_element * tileElement, uint16_t type)
{
    tileElement->properties.scenerymultiple.type &= ~TILE_ELEMENT_LARGE_TYPE_MASK;
    tileElement->properties.scenerymultiple.type |= (type & TILE_ELEMENT_LARGE_TYPE_MASK);
}

void scenery_large_set_sequence(rct_tile_element * tileElement, uint16_t sequence)
{
    tileElement->properties.scenerymultiple.type &= TILE_ELEMENT_LARGE_TYPE_MASK;
    tileElement->properties.scenerymultiple.type |= (sequence << 10);
}
