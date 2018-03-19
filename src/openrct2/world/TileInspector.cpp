#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

#include "Banner.h"
#include "../common.h"
#include "../Context.h"
#include "../Game.h"
#include "../core/Guard.hpp"
#include "../interface/Window.h"
#include "../interface/Window_internal.h"
#include "../localisation/Localisation.h"
#include "../ride/Track.h"
#include "../windows/Intent.h"
#include "../windows/tile_inspector.h"
#include "Footpath.h"
#include "LargeScenery.h"
#include "Map.h"
#include "Scenery.h"
#include "TileInspector.h"
#include "../ride/Station.h"
#include "Park.h"

uint32 windowTileInspectorTileX;
uint32 windowTileInspectorTileY;
sint32 windowTileInspectorElementCount = 0;

static void window_tile_inspector_set_page(rct_window * w, const TILE_INSPECTOR_PAGE page)
{
    auto intent = Intent(INTENT_ACTION_SET_TILE_INSPECTOR_PAGE);
    intent.putExtra(INTENT_EXTRA_PAGE, page);
    context_broadcast_intent(&intent);
}

static void window_tile_inspector_auto_set_buttons(rct_window * w)
{
    auto intent = Intent(INTENT_ACTION_SET_TILE_INSPECTOR_BUTTONS);
    context_broadcast_intent(&intent);
}

