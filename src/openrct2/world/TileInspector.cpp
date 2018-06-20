/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Banner.h"
#include "../common.h"
#include "../Context.h"
#include "../Game.h"
#include "../core/Guard.hpp"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../ride/Track.h"
#include "../windows/Intent.h"
#include "../windows/tile_inspector.h"
#include "Footpath.h"
#include "LargeScenery.h"
#include "Map.h"
#include "Scenery.h"
#include "Surface.h"
#include "TileInspector.h"
#include "../ride/Station.h"
#include "Park.h"

using namespace OpenRCT2;

uint32_t windowTileInspectorTileX;
uint32_t windowTileInspectorTileY;
int32_t windowTileInspectorElementCount = 0;
int32_t windowTileInspectorSelectedIndex;

static bool map_swap_elements_at(int32_t x, int32_t y, int16_t first, int16_t second)
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
    if ((firstElement)->IsLastForTile() || (secondElement)->IsLastForTile())
    {
        firstElement->flags ^= TILE_ELEMENT_FLAG_LAST_TILE;
        secondElement->flags ^= TILE_ELEMENT_FLAG_LAST_TILE;
    }

    return true;
}

/**
 * Inserts a corrupt element under a given element on a given tile
 * @param x The x coordinate of the tile
 * @param y The y coordinate of the tile
 * @param elementIndex The nth element on this tile
 * Returns 0 on success, MONEY_UNDEFINED otherwise.
 */
int32_t tile_inspector_insert_corrupt_at(int32_t x, int32_t y, int16_t elementIndex, int32_t flags)
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
        for (int16_t i = 0; i < elementIndex; i++)
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
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            windowTileInspectorElementCount++;

            // Keep other elements (that are not being hidden) selected
            if (windowTileInspectorSelectedIndex > elementIndex)
            {
                windowTileInspectorSelectedIndex++;
            }

            window_invalidate(tileInspectorWindow);
        }
    }

    // Nothing went wrong
    return 0;
}

/**
 * Forcefully removes an element for a given tile
 * @param x The x coordinate of the tile
 * @param y The y coordinate of the tile
 * @param elementIndex The nth element on this tile
 */
