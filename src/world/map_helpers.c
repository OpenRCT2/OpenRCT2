/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "map.h"
#include "map_helpers.h"

/**
 * Not perfect, this still leaves some particular tiles unsmoothed.
 */
int map_smooth(int l, int t, int r, int b)
{
	int i, x, y, highest, count, cornerHeights[4], doubleCorner, raisedLand = 0;
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
				int canCompensate = 1;
				for (i = 0; i < 4; i++)
					if (cornerHeights[i] == highest){
						count++;

						// Check if surrounding corners aren't too high. The current tile 
						// can't compensate for all the height differences anymore if it has 
						// the extra height slope.
						int highestOnLowestSide;
						switch (i){
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

int map_get_corner_height(int x, int y, int corner)
{
	rct_map_element *mapElement = map_get_surface_element_at(x, y);
	int baseHeight = mapElement->base_height;
	int slope = mapElement->properties.surface.slope;
	int doubleCorner = slope & 16;
	if (doubleCorner) {
		if (!(slope & 1)) doubleCorner = 4;
		else if (!(slope & 2)) doubleCorner = 8;
		else if (!(slope & 4)) doubleCorner = 1;
		else if (!(slope & 8)) doubleCorner = 2;
	}

	switch (corner) {
	case 0:
		return baseHeight + (slope & 1 ? (doubleCorner == 1 ? 4 : 2) : 0);
	case 1:
		return baseHeight + (slope & 8 ? (doubleCorner == 8 ? 4 : 2) : 0);
	case 2:
		return baseHeight + (slope & 2 ? (doubleCorner == 2 ? 4 : 2) : 0);
	case 3:
		return baseHeight + (slope & 4 ? (doubleCorner == 4 ? 4 : 2) : 0);
	default:
		return baseHeight;
	}
}

/**
 * There are non-smoothed tiles with this version, but diagonal land blocks end up being wavy.
 */
int map_smooth_wavy(int l, int t, int r, int b)
{
	int i, x, y, highest, count, cornerHeights[4], doubleCorner, raisedLand = 0;
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