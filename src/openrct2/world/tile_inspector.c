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
#include "../game.h"
#include "../interface/window.h"
#include "../windows/tile_inspector.h"
#include "footpath.h"
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
				window_tile_inspector_set_page(tile_inspector_window, TILE_INSPECTOR_PAGE_CORRUPT);
			}

			window_tile_inspector_auto_set_buttons(tile_inspector_window);
			window_invalidate(tile_inspector_window);
		}
	}

	// Nothing went wrong
	return 0;
}

/**
* Forcefully removes an element for a given tile
* @param x, y: The coordinates of the tile
* @param element_index: The nth element on this tile
*/
sint32 tile_inspector_remove_element_at(sint32 x, sint32 y, sint16 element_index, sint32 flags)
{
	if (flags & GAME_COMMAND_FLAG_APPLY)
	{
		// Forcefully remove the element
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
				window_tile_inspector_set_page(tile_inspector_window, TILE_INSPECTOR_PAGE_DEFAULT);
			}

			window_tile_inspector_auto_set_buttons(tile_inspector_window);
			window_invalidate(tile_inspector_window);
		}
	}

	return 0;
}

sint32 tile_inspector_swap_elements_at(sint32 x, sint32 y, sint16 first, sint16 second, sint32 flags)
{
	if (flags & GAME_COMMAND_FLAG_APPLY)
	{
		map_swap_elements_at(x, y, first, second);
		map_invalidate_tile_full(x << 5, y << 5);

		// Update the window
		rct_window *const tile_inspector_window = window_find_by_class(WC_TILE_INSPECTOR);
		if (tile_inspector_window != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
		{
			// If one of them was selected, update selected list item
			if (tile_inspector_window->selected_list_item == first)
				tile_inspector_window->selected_list_item = second;
			else if (tile_inspector_window->selected_list_item == second)
				tile_inspector_window->selected_list_item = first;

			window_tile_inspector_auto_set_buttons(tile_inspector_window);
			window_invalidate(tile_inspector_window);
		}
	}

	return 0;
}

sint32 tile_inspector_rotate_element_at(sint32 x, sint32 y, sint32 element_index, sint32 flags)
{
	if (flags & GAME_COMMAND_FLAG_APPLY)
	{
		uint8 newRotation, pathEdges, pathCorners;

		rct_map_element *const mapElement = map_get_first_element_at(x, y) + element_index;
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
		case MAP_ELEMENT_TYPE_FENCE:
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

		rct_window *const tile_inspector_window = window_find_by_class(WC_TILE_INSPECTOR);
		if (tile_inspector_window != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
		{
			windowTileInspectorElementCount++;

			// Select new element if there was none selected already
			sint16 new_index = (sint16)(pastedElement - map_get_first_element_at(x, y));
			if (tile_inspector_window->selected_list_item == -1)
				tile_inspector_window->selected_list_item = new_index;
			else if (tile_inspector_window->selected_list_item >= new_index)
				tile_inspector_window->selected_list_item++;

			window_tile_inspector_auto_set_buttons(tile_inspector_window);
			window_invalidate(tile_inspector_window);
		}
	}

	return 0;
}

sint32 tile_inspector_sort_elements_at(sint32 x, sint32 y, sint32 flags)
{
	if (flags & GAME_COMMAND_FLAG_APPLY)
	{
		const rct_map_element *const first_element = map_get_first_element_at(x, y);

		// Count elements on tile
		sint32 num_element = 0;
		const rct_map_element *element_iterator = first_element;
		do
		{
			num_element++;
		} while (!map_element_is_last_for_tile(element_iterator++));

		// Bubble sort
		for (sint32 loopStart = 1; loopStart < num_element; loopStart++)
		{
			sint32 currentId = loopStart;
			const rct_map_element *currentElement = first_element + currentId;
			const rct_map_element *otherElement = currentElement - 1;

			// While current element's base height is lower, or (when their baseheight is the same) the other map element's clearance height is lower...
			while (currentId > 0 &&
				(otherElement->base_height > currentElement->base_height ||
				(otherElement->base_height == currentElement->base_height && otherElement->clearance_height > currentElement->clearance_height)))
			{
				map_swap_elements_at(x, y, currentId - 1, currentId);
				currentId--;
				currentElement--;
				otherElement--;
			}
		}

		map_invalidate_tile_full(x << 5, y << 5);

		// Deselect tile for clients who had it selected
		rct_window *const tile_inspector_window = window_find_by_class(WC_TILE_INSPECTOR);
		if (tile_inspector_window != NULL && (uint32)x == windowTileInspectorTileX && (uint32)y == windowTileInspectorTileY)
		{
			window_tile_inspector_set_page(tile_inspector_window, TILE_INSPECTOR_PAGE_DEFAULT);
			tile_inspector_window->selected_list_item = -1;
			window_tile_inspector_auto_set_buttons(tile_inspector_window);
			window_invalidate(tile_inspector_window);
		}
	}

	return 0;
}

sint32 tile_inspector_change_base_height_at(sint32 x, sint32 y, sint16 element_index, sint8 height_offset, sint32 flags)
{
	rct_map_element *const map_element = map_get_first_element_at(x, y) + element_index;
	sint16 new_base_height = (sint16)map_element->base_height + height_offset;
	sint16 new_clearance_height = (sint16)map_element->clearance_height + height_offset;
	if (new_base_height < 0 || new_base_height > 0xff || new_clearance_height < 0 || new_clearance_height > 0xff)
	{
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY)
	{
		map_element->base_height += height_offset;
		map_element->clearance_height += height_offset;

		map_invalidate_tile_full(x << 5, y << 5);
		window_invalidate_by_class(WC_TILE_INSPECTOR);
	}

	return 0;
}

sint32 tile_inspector_surface_show_park_fences(sint32 x, sint32 y, bool show_fences, sint32 flags)
{
	rct_map_element *const surface = map_get_surface_element_at(x, y);

	// No surface element on tile
	if (surface == NULL)
		return MONEY32_UNDEFINED;

	if (flags & GAME_COMMAND_FLAG_APPLY)
	{
		if (!show_fences)
			surface->properties.surface.ownership &= ~0x0F;
		else
			update_park_fences(x << 5, y << 5);

		map_invalidate_tile_full(x << 5, y << 5);
	}

	return 0;
}

sint32 tile_inspector_surface_toggle_corner(sint32 x, sint32 y, sint32 corner_index, sint32 flags)
{
	rct_map_element *const surface = map_get_surface_element_at(x, y);

	// No surface element on tile
	if (surface == NULL)
		return MONEY32_UNDEFINED;

	if (flags & GAME_COMMAND_FLAG_APPLY)
	{
		const uint8 originalSlope = surface->properties.surface.slope;
		const bool diagonal = (originalSlope & 0x10) >> 4;

		surface->properties.surface.slope ^= 1 << corner_index;
		if (surface->properties.surface.slope & 0x0F)
		{
			surface->clearance_height = surface->base_height + 2;
		}
		else
		{
			surface->clearance_height = surface->base_height;
		}

		// All corners are raised
		if ((surface->properties.surface.slope & 0x0F) == 0x0F)
		{
			surface->properties.surface.slope &= ~0x1F;

			if (diagonal)
			{
				switch (originalSlope & 0x0F)
				{
				case 0b1011: surface->properties.surface.slope |= (1 << 0); break;
				case 0b0111: surface->properties.surface.slope |= (1 << 1); break;
				case 0b1110: surface->properties.surface.slope |= (1 << 2); break;
				case 0b1101: surface->properties.surface.slope |= (1 << 3); break;
				}
			}

			// Update base and clearance heights
			surface->base_height += 2;
			surface->clearance_height = surface->base_height + (diagonal ? 2 : 0);
		}

		map_invalidate_tile_full(x << 5, y << 5);
		window_invalidate_by_class(WC_TILE_INSPECTOR);
	}

	return 0;
}
