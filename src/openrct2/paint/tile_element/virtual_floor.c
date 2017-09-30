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
    if (!input_test_place_object_modifier(PLACE_OBJECT_MODIFIER_COPY_Z | PLACE_OBJECT_MODIFIER_SHIFT_Z))
    {
        return;
    }

    bool    showFade = false;
    bool    showLit = false;

    if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE) &&
        session->MapPosition.x >= gMapSelectPositionA.x - gMapVirtualFloorBaseSize &&
        session->MapPosition.y >= gMapSelectPositionA.y - gMapVirtualFloorBaseSize &&
        session->MapPosition.x <= gMapSelectPositionB.x + gMapVirtualFloorBaseSize &&
        session->MapPosition.y <= gMapSelectPositionB.y + gMapVirtualFloorBaseSize)
    {
        showFade = true;
        if (session->MapPosition.x >= gMapSelectPositionA.x &&
            session->MapPosition.y >= gMapSelectPositionA.y &&
            session->MapPosition.x <= gMapSelectPositionB.x &&
            session->MapPosition.y <= gMapSelectPositionB.y)
        {
            showLit = true;
        }
    }

    if (!showFade && !(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT))
    {
        return;
    }

    LocationXY16 * tile;
    for (tile = gMapSelectionTiles; tile->x != -1; tile++) {
        if (session->MapPosition.x >= tile->x - gMapVirtualFloorBaseSize &&
            session->MapPosition.y >= tile->y - gMapVirtualFloorBaseSize &&
            session->MapPosition.x <= tile->x + gMapVirtualFloorBaseSize &&
            session->MapPosition.y <= tile->y + gMapVirtualFloorBaseSize)
        {
            showFade = true;
        }
    }

    if (!showFade)
    {
        return;
    }

    session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;

    sint16 virtualFloorClipHeight = gMapVirtualFloorHeight / 8;
    uint8 occupiedEdges = 0;

        // Check for occupation and walls

    bool    weAreOccupied       = false;
    bool    weAreBelowGround    = false;

    rct_tile_element * tileElement = map_get_first_element_at(session->MapPosition.x >> 5, session->MapPosition.y >> 5);
    do
    {
        sint32 elementType = tile_element_get_type(tileElement);

        if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST)
        {
            continue;
        }

        if (elementType == TILE_ELEMENT_TYPE_SURFACE)
        {
            if (virtualFloorClipHeight < tileElement->base_height)
            {
                weAreBelowGround    = true;
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

        weAreOccupied = true;
    }
    while (!tile_element_is_last_for_tile(tileElement++));

    for (uint8 i = 0; i < 4; i++)
    {
        uint8 effectiveRotation = (4 + i - get_current_rotation()) % 4;
        sint16 theirLocationX = session->MapPosition.x + offsets[effectiveRotation].x;
        sint16 theirLocationY = session->MapPosition.y + offsets[effectiveRotation].y;

        bool theyAreOccupied        = false;
        bool theyAreBelowGround     = false;

        tileElement = map_get_first_element_at(theirLocationX >> 5, theirLocationY >> 5);
        do
        {
            if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST)
            {
                continue;
            }

            sint32 elementType = tile_element_get_type(tileElement);

            if (elementType == TILE_ELEMENT_TYPE_SURFACE)
            {
                if (virtualFloorClipHeight < tileElement->base_height)
                {
                    theyAreBelowGround    = true;
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

            theyAreOccupied = true;
        }
        while (!tile_element_is_last_for_tile(tileElement++));

        if (weAreOccupied != theyAreOccupied ||
            weAreBelowGround != theyAreBelowGround)
        {
            occupiedEdges   |= 1 << i;
            continue;
        }
    }

    uint32 remap_base = COLOUR_DARK_PURPLE << 19 | IMAGE_TYPE_REMAP;
    uint32 remap_edge = COLOUR_WHITE << 19 | IMAGE_TYPE_REMAP;

    if (weAreOccupied)
    {
        remap_base = COLOUR_BLACK << 19 | IMAGE_TYPE_REMAP;
    }
    if (showLit)
    {
        remap_base = COLOUR_YELLOW << 19 | IMAGE_TYPE_REMAP;
        if (weAreOccupied)
        {
            remap_base = COLOUR_DARK_YELLOW << 19 | IMAGE_TYPE_REMAP;
        }
    }

    sub_98197C(session, SPR_G2_SELECTION_EDGE_NW | ((occupiedEdges & 0x8)? remap_edge : remap_base), 0, 0, 1, 1, 1, gMapVirtualFloorHeight,  5, 16, gMapVirtualFloorHeight + 1, get_current_rotation());
    sub_98197C(session, SPR_G2_SELECTION_EDGE_SW | ((occupiedEdges & 0x4)? remap_edge : remap_base), 0, 0, 1, 1, 1, gMapVirtualFloorHeight, 27, 16, gMapVirtualFloorHeight + 1, get_current_rotation());
    sub_98197C(session, SPR_G2_SELECTION_EDGE_NE | ((occupiedEdges & 0x1)? remap_edge : remap_base), 0, 0, 1, 1, 1, gMapVirtualFloorHeight, 16,  5, gMapVirtualFloorHeight + 1, get_current_rotation());
    sub_98197C(session, SPR_G2_SELECTION_EDGE_SE | ((occupiedEdges & 0x2)? remap_edge : remap_base), 0, 0, 1, 1, 1, gMapVirtualFloorHeight, 16, 27, gMapVirtualFloorHeight + 1, get_current_rotation());
}
