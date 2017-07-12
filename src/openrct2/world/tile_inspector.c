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
#include "../core/Guard.hpp"
#include "../game.h"
#include "../interface/window.h"
#include "../ride/track.h"
#include "../windows/tile_inspector.h"
#include "footpath.h"
#include "map.h"
#include "tile_inspector.h"

static bool map_swap_elements_at(sint32 x, sint32 y, sint16 first, sint16 second)
{
    rct_map_element *const firstElement = map_get_nth_element_at(x, y, first);
    rct_map_element *const secondElement = map_get_nth_element_at(x, y, second);

    if (firstElement == NULL) {
        log_error("First element is out of range for the tile");
        return false;
    }
    if (secondElement == NULL) {
        log_error("Second element is out of range for the tile");
        return false;
    }
    if (firstElement == secondElement) {
        log_error("Can't swap the element with itself");
        return false;
    }

    // Swap their memory
    rct_map_element temp = *firstElement;
    *firstElement = *secondElement;
    *secondElement = temp;

    // Swap the 'last map element for tile' flag if either one of them was last
    if (map_element_is_last_for_tile(firstElement) || map_element_is_last_for_tile(secondElement))
    {
        firstElement->flags ^= MAP_ELEMENT_FLAG_LAST_TILE;
        secondElement->flags ^= MAP_ELEMENT_FLAG_LAST_TILE;
    }

    return true;
}

/**
 * Inserts a corrupt element under a given element on a given tile
 * @param x, y: The coordinates of the tile
 * @param elementIndex: The nth element on this tile
 * Returns 0 on success, MONEY_UNDEFINED otherwise.
 */
sint32 tile_inspector_insert_corrupt_at(sint32 x, sint32 y, sint16 elementIndex, sint32 flags)
{
    // Make sure there is enough space for the new element
    if (!map_check_free_elements_and_reorganise(1))
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        // Create new corrupt element
        rct_map_element *corruptElement = map_element_insert(x, y, -1, 0); // Ugly hack: -1 guarantees this to be placed first
        if (corruptElement == NULL)
        {
            log_warning("Failed to insert corrupt element.");
            return MONEY32_UNDEFINED;
        }
        corruptElement->type = MAP_ELEMENT_TYPE_CORRUPT;

        // Set the base height to be the same as the selected element
        rct_map_element *const selectedElement = map_get_nth_element_at(x, y, elementIndex + 1);
        if (!selectedElement) {
            return MONEY32_UNDEFINED;
        }
        corruptElement->base_height = corruptElement->clearance_height = selectedElement->base_height;

        // Move the corrupt element up until the selected list item is reached
        // this way it's placed under the selected element, even when there are multiple elements with the same base height
        for (sint16 i = 0; i < elementIndex; i++)
        {
            if (!map_swap_elements_at(x, y, i, i + 1)) {
                // don't return error here, we've already inserted an element
                // and moved it as far as we could, the only sensible thing left
                // to do is to invalidate the window.
                break;
            }
        }

        map_invalidate_tile_full(x << 5, y << 5);

        // Update the tile inspector's list for everyone who has the tile selected
        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            windowTileInspectorElementCount++;

            // Keep other elements (that are not being hidden) selected
            if (tileInspectorWindow->selected_list_item > elementIndex)
            {
                tileInspectorWindow->selected_list_item++;
            }

            if (tileInspectorWindow->selected_list_item == elementIndex)
            {
                window_tile_inspector_set_page(tileInspectorWindow, TILE_INSPECTOR_PAGE_CORRUPT);
            }

            window_tile_inspector_auto_set_buttons(tileInspectorWindow);
            window_invalidate(tileInspectorWindow);
        }
    }

    // Nothing went wrong
    return 0;
}

