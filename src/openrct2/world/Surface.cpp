/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Surface.h"

int32_t surface_get_terrain(const rct_tile_element * element)
{
    int32_t terrain = (element->properties.surface.terrain >> 5) & 7;
    if (element->type & 1)
        terrain |= (1 << 3);
    return terrain;
}

int32_t surface_get_terrain_edge(const rct_tile_element * element)
{
    int32_t terrain_edge = (element->properties.surface.slope >> 5) & 7;
    if (element->type & 128)
        terrain_edge |= (1 << 3);
    return terrain_edge;
}

void surface_set_terrain(rct_tile_element * element, int32_t terrain)
{
    // Bit 3 for terrain is stored in element.type bit 0
    if (terrain & 8)
        element->type |= 1;
    else
        element->type &= ~1;

    // Bits 0, 1, 2 for terrain are stored in element.terrain bit 5, 6, 7
    element->properties.surface.terrain &= ~0xE0;
    element->properties.surface.terrain |= (terrain & 7) << 5;
}

void surface_set_terrain_edge(rct_tile_element * element, int32_t terrain)
{
    // Bit 3 for terrain is stored in element.type bit 7
    if (terrain & 8)
        element->type |= 128;
    else
        element->type &= ~128;

    // Bits 0, 1, 2 for terrain are stored in element.slope bit 5, 6, 7
    element->properties.surface.slope &= ~TILE_ELEMENT_SURFACE_EDGE_STYLE_MASK;
    element->properties.surface.slope |= (terrain & 7) << 5;
}

int32_t surface_get_water_height(const rct_tile_element * tileElement)
{
    return tileElement->properties.surface.terrain & TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
}