static bool map_swap_elements_at(sint32 x, sint32 y, sint16 first, sint16 second)
{
    rct_tile_element * const firstElement  = map_get_nth_element_at(x, y, first);
    rct_tile_element * const secondElement = map_get_nth_element_at(x, y, second);

    if (firstElement == nullptr)
    {
        log_error("First element is out of range for the tile");
        return false;
    }
    if (secondElement == nullptr)
    {
        log_error("Second element is out of range for the tile");
        return false;
    }
    if (firstElement == secondElement)
    {
        log_error("Can't swap the element with itself");
        return false;
    }

    // Swap their memory
    rct_tile_element temp = *firstElement;
    *firstElement         = *secondElement;
    *secondElement        = temp;

    // Swap the 'last map element for tile' flag if either one of them was last
    if (tile_element_is_last_for_tile(firstElement) || tile_element_is_last_for_tile(secondElement))
    {
        firstElement->flags ^= TILE_ELEMENT_FLAG_LAST_TILE;
        secondElement->flags ^= TILE_ELEMENT_FLAG_LAST_TILE;
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
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        // Create new corrupt element
        rct_tile_element * corruptElement = tile_element_insert(x, y, -1, 0); // Ugly hack: -1 guarantees this to be placed first
        if (corruptElement == nullptr)
        {
            log_warning("Failed to insert corrupt element.");
            return MONEY32_UNDEFINED;
        }
        corruptElement->type = TILE_ELEMENT_TYPE_CORRUPT;

        // Set the base height to be the same as the selected element
        rct_tile_element * const selectedElement = map_get_nth_element_at(x, y, elementIndex + 1);
        if (!selectedElement)
        {
            return MONEY32_UNDEFINED;
        }
        corruptElement->base_height = corruptElement->clearance_height = selectedElement->base_height;

        // Move the corrupt element up until the selected list item is reached
        // this way it's placed under the selected element, even when there are multiple elements with the same base height
        for (sint16 i = 0; i < elementIndex; i++)
        {
            if (!map_swap_elements_at(x, y, i, i + 1))
            {
                // don't return error here, we've already inserted an element
                // and moved it as far as we could, the only sensible thing left
                // to do is to invalidate the window.
                break;
            }
        }

        map_invalidate_tile_full(x << 5, y << 5);

        // Update the tile inspector's list for everyone who has the tile selected
        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
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
        rct_tile_element * const tileElement = map_get_nth_element_at(x, y, elementIndex);
        if (!tileElement)
        {
            return MONEY32_UNDEFINED;
        }
        tile_element_remove(tileElement);
        map_invalidate_tile_full(x << 5, y << 5);

        // Update the window
        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
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
        if (!map_swap_elements_at(x, y, first, second))
        {
            return MONEY32_UNDEFINED;
        }
        map_invalidate_tile_full(x << 5, y << 5);

        // Update the window
        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
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

        rct_tile_element * const tileElement = map_get_nth_element_at(x, y, elementIndex);
        if (!tileElement)
        {
            return MONEY32_UNDEFINED;
        }
        switch (tile_element_get_type(tileElement))
        {
        case TILE_ELEMENT_TYPE_PATH:
            if (footpath_element_is_sloped(tileElement))
            {
                newRotation = (footpath_element_get_slope_direction(tileElement) + 1) & TILE_ELEMENT_DIRECTION_MASK;
                tileElement->properties.path.type &= ~TILE_ELEMENT_DIRECTION_MASK;
                tileElement->properties.path.type |= newRotation;
            }
            pathEdges                          = tileElement->properties.path.edges & 0x0F;
            pathCorners                        = tileElement->properties.path.edges & 0xF0;
            tileElement->properties.path.edges = 0;
            tileElement->properties.path.edges |= ((pathEdges << 1) | (pathEdges >> 3)) & 0x0F;
            tileElement->properties.path.edges |= ((pathCorners << 1) | (pathCorners >> 3)) & 0xF0;
            break;
        case TILE_ELEMENT_TYPE_ENTRANCE:
        {
            // Update element rotation
            newRotation = tile_element_get_direction_with_offset(tileElement, 1);
            tileElement->type &= ~TILE_ELEMENT_DIRECTION_MASK;
            tileElement->type |= newRotation;

            // Update ride's known entrance/exit rotation
            Ride * ride         = get_ride(tileElement->properties.entrance.ride_index);
            uint8  stationIndex = tileElement->properties.entrance.index;
            auto   entrance     = ride_get_entrance_location(ride, stationIndex);
            auto   exit         = ride_get_exit_location(ride, stationIndex);
            uint8  entranceType = entrance_element_get_type(tileElement);
            uint8  z            = tileElement->base_height;

            // Make sure this is the correct entrance or exit
            if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE && entrance.x == x && entrance.y == y && entrance.z == z)
            {
                ride_set_entrance_location(ride, stationIndex, { entrance.x, entrance.y, entrance.z, newRotation });
            }
            else if (entranceType == ENTRANCE_TYPE_RIDE_EXIT && exit.x == x && exit.y == y && exit.z == z)
            {
                ride_set_exit_location(ride, stationIndex, { exit.x, exit.y, exit.z, newRotation });
            }
            break;
        }
        case TILE_ELEMENT_TYPE_TRACK:
        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
        case TILE_ELEMENT_TYPE_WALL:
            newRotation = tile_element_get_direction_with_offset(tileElement, 1);
            tileElement->type &= ~TILE_ELEMENT_DIRECTION_MASK;
            tileElement->type |= newRotation;
            break;
        case TILE_ELEMENT_TYPE_BANNER:
            tileElement->properties.banner.flags ^= 1 << tileElement->properties.banner.position;
            tileElement->properties.banner.position++;
            tileElement->properties.banner.position &= 3;
            tileElement->properties.banner.flags ^= 1 << tileElement->properties.banner.position;
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

sint32 tile_inspector_paste_element_at(sint32 x, sint32 y, rct_tile_element element, sint32 flags)
{
    // Make sure there is enough space for the new element
    if (!map_check_free_elements_and_reorganise(1))
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        // Check if the element to be pasted refers to a banner index
        sint32 bannerIndex = tile_element_get_banner_index(&element);
        if (bannerIndex != BANNER_INDEX_NULL)
        {
            // The element to be pasted refers to a banner index - make a copy of it
            sint32 newBannerIndex = create_new_banner(flags);
            if (newBannerIndex == BANNER_NULL)
            {
                return MONEY32_UNDEFINED;
            }
            rct_banner & newBanner = gBanners[newBannerIndex];
            newBanner              = gBanners[bannerIndex];
            newBanner.x            = x;
            newBanner.y            = y;

            // Use the new banner index
            tile_element_set_banner_index(&element, newBannerIndex);

            // Duplicate user string if needed
            rct_string_id stringIdx = newBanner.string_idx;
            if (is_user_string_id(stringIdx))
            {
                utf8 buffer[USER_STRING_MAX_LENGTH];
                format_string(buffer, USER_STRING_MAX_LENGTH, stringIdx, nullptr);
                rct_string_id newStringIdx = user_string_allocate(USER_STRING_DUPLICATION_PERMITTED, buffer);
                if (newStringIdx == 0)
                {
                    return MONEY32_UNDEFINED;
                }
                gBanners[newBannerIndex].string_idx = newStringIdx;
            }
        }

        rct_tile_element * const pastedElement = tile_element_insert(x, y, element.base_height, 0);

        bool lastForTile = tile_element_is_last_for_tile(pastedElement);
        *pastedElement   = element;
        pastedElement->flags &= ~TILE_ELEMENT_FLAG_LAST_TILE;
        if (lastForTile)
        {
            pastedElement->flags |= TILE_ELEMENT_FLAG_LAST_TILE;
        }

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
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
        const rct_tile_element * const firstElement = map_get_first_element_at(x, y);

        // Count elements on tile
        sint32                   numElement      = 0;
        const rct_tile_element * elementIterator = firstElement;
        do
        {
            numElement++;
        } while (!tile_element_is_last_for_tile(elementIterator++));

        // Bubble sort
        for (sint32 loopStart = 1; loopStart < numElement; loopStart++)
        {
            sint32                   currentId      = loopStart;
            const rct_tile_element * currentElement = firstElement + currentId;
            const rct_tile_element * otherElement   = currentElement - 1;

            // While current element's base height is lower, or (when their baseheight is the same) the other map element's
            // clearance height is lower...
            while (currentId > 0 && (otherElement->base_height > currentElement->base_height ||
                                     (otherElement->base_height == currentElement->base_height &&
                                      otherElement->clearance_height > currentElement->clearance_height)))
            {
                if (!map_swap_elements_at(x, y, currentId - 1, currentId))
                {
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
        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
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
    rct_tile_element * const tileElement = map_get_nth_element_at(x, y, elementIndex);
    if (tileElement == nullptr)
        return MONEY32_UNDEFINED;

    sint16 newBaseHeight      = (sint16)tileElement->base_height + heightOffset;
    sint16 newClearanceHeight = (sint16)tileElement->clearance_height + heightOffset;
    if (newBaseHeight < 0 || newBaseHeight > 0xff || newClearanceHeight < 0 || newClearanceHeight > 0xff)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_ENTRANCE)
        {
            uint8 entranceType = tileElement->properties.entrance.type;
            if (entranceType != ENTRANCE_TYPE_PARK_ENTRANCE)
            {
                // Update the ride's known entrance or exit height
                Ride * ride          = get_ride(tileElement->properties.entrance.ride_index);
                uint8  entranceIndex = tileElement->properties.entrance.index;
                auto   entrance      = ride_get_entrance_location(ride, entranceIndex);
                auto   exit          = ride_get_exit_location(ride, entranceIndex);
                uint8 z = tileElement->base_height;

                // Make sure this is the correct entrance or exit
                if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE && entrance.x == x && entrance.y == y && entrance.z == z)
                    ride_set_entrance_location(
                        ride, entranceIndex, { entrance.x, entrance.y, z + heightOffset, entrance.direction });
                else if (entranceType == ENTRANCE_TYPE_RIDE_EXIT && exit.x == x && exit.y == y && exit.z == z)
                    ride_set_exit_location(
                        ride, entranceIndex, { exit.x, exit.y, z + heightOffset, exit.direction });
            }
        }

        tileElement->base_height += heightOffset;
        tileElement->clearance_height += heightOffset;

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_surface_show_park_fences(sint32 x, sint32 y, bool showFences, sint32 flags)
{
    rct_tile_element * const surfaceelement = map_get_surface_element_at(x, y);

    // No surface element on tile
    if (surfaceelement == nullptr)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (!showFences)
            surfaceelement->properties.surface.ownership &= ~0x0F;
        else
            update_park_fences(x << 5, y << 5);

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_surface_toggle_corner(sint32 x, sint32 y, sint32 cornerIndex, sint32 flags)
{
    rct_tile_element * const surfaceElement = map_get_surface_element_at(x, y);

    // No surface element on tile
    if (surfaceElement == nullptr)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        const uint8 originalSlope = surfaceElement->properties.surface.slope;
        const bool  diagonal      = (originalSlope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT) >> 4;

        surfaceElement->properties.surface.slope ^= 1 << cornerIndex;
        if (surfaceElement->properties.surface.slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            surfaceElement->clearance_height = surfaceElement->base_height + 2;
        }
        else
        {
            surfaceElement->clearance_height = surfaceElement->base_height;
        }

        // All corners are raised
        if ((surfaceElement->properties.surface.slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) == TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            surfaceElement->properties.surface.slope &= ~TILE_ELEMENT_SURFACE_SLOPE_MASK;

            if (diagonal)
            {
                switch (originalSlope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
                {
                case TILE_ELEMENT_SLOPE_S_CORNER_DN:
                    surfaceElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_N_CORNER_UP;
                    break;
                case TILE_ELEMENT_SLOPE_W_CORNER_DN:
                    surfaceElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_E_CORNER_UP;
                    break;
                case TILE_ELEMENT_SLOPE_N_CORNER_DN:
                    surfaceElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_S_CORNER_UP;
                    break;
                case TILE_ELEMENT_SLOPE_E_CORNER_DN:
                    surfaceElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_W_CORNER_UP;
                    break;
                }
            }

            // Update base and clearance heights
            surfaceElement->base_height += 2;
            surfaceElement->clearance_height = surfaceElement->base_height + (diagonal ? 2 : 0);
        }

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_surface_toggle_diagonal(sint32 x, sint32 y, sint32 flags)
{
    rct_tile_element * const surfaceElement = map_get_surface_element_at(x, y);

    // No surface element on tile
    if (surfaceElement == nullptr)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        surfaceElement->properties.surface.slope ^= TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT;
        if (surfaceElement->properties.surface.slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
        {
            surfaceElement->clearance_height = surfaceElement->base_height + 4;
        }
        else if (surfaceElement->properties.surface.slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            surfaceElement->clearance_height = surfaceElement->base_height + 2;
        }
        else
        {
            surfaceElement->clearance_height = surfaceElement->base_height;
        }

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_path_set_sloped(sint32 x, sint32 y, sint32 elementIndex, bool sloped, sint32 flags)
{
    rct_tile_element * const pathElement = map_get_nth_element_at(x, y, elementIndex);

    if (pathElement == nullptr || tile_element_get_type(pathElement) != TILE_ELEMENT_TYPE_PATH)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        pathElement->properties.path.type &= ~(1 << 2);
        if (sloped)
        {
            pathElement->properties.path.type |= (1 << 2);
        }

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_path_toggle_edge(sint32 x, sint32 y, sint32 elementIndex, sint32 edgeIndex, sint32 flags)
{
    rct_tile_element * const pathElement = map_get_nth_element_at(x, y, elementIndex);

    if (pathElement == nullptr || tile_element_get_type(pathElement) != TILE_ELEMENT_TYPE_PATH)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        pathElement->properties.path.edges ^= 1 << edgeIndex;

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_entrance_make_usable(sint32 x, sint32 y, sint32 elementIndex, sint32 flags)
{
    rct_tile_element * const entranceElement = map_get_nth_element_at(x, y, elementIndex);

    if (entranceElement == nullptr || tile_element_get_type(entranceElement) != TILE_ELEMENT_TYPE_ENTRANCE)
        return MONEY32_UNDEFINED;

    Ride * ride = get_ride(entranceElement->properties.entrance.ride_index);

    if (ride == nullptr)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        uint8 stationIndex = entranceElement->properties.entrance.index >> 6;

        switch (entranceElement->properties.entrance.type)
        {
        case ENTRANCE_TYPE_RIDE_ENTRANCE:
            ride_set_entrance_location(ride, stationIndex, { x, y, entranceElement->base_height, (uint8)tile_element_get_direction(entranceElement) });
            break;
        case ENTRANCE_TYPE_RIDE_EXIT:
            ride_set_exit_location(ride, stationIndex, { x, y, entranceElement->base_height, (uint8)tile_element_get_direction(entranceElement) });
            break;
        }

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

sint32 tile_inspector_wall_set_slope(sint32 x, sint32 y, sint32 elementIndex, sint32 slopeValue, sint32 flags)
{
    rct_tile_element * const wallElement = map_get_nth_element_at(x, y, elementIndex);

    if (wallElement == nullptr || tile_element_get_type(wallElement) != TILE_ELEMENT_TYPE_WALL)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        // Set new slope value
        wallElement->type &= ~0xC0;
        wallElement->type |= slopeValue;

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
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
    rct_tile_element * const trackElement = map_get_nth_element_at(x, y, elementIndex);

    if (offset == 0)
        return 0;

    if (trackElement == nullptr || tile_element_get_type(trackElement) != TILE_ELEMENT_TYPE_TRACK)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        uint8                     type       = track_element_get_type(trackElement);
        sint16                    originX    = x << 5;
        sint16                    originY    = y << 5;
        sint16                    originZ    = trackElement->base_height * 8;
        uint8                     rotation   = tile_element_get_direction(trackElement);
        uint8                     rideIndex  = track_element_get_ride_index(trackElement);
        Ride *                    ride       = get_ride(rideIndex);
        const rct_preview_track * trackBlock = get_track_def_from_ride(ride, type);
        trackBlock += tile_element_get_track_sequence(trackElement);

        uint8 originDirection = tile_element_get_direction(trackElement);
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

            bool               found       = false;
            rct_tile_element * tileElement = map_get_first_element_at(elemX >> 5, elemY >> 5);
            do
            {
                if (tileElement->base_height != elemZ / 8)
                    continue;

                if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_TRACK)
                    continue;

                if ((tile_element_get_direction(tileElement)) != rotation)
                    continue;

                if (tile_element_get_track_sequence(tileElement) != trackBlock->index)
                    continue;

                if (track_element_get_type(tileElement) != type)
                    continue;

                found = true;
                break;
            } while (!tile_element_is_last_for_tile(tileElement++));

            if (!found)
            {
                log_error("Track map element part not found!");
                return MONEY32_UNDEFINED;
            }

            // track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is for when
            // you decrease the map size.
            openrct2_assert(map_get_surface_element_at({elemX, elemY}) != nullptr, "No surface at %d,%d", elemX >> 5,
                            elemY >> 5);

            // Keep?
            // invalidate_test_results(rideIndex);

            tileElement->base_height += offset;
            tileElement->clearance_height += offset;
        }
    }

    // TODO: Only invalidate when one of the affected tiles is selected
    window_invalidate_by_class(WC_TILE_INSPECTOR);

    return 0;
}

// Sets chainlift, optionally for an entire track block
// Broxzier: Basically a copy of the above function, with just two different lines... should probably be combined somehow
sint32 tile_inspector_track_set_chain(sint32 x, sint32 y, sint32 elementIndex, bool entireTrackBlock, bool setChain,
                                      sint32 flags)
{
    rct_tile_element * const trackElement = map_get_nth_element_at(x, y, elementIndex);

    if (trackElement == nullptr || tile_element_get_type(trackElement) != TILE_ELEMENT_TYPE_TRACK)
        return MONEY32_UNDEFINED;

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

        uint8                     type       = track_element_get_type(trackElement);
        sint16                    originX    = x << 5;
        sint16                    originY    = y << 5;
        sint16                    originZ    = trackElement->base_height * 8;
        uint8                     rotation   = tile_element_get_direction(trackElement);
        uint8                     rideIndex  = track_element_get_ride_index(trackElement);
        Ride *                    ride       = get_ride(rideIndex);
        const rct_preview_track * trackBlock = get_track_def_from_ride(ride, type);
        trackBlock += tile_element_get_track_sequence(trackElement);

        uint8 originDirection = tile_element_get_direction(trackElement);
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

            bool               found       = false;
            rct_tile_element * tileElement = map_get_first_element_at(elemX >> 5, elemY >> 5);
            do
            {
                if (tileElement->base_height != elemZ / 8)
                    continue;

                if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_TRACK)
                    continue;

                if ((tile_element_get_direction(tileElement)) != rotation)
                    continue;

                if (tile_element_get_track_sequence(tileElement) != trackBlock->index)
                    continue;

                if (track_element_get_type(tileElement) != type)
                    continue;

                found = true;
                break;
            } while (!tile_element_is_last_for_tile(tileElement++));

            if (!found)
            {
                log_error("Track map element part not found!");
                return MONEY32_UNDEFINED;
            }

            // track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is for when
            // you decrease the map size.
            openrct2_assert(map_get_surface_element_at({elemX, elemY}) != nullptr, "No surface at %d,%d", elemX >> 5,
                            elemY >> 5);

            // Keep?
            // invalidate_test_results(rideIndex);

            if (track_element_is_lift_hill(tileElement) != setChain)
            {
                tileElement->type ^= TRACK_ELEMENT_FLAG_CHAIN_LIFT;
            }
        }
    }

    // TODO: Only invalidate when one of the affected tiles is selected
    window_invalidate_by_class(WC_TILE_INSPECTOR);

    return 0;
}

sint32 tile_inspector_scenery_set_quarter_location(sint32 x, sint32 y, sint32 elementIndex, sint32 quarterIndex, sint32 flags)
{
    rct_tile_element * const tileElement = map_get_nth_element_at(x, y, elementIndex);

    if (tileElement == nullptr || tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_SMALL_SCENERY)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        // Set quadrant index
        tileElement->type &= ~TILE_ELEMENT_QUADRANT_MASK;
        tileElement->type |= quarterIndex << 6;

        // Update collision
        tileElement->flags &= 0xF0;
        tileElement->flags |= 1 << ((quarterIndex + 2) & 3);

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
    rct_tile_element * const tileElement = map_get_nth_element_at(x, y, elementIndex);

    if (tileElement == nullptr || tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_SMALL_SCENERY)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        tileElement->flags ^= 1 << quarterIndex;

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
    rct_tile_element * const bannerElement = map_get_nth_element_at(x, y, elementIndex);

    if (bannerElement == nullptr || tile_element_get_type(bannerElement) != TILE_ELEMENT_TYPE_BANNER)
        return MONEY32_UNDEFINED;

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
    rct_tile_element * const corruptElement = map_get_nth_element_at(x, y, elementIndex);

    if (corruptElement == nullptr || tile_element_get_type(corruptElement) != TILE_ELEMENT_TYPE_CORRUPT)
        return MONEY32_UNDEFINED;

    if (tile_element_is_last_for_tile(corruptElement))
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        rct_tile_element * const nextElement = corruptElement + 1;
        corruptElement->base_height = corruptElement->clearance_height = nextElement->base_height;

        if ((uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}
