#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "map.h"
#include "map_helpers.h"

/**
 * Not perfect, this still leaves some particular tiles unsmoothed.
 */
sint32 map_smooth(sint32 l, sint32 t, sint32 r, sint32 b)
{
	sint32 i, x, y, highest, count, cornerHeights[4], doubleCorner, raisedLand = 0;
	rct_map_element *mapElement, *mapElement2;
	for (y = t; y < b; y++) {
		for (x = l; x < r; x++) {
			mapElement = map_get_surface_element_at(x, y);
			mapElement->properties.surface.slope &= ~0x1F;

			// Raise to edge height - 2
			highest = mapElement->base_height;
			highest = max(highest, map_get_surface_element_at(x - 1, y + 0)->base_height);
			highest = max(highest, map_get_surface_element_at(x + 1, y + 0)->base_height);
			highest = max(highest, map_get_surface_element_at(x + 0, y - 1)->base_height);
			highest = max(highest, map_get_surface_element_at(x + 0, y + 1)->base_height);
			if (mapElement->base_height < highest - 2) {
				raisedLand = 1;
				mapElement->base_height = mapElement->clearance_height = highest - 2;
			}

			// Check corners
			doubleCorner = -1;
			cornerHeights[0] = map_get_surface_element_at(x - 1, y - 1)->base_height;
			cornerHeights[1] = map_get_surface_element_at(x + 1, y - 1)->base_height;
			cornerHeights[2] = map_get_surface_element_at(x + 1, y + 1)->base_height;
			cornerHeights[3] = map_get_surface_element_at(x - 1, y + 1)->base_height;
			highest = mapElement->base_height;
			for (i = 0; i < 4; i++)
				highest = max(highest, cornerHeights[i]);

			if (highest >= mapElement->base_height + 4) {
				count = 0;
				sint32 canCompensate = 1;
				for (i = 0; i < 4; i++)
					if (cornerHeights[i] == highest){
						count++;

						// Check if surrounding corners aren't too high. The current tile
						// can't compensate for all the height differences anymore if it has
						// the extra height slope.
						sint32 highestOnLowestSide;
						switch (i){
						default:
						case 0:
							highestOnLowestSide = max(
								map_get_surface_element_at(x + 1, y)->base_height,
								map_get_surface_element_at(x, y + 1)->base_height);
							break;
						case 1:
							highestOnLowestSide = max(
								map_get_surface_element_at(x - 1, y)->base_height,
								map_get_surface_element_at(x, y + 1)->base_height);
							break;
						case 2:
							highestOnLowestSide = max(
								map_get_surface_element_at(x - 1, y)->base_height,
								map_get_surface_element_at(x, y - 1)->base_height);
							break;
						case 3:
							highestOnLowestSide = max(
								map_get_surface_element_at(x + 1, y)->base_height,
								map_get_surface_element_at(x, y - 1)->base_height);
							break;
						}

						if (highestOnLowestSide > mapElement->base_height){
							mapElement->base_height = mapElement->clearance_height = highestOnLowestSide;
							raisedLand = 1;
							canCompensate = 0;
						}
					}

				if (count == 1 && canCompensate) {
					if (mapElement->base_height < highest - 4) {
						mapElement->base_height = mapElement->clearance_height = highest - 4;
						raisedLand = 1;
					}
					if (cornerHeights[0] == highest && cornerHeights[2] <= cornerHeights[0] - 4)
						doubleCorner = 0;
					else if (cornerHeights[1] == highest && cornerHeights[3] <= cornerHeights[1] - 4)
						doubleCorner = 1;
					else if (cornerHeights[2] == highest && cornerHeights[0] <= cornerHeights[2] - 4)
						doubleCorner = 2;
					else if (cornerHeights[3] == highest && cornerHeights[1] <= cornerHeights[3] - 4)
						doubleCorner = 3;
				} else {
					if (mapElement->base_height < highest - 2) {
						mapElement->base_height = mapElement->clearance_height = highest - 2;
						raisedLand = 1;
					}
				}
			}

			if (doubleCorner != -1) {
				mapElement->properties.surface.slope |= 16;
				switch (doubleCorner) {
				case 0:
					mapElement->properties.surface.slope |= 2 | 4 | 8;
					break;
				case 1:
					mapElement->properties.surface.slope |= 1 | 2 | 4;
					break;
				case 2:
					mapElement->properties.surface.slope |= 1 | 2 | 8;
					break;
				case 3:
					mapElement->properties.surface.slope |= 1 | 4 | 8;
					break;
				}
			} else {
				// Corners
				mapElement2 = map_get_surface_element_at(x + 1, y + 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 1;

				mapElement2 = map_get_surface_element_at(x - 1, y + 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 8;

				mapElement2 = map_get_surface_element_at(x + 1, y - 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 2;

				mapElement2 = map_get_surface_element_at(x - 1, y - 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 4;

				// Sides
				mapElement2 = map_get_surface_element_at(x + 1, y + 0);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 1 | 2;

				mapElement2 = map_get_surface_element_at(x - 1, y + 0);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 4 | 8;

				mapElement2 = map_get_surface_element_at(x + 0, y - 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 2 | 4;

				mapElement2 = map_get_surface_element_at(x + 0, y + 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 1 | 8;

				// Raise
				if (mapElement->properties.surface.slope == (1 | 2 | 4 | 8)) {
					mapElement->properties.surface.slope &= ~0x1F;
					mapElement->base_height = mapElement->clearance_height += 2;
				}
			}
		}
	}

	return raisedLand;
}

static sint32 map_get_corner_height(sint32 x, sint32 y, sint32 corner)
{
	rct_map_element *mapElement = map_get_surface_element_at(x, y);
	sint32 baseHeight = mapElement->base_height;
	sint32 slope = mapElement->properties.surface.slope;
	sint32 doubleCorner = slope & 16;
	if (doubleCorner) {
		if (!(slope & 1)) doubleCorner = 4;
		else if (!(slope & 2)) doubleCorner = 8;
		else if (!(slope & 4)) doubleCorner = 1;
		else if (!(slope & 8)) doubleCorner = 2;
	}

	switch (corner) {
	case 0:
		return baseHeight + ((slope & 1) ? (doubleCorner == 1 ? 4 : 2) : 0);
	case 1:
		return baseHeight + ((slope & 8) ? (doubleCorner == 8 ? 4 : 2) : 0);
	case 2:
		return baseHeight + ((slope & 2) ? (doubleCorner == 2 ? 4 : 2) : 0);
	case 3:
		return baseHeight + ((slope & 4) ? (doubleCorner == 4 ? 4 : 2) : 0);
	default:
		return baseHeight;
	}
}

/**
 * There are non-smoothed tiles with this version, but diagonal land blocks end up being wavy.
 */
static sint32 map_smooth_wavy(sint32 l, sint32 t, sint32 r, sint32 b)
{
	sint32 i, x, y, highest, count, cornerHeights[4], doubleCorner, raisedLand = 0;
	rct_map_element *mapElement;
	for (y = t; y < b; y++) {
		for (x = l; x < r; x++) {
			mapElement = map_get_surface_element_at(x, y);
			mapElement->properties.surface.slope &= ~0x1F;

			// Raise to edge height - 2
			highest = mapElement->base_height;
			highest = max(highest, map_get_surface_element_at(x - 1, y + 0)->base_height);
			highest = max(highest, map_get_surface_element_at(x + 1, y + 0)->base_height);
			highest = max(highest, map_get_surface_element_at(x + 0, y - 1)->base_height);
			highest = max(highest, map_get_surface_element_at(x + 0, y + 1)->base_height);
			if (mapElement->base_height < highest - 2) {
				raisedLand = 1;
				mapElement->base_height = mapElement->clearance_height = highest - 2;
			}

			// Check corners
			doubleCorner = -1;
			cornerHeights[0] = max(map_get_corner_height(x - 1, y - 1, 0), max(map_get_corner_height(x + 1, y + 0, 1), map_get_corner_height(x + 0, y + 1, 2)));
			cornerHeights[1] = max(map_get_corner_height(x + 1, y - 1, 1), max(map_get_corner_height(x - 1, y + 0, 0), map_get_corner_height(x + 0, y + 1, 3)));
			cornerHeights[2] = max(map_get_corner_height(x + 1, y + 1, 3), max(map_get_corner_height(x + 1, y + 0, 3), map_get_corner_height(x + 0, y - 1, 0)));
			cornerHeights[3] = max(map_get_corner_height(x - 1, y + 1, 2), max(map_get_corner_height(x - 1, y + 0, 2), map_get_corner_height(x + 0, y - 1, 1)));
			highest = mapElement->base_height;
			for (i = 0; i < 4; i++)
				highest = max(highest, cornerHeights[i]);

			if (highest >= mapElement->base_height + 4) {
				count = 0;
				for (i = 0; i < 4; i++)
					if (cornerHeights[i] == highest)
						count++;

				if (count == 1) {
					if (mapElement->base_height < highest - 4) {
						mapElement->base_height = mapElement->clearance_height = highest - 4;
						raisedLand = 1;
					}
					if (cornerHeights[0] == highest && cornerHeights[2] <= cornerHeights[0] - 4)
						doubleCorner = 0;
					else if (cornerHeights[1] == highest && cornerHeights[3] <= cornerHeights[1] - 4)
						doubleCorner = 1;
					else if (cornerHeights[2] == highest && cornerHeights[0] <= cornerHeights[2] - 4)
						doubleCorner = 2;
					else if (cornerHeights[3] == highest && cornerHeights[1] <= cornerHeights[3] - 4)
						doubleCorner = 3;
				} else {
					if (mapElement->base_height < highest - 2) {
						mapElement->base_height = mapElement->clearance_height = highest - 2;
						raisedLand = 1;
					}
				}
			}

			if (doubleCorner != -1) {
				mapElement->properties.surface.slope |= 16;
				switch (doubleCorner) {
				case 0:
					mapElement->properties.surface.slope |= 2 | 4 | 8;
					break;
				case 1:
					mapElement->properties.surface.slope |= 1 | 2 | 4;
					break;
				case 2:
					mapElement->properties.surface.slope |= 1 | 2 | 8;
					break;
				case 3:
					mapElement->properties.surface.slope |= 1 | 4 | 8;
					break;
				}
			} else {
				// Corners
				if (
					map_get_corner_height(x + 1, y + 1, 3) > mapElement->base_height ||
					map_get_corner_height(x + 1, y + 0, 1) > mapElement->base_height ||
					map_get_corner_height(x + 0, y + 1, 2) > mapElement->base_height
				)
					mapElement->properties.surface.slope |= 1;

				if (
					map_get_corner_height(x - 1, y + 1, 2) > mapElement->base_height ||
					map_get_corner_height(x - 1, y + 0, 0) > mapElement->base_height ||
					map_get_corner_height(x + 0, y + 1, 3) > mapElement->base_height
				)
					mapElement->properties.surface.slope |= 8;

				if (
					map_get_corner_height(x + 1, y - 1, 1) > mapElement->base_height ||
					map_get_corner_height(x + 1, y + 0, 3) > mapElement->base_height ||
					map_get_corner_height(x + 0, y - 1, 0) > mapElement->base_height
				)
					mapElement->properties.surface.slope |= 2;

				if (
					map_get_corner_height(x - 1, y - 1, 0) > mapElement->base_height ||
					map_get_corner_height(x - 1, y + 0, 2) > mapElement->base_height ||
					map_get_corner_height(x + 0, y - 1, 1) > mapElement->base_height
				)
					mapElement->properties.surface.slope |= 4;

				// Raise
				if (mapElement->properties.surface.slope == (1 | 2 | 4 | 8)) {
					mapElement->properties.surface.slope &= ~0x1F;
					mapElement->base_height = mapElement->clearance_height += 2;
				}
			}
		}
	}

	return raisedLand;
}

/**
 * Raises the corners based on the height offset of neightbour tiles.
 * This does not change the base height, unless all corners have been raised.
 * @returns 0 if no edits were made, 1 otherwise
 */
sint32 tile_smooth(sint32 x, sint32 y)
{
	rct_map_element *const surfaceElement = map_get_surface_element_at(x, y);

	// +-----+-----+-----+
	// | NW  |  N  | NE  |
	// |  2  |  1  |  0  |
	// +-----+-----+-----+
	// |  W  |  _  |  E  |
	// |  4  |     |  3  |
	// +-----+-----+-----+
	// | SW  |  S  | SE  |
	// |  7  |  6  |  5  |
	// +-----+-----+-----+
	union
	{
		sint32 baseheight[8];
		struct { sint32 NE, N, NW, E, W, SE, S, SW; };
	} neighbourHeightOffset = { 0 };

	// Find the neighbour base heights
	for (sint32 index = 0, y_offset = -1; y_offset <= 1; y_offset++)
	{
		for (sint32 x_offset = -1; x_offset <= 1; x_offset++)
		{
			// Skip self
			if (y_offset == 0 && x_offset == 0)
				continue;

			// Get neightbour height. If the element is not valid (outside of map) assume the same height
			rct_map_element *neightbour_element = map_get_surface_element_at(x + x_offset, y + y_offset);
			neighbourHeightOffset.baseheight[index] = neightbour_element ? neightbour_element->base_height : surfaceElement->base_height;

			// Make the height relative to the current surface element
			neighbourHeightOffset.baseheight[index] -= surfaceElement->base_height;

			index++;
		}
	}

	// Count number from the three tiles that is currently higher
	sint8 thresholdNW = clamp(neighbourHeightOffset.W, 0, 1) + clamp(neighbourHeightOffset.NW, 0, 1) + clamp(neighbourHeightOffset.N, 0, 1);
	sint8 thresholdNE = clamp(neighbourHeightOffset.N, 0, 1) + clamp(neighbourHeightOffset.NE, 0, 1) + clamp(neighbourHeightOffset.E, 0, 1);
	sint8 thresholdSE = clamp(neighbourHeightOffset.E, 0, 1) + clamp(neighbourHeightOffset.SE, 0, 1) + clamp(neighbourHeightOffset.S, 0, 1);
	sint8 thresholdSW = clamp(neighbourHeightOffset.S, 0, 1) + clamp(neighbourHeightOffset.SW, 0, 1) + clamp(neighbourHeightOffset.W, 0, 1);

	uint8 slope = 0;
	slope |= (thresholdNW >= 1) << 1;
	slope |= (thresholdNE >= 1) << 2;
	slope |= (thresholdSE >= 1) << 3;
	slope |= (thresholdSW >= 1) << 0;

	// Set diagonal when three corners have been raised, and the middle one can be raised one more
	if ((slope == 0b0111 && neighbourHeightOffset.NW >= 4) || (slope == 0b1011 && neighbourHeightOffset.SW >= 4) ||
		(slope == 0b1101 && neighbourHeightOffset.SE >= 4) || (slope == 0b1110 && neighbourHeightOffset.NE >= 4))
	{
		slope |= 1 << 4;
	}

	// Check if the calculated slope is the same already
	uint8 currentSlope = surfaceElement->properties.surface.slope & 0x1F;
	if (currentSlope == slope)
	{
		return 0;
	}

	// Remove old slope value
	surfaceElement->properties.surface.slope &= ~0x1F;
	if ((slope & 0xF) == 0xF)
	{
		// All corners are raised, raise the entire tile instead.
		surfaceElement->base_height = (surfaceElement->clearance_height += 2);
	}
	else
	{
		// Apply the slope to this tile
		surfaceElement->properties.surface.slope |= slope;

		// Set correct clearance height
		if (slope & 0x10)
			surfaceElement->clearance_height = surfaceElement->base_height + 4;
		else if (slope & 0x0F)
			surfaceElement->clearance_height = surfaceElement->base_height + 2;
	}

	return 1;
}
