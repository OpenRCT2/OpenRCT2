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

#include "../game.h"
#include "../interface/window.h"
#include "../windows/tile_inspector.h"
#include "map.h"
#include "tile_inspector.h"

static void map_swap_elements_at(sint32 x, sint32 y, sint16 first, sint16 second)
{
	rct_map_element *mapElement = map_get_first_element_at(x, y);
	rct_map_element *const firstElement = mapElement + first;
	rct_map_element *const secondElement = mapElement + second;

	// swap_elements shouldn't be called when there is only one element on the tile
	assert(!map_element_is_last_for_tile(mapElement));

	// Make sure both elements are actually on the current tile
	sint16 elementCount = 0;
	do
	{
		elementCount++;
	} while (!map_element_is_last_for_tile(mapElement++));
	assert(elementCount > max(first, second));

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
}

/**
 * Inserts a corrupt element under a given element on a given tile
 * @param x, y: The coordinates of the tile
 * @param element_index: The nth element on this tile
 * Returns 0 on success, MONEY_UNDEFINED otherwise.
 */
sint32 tile_inspector_insert_corrupt_at(sint32 x, sint32 y, sint16 element_index, sint32 flags)
{
	// Make sure there is enough space for the new element
	if (!map_check_free_elements_and_reorganise(1))
	{
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY)
	{
		// Create new corrupt element
		rct_map_element *curruptElement = map_element_insert(x, y, -1, 0); // Ugly hack: -1 guarantees this to be placed first
		if (curruptElement == NULL)
		{
			log_warning("Failed to insert corrupt element.");
			return MONEY32_UNDEFINED;
		}
		curruptElement->type = MAP_ELEMENT_TYPE_CORRUPT;

		// Set the base height to be the same as the selected element
		rct_map_element *const selectedElement = map_get_first_element_at(x, y) + element_index;
		curruptElement->base_height = curruptElement->clearance_height = selectedElement->base_height;

		// Move the corrupt element up until the selected list item is reached
		// this way it's placed under the selected element, even when there are multiple elements with the same base height
		for (sint16 i = 0; i < element_index; i++)
		{
			map_swap_elements_at(x, y, i, i + 1);
		}

		map_invalidate_tile_full(x << 5, y << 5);

		// Update the tile inspector's list for everyone who has the tile selected
		rct_window *const tile_inspector_window = window_find_by_class(WC_TILE_INSPECTOR);
		if (tile_inspector_window != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
		{
			windowTileInspectorElementCount++;

			// Keep other elements (that are not being hidden) selected
			if (tile_inspector_window->selected_list_item > element_index)
			{
				tile_inspector_window->selected_list_item++;
			}

			if (tile_inspector_window->selected_list_item == element_index)
			{
				window_tile_inspector_set_page(tile_inspector_window, PAGE_CORRUPT);
			}

			window_tile_inspector_auto_set_buttons(tile_inspector_window);
			window_invalidate(tile_inspector_window);
		}
	}

	// Nothing went wrong
	return 0;
}

sint32 tile_inspector_remove_element_at(sint32 x, sint32 y, sint16 element_index, sint32 flags)
{
	if (flags & GAME_COMMAND_FLAG_APPLY)
	{
		// Forcefully the element
		rct_map_element *const mapElement = map_get_first_element_at(x, y) + element_index;
		map_element_remove(mapElement);
		map_invalidate_tile_full(x << 5, y << 5);

		// Update the window
		rct_window *const tile_inspector_window = window_find_by_class(WC_TILE_INSPECTOR);
		if (tile_inspector_window != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
		{
			windowTileInspectorElementCount--;

			if (tile_inspector_window->selected_list_item > element_index)
			{
				tile_inspector_window->selected_list_item--;
			}
			else if (tile_inspector_window->selected_list_item == element_index)
			{
				tile_inspector_window->selected_list_item = -1;
				window_tile_inspector_set_page(tile_inspector_window, PAGE_DEFAULT);
			}

			window_tile_inspector_auto_set_buttons(tile_inspector_window);
			window_invalidate(tile_inspector_window);
		}
	}

	return 0;
}
