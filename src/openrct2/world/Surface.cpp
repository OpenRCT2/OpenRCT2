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

#include "Surface.h"

sint32 surface_get_terrain(const rct_tile_element * element)
{
    sint32 terrain = (element->properties.surface.terrain >> 5) & 7;
    if (element->type & 1)
        terrain |= (1 << 3);
    return terrain;
}

sint32 surface_get_terrain_edge(const rct_tile_element * element)
{
    sint32 terrain_edge = (element->properties.surface.slope >> 5) & 7;
    if (element->type & 128)
        terrain_edge |= (1 << 3);
    return terrain_edge;
}

void surface_set_terrain(rct_tile_element * element, sint32 terrain)
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

void surface_set_terrain_edge(rct_tile_element * element, sint32 terrain)
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

sint32 surface_get_water_height(const rct_tile_element * tileElement)
{
    return tileElement->properties.surface.terrain & TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
}
