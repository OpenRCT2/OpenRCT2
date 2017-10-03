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

#include "TileElement.h"
#include "../Paint.h"
#include "../../Input.h"
#include "../../interface/Viewport.h"
#include "../../sprites.h"
#include "../../world/Map.h"

static const rct_xy_element offsets[4] =
{
    { -32,   0 },
    {   0,  32 },
    {  32,   0 },
    {   0, -32 }
};

void virtual_floor_paint(paint_session * session)
{
        // We only show when the placement modifier keys are active
    if (!input_test_place_object_modifier(PLACE_OBJECT_MODIFIER_COPY_Z | PLACE_OBJECT_MODIFIER_SHIFT_Z))
    {
        return;
    }

    LocationXY16 * tile;

    bool    show        = false;
    bool    weAreLit    = false;

        // Check if map selection (usually single tiles) are enabled
        //  and if the current tile is near or on them
    if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE) &&
        session->MapPosition.x >= gMapSelectPositionA.x - gMapVirtualFloorBaseSize &&
        session->MapPosition.y >= gMapSelectPositionA.y - gMapVirtualFloorBaseSize &&
        session->MapPosition.x <= gMapSelectPositionB.x + gMapVirtualFloorBaseSize &&
        session->MapPosition.y <= gMapSelectPositionB.y + gMapVirtualFloorBaseSize)
    {
        show = true;
        if (session->MapPosition.x >= gMapSelectPositionA.x &&
            session->MapPosition.y >= gMapSelectPositionA.y &&
            session->MapPosition.x <= gMapSelectPositionB.x &&
            session->MapPosition.y <= gMapSelectPositionB.y)
        {
            weAreLit = true;
        }
    }

    if (!show)
    {
            // Check if we are anywhere near the selection tiles (larger scenery / rides)
        if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
        {
            for (tile = gMapSelectionTiles; tile->x != -1; tile++)
            {
                if (session->MapPosition.x >= tile->x - gMapVirtualFloorBaseSize &&
                    session->MapPosition.y >= tile->y - gMapVirtualFloorBaseSize &&
                    session->MapPosition.x <= tile->x + gMapVirtualFloorBaseSize &&
                    session->MapPosition.y <= tile->y + gMapVirtualFloorBaseSize)
                {
                    show = true;
                    break;
                }
            }
            if (!show)
            {
                return;
            }
        }
        else
        {
            return;
        }
    }

        // See if we are on top of the selectiontiles
    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        for (tile = gMapSelectionTiles; tile->x != -1; tile++)
        {
            if (session->MapPosition.x == tile->x &&
                session->MapPosition.y == tile->y &&
                session->MapPosition.x == tile->x &&
                session->MapPosition.y == tile->y)
            {
                weAreLit = true;
                break;
            }
        }
    }

        // This is a virtual floor, so no interactions
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;

    sint16 virtualFloorClipHeight = gMapVirtualFloorHeight / 8;
    uint8 occupiedEdges = 0;
    uint8 litEdges = 0;

        // Check for occupation and walls

    bool    weAreOccupied       = false;
    bool    weAreBelowGround    = false;

        // Iterate through the map elements of the current tile to find:
        //  * Surfaces, which may put us underground
        //  * Walls / banners, which are displayed as occupied edges
        //  * Ghost objects, which are displayed as lit squares
    rct_tile_element * tileElement = map_get_first_element_at(session->MapPosition.x >> 5, session->MapPosition.y >> 5);
    do
    {
        sint32 elementType = tile_element_get_type(tileElement);

        if (elementType == TILE_ELEMENT_TYPE_SURFACE)
        {
            if (virtualFloorClipHeight < tileElement->clearance_height)
            {
                weAreBelowGround    = true;
            }
            else if (virtualFloorClipHeight < tileElement->base_height + 2 &&
                 tileElement->properties.surface.slope != 0)
            {
                weAreBelowGround    = true;
                weAreOccupied       = true;
            }
            continue;
        }

        if (virtualFloorClipHeight >= tileElement->clearance_height ||
            virtualFloorClipHeight < tileElement->base_height)
        {
            continue;
        }

        if (elementType == TILE_ELEMENT_TYPE_WALL ||
            elementType == TILE_ELEMENT_TYPE_BANNER)
        {
            sint32 direction = tile_element_get_direction(tileElement);
            occupiedEdges   |= 1 << ((direction + get_current_rotation()) % 4);
            continue;
        }

        if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST)
        {
            weAreLit = true;
            continue;
        }

        weAreOccupied = true;
    }
    while (!tile_element_is_last_for_tile(tileElement++));

        // Try the four tiles next to us for the same parameters as above,
        //  if our parameters differ we set an edge towards that tile
    for (uint8 i = 0; i < 4; i++)
    {
        uint8 effectiveRotation = (4 + i - get_current_rotation()) % 4;
        sint16 theirLocationX = session->MapPosition.x + offsets[effectiveRotation].x;
        sint16 theirLocationY = session->MapPosition.y + offsets[effectiveRotation].y;

        bool theyAreOccupied        = false;
        bool theyAreBelowGround     = false;
        bool theyAreLit             = false;

        if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE) &&
            theirLocationX >= gMapSelectPositionA.x &&
            theirLocationY >= gMapSelectPositionA.y &&
            theirLocationX <= gMapSelectPositionB.x &&
            theirLocationY <= gMapSelectPositionB.y)
        {
            theyAreLit = true;
        }
        if (!theyAreLit)
        {
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
            {
                for (tile = gMapSelectionTiles; tile->x != -1; tile++)
                {
                    if (theirLocationX == tile->x &&
                        theirLocationY == tile->y)
                    {
                        theyAreLit = true;
                    }
                }
            }
        }

        tileElement = map_get_first_element_at(theirLocationX >> 5, theirLocationY >> 5);
        do
        {
            sint32 elementType = tile_element_get_type(tileElement);

            if (elementType == TILE_ELEMENT_TYPE_SURFACE)
            {
                if (virtualFloorClipHeight < tileElement->clearance_height)
                {
                    theyAreBelowGround    = true;
                }
                else if (virtualFloorClipHeight < tileElement->base_height + 2 &&
                     tileElement->properties.surface.slope != 0)
                {
                    theyAreBelowGround    = true;
                    theyAreOccupied       = true;
                }
                continue;
            }

            if (virtualFloorClipHeight >= tileElement->clearance_height ||
                virtualFloorClipHeight < tileElement->base_height)
            {
                continue;
            }

            if (elementType == TILE_ELEMENT_TYPE_WALL ||
                elementType == TILE_ELEMENT_TYPE_BANNER)
            {
                sint32 direction = tile_element_get_direction(tileElement);
                if (direction == (effectiveRotation + 2) % 4)
                {
                    occupiedEdges   |= 1 << i;
                    break;
                }
                continue;
            }

            if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST)
            {
                theyAreLit = true;
            }
            else
            {
                theyAreOccupied = true;
            }
        }

        while (!tile_element_is_last_for_tile(tileElement++));

        if (weAreLit != theyAreLit)
        {
            litEdges        |= 1 << i;
        }
        else if (weAreOccupied != theyAreOccupied ||
                 weAreBelowGround != theyAreBelowGround)
        {
            occupiedEdges   |= 1 << i;
        }
    }

    uint32 remap_base = COLOUR_DARK_PURPLE << 19 | IMAGE_TYPE_REMAP;
    uint32 remap_edge = COLOUR_WHITE << 19 | IMAGE_TYPE_REMAP;
    uint32 remap_lit  = COLOUR_DARK_BROWN << 19 | IMAGE_TYPE_REMAP;

        // Edges which are internal to objects (i.e., the tile on both sides
        //  is occupied/lit) are not rendered to provide visual clarity.
    uint8 dullEdges = 0xF & ~occupiedEdges & ~litEdges;
    uint8 paintEdges = (weAreOccupied || weAreLit)? ~dullEdges : 0xF;

    if (paintEdges & 0x8)
    {
        sub_98197C(session, SPR_G2_SELECTION_EDGE_NW | (!(occupiedEdges & 0x8)? ((litEdges & 0x8)? remap_lit : remap_base) : remap_edge),
            0, 0, 1, 1, 1, gMapVirtualFloorHeight,  5,  5, gMapVirtualFloorHeight + ((dullEdges & 0x8)? -2 : 0), get_current_rotation());
    }
    if (paintEdges & 0x4)
    {
        sub_98197C(session, SPR_G2_SELECTION_EDGE_SW | (!(occupiedEdges & 0x4)? ((litEdges & 0x4)? remap_lit : remap_base) : remap_edge),
            0, 0, 1, 1, 1, gMapVirtualFloorHeight, 27, 16, gMapVirtualFloorHeight + ((dullEdges & 0x4)? -2 : 0), get_current_rotation());
    }
    if (paintEdges & 0x1)
    {
        sub_98197C(session, SPR_G2_SELECTION_EDGE_NE | (!(occupiedEdges & 0x1)? ((litEdges & 0x1)? remap_lit : remap_base) : remap_edge),
            0, 0, 1, 1, 1, gMapVirtualFloorHeight,  5,  5, gMapVirtualFloorHeight + ((dullEdges & 0x1)? -2 : 0), get_current_rotation());
    }
    if (paintEdges & 0x2)
    {
        sub_98197C(session, SPR_G2_SELECTION_EDGE_SE | (!(occupiedEdges & 0x2)? ((litEdges & 0x2)? remap_lit : remap_base) : remap_edge),
            0, 0, 1, 1, 1, gMapVirtualFloorHeight, 16, 27, gMapVirtualFloorHeight + ((dullEdges & 0x2)? -2 : 0), get_current_rotation());
    }
}