/**
* Forcefully removes an element for a given tile
* @param x, y: The coordinates of the tile
* @param elementIndex: The nth element on this tile
*/
sint32 tile_inspector_remove_element_at(sint32 x, sint32 y, sint16 elementIndex, sint32 flags)
{
    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        // Forcefully remove the element
        rct_map_element *const mapElement = map_get_nth_element_at(x, y, elementIndex);
        if (!mapElement) {
            return MONEY32_UNDEFINED;
        }
        map_element_remove(mapElement);
        map_invalidate_tile_full(x << 5, y << 5);

        // Update the window
        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            windowTileInspectorElementCount--;

            if (tileInspectorWindow->selected_list_item > elementIndex)
            {
                tileInspectorWindow->selected_list_item--;
            }
            else if (tileInspectorWindow->selected_list_item == elementIndex)
            {
                tileInspectorWindow->selected_list_item = -1;
                window_tile_inspector_set_page(tileInspectorWindow, TILE_INSPECTOR_PAGE_DEFAULT);
            }

            window_tile_inspector_auto_set_buttons(tileInspectorWindow);
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_swap_elements_at(sint32 x, sint32 y, sint16 first, sint16 second, sint32 flags)
{
    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (!map_swap_elements_at(x, y, first, second)) {
            return MONEY32_UNDEFINED;
        }
        map_invalidate_tile_full(x << 5, y << 5);

        // Update the window
        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            // If one of them was selected, update selected list item
            if (tileInspectorWindow->selected_list_item == first)
                tileInspectorWindow->selected_list_item = second;
            else if (tileInspectorWindow->selected_list_item == second)
                tileInspectorWindow->selected_list_item = first;

            window_tile_inspector_auto_set_buttons(tileInspectorWindow);
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_rotate_element_at(sint32 x, sint32 y, sint32 elementIndex, sint32 flags)
{
    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        uint8 newRotation, pathEdges, pathCorners;

        rct_map_element *const mapElement = map_get_nth_element_at(x, y, elementIndex);
        if (!mapElement) {
            return MONEY32_UNDEFINED;
        }
        switch (map_element_get_type(mapElement))
        {
        case MAP_ELEMENT_TYPE_PATH:
            if (footpath_element_is_sloped(mapElement))
            {
                newRotation = (footpath_element_get_slope_direction(mapElement) + 1) & MAP_ELEMENT_DIRECTION_MASK;
                mapElement->properties.path.type &= ~MAP_ELEMENT_DIRECTION_MASK;
                mapElement->properties.path.type |= newRotation;
            }
            pathEdges = mapElement->properties.path.edges & 0x0F;
            pathCorners = mapElement->properties.path.edges & 0xF0;
            mapElement->properties.path.edges = 0;
            mapElement->properties.path.edges |= ((pathEdges << 1) | (pathEdges >> 3)) & 0x0F;
            mapElement->properties.path.edges |= ((pathCorners << 1) | (pathCorners >> 3)) & 0xF0;
            break;
        case MAP_ELEMENT_TYPE_TRACK:
        case MAP_ELEMENT_TYPE_SCENERY:
        case MAP_ELEMENT_TYPE_ENTRANCE:
        case MAP_ELEMENT_TYPE_WALL:
            newRotation = (mapElement->type + 1) & MAP_ELEMENT_DIRECTION_MASK;
            mapElement->type &= ~MAP_ELEMENT_DIRECTION_MASK;
            mapElement->type |= newRotation;
            break;
        case MAP_ELEMENT_TYPE_BANNER:
            mapElement->properties.banner.flags ^= 1 << mapElement->properties.banner.position;
            mapElement->properties.banner.position++;
            mapElement->properties.banner.position &= 3;
            mapElement->properties.banner.flags ^= 1 << mapElement->properties.banner.position;
            break;
        }

        map_invalidate_tile_full(x << 5, y << 5);

        if ((uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}

sint32 tile_inspector_paste_element_at(sint32 x, sint32 y, rct_map_element element, sint32 flags)
{
    // Make sure there is enough space for the new element
    if (!map_check_free_elements_and_reorganise(1))
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        rct_map_element *const pastedElement = map_element_insert(x, y, element.base_height, 0);

        bool lastForTile = map_element_is_last_for_tile(pastedElement);
        *pastedElement = element;
        pastedElement->flags &= ~MAP_ELEMENT_FLAG_LAST_TILE;
        if (lastForTile)
        {
            pastedElement->flags |= MAP_ELEMENT_FLAG_LAST_TILE;
        }

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            windowTileInspectorElementCount++;

            // Select new element if there was none selected already
            sint16 newIndex = (sint16)(pastedElement - map_get_first_element_at(x, y));
            if (tileInspectorWindow->selected_list_item == -1)
                tileInspectorWindow->selected_list_item = newIndex;
            else if (tileInspectorWindow->selected_list_item >= newIndex)
                tileInspectorWindow->selected_list_item++;

            window_tile_inspector_auto_set_buttons(tileInspectorWindow);
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_sort_elements_at(sint32 x, sint32 y, sint32 flags)
{
    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        const rct_map_element *const firstElement = map_get_first_element_at(x, y);

        // Count elements on tile
        sint32 numElement = 0;
        const rct_map_element *elementIterator = firstElement;
        do
        {
            numElement++;
        } while (!map_element_is_last_for_tile(elementIterator++));

        // Bubble sort
        for (sint32 loopStart = 1; loopStart < numElement; loopStart++)
        {
            sint32 currentId = loopStart;
            const rct_map_element *currentElement = firstElement + currentId;
            const rct_map_element *otherElement = currentElement - 1;

            // While current element's base height is lower, or (when their baseheight is the same) the other map element's clearance height is lower...
            while (currentId > 0 &&
                (otherElement->base_height > currentElement->base_height ||
                (otherElement->base_height == currentElement->base_height && otherElement->clearance_height > currentElement->clearance_height)))
            {
                if (!map_swap_elements_at(x, y, currentId - 1, currentId)) {
                    // don't return error here, we've already ran some actions
                    // and moved things as far as we could, the only sensible
                    // thing left to do is to invalidate the window.
                    loopStart = numElement;
                    break;
                }
                currentId--;
                currentElement--;
                otherElement--;
            }
        }

        map_invalidate_tile_full(x << 5, y << 5);

        // Deselect tile for clients who had it selected
        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_tile_inspector_set_page(tileInspectorWindow, TILE_INSPECTOR_PAGE_DEFAULT);
            tileInspectorWindow->selected_list_item = -1;
            window_tile_inspector_auto_set_buttons(tileInspectorWindow);
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_any_base_height_offset(sint32 x, sint32 y, sint16 elementIndex, sint8 heightOffset, sint32 flags)
{
    rct_map_element *const mapElement = map_get_nth_element_at(x, y, elementIndex);
    if (!mapElement) {
        return MONEY32_UNDEFINED;
    }
    sint16 newBaseHeight = (sint16)mapElement->base_height + heightOffset;
    sint16 newClearanceHeight = (sint16)mapElement->clearance_height + heightOffset;
    if (newBaseHeight < 0 || newBaseHeight > 0xff || newClearanceHeight < 0 || newClearanceHeight > 0xff)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        mapElement->base_height += heightOffset;
        mapElement->clearance_height += heightOffset;

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_surface_show_park_fences(sint32 x, sint32 y, bool showFences, sint32 flags)
{
    rct_map_element *const surfaceelement = map_get_surface_element_at(x, y);

    // No surface element on tile
    if (surfaceelement == NULL)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (!showFences)
            surfaceelement->properties.surface.ownership &= ~0x0F;
        else
            update_park_fences(x << 5, y << 5);

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_surface_toggle_corner(sint32 x, sint32 y, sint32 cornerIndex, sint32 flags)
{
    rct_map_element *const surfaceElement = map_get_surface_element_at(x, y);

    // No surface element on tile
    if (surfaceElement == NULL)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        const uint8 originalSlope = surfaceElement->properties.surface.slope;
        const bool diagonal = (originalSlope & 0x10) >> 4;

        surfaceElement->properties.surface.slope ^= 1 << cornerIndex;
        if (surfaceElement->properties.surface.slope & 0x0F)
        {
            surfaceElement->clearance_height = surfaceElement->base_height + 2;
        }
        else
        {
            surfaceElement->clearance_height = surfaceElement->base_height;
        }

        // All corners are raised
        if ((surfaceElement->properties.surface.slope & 0x0F) == 0x0F)
        {
            surfaceElement->properties.surface.slope &= ~0x1F;

            if (diagonal)
            {
                switch (originalSlope & 0x0F)
                {
                case 0b1011: surfaceElement->properties.surface.slope |= (1 << 0); break;
                case 0b0111: surfaceElement->properties.surface.slope |= (1 << 1); break;
                case 0b1110: surfaceElement->properties.surface.slope |= (1 << 2); break;
                case 0b1101: surfaceElement->properties.surface.slope |= (1 << 3); break;
                }
            }

            // Update base and clearance heights
            surfaceElement->base_height += 2;
            surfaceElement->clearance_height = surfaceElement->base_height + (diagonal ? 2 : 0);
        }

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_surface_toggle_diagonal(sint32 x, sint32 y, sint32 flags)
{
    rct_map_element *const surfaceElement = map_get_surface_element_at(x, y);

    // No surface element on tile
    if (surfaceElement == NULL)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        surfaceElement->properties.surface.slope ^= 0x10;
        if (surfaceElement->properties.surface.slope & 0x10)
        {
            surfaceElement->clearance_height = surfaceElement->base_height + 4;
        }
        else if (surfaceElement->properties.surface.slope & 0x0F)
        {
            surfaceElement->clearance_height = surfaceElement->base_height + 2;
        }
        else
        {
            surfaceElement->clearance_height = surfaceElement->base_height;
        }

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_path_set_sloped(sint32 x, sint32 y, sint32 elementIndex, bool sloped, sint32 flags)
{
    rct_map_element *const pathElement = map_get_nth_element_at(x, y, elementIndex);

    if (!pathElement || map_element_get_type(pathElement) != MAP_ELEMENT_TYPE_PATH)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        pathElement->properties.path.type &= ~(1 << 2);
        if (sloped)
        {
            pathElement->properties.path.type |= (1 << 2);
        }

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_path_toggle_edge(sint32 x, sint32 y, sint32 elementIndex, sint32 edgeIndex, sint32 flags)
{
    rct_map_element *const pathElement = map_get_nth_element_at(x, y, elementIndex);

    if (!pathElement || map_element_get_type(pathElement) != MAP_ELEMENT_TYPE_PATH)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        pathElement->properties.path.edges ^= 1 << edgeIndex;

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_wall_set_slope(sint32 x, sint32 y, sint32 elementIndex, sint32 slopeValue, sint32 flags)
{
    rct_map_element *const wallElement = map_get_nth_element_at(x, y, elementIndex);

    if (!wallElement || map_element_get_type(wallElement) != MAP_ELEMENT_TYPE_WALL)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        // Set new slope value
        wallElement->type &= ~0xC0;
        wallElement->type |= slopeValue;

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window *const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

// Changes the height of all track elements that belong to the same track piece
// Broxzier: Copied from track_remove and stripped of unneeded code, but I think this should be smaller
sint32 tile_inspector_track_base_height_offset(sint32 x, sint32 y, sint32 elementIndex, sint8 offset, sint32 flags)
{
    rct_map_element *const trackElement = map_get_nth_element_at(x, y, elementIndex);

    if (offset == 0)
    {
        return MONEY32_UNDEFINED;
    }

    if (!trackElement || map_element_get_type(trackElement) != MAP_ELEMENT_TYPE_TRACK)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        uint8 type = trackElement->properties.track.type;
        sint16 originX = x << 5;
        sint16 originY = y << 5;
        sint16 originZ = trackElement->base_height * 8;
        uint8 rotation = map_element_get_direction(trackElement);
        uint8 rideIndex = trackElement->properties.track.ride_index;
        rct_ride* ride = get_ride(rideIndex);
        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, type);
        trackBlock += trackElement->properties.track.sequence & 0x0F;

        uint8 originDirection = map_element_get_direction(trackElement);
        switch (originDirection)
        {
        case 0:
            originX -= trackBlock->x;
            originY -= trackBlock->y;
            break;
        case 1:
            originX -= trackBlock->y;
            originY += trackBlock->x;
            break;
        case 2:
            originX += trackBlock->x;
            originY += trackBlock->y;
            break;
        case 3:
            originX += trackBlock->y;
            originY -= trackBlock->x;
            break;
        }

        originZ -= trackBlock->z;

        trackBlock = get_track_def_from_ride(ride, type);
        for (; trackBlock->index != 255; trackBlock++)
        {
            sint16 elemX = originX, elemY = originY, elemZ = originZ;

            switch (originDirection)
            {
            case 0:
                elemX += trackBlock->x;
                elemY += trackBlock->y;
                break;
            case 1:
                elemX += trackBlock->y;
                elemY -= trackBlock->x;
                break;
            case 2:
                elemX -= trackBlock->x;
                elemY -= trackBlock->y;
                break;
            case 3:
                elemX -= trackBlock->y;
                elemY += trackBlock->x;
                break;
            }

            elemZ += trackBlock->z;

            map_invalidate_tile_full(elemX, elemY);

            bool found = false;
            rct_map_element *mapElement = map_get_first_element_at(elemX >> 5, elemY >> 5);
            do
            {
                if (mapElement->base_height != elemZ / 8)
                    continue;

                if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                    continue;

                if ((mapElement->type & MAP_ELEMENT_DIRECTION_MASK) != rotation)
                    continue;

                if ((mapElement->properties.track.sequence & 0x0F) != trackBlock->index)
                    continue;

                if (mapElement->properties.track.type != type)
                    continue;

                found = true;
                break;
            } while (!map_element_is_last_for_tile(mapElement++));

            if (!found)
            {
                log_error("Track map element part not found!");
                return MONEY32_UNDEFINED;
            }

            // track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is for when you decrease the map size.
            openrct2_assert(map_get_surface_element_at(elemX >> 5, elemY >> 5) != NULL, "No surface at %d,%d", elemX >> 5, elemY >> 5);

            // Keep?
            //invalidate_test_results(rideIndex);

            mapElement->base_height += offset;
            mapElement->clearance_height += offset;
        }
    }

    // TODO: Only invalidate when one of the affected tiles is selected
    window_invalidate_by_class(WC_TILE_INSPECTOR);

    return 0;
}

// Sets chainlift, optionally for an entire track block
// Broxzier: Basically a copy of the above function, with just two different lines... should probably be combined somehow
sint32 tile_inspector_track_set_chain(sint32 x, sint32 y, sint32 elementIndex, bool entireTrackBlock, bool setChain, sint32 flags)
{
    rct_map_element *const trackElement = map_get_nth_element_at(x, y, elementIndex);

    if (!trackElement || map_element_get_type(trackElement) != MAP_ELEMENT_TYPE_TRACK)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (!entireTrackBlock)
        {
            // Set chain for only the selected piece
            if (track_element_is_lift_hill(trackElement) != setChain)
            {
                trackElement->type ^= TRACK_ELEMENT_FLAG_CHAIN_LIFT;
            }

            return 0;
        }

        uint8 type = trackElement->properties.track.type;
        sint16 originX = x << 5;
        sint16 originY = y << 5;
        sint16 originZ = trackElement->base_height * 8;
        uint8 rotation = map_element_get_direction(trackElement);
        uint8 rideIndex = trackElement->properties.track.ride_index;
        rct_ride* ride = get_ride(rideIndex);
        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, type);
        trackBlock += trackElement->properties.track.sequence & 0x0F;

        uint8 originDirection = map_element_get_direction(trackElement);
        switch (originDirection)
        {
        case 0:
            originX -= trackBlock->x;
            originY -= trackBlock->y;
            break;
        case 1:
            originX -= trackBlock->y;
            originY += trackBlock->x;
            break;
        case 2:
            originX += trackBlock->x;
            originY += trackBlock->y;
            break;
        case 3:
            originX += trackBlock->y;
            originY -= trackBlock->x;
            break;
        }

        originZ -= trackBlock->z;

        trackBlock = get_track_def_from_ride(ride, type);
        for (; trackBlock->index != 255; trackBlock++)
        {
            sint16 elemX = originX, elemY = originY, elemZ = originZ;

            switch (originDirection)
            {
            case 0:
                elemX += trackBlock->x;
                elemY += trackBlock->y;
                break;
            case 1:
                elemX += trackBlock->y;
                elemY -= trackBlock->x;
                break;
            case 2:
                elemX -= trackBlock->x;
                elemY -= trackBlock->y;
                break;
            case 3:
                elemX -= trackBlock->y;
                elemY += trackBlock->x;
                break;
            }

            elemZ += trackBlock->z;

            map_invalidate_tile_full(elemX, elemY);

            bool found = false;
            rct_map_element *mapElement = map_get_first_element_at(elemX >> 5, elemY >> 5);
            do
            {
                if (mapElement->base_height != elemZ / 8)
                    continue;

                if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                    continue;

                if ((mapElement->type & MAP_ELEMENT_DIRECTION_MASK) != rotation)
                    continue;

                if ((mapElement->properties.track.sequence & 0x0F) != trackBlock->index)
                    continue;

                if (mapElement->properties.track.type != type)
                    continue;

                found = true;
                break;
            } while (!map_element_is_last_for_tile(mapElement++));

            if (!found)
            {
                log_error("Track map element part not found!");
                return MONEY32_UNDEFINED;
            }

            // track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is for when you decrease the map size.
            openrct2_assert(map_get_surface_element_at(elemX >> 5, elemY >> 5) != NULL, "No surface at %d,%d", elemX >> 5, elemY >> 5);

            // Keep?
            //invalidate_test_results(rideIndex);

            if (track_element_is_lift_hill(mapElement) != setChain)
            {
                mapElement->type ^= TRACK_ELEMENT_FLAG_CHAIN_LIFT;
            }
        }
    }

    // TODO: Only invalidate when one of the affected tiles is selected
    window_invalidate_by_class(WC_TILE_INSPECTOR);

    return 0;
}

sint32 tile_inspector_scenery_set_quarter_location(sint32 x, sint32 y, sint32 elementIndex, sint32 quarterIndex, sint32 flags)
{
    rct_map_element *const mapElement = map_get_nth_element_at(x, y, elementIndex);

    if (!mapElement || map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_SCENERY)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        // Set quadrant index
        mapElement->type &= ~MAP_ELEMENT_QUADRANT_MASK;
        mapElement->type |= quarterIndex << 6;

        // Update collision
        mapElement->flags &= 0xF0;
        mapElement->flags |= 1 << ((quarterIndex + 2) & 3);

        map_invalidate_tile_full(x << 5, y << 5);
        if ((uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}

sint32 tile_inspector_scenery_set_quarter_collision(sint32 x, sint32 y, sint32 elementIndex, sint32 quarterIndex, sint32 flags)
{
    rct_map_element *const mapElement = map_get_nth_element_at(x, y, elementIndex);

    if (!mapElement || map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_SCENERY)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        mapElement->flags ^= 1 << quarterIndex;

        map_invalidate_tile_full(x << 5, y << 5);
        if ((uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}

sint32 tile_inspector_banner_toggle_blocking_edge(sint32 x, sint32 y, sint32 elementIndex, sint32 edgeIndex, sint32 flags)
{
    rct_map_element *const bannerElement = map_get_nth_element_at(x, y, elementIndex);

    if (!bannerElement || map_element_get_type(bannerElement) != MAP_ELEMENT_TYPE_BANNER)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        bannerElement->properties.banner.flags ^= 1 << edgeIndex;

        if ((uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}

sint32 tile_inspector_corrupt_clamp(sint32 x, sint32 y, sint32 elementIndex, sint32 flags)
{
    rct_map_element *const corruptElement = map_get_nth_element_at(x, y, elementIndex);

    if (!corruptElement || map_element_get_type(corruptElement) != MAP_ELEMENT_TYPE_CORRUPT)
    {
        return MONEY32_UNDEFINED;
    }

    if (map_element_is_last_for_tile(corruptElement))
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        rct_map_element *const nextElement = corruptElement + 1;
        corruptElement->base_height = corruptElement->clearance_height = nextElement->base_height;

        if ((uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}
