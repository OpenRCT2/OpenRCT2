#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

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

sint32 scenery_large_get_banner_id(const rct_tile_element * tileElement)
{
    return (tileElement->type & 0xC0) |
           (((tileElement->properties.scenerymultiple.colour[0]) &~ TILE_ELEMENT_COLOUR_MASK) >> 2) |
           (((tileElement->properties.scenerymultiple.colour[1]) &~ TILE_ELEMENT_COLOUR_MASK) >> 5);
}

void scenery_large_set_banner_id(rct_tile_element * tileElement, uint8 bannerIndex)
{
    tileElement->type |= bannerIndex & 0xC0;
    tileElement->properties.scenerymultiple.colour[0] |= (bannerIndex & 0x38) << 2;
    tileElement->properties.scenerymultiple.colour[1] |= (bannerIndex & 7) << 5;
}

sint32 scenery_large_get_type(const rct_tile_element * tileElement)
{
    return (tileElement->properties.scenerymultiple.type & TILE_ELEMENT_LARGE_TYPE_MASK);
}

sint32 scenery_large_get_sequence(const rct_tile_element * tileElement)
{
    return (tileElement->properties.scenerymultiple.type >> 10);
}

void scenery_large_set_type(rct_tile_element * tileElement, uint16 type)
{
    tileElement->properties.scenerymultiple.type &= ~TILE_ELEMENT_LARGE_TYPE_MASK;
    tileElement->properties.scenerymultiple.type |= (type & TILE_ELEMENT_LARGE_TYPE_MASK);
}

void scenery_large_set_sequence(rct_tile_element * tileElement, uint16 sequence)
{
    tileElement->properties.scenerymultiple.type &= TILE_ELEMENT_LARGE_TYPE_MASK;
    tileElement->properties.scenerymultiple.type |= (sequence << 10);
}