int32_t tile_inspector_remove_element_at(int32_t x, int32_t y, int16_t elementIndex, int32_t flags)
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
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            windowTileInspectorElementCount--;

            if (windowTileInspectorSelectedIndex > elementIndex)
            {
                windowTileInspectorSelectedIndex--;
            }
            else if (windowTileInspectorSelectedIndex == elementIndex)
            {
                windowTileInspectorSelectedIndex = -1;
            }

            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_swap_elements_at(int32_t x, int32_t y, int16_t first, int16_t second, int32_t flags)
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
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            // If one of them was selected, update selected list item
            if (windowTileInspectorSelectedIndex == first)
                windowTileInspectorSelectedIndex = second;
            else if (windowTileInspectorSelectedIndex == second)
                windowTileInspectorSelectedIndex = first;

            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_rotate_element_at(int32_t x, int32_t y, int32_t elementIndex, int32_t flags)
{
    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        uint8_t newRotation, pathEdges, pathCorners;

        rct_tile_element * const tileElement = map_get_nth_element_at(x, y, elementIndex);
        if (!tileElement)
        {
            return MONEY32_UNDEFINED;
        }
        switch (tileElement->GetType())
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
            uint8_t  stationIndex = tileElement->properties.entrance.index;
            auto   entrance     = ride_get_entrance_location(ride, stationIndex);
            auto   exit         = ride_get_exit_location(ride, stationIndex);
            uint8_t  entranceType = entrance_element_get_type(tileElement);
            uint8_t  z            = tileElement->base_height;

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
        {
            uint8_t unblockedEdges = tileElement->properties.banner.flags & 0xF;
            unblockedEdges = (unblockedEdges << 1 | unblockedEdges >> 3) & 0xF;
            tileElement->properties.banner.flags &= ~0xF;
            tileElement->properties.banner.flags |= unblockedEdges;
            tileElement->properties.banner.position++;
            tileElement->properties.banner.position &= 3;
            break;
        }
        }

        map_invalidate_tile_full(x << 5, y << 5);

        if ((uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}

int32_t tile_inspector_paste_element_at(int32_t x, int32_t y, rct_tile_element element, int32_t flags)
{
    // Make sure there is enough space for the new element
    if (!map_check_free_elements_and_reorganise(1))
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        // Check if the element to be pasted refers to a banner index
        BannerIndex bannerIndex = tile_element_get_banner_index(&element);
        if (bannerIndex != BANNER_INDEX_NULL)
        {
            // The element to be pasted refers to a banner index - make a copy of it
            BannerIndex newBannerIndex = create_new_banner(flags);
            if (newBannerIndex == BANNER_INDEX_NULL)
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

        bool lastForTile = pastedElement->IsLastForTile();
        *pastedElement   = element;
        pastedElement->flags &= ~TILE_ELEMENT_FLAG_LAST_TILE;
        if (lastForTile)
        {
            pastedElement->flags |= TILE_ELEMENT_FLAG_LAST_TILE;
        }

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            windowTileInspectorElementCount++;

            // Select new element if there was none selected already
            int16_t newIndex = (int16_t)(pastedElement - map_get_first_element_at(x, y));
            if (windowTileInspectorSelectedIndex == -1)
                windowTileInspectorSelectedIndex = newIndex;
            else if (windowTileInspectorSelectedIndex >= newIndex)
                windowTileInspectorSelectedIndex++;

            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_sort_elements_at(int32_t x, int32_t y, int32_t flags)
{
    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        const rct_tile_element * const firstElement = map_get_first_element_at(x, y);

        // Count elements on tile
        int32_t                   numElement      = 0;
        const rct_tile_element * elementIterator = firstElement;
        do
        {
            numElement++;
        } while (!(elementIterator++)->IsLastForTile());

        // Bubble sort
        for (int32_t loopStart = 1; loopStart < numElement; loopStart++)
        {
            int32_t                   currentId      = loopStart;
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
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            windowTileInspectorSelectedIndex = -1;
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_any_base_height_offset(int32_t x, int32_t y, int16_t elementIndex, int8_t heightOffset, int32_t flags)
{
    rct_tile_element * const tileElement = map_get_nth_element_at(x, y, elementIndex);
    if (tileElement == nullptr)
        return MONEY32_UNDEFINED;

    int16_t newBaseHeight      = (int16_t)tileElement->base_height + heightOffset;
    int16_t newClearanceHeight = (int16_t)tileElement->clearance_height + heightOffset;
    if (newBaseHeight < 0 || newBaseHeight > 0xff || newClearanceHeight < 0 || newClearanceHeight > 0xff)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
        {
            uint8_t entranceType = tileElement->properties.entrance.type;
            if (entranceType != ENTRANCE_TYPE_PARK_ENTRANCE)
            {
                // Update the ride's known entrance or exit height
                Ride * ride          = get_ride(tileElement->properties.entrance.ride_index);
                uint8_t  entranceIndex = tileElement->properties.entrance.index;
                auto   entrance      = ride_get_entrance_location(ride, entranceIndex);
                auto   exit          = ride_get_exit_location(ride, entranceIndex);
                uint8_t z = tileElement->base_height;

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
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_surface_show_park_fences(int32_t x, int32_t y, bool showFences, int32_t flags)
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
            update_park_fences({x << 5, y << 5});

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_surface_toggle_corner(int32_t x, int32_t y, int32_t cornerIndex, int32_t flags)
{
    rct_tile_element * const surfaceElement = map_get_surface_element_at(x, y);

    // No surface element on tile
    if (surfaceElement == nullptr)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        const uint8_t originalSlope = surfaceElement->properties.surface.slope;
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
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_surface_toggle_diagonal(int32_t x, int32_t y, int32_t flags)
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
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_path_set_sloped(int32_t x, int32_t y, int32_t elementIndex, bool sloped, int32_t flags)
{
    rct_tile_element * const pathElement = map_get_nth_element_at(x, y, elementIndex);

    if (pathElement == nullptr || pathElement->GetType() != TILE_ELEMENT_TYPE_PATH)
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
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_path_toggle_edge(int32_t x, int32_t y, int32_t elementIndex, int32_t edgeIndex, int32_t flags)
{
    rct_tile_element * const pathElement = map_get_nth_element_at(x, y, elementIndex);

    if (pathElement == nullptr || pathElement->GetType() != TILE_ELEMENT_TYPE_PATH)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        pathElement->properties.path.edges ^= 1 << edgeIndex;

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_entrance_make_usable(int32_t x, int32_t y, int32_t elementIndex, int32_t flags)
{
    rct_tile_element * const entranceElement = map_get_nth_element_at(x, y, elementIndex);

    if (entranceElement == nullptr || entranceElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
        return MONEY32_UNDEFINED;

    Ride * ride = get_ride(entranceElement->properties.entrance.ride_index);

    if (ride == nullptr)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        uint8_t stationIndex = entranceElement->properties.entrance.index >> 6;

        switch (entranceElement->properties.entrance.type)
        {
        case ENTRANCE_TYPE_RIDE_ENTRANCE:
            ride_set_entrance_location(ride, stationIndex, { x, y, entranceElement->base_height, (uint8_t)tile_element_get_direction(entranceElement) });
            break;
        case ENTRANCE_TYPE_RIDE_EXIT:
            ride_set_exit_location(ride, stationIndex, { x, y, entranceElement->base_height, (uint8_t)tile_element_get_direction(entranceElement) });
            break;
        }

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

int32_t tile_inspector_wall_set_slope(int32_t x, int32_t y, int32_t elementIndex, int32_t slopeValue, int32_t flags)
{
    rct_tile_element * const wallElement = map_get_nth_element_at(x, y, elementIndex);

    if (wallElement == nullptr || wallElement->GetType() != TILE_ELEMENT_TYPE_WALL)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        // Set new slope value
        wallElement->type &= ~0xC0;
        wallElement->type |= slopeValue;

        map_invalidate_tile_full(x << 5, y << 5);

        rct_window * const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && (uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate(tileInspectorWindow);
        }
    }

    return 0;
}

// Changes the height of all track elements that belong to the same track piece
// Broxzier: Copied from track_remove and stripped of unneeded code, but I think this should be smaller
int32_t tile_inspector_track_base_height_offset(int32_t x, int32_t y, int32_t elementIndex, int8_t offset, int32_t flags)
{
    rct_tile_element * const trackElement = map_get_nth_element_at(x, y, elementIndex);

    if (offset == 0)
        return 0;

    if (trackElement == nullptr || trackElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        uint8_t                     type       = track_element_get_type(trackElement);
        int16_t                    originX    = x << 5;
        int16_t                    originY    = y << 5;
        int16_t                    originZ    = trackElement->base_height * 8;
        uint8_t                     rotation   = tile_element_get_direction(trackElement);
        uint8_t                     rideIndex  = track_element_get_ride_index(trackElement);
        Ride *                    ride       = get_ride(rideIndex);
        const rct_preview_track * trackBlock = get_track_def_from_ride(ride, type);
        trackBlock += tile_element_get_track_sequence(trackElement);

        uint8_t originDirection = tile_element_get_direction(trackElement);
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
            int16_t elemX = originX, elemY = originY, elemZ = originZ;

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

                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;

                if ((tile_element_get_direction(tileElement)) != rotation)
                    continue;

                if (tile_element_get_track_sequence(tileElement) != trackBlock->index)
                    continue;

                if (track_element_get_type(tileElement) != type)
                    continue;

                found = true;
                break;
            } while (!(tileElement++)->IsLastForTile());

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
int32_t tile_inspector_track_set_chain(int32_t x, int32_t y, int32_t elementIndex, bool entireTrackBlock, bool setChain,
                                      int32_t flags)
{
    rct_tile_element * const trackElement = map_get_nth_element_at(x, y, elementIndex);

    if (trackElement == nullptr || trackElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (!entireTrackBlock)
        {
            // Set chain for only the selected piece
            if (track_element_is_lift_hill(trackElement) != setChain)
            {
                trackElement->type ^= TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
            }

            return 0;
        }

        uint8_t                     type       = track_element_get_type(trackElement);
        int16_t                    originX    = x << 5;
        int16_t                    originY    = y << 5;
        int16_t                    originZ    = trackElement->base_height * 8;
        uint8_t                     rotation   = tile_element_get_direction(trackElement);
        uint8_t                     rideIndex  = track_element_get_ride_index(trackElement);
        Ride *                    ride       = get_ride(rideIndex);
        const rct_preview_track * trackBlock = get_track_def_from_ride(ride, type);
        trackBlock += tile_element_get_track_sequence(trackElement);

        uint8_t originDirection = tile_element_get_direction(trackElement);
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
            int16_t elemX = originX, elemY = originY, elemZ = originZ;

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

                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;

                if ((tile_element_get_direction(tileElement)) != rotation)
                    continue;

                if (tile_element_get_track_sequence(tileElement) != trackBlock->index)
                    continue;

                if (track_element_get_type(tileElement) != type)
                    continue;

                found = true;
                break;
            } while (!(tileElement++)->IsLastForTile());

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
                tileElement->type ^= TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
            }
        }
    }

    // TODO: Only invalidate when one of the affected tiles is selected
    window_invalidate_by_class(WC_TILE_INSPECTOR);

    return 0;
}

int32_t tile_inspector_scenery_set_quarter_location(int32_t x, int32_t y, int32_t elementIndex, int32_t quarterIndex, int32_t flags)
{
    rct_tile_element * const tileElement = map_get_nth_element_at(x, y, elementIndex);

    if (tileElement == nullptr || tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
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
        if ((uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}

int32_t tile_inspector_scenery_set_quarter_collision(int32_t x, int32_t y, int32_t elementIndex, int32_t quarterIndex, int32_t flags)
{
    rct_tile_element * const tileElement = map_get_nth_element_at(x, y, elementIndex);

    if (tileElement == nullptr || tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        tileElement->flags ^= 1 << quarterIndex;

        map_invalidate_tile_full(x << 5, y << 5);
        if ((uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}

int32_t tile_inspector_banner_toggle_blocking_edge(int32_t x, int32_t y, int32_t elementIndex, int32_t edgeIndex, int32_t flags)
{
    rct_tile_element * const bannerElement = map_get_nth_element_at(x, y, elementIndex);

    if (bannerElement == nullptr || bannerElement->GetType() != TILE_ELEMENT_TYPE_BANNER)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        bannerElement->properties.banner.flags ^= 1 << edgeIndex;

        if ((uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}

int32_t tile_inspector_corrupt_clamp(int32_t x, int32_t y, int32_t elementIndex, int32_t flags)
{
    rct_tile_element * const corruptElement = map_get_nth_element_at(x, y, elementIndex);

    if (corruptElement == nullptr || corruptElement->GetType() != TILE_ELEMENT_TYPE_CORRUPT)
        return MONEY32_UNDEFINED;

    if (corruptElement->IsLastForTile())
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        rct_tile_element * const nextElement = corruptElement + 1;
        corruptElement->base_height = corruptElement->clearance_height = nextElement->base_height;

        if ((uint32_t)x == windowTileInspectorTileX && (uint32_t)y == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return 0;
}
